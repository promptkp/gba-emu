#include <cpu.h>
#include <iostream>

CPU::CPU(std::vector<uint8_t> &buffer) : buffer_{buffer} {
  /*
  for (int i = 0; i < buffer.size(); ++i) {
    std::cout << std::hex << static_cast<int>(buffer[i]) << std::dec << std::endl;
  }
  */
  std::cout << "Constructor done" << std::endl;
}

uint32_t CPU::ReadStatusReg(OperatingMode mode) {return 0;}
uint32_t CPU::ReadCPSR() {return 0;}

uint32_t CPU::ReadReg(uint8_t reg, OperatingMode mode) {
  if (reg > 15) {
    std::cerr << "Reg must be <= 15" << std::endl;
  }
  if (mode == OperatingMode::User || mode == OperatingMode::System) {
    return general_registers_[reg];
  } else {
    // todo: read reg for other modes
    return -1;
  }
}

uint32_t CPU::ReadReg(uint8_t reg) {
  return ReadReg(reg, operating_mode_);
}

bool CPU::IsDone() {
  return ReadReg(15) >= buffer_.size();
}

void CPU::RunTest() {
  while (!IsDone()) {
    Step();
  }
}

#define PrintRegisterPrefix ("REG,")

void CPU::PrintRegister() {
  // 16 regs
  for (int i = 0; i < 16; ++i) {
    std::cout << PrintRegisterPrefix << "r" << i << "," << ReadReg(i, OperatingMode::User) << std::endl;
  }

  // 5 regs
  for (int i = 8; i <= 12; ++i) {
    std::cout << PrintRegisterPrefix << "r" << i << "_fiq," << ReadReg(i, OperatingMode::FastInterrupt) << std::endl;
  }

  std::pair<OperatingMode, std::string> r13_14_spsr_banked_mode[] = {
    {OperatingMode::FastInterrupt, "fiq"},
    {OperatingMode::Supervisor, "svc"},
    {OperatingMode::Abort, "abt"},
    {OperatingMode::Interrupt, "irq"},
    {OperatingMode::Undefined, "und"}
  };

  // 5 * 3 = 15 regs
  for (auto &[mode, name] : r13_14_spsr_banked_mode) {
    for (int i = 13; i <= 14; ++i) {
      std::cout << PrintRegisterPrefix << "r" << i << "_" << name << "," << ReadReg(i, mode) << std::endl;;
    }

    std::cout << PrintRegisterPrefix << "spsr" << "_" << name << "," << ReadStatusReg(mode) << std::endl;
  }

  // 1 reg
  std::cout << PrintRegisterPrefix << "cpsr" << "," << ReadCPSR() << std::endl;
}


void CPU::Step() {
  std::cout << "step" << std::endl;
  Fetch();
  DecodeAndExecute();
}


void CPU::WriteReg(uint8_t reg, OperatingMode mode, uint32_t val) {
  if (reg > 15) {
    std::cerr << "WriteReg: reg must be <= 15" << std::endl;
  }
  if (mode == OperatingMode::User || mode == OperatingMode::System) {
    general_registers_[reg] = val;
  } else {
    // todo: other mode
    std::cerr << "WriteReg: not implemented" << std::endl;
  }
}

void CPU::WriteReg(uint8_t reg, uint32_t val) {
  WriteReg(reg, operating_mode_, val);
}

void CPU::Fetch() {
  uint8_t instr_size_bytes;
  if (operating_state_ == OperatingState::ARM) {
    instr_size_bytes = 4;
  } else {
    instr_size_bytes = 2;
  }

  uint32_t pc = ReadReg(15);
  if (pc + instr_size_bytes - 1 >= buffer_.size()) {
    std::cerr << "PC is invalid (too large); aborting..." << std::endl;
    exit(1);
  }

  encoded_instr_ = 0;
  for (int i = 0; i < instr_size_bytes; ++i) {
    // fix endian
    encoded_instr_ |= (uint32_t)buffer_[pc++] << (8 * i);
  }

  // todo: check when i should update PC. for now update after fetch but this
  // will have side effect if current instr reads from PC.
  WriteReg(15, pc);

  std::cout << "Fetched instr = " << std::hex << encoded_instr_ << std::dec << std::endl;
}

