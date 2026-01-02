from utils import *

# first test a simple way to set c flag to 1 and 0

SET_C_FLAG = "movs r0, #0x80000000\n"

def test_shifter_op_set_c_flag(output_dir):
    assembly, checks = (
        """
            movs    r0, #0x80000000
        """,
        [
            RegEqual("r0", 0x80000000),
            RegEqual("c", 1),
        ])
    run_test(output_dir, assembly, checks)


def test_shifter_op_clear_c_flag(output_dir):
    assembly, checks = (
        """
            movs    r0, #0x80000000
            movs    r1, #0x40000000
        """,
        [
            RegEqual("r0", 0x80000000),
            RegEqual("r1", 0x40000000),
            RegEqual("c", 0),
        ])
    run_test(output_dir, assembly, checks)


# Now test each shifter op (A5.1)


def test_shifter_op_imm_value(output_dir):
    assembly, checks = (
        """
            mov     r0, #0xFF000000
            mov     r1, #0x00FF0000
            mov     r2, #0x0000FF00
            mov     r3, #0x000000FF
        """,
        [
            RegEqual("r0", 0xFF000000),
            RegEqual("r1", 0x00FF0000),
            RegEqual("r2", 0x0000FF00),
            RegEqual("r3", 0x000000FF),
        ])
    run_test(output_dir, assembly, checks)


def test_shifter_op_imm_rotate_zero_set_c(output_dir):
    assembly, checks = (
        f"""
            {SET_C_FLAG}
            movs    r0, #0x000000FF
        """,
        [
            RegEqual("r0", 0xFF),
            RegEqual("c", 1),
        ])
    run_test(output_dir, assembly, checks)


def test_shifter_op_imm_rotate_zero_clear_c(output_dir):
    assembly, checks = (
        f"""
            movs    r0, #0x000000FF
        """,
        [
            RegEqual("r0", 0xFF),
            RegEqual("c", 0),
        ])
    run_test(output_dir, assembly, checks)


def test_shifter_op_imm_rotate_non_zero_set_c(output_dir):
    assembly, checks = (
        f"""
            movs    r0, #0xFF000000
        """,
        [
            RegEqual("r0", 0xFF000000),
            RegEqual("c", 1),
        ])
    run_test(output_dir, assembly, checks)


def test_shifter_op_imm_rotate_non_zero_clear_c(output_dir):
    assembly, checks = (
        f"""
            {SET_C_FLAG}
            movs    r0, #0x8F000000
        """,
        [
            RegEqual("r0", 0x8F000000),
            RegEqual("c", 0),
        ])
    run_test(output_dir, assembly, checks)



# def test_mov_r15_rd(output_dir):
#     """
#     If S=1, Rd=R15; should not be used in user mode:
#     CPSR = SPSR_<current mode>
#     PC = result
#     For example: MOVS PC,R14  ;return from SWI (PC=R14_svc, CPSR=SPSR_svc).
#     """
#     raise NotImplementedError
#
#
# def test_mov_r15_rm(output_dir):
#     pass
#
#
# def test_mov_r15_rn(output_dir):
#     pass
