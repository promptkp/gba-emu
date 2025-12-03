def reg_equal(src, expected):
    pass

def compile(assembly):
    pass

class CPU():
    def run(self, bin):
        pass

    def check(self, expr):
        pass

input = \
"""
mov     r0, #0
add     r0, r0, #1
"""

checks = [
    reg_equal("r0", 1),
]

test_bin = compile(input)
cpu = CPU()
cpu.run(test_bin)
for check in checks:
    cpu.check(check)
