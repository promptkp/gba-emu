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

def cleanup_dir(dir_name):
    tmp_bin_dir = Path(dir_name)
    if tmp_bin_dir.is_dir():
        shutil.rmtree(tmp_bin_dir)

@pytest.fixture(scope="session")
def shared_dir():
    overwrite_dir(TEST_RESULT_DIR)
    yield Path(TEST_RESULT_DIR)

@pytest.fixture
def output_dir(shared_dir, request):
    test_dir = shared_dir / request.node.name
    test_dir.mkdir(exist_ok=True)
    return test_dir

def run_test(output_dir, assembly, checks):
    bin_path = compile(output_dir, assembly)
    cpu_output_path = run_bin(output_dir, bin_path)
    run_checks(cpu_output_path, checks)

def remove_old_and_create_new_tmp_dir():
    overwrite_dir(TMP_BIN_DIR)

def clean_up_tmp_dir():
    cleanup_dir(TMP_BIN_DIR)

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
                pytest.fail(f"{check.reg} expected {check.val} but got {reg_map[check.reg]}")
        else:
            raise NotImplementedError
