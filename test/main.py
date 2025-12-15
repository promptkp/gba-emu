#!/usr/bin/env python3

from utils import *

"""
Assemble into ELF then into raw machine code bytes:

    arm-none-eabi-as -mcpu=arm7tdmi test_assembly.s
    arm-none-eabi-objcopy -O binary a.out a.bin
"""

"""
Mov:
    Reg
        shift by imm/reg
        shift type
    Imm
        8bit imm
        shift by imm

Then, we have the Movs of course.
"""

tests = [
    {
        "name": "test_one_mov_imm",
        "assembly": """
            mov     r0, #1
        """,
        "checks": [
            reg_equal("r0", 1),
        ],
    },
    {
        "name": "test_one_mov_imm_shifted",
        "assembly": """
            mov     r0, #0x00011000
        """,
        "checks": [
            reg_equal("r0", 0x00011000),
        ],
    },
    {
        "name": "test_one_mov_reg",
        "assembly": """
            mov     r0, #1
            mov     r1, r0
        """,
        "checks": [
            reg_equal("r0", 1),
            reg_equal("r1", 1),
        ],
    },
    {
        "name": "test_one_mov_reg_lsl_reg",
        "assembly": """
            mov     r0, #1
            mov     r1, #4
            mov     r2, r0, lsl r1
        """,
        "checks": [
            reg_equal("r2", 1 << 4)
        ],
    },
    {
        "name": "test_one_mov_reg_lsl_imm",
        "assembly": """
            mov     r0, #1
            mov     r1, r0, lsl #4
        """,
        "checks": [
            reg_equal("r1", 1 << 4)
        ],
    },
]

remove_old_and_create_new_tmp_dir()

failed_tests = []
for test in tests:
    name = test["name"]
    input = test["assembly"]
    checks = test["checks"]
    test_bin = compile(name, input)

    if test_bin is None:
        print(f"Error in {name}, skipping.")
        failed_tests.append(name)
        continue

    test_output = run_bin(name, test_bin)

    if test_output is None:
        print(f"Error running binary {name}, skipping")
        failed_tests.append(name)
        continue

    ok = run_checks(test_output, checks)
    if not ok:
        print(f"Checks failed for {name}, skipping")
        failed_tests.append(name)
        continue

    print(f"{name}: Passed!")

if len(failed_tests) == 0:
    print("All tests passed!")
else:
    print(f"{len(failed_tests)} tests failed.")

clean_up_tmp_dir()
