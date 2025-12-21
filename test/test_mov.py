from utils import *

"""
Test schemes:

op2: base imm, base reg

(done) base imm: shfited, not shifted

base reg:
op2 base reg: pc, r0-14
shift type: 0 1 2 3
shift type/amount: imm 0, imm 1-31, reg 0, reg 1-31, reg 32, reg > 32, reg >= 256
"""

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
            RegEqual("r2", 1 << 4),
        ])
    run_test(output_dir, assembly, checks)


def test_mov_reg_lsl_imm_non_zero(output_dir):
    assembly, checks = (
        """
            mov     r0, #1
            mov     r1, r0, lsl #4
        """,
        [
            RegEqual("r1", 1 << 4),
        ])
    run_test(output_dir, assembly, checks)



def test_mov_reg_lsl_imm_zero(output_dir):
    assembly, checks = (
        """
            mov     r0, #1
            mov     r1, r0, lsl #0
        """,
        [
            RegEqual("r1", 1),
        ])
    run_test(output_dir, assembly, checks)


def test_mov_reg_lsr_imm_zero(output_dir):
    assembly, checks = (
        """
            mov     r0, #0x80000000
            movs    r1, r0, lsr #32
        """,
        [
            RegEqual("r1", 0),
            RegEqual("c", 1),
        ])
    run_test(output_dir, assembly, checks)


def test_mov_reg_lsr_imm_zero_c_zero(output_dir):
    assembly, checks = (
        """
            mov     r2, #0x00000001
            movs    r3, r2, lsr #32
        """,
        [
            RegEqual("r3", 0),
            RegEqual("c", 0),
        ])
    run_test(output_dir, assembly, checks)


def test_mov_reg_asr_imm_zero_c_one(output_dir):
    assembly, checks = (
        """
            mov     r2, #0x80000000
            movs    r3, r2, asr #32
        """,
        [
            RegEqual("r3", 0xFFFFFFFF),
            RegEqual("c", 1),
        ])
    run_test(output_dir, assembly, checks)


def test_mov_reg_asr_imm_zero_c_zero(output_dir):
    assembly, checks = (
        """
            mov     r2, #0x00000001
            movs    r3, r2, asr #32
        """,
        [
            RegEqual("r3", 0),
            RegEqual("c", 0),
        ])
    run_test(output_dir, assembly, checks)


def test_mov_reg_rrx_no_c(output_dir):
    assembly, checks = (
        """
            mov     r2, #0x80000001
            movs    r3, r2, rrx #1
        """,
        [
            RegEqual("r3", 0x40000000),
            RegEqual("c", 1),
        ])
    run_test(output_dir, assembly, checks)


def test_mov_reg_rrx_with_c(output_dir):
    assembly, checks = (
        """
            mov     r0, #0x80000000
            movs    r1, r0, asr #32 ; set c to 1
            mov     r2, #0x80000001
            movs    r3, r2, rrx #1
        """,
        [
            RegEqual("r3", 0xC0000000),
            RegEqual("c", 1),
        ])
    run_test(output_dir, assembly, checks)
