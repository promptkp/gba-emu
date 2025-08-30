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

void Instruction::handleSoftwareInterrupt() {
}

void Instruction::handleCoprocessorRegisterTransfer() {
}

void Instruction::handleCoprocessorDataOperation() {
}

void Instruction::handleCoprocessorDataTransfer() {
}

void Instruction::handleBranch() {
}

void Instruction::handleBlockDataTransfer() {
}

void Instruction::handleUndefined() {
}

void Instruction::handleSingleDataTransfer() {
}

void Instruction::handleBranchAndExchange() {
}

void Instruction::handleSingleDataSwap() {
}

void Instruction::handleMultiply() {
}

void Instruction::handleMultiplyLong() {
}

void Instruction::handleHalfwordDataTransferRegister() {
}

void Instruction::handleHalfwordDataTransferImm() {
}

void Instruction::handleDataProcessing() {
}
