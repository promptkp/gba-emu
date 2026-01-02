from pathlib import Path
import os
import shutil
import subprocess
import pytest

TEST_RESULT_DIR = "test_results"
TMP_BIN_DIR = "tmp_bin_dir"
TEST_CPU_BIN = "bin/test_cpu"

def overwrite_dir(dir_name):
    tmp_bin_dir = Path(dir_name)
    if tmp_bin_dir.is_dir():
        shutil.rmtree(tmp_bin_dir)
    os.mkdir(tmp_bin_dir)

def run_test(output_dir, assembly, checks):
    bin_path = compile(output_dir, assembly)
    cpu_output_path = run_bin(output_dir, bin_path)
    run_checks(cpu_output_path, checks)

def compile(output_dir, assembly):
    """
    Returns output binary name
    """
    assembly_file = output_dir / "assembly"
    elf_file = output_dir / "elf"
    raw_file = output_dir / "raw"

    # Write assembly to file
    with open(assembly_file, "w") as f:
        f.write(assembly)

    result = subprocess.run(
        ["arm-none-eabi-as", "-mcpu=arm7tdmi", assembly_file, "-o", elf_file]
    )

    if result.returncode != 0:
        pytest.fail("Compile failed")

    result = subprocess.run(
        ["arm-none-eabi-objcopy", "-O", "binary", elf_file, raw_file]
    )

    if result.returncode != 0:
        pytest.fail("ELF to machine code failed")

    return raw_file

class RegEqual:
    def __init__(self, reg, val):
        self.reg = reg
        self.val = val

def run_bin(output_dir, bin):
    """
    Returns output file name
    """
    output_file = output_dir / "cpu_output"
    result = subprocess.run(
        [Path(TEST_CPU_BIN), bin],
        capture_output=True,
        text=True,
    )

    if result.returncode != 0:
        pytest.fail("Failed when running binary")

    with open(output_file, "w") as f:
        f.write(result.stdout)

    return output_file

def get_bit(x, bit_pos):
    return (x >> bit_pos) & 1

def parse_output(output_lines):
    tmp = []
    for line in output_lines:
        tokens = line.strip().split(',')
        if tokens[0] != "REG":
            continue
        if tokens[1] == "cpsr":
            val = int(tokens[2])
            flags = [
                (31, "n"),
                (30, "z"),
                (29, "c"),
                (28, "v"),
                (27, "q"),
                (24, "j"),
                (9, "e"),
                (8, "a"),
                (7, "i"),
                (6, "f"),
                (5, "t"),
                (4, "m4"),
                (3, "m3"),
                (2, "m2"),
                (1, "m1"),
                (0, "m0"),
            ]
            for bit_pos, flag in flags:
                tmp.append([flag, get_bit(val, bit_pos)])
        else:
            tmp.append(tokens[1:])
    return {reg: int(val) for [reg, val] in tmp}

def run_checks(output_file_name, checks):
    with open(output_file_name, 'r') as f:
        output_lines = f.readlines()
        reg_map = parse_output(output_lines)

    for check in checks:
        if isinstance(check, RegEqual):
            if reg_map[check.reg] != check.val:
                pytest.fail(f"{check.reg} expected {check.val} but got {reg_map[check.reg]}")
        else:
            raise NotImplementedError