void CPU::DecodeAndExecute() {
  if (operating_state_ == OperatingState::ARM) {
    std::cout << "DecodeAndExecute (arm state)" << std::endl;
    // todo: check cond here
    cond_ = encoded_instr_ >> 28;
    if (cond_ == 0xF) {
      std::cerr << "Warning cond is 0b1111; instruction is unpredictable (section A3.2.1)" << std::endl;
    }

    for (const auto& [mask, target, handler] : handlers_) {
      if ((encoded_instr_ & mask) == target) {
	std::cout << "DecodeAndExecute: ";
        std::cout << std::hex << "Encoded instr = " << encoded_instr_  << " mask = " << mask << " " <<
          (encoded_instr_ & mask) << " " << target << std::dec << std::endl;
        (this->*handler)();
        return;
      }
    }
    std::cerr << "Unknown instr: " << std::hex << encoded_instr_ << std::dec << std::endl;
  } else {
    // todo: decode and execute in thumb mode
    std::cerr << "Thumb mode decode and execute not implemented" << std::endl;
  }
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
  bool i_bit = (encoded_instr_ >> 25) & 1;
  bool p_bit = (encoded_instr_ >> 24) & 1;
  bool u_bit = (encoded_instr_ >> 23) & 1;
  bool b_bit = (encoded_instr_ >> 22) & 1;
  bool w_bit = (encoded_instr_ >> 21) & 1;
  bool l_bit = (encoded_instr_ >> 20) & 1;
  uint8_t reg_n = (encoded_instr_ >> 16) & 0xF;
  uint8_t reg_d = (encoded_instr_ >> 16) & 0xF;
  uint16_t offset = encoded_instr_ & 0xFFF;

  // TODO: decision tree
  ExecuteLDRSTR(i_bit, p_bit, u_bit, b_bit, w_bit, l_bit, reg_n, reg_d, offset);
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
  uint8_t reg_s = (encoded_instr_ >> 8) & 0xF;
  uint8_t reg_m = encoded_instr_ & 0xF;

  if (u_bit) {
    if (a_bit) {
      ExecuteUMLAL(reg_d_hi, reg_d_lo, reg_s, reg_m, s_bit);
    } else {
      ExecuteUMULL(reg_d_hi, reg_d_lo, reg_s, reg_m, s_bit);
    }
  } else {
    if (a_bit) {
      ExecuteSMLAL(reg_d_hi, reg_d_lo, reg_s, reg_m, s_bit);
    } else {
      ExecuteSMULL(reg_d_hi, reg_d_lo, reg_s, reg_m, s_bit);
    }
  }
}

void CPU::HandleHalfwordDataTransferRegister() {
  bool p_bit = (encoded_instr_ >> 24) & 1;
  bool u_bit = (encoded_instr_ >> 23) & 1;
  bool w_bit = (encoded_instr_ >> 21) & 1;
  bool l_bit = (encoded_instr_ >> 20) & 1;
  uint8_t reg_n = (encoded_instr_ >> 16) & 0xF;
  uint8_t reg_d = (encoded_instr_ >> 12) & 0xF;
  bool s_bit = (encoded_instr_ >> 6) & 1;
  bool h_bit = (encoded_instr_ >> 5) & 1;
  uint8_t reg_m = encoded_instr_ & 0xF;
}

void CPU::HandleHalfwordDataTransferImm() {
  bool p_bit = (encoded_instr_ >> 24) & 1;
  bool u_bit = (encoded_instr_ >> 23) & 1;
  bool w_bit = (encoded_instr_ >> 21) & 1;
  bool l_bit = (encoded_instr_ >> 20) & 1;
  uint8_t reg_n = (encoded_instr_ >> 16) & 0xF;
  uint8_t reg_d = (encoded_instr_ >> 12) & 0xF;
  bool s_bit = (encoded_instr_ >> 6) & 1;
  bool h_bit = (encoded_instr_ >> 5) & 1;
  uint8_t offset = ((encoded_instr_ >> 4) & 0xF0) | (encoded_instr_ & 0xF);
}

