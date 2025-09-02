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
  // bit 25 is i-bit, but maybe armv4T doesn't support i-bit == 0
  bool p_bit = (encoded_instr_ >> 24) & 1;
  bool u_bit = (encoded_instr_ >> 23) & 1;
  bool b_bit = (encoded_instr_ >> 22) & 1;
  bool w_bit = (encoded_instr_ >> 21) & 1;
  bool l_bit = (encoded_instr_ >> 20) & 1;
  uint8_t reg_n = (encoded_instr_ >> 16) & 0xF;
  uint8_t reg_d = (encoded_instr_ >> 16) & 0xF;
  uint16_t offset = encoded_instr_ & 0xFFF;
}

// Handle BX (section A4.1.10)
void CPU::HandleBranchAndExchange() {
  CPU::ExecuteBranchAndExchange(encoded_instr_ & 0xF);
}

void CPU::HandleSingleDataSwap() {
}

// Handle MUL, MLA
void CPU::HandleMultiply() {
  bool a_bit = (encoded_instr_ >> 21) & 1;
  bool s_bit = (encoded_instr_ >> 20) & 1;
  uint8_t reg_d = (encoded_instr_ >> 16) & 0xF;
  uint8_t reg_n = (encoded_instr_ >> 12) & 0xF;
  uint8_t reg_s = (encoded_instr_ >> 8) & 0xF;
  uint8_t reg_m = encoded_instr_ & 0xF;

  if (a_bit) {
    ExecuteMLA(reg_d, reg_n, reg_s, reg_m, s_bit);
  } else {
    ExecuteMUL(reg_d, reg_n, reg_s, reg_m, s_bit);
  }
}

// Handle UMULL, UMLAL, SMULL, SMLAL
void CPU::HandleMultiplyLong() {
  bool u_bit = (encoded_instr_ >> 22) & 1;
  bool a_bit = (encoded_instr_ >> 21) & 1;
  bool s_bit = (encoded_instr_ >> 20) & 1;
  uint8_t reg_d_hi = (encoded_instr_ >> 16) & 0xF;
  uint8_t reg_d_lo = (encoded_instr_ >> 12) & 0xF;
  uint8_t reg_n = (encoded_instr_ >> 8) & 0xF;
  uint8_t reg_m = encoded_instr_ & 0xF;

  if (u_bit) {
    if (a_bit) {
      ExecuteUMLAL(reg_d_hi, reg_d_lo, reg_n, reg_m, s_bit);
    } else {
      ExecuteUMULL(reg_d_hi, reg_d_lo, reg_n, reg_m, s_bit);
    }
  } else {
    if (a_bit) {
      ExecuteSMLAL(reg_d_hi, reg_d_lo, reg_n, reg_m, s_bit);
    } else {
      ExecuteSMULL(reg_d_hi, reg_d_lo, reg_n, reg_m, s_bit);
    }
  }
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

// TODO: data processing
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

// TODO: Normal multiply
void CPU::ExecuteMLA(uint8_t reg_d, uint8_t reg_n, uint8_t reg_s, uint8_t reg_m, bool s_bit) {}
void CPU::ExecuteMUL(uint8_t reg_d, uint8_t reg_n, uint8_t reg_s, uint8_t reg_m, bool s_bit) {}

// TODO: Long multiply
void CPU::ExecuteUMULL(uint8_t reg_d_hi, uint8_t reg_d_lo, uint8_t reg_m, uint8_t reg_s, bool s_bit) {}
void CPU::ExecuteUMLAL(uint8_t reg_d_hi, uint8_t reg_d_lo, uint8_t reg_m, uint8_t reg_s, bool s_bit) {}
void CPU::ExecuteSMULL(uint8_t reg_d_hi, uint8_t reg_d_lo, uint8_t reg_m, uint8_t reg_s, bool s_bit) {}
void CPU::ExecuteSMLAL(uint8_t reg_d_hi, uint8_t reg_d_lo, uint8_t reg_m, uint8_t reg_s, bool s_bit) {}
