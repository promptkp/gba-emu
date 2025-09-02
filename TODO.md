## TODO (CPU)
 * Instruction decoding (in progress)
 [ ] SWI
 [ ] Coprocessor reg transfer
 [ ] Coprocessor data op
 [ ] Coprocessor data transfer
 [x] Branch
 [ ] Block data transfer
 [ ] Undefined
 [ ] Single data transfer
 [ ] Halfword data transfer, imm offset
 [ ] Halfword data transfer, reg offset
 [x] Branch and exchange
 [ ] Single data swap
 [ ] Multiply long
 [x] Multiply
 [x] Data processing 

 Don't know which group section 3.10 belongs to.

 * Instruction executors (not started, make sure this list covers every instruction in armv4T manual)
 [ ] B
 [ ] BL
 [ ] BLX
 [ ] 16 data processing
 [ ] MUL
 [ ] MLA
 [ ] MRS
 [ ] MSR (three types)


 * shifter operand for data processing (addressing mode 1)
 * load/store word/unsigned byte (addressing mode 2)
 * load/store halfword (addressing mode 3)
 * load/store multiple (addressing mode 4)
 * Thumb decoder, handler, executor

 * Write test cases for fetch, decode, execute
  - write test in armv4T assembly
  - learn how to compile to binary
 * Translate cond to bitmask
 * If a handler handles similar instructions, combine handler and executor.
 * Add getter/setter for registers
 * CPU fetches
 * CPU executes
 * CPU memory
