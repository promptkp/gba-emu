#include <cpu.h>
#include <iostream>

Instruction::Instruction(uint32_t encoded_instr) : encoded_instr_(encoded_instr) {
  for (const auto& [mask, target, handler] : handlers_) {
    if ((encoded_instr_ & mask) == target) {
      (this->*handler)();
    }
  }
  std::cerr << "Unknown instr: " << encoded_instr_ << std::endl;
}
