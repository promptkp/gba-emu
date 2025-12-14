from pathlib import Path
import os
import shutil
import subprocess

TMP_BIN_DIR = "tmp_bin_dir"

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

def reg_equal(src, expected):
    pass

class CPU():
    def __init__(self):
        pass

    def run(self, bin):
        pass

    def check(self, expr):
        pass
