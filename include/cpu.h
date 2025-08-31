#pragma once

#include <cstddef>
#include <cstdint>
#include <array>

// Section 2.2
enum class OperatingState {
  ARM,
  Thumb
};

// Section 2.6
enum class Register {
  r0,
  r1
};


class Instruction {
public:
  Instruction(uint32_t encoded_instr_);
private:
  void handleSoftwareInterrupt();
  void handleCoprocessorRegisterTransfer();
  void handleCoprocessorDataOperation();
  void handleCoprocessorDataTransfer();
  void handleBranch();
  void handleBlockDataTransfer();
  void handleUndefined();
  void handleSingleDataTransfer();
  void handleBranchAndExchange();
  void handleSingleDataSwap();
  void handleMultiply();
  void handleMultiplyLong();
  void handleHalfwordDataTransferRegister();
  void handleHalfwordDataTransferImm();
  void handleDataProcessing();

  // Pointer to member function
  typedef void (Instruction::*Instruction_pmf)();
  struct InstructionType {
    uint32_t mask;
    uint32_t target;
    Instruction_pmf handler;
  };

  static constexpr std::size_t kNumInstructionTypes = 15;

  // Order matters
  static constexpr std::array<const InstructionType, kNumInstructionTypes> handlers_ = {{
    { 0x0F000000, 0x0F000000, &Instruction::handleSoftwareInterrupt },
    { 0x0F000010, 0x0E000010, &Instruction::handleCoprocessorRegisterTransfer },
    { 0x0F000010, 0x0E000000, &Instruction::handleCoprocessorDataOperation },
    { 0x0E000000, 0x0C000000, &Instruction::handleCoprocessorDataTransfer },
    { 0x0E000000, 0x0A000000, &Instruction::handleBranch },
    { 0x0E000000, 0x08000000, &Instruction::handleBlockDataTransfer },
    { 0x0E000010, 0x06000010, &Instruction::handleUndefined },
    { 0x0E000000, 0x06000000, &Instruction::handleSingleDataTransfer },
    { 0x0FFFFFF0, 0x012FFF10, &Instruction::handleBranchAndExchange },
    { 0x0FB00FF0, 0x01000090, &Instruction::handleSingleDataSwap },
    { 0x0FC000F0, 0x00000090, &Instruction::handleMultiply },
    { 0x0F8000F0, 0x00800090, &Instruction::handleMultiplyLong },
    { 0x0E400F90, 0x00000090, &Instruction::handleHalfwordDataTransferRegister },
    { 0x0E400090, 0x00400090, &Instruction::handleHalfwordDataTransferImm },
    { 0x0E000000, 0x02000000, &Instruction::handleDataProcessing },
  }};

  const uint32_t encoded_instr_;
  uint8_t cond;
  uint8_t opcode;
};

// Section 2.5
// Not sure if all states will be used
enum class OperatingMode {
  User,
  FastInterrupt,
  Interrupt,
  Supervisor,
  Abort,
  System,
  Undefined
};

// ARM7TDMI Processor
// 16.7772 MHz
class CPU {
public:
  CPU();

  // Overwrite fetched_instr_
  void fetch();

  // Decode fetched_instr_
  void decode();

  // Execute decoded_instr_
  void execute();

  // Do all three
  void step();

  // Section 2.6
  // State and mode defines accesible registers
  uint32_t readReg(Register reg);

  // Section 2.8
  void handleException();

private:
  /*
   * Registers (section 2.6)
   * 0-15 => r0-r15
   * 16-22 => r8_fiq-r14_fig
   * 23-24 => r13_svc-r14_svc
   * 25-26 => r13_abt-r14_abt
   * 27-28 => r13_irq-r14_irq
   * 29-30 => r13_und-r14_und
   * Note:
   *  r13 also called SP
   *  r14 also called LR
   *  r15 also called PC
   *
   */
  static constexpr std::size_t kNumGeneralRegisters = 31;
  static constexpr std::size_t kNumStatusRegisters = 5; // only Saved Program Status Registers (SPSR)
  std::array<uint32_t, kNumGeneralRegisters> general_registers_;
  std::array<uint32_t, kNumStatusRegisters> spsr_banks_;

  /*
   * Section 2.7.2
   *
   * bit 31 => N (negative or less than)
   * bit 30 => Z (zero)
   * bit 29 => C (carry/borrow/extend)
   * bit 28 => O (overflow)
   * ...
   * bit 7 => Disable IRQ
   * bit 6 => Disable FIQ
   * bit 5 => Thumb state
   * bit 4-0 => Modes (Table 2-2)
   */
  uint32_t cpsr_; // Current Program Status Registers

  // State and Mode
  OperatingState operating_state_;
  OperatingMode operating_mode_;

  uint32_t fetched_instr_;
  Instruction decoded_instr_;
};
