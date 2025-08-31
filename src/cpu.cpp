#include <cpu.h>
#include <iostream>

void CPU::DecodeAndExecute() {
  cond_ = encoded_instr_ >> 28;
  if (cond_ == 0xF) {
    std::cerr << "Warning cond is 0b1111; instruction is unpredictable (section A3.2.1)" << std::endl;
  }

  for (const auto& [mask, target, handler] : handlers_) {
    if ((encoded_instr_ & mask) == target) {
      (this->*handler)();
    }
  }
  std::cerr << "Unknown instr: " << encoded_instr_ << std::endl;
}

void CPU::HandleSoftwareInterrupt() {
}

void CPU::HandleCoprocessorRegisterTransfer() {
}

void CPU::HandleCoprocessorDataOperation() {
}

void CPU::HandleCoprocessorDataTransfer() {
}

// Handle B, BL (section A4.1.5)
void CPU::HandleBranch() {
  uint32_t offset = encoded_instr_ & 0x00FFFFFF;

  // Sign extend 24 to 32 bit
  if (offset & 0x00800000) {
    offset |= 0xFF000000;
  }

  offset = offset << 2;
  CPU::ExecuteBranch(offset, (encoded_instr_ >> 24) & 1);
}

void CPU::HandleBlockDataTransfer() {
}

void CPU::HandleUndefined() {
}

void CPU::HandleSingleDataTransfer() {
}

// Handle BX (section A4.1.10)
void CPU::HandleBranchAndExchange() {
  CPU::ExecuteBranchAndExchange((uint8_t)(encoded_instr_ & 0xF));
}

void CPU::HandleSingleDataSwap() {
}

void CPU::HandleMultiply() {
}

void CPU::HandleMultiplyLong() {
}

void CPU::HandleHalfwordDataTransferRegister() {
}

void CPU::HandleHalfwordDataTransferImm() {
}

void CPU::HandleDataProcessing() {
}

void CPU::ExecuteBranch(uint32_t offset, bool isBL) {}
void CPU::ExecuteBranchAndExchange(uint8_t reg_n) {}
