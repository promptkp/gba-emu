from pathlib import Path
import os
import shutil
import subprocess

TMP_BIN_DIR = "tmp_bin_dir"
TEST_CPU_BIN = "bin/test_cpu"

def remove_old_and_create_new_tmp_dir():
    tmp_bin_dir = Path(TMP_BIN_DIR)
    if tmp_bin_dir.is_dir():
        shutil.rmtree(tmp_bin_dir)
    os.mkdir(tmp_bin_dir)

def clean_up_tmp_dir():
    tmp_bin_dir = Path(TMP_BIN_DIR)
    if tmp_bin_dir.is_dir():
        shutil.rmtree(tmp_bin_dir)

def compile(name, assembly):
    """
    Returns output binary name
    """
    assembly_file = Path(TMP_BIN_DIR, name + "_assembly")
    elf_file = Path(TMP_BIN_DIR, name + "_elf")
    raw_file = Path(TMP_BIN_DIR, name + "_raw")

    # Write assembly to file
    with open(assembly_file, "w") as f:
        f.write(assembly)

    result = subprocess.run(
        ["arm-none-eabi-as", "-mcpu=arm7tdmi", assembly_file, "-o", elf_file]
    )

    if result.returncode != 0:
        print(f"{name}: Failed to compile")
        print(f"{name}: Assembly file content:")
        with open(assembly_file, "r") as f:
            print(f.read())
        return None

    result = subprocess.run(
        ["arm-none-eabi-objcopy", "-O", "binary", elf_file, raw_file]
    )

    if result.returncode != 0:
        print(f"{name}: Failed to convert ELF to machine code")
        return None

    return raw_file

class RegEqual:
    def __init__(self, reg, val):
        self.reg = reg
        self.val = val

def run_bin(name, bin):
    """
    Returns output file name
    """
    output_file = Path(TMP_BIN_DIR, name + "_output")
    result = subprocess.run(
        [Path(TEST_CPU_BIN), bin],
        capture_output=True,
        text=True,
    )

    if result.returncode != 0:
        print(f"{name}: Failed when running binary")
        return None

    with open(output_file, "w") as f:
        f.write(result.stdout)

    return output_file

def parse_output(output_lines):
    tmp = []
    for line in output_lines:
        tokens = line.strip().split(',')
        if tokens[0] != "REG":
            continue
        tmp.append(tokens[1:])
    return {reg: int(val) for [reg, val] in tmp}

def run_checks(output_file_name, checks):
    with open(output_file_name, 'r') as f:
        output_lines = f.readlines()
        reg_map = parse_output(output_lines)

    for check in checks:
        if isinstance(check, RegEqual):
            if reg_map[check.reg] != check.val:
                print(f"{check.reg} expected {check.val} but got {reg_map[check.reg]}")
                return False

    return True
