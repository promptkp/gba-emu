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
  CPU::ExecuteBranchAndExchange(encoded_instr_ & 0xF);
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
  bool i_bit = (encoded_instr_ >> 25) & 1;
  bool s_bit = (encoded_instr_ >> 20) & 1;
  uint8_t opcode = (encoded_instr_ >> 21) & 0xF;
  uint8_t reg_n = (encoded_instr_ >> 16) & 0xF;
  uint8_t reg_d = (encoded_instr_ >> 12) & 0xF;
  uint16_t shifter_operand = encoded_instr_ & 0xFFF;

  (this->*data_processing_executors_[opcode])(reg_d, reg_n, shifter_operand, i_bit, s_bit);
}

// TODO
void CPU::ExecuteBranch(uint32_t offset, bool isBL) {}
void CPU::ExecuteBranchAndExchange(uint8_t reg_n) {}
void CPU::ExecuteAND(uint8_t reg_d, uint8_t reg_n, uint16_t shifter_operand, bool i_bit, bool s_bit) {}
void CPU::ExecuteEOR(uint8_t reg_d, uint8_t reg_n, uint16_t shifter_operand, bool i_bit, bool s_bit) {}
void CPU::ExecuteSUB(uint8_t reg_d, uint8_t reg_n, uint16_t shifter_operand, bool i_bit, bool s_bit) {}
void CPU::ExecuteRSB(uint8_t reg_d, uint8_t reg_n, uint16_t shifter_operand, bool i_bit, bool s_bit) {}
void CPU::ExecuteADD(uint8_t reg_d, uint8_t reg_n, uint16_t shifter_operand, bool i_bit, bool s_bit) {}
void CPU::ExecuteADC(uint8_t reg_d, uint8_t reg_n, uint16_t shifter_operand, bool i_bit, bool s_bit) {}
void CPU::ExecuteSBC(uint8_t reg_d, uint8_t reg_n, uint16_t shifter_operand, bool i_bit, bool s_bit) {}
void CPU::ExecuteRSC(uint8_t reg_d, uint8_t reg_n, uint16_t shifter_operand, bool i_bit, bool s_bit) {}
void CPU::ExecuteTST(uint8_t reg_d, uint8_t reg_n, uint16_t shifter_operand, bool i_bit, bool s_bit) {}
void CPU::ExecuteTEQ(uint8_t reg_d, uint8_t reg_n, uint16_t shifter_operand, bool i_bit, bool s_bit) {}
void CPU::ExecuteCMP(uint8_t reg_d, uint8_t reg_n, uint16_t shifter_operand, bool i_bit, bool s_bit) {}
void CPU::ExecuteCMN(uint8_t reg_d, uint8_t reg_n, uint16_t shifter_operand, bool i_bit, bool s_bit) {}
void CPU::ExecuteORR(uint8_t reg_d, uint8_t reg_n, uint16_t shifter_operand, bool i_bit, bool s_bit) {}
void CPU::ExecuteMOV(uint8_t reg_d, uint8_t reg_n, uint16_t shifter_operand, bool i_bit, bool s_bit) {}
void CPU::ExecuteBIC(uint8_t reg_d, uint8_t reg_n, uint16_t shifter_operand, bool i_bit, bool s_bit) {}
void CPU::ExecuteMVN(uint8_t reg_d, uint8_t reg_n, uint16_t shifter_operand, bool i_bit, bool s_bit) {}