// Handle 16 opcodes + PSR transfer (MRS, MSR)
void CPU::HandleDataProcessing() {
  bool i_bit = (encoded_instr_ >> 25) & 1;
  bool s_bit = (encoded_instr_ >> 20) & 1;
  uint8_t opcode = (encoded_instr_ >> 21) & 0xF;
  uint8_t reg_n = (encoded_instr_ >> 16) & 0xF;
  uint8_t reg_d = (encoded_instr_ >> 12) & 0xF;
  uint16_t shifter_operand = encoded_instr_ & 0xFFF;

  // Check for PSR transfer instructions first
  // These executors take different arguments, so decode them within each executor
  static constexpr std::array<const InstructionHandler, 4> psr_transfer_executor = {{
    { 0x0FBF0FFF, 0x010F0000, &CPU::ExecuteMRS },
    { 0x0FBFFFF0, 0x0129F000, &CPU::ExecuteMSRregToPSR },
    { 0x0DBFF000, 0x0128F000, &CPU::ExecuteMSRregToFlag },
    { 0x0DBFF000, 0x0328F000, &CPU::ExecuteMSRimmToFlag },
  }};

  for (const auto& [mask, target, executor] : psr_transfer_executor) {
    if ((encoded_instr_ & mask) == target) {
      (this->*executor)();
      return;
    }
  }


  std::cout << "HandleDataProcessing: " << "calling executor opcode = " << opcode << std::endl;
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

uint32_t rotateRight(uint32_t base, uint32_t shift) {
  uint32_t low_mask = (1 << shift) - 1;

  // low is shift least significant bits
  uint32_t low = base & low_mask;
  
  // right shift everything and put low at most significant bits
  return (base >> shift) | (low << (32 - shift));
}

uint32_t CPU::GetCFlag() {
  return (cpsr_ >> 29) & 1;
}

uint32_t CPU::InterpretShifterOp(uint16_t shifter_operand, bool i_bit) {
  if (i_bit) {
    uint32_t imm = shifter_operand & 0xFF;
    uint32_t shift = ((shifter_operand >> 8) & 0xF) << 1;
    std::cout << "shifter_op imm " << imm << " shift " << shift << std::endl;
    return rotateRight(imm, shift);
  } else {
    bool is_shift_by_register = (shifter_operand >> 4) & 1;

    // 0 = LSL, 1 = LSR, 2 = ASR, 3 = ROR
    uint32_t shift_type = (shifter_operand >> 5) & 0b11;

    uint32_t base_reg_val = ReadReg(shifter_operand & 0xF);

    uint32_t shift_amount;

    // Compute shift_amount for each case
    if (is_shift_by_register) {
      if ((shifter_operand >> 7) & 1) {
        std::cerr << "7th bit of shifter operand is reserved (must be zero) if shifting by register" << std::endl;
        exit(1);
      }
      
      uint16_t shift_register = (shifter_operand >> 8) & 0xF;
      if (shift_register == 15) {
        std::cerr << "shift register cannot be r15 in shifter operand" << std::endl;
        exit(1);
      }

      shift_amount = ReadReg(shift_register) & 0xFF;
    } else {
      shift_amount = (shifter_operand >> 7) & 0x1F;

      if (shift_amount == 0) {
        switch (shift_type) {
          case 0:
            return base_reg_val;
          case 1:
            // todo: if s_flag is set, C flag becomes bit 31 of base_reg_val
            return 0;
          case 2:
            // todo: if s_flag is set, C flag becomes bit 31 of base_reg_val
            return (base_reg_val >> 31) & 1;
          default:
            // todo: if s_flag is set, C flag becomes bit 0 of base_reg_val
            return (base_reg_val >> 1) | (GetCFlag() << 31);
        }
      }
    }

    switch (shift_type) {
      // LSL
      case 0:
        return base_reg_val << shift_amount;
      // LSR
      case 1:
        return base_reg_val >> shift_amount;
      // ASR
      case 2:
        return static_cast<uint32_t>(static_cast<int>(base_reg_val) >> shift_amount);
      // ROR
      default:
        return rotateRight(base_reg_val, shift_amount);
    }
  }
}

void CPU::ExecuteMOV(uint8_t reg_d, uint8_t reg_n, uint16_t shifter_operand, bool i_bit, bool s_bit) {
  if (reg_n != 0) {
    std::cerr << "reg_n must be zero for mov; aborting..." << std::endl;
    exit(1);
  }
  if (s_bit == 1) {
    // todo: mov s_bit == 1
    std::cerr << "mov s_bit == 1 not implemented" << std::endl;
  }

  uint32_t val = InterpretShifterOp(shifter_operand, i_bit);

  std::cout << "Execute mov: reg_d " << reg_d << " val " << val << std::endl;
  WriteReg(reg_d, val);
}
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

// TODO: PSR Transfer
void CPU::ExecuteMRS() {}
void CPU::ExecuteMSRregToPSR() {}
void CPU::ExecuteMSRregToFlag() {}
void CPU::ExecuteMSRimmToFlag() {}

// TODO: Memory ops
void CPU::ExecuteLDRSTR(bool i_bit, bool p_bit, bool u_bit, bool b_bit, bool w_bit, bool l_bit,
                   uint8_t reg_n, uint8_t reg_d, uint16_t offset) {}
void CPU::ExecuteHalfwordReg(bool p_bit, bool u_bit, bool w_bit, bool l_bit,
                             uint8_t reg_n, uint8_t reg_d, bool s_bit, bool
                             h_bit, uint8_t reg_m) {}
void CPU::ExecuteHalfwordImm(bool p_bit, bool u_bit, bool w_bit, bool l_bit,
                             uint8_t reg_n, uint8_t reg_d, bool s_bit, bool
                             h_bit, uint8_t offset) {}
