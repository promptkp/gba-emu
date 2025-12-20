from utils import *

def test_mov_imm(output_dir):
    assembly, checks = (
        """
            mov     r0, #1
        """,
        [
            RegEqual("r0", 1),
        ])
    run_test(output_dir, assembly, checks)


def test_mov_imm_shifted(output_dir):
    assembly, checks = (
        """
            mov     r0, #0x100
        """,
        [
            RegEqual("r0", 0x100),
        ])
    run_test(output_dir, assembly, checks)


def test_mov_reg(output_dir):
    assembly, checks = (
        """
            mov     r0, #1
            mov     r1, r0
        """,
        [
            RegEqual("r0", 1),
            RegEqual("r1", 1),
        ])
    run_test(output_dir, assembly, checks)


def test_mov_chain(output_dir):
    assembly, checks = (
        """
            mov     r0, #1
            mov     r1, r0
            mov     r2, r1
            mov     r3, r2
            mov     r4, r2
        """,
        [
            RegEqual("r0", 1),
            RegEqual("r1", 1),
            RegEqual("r2", 1),
            RegEqual("r3", 1),
            RegEqual("r4", 1),
        ])
    run_test(output_dir, assembly, checks)


def test_mov_reg_lsl_reg(output_dir):
    assembly, checks = (
        """
            mov     r0, #1
            mov     r1, #4
            mov     r2, r0, lsl r1
        """,
        [
            RegEqual("r2", 1 << 4)
        ])
    run_test(output_dir, assembly, checks)


def test_mov_reg_lsl_imm(output_dir):
    assembly, checks = (
        """
            mov     r0, #1
            mov     r1, r0, lsl #4
        """,
        [
            RegEqual("r1", 1 << 4)
        ])
    run_test(output_dir, assembly, checks)


