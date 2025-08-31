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

  // Overwrite encoded_instr_
  void Fetch();

  // Decode and execute encoded_instr_
  void DecodeAndExecute();

  // Perform fetch, decode, and execute
  void Step();

  // Section 2.6
  // State and mode defines accesible registers
  uint32_t ReadReg(Register reg);

  // Section 2.8
  // TODO: understand how exception works
  void HandleException();

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

  uint32_t encoded_instr_;
  uint8_t cond_;

  // The pipeline is encoded instruction -> handler -> executor
  void HandleSoftwareInterrupt();
  void HandleCoprocessorRegisterTransfer();
  void HandleCoprocessorDataOperation();
  void HandleCoprocessorDataTransfer();
  void HandleBranch();
  void HandleBlockDataTransfer();
  void HandleUndefined();
  void HandleSingleDataTransfer();
  void HandleBranchAndExchange();
  void HandleSingleDataSwap();
  void HandleMultiply();
  void HandleMultiplyLong();
  void HandleHalfwordDataTransferRegister();
  void HandleHalfwordDataTransferImm();
  void HandleDataProcessing();

  // Might combine handler and executor into one function.
  // This depends on whether a handler handles similar instr or not.
  void ExecuteBranch(uint32_t offset, bool isBL);
  void ExecuteBranchAndExchange(uint8_t reg_n);

  // Pointer to member function
  using InstructionHandlerPointer = void (CPU::*)();
  struct InstructionHandler {
    uint32_t mask;
    uint32_t target;
    InstructionHandlerPointer handler;
  };

  static constexpr std::size_t kNumInstructionTypes = 15;
  // Order matters
  static constexpr std::array<const InstructionHandler, kNumInstructionTypes> handlers_ = {{
    { 0x0F000000, 0x0F000000, &CPU::HandleSoftwareInterrupt },
    { 0x0F000010, 0x0E000010, &CPU::HandleCoprocessorRegisterTransfer },
    { 0x0F000010, 0x0E000000, &CPU::HandleCoprocessorDataOperation },
    { 0x0E000000, 0x0C000000, &CPU::HandleCoprocessorDataTransfer },
    { 0x0E000000, 0x0A000000, &CPU::HandleBranch },
    { 0x0E000000, 0x08000000, &CPU::HandleBlockDataTransfer },
    { 0x0E000010, 0x06000010, &CPU::HandleUndefined },
    { 0x0E000000, 0x06000000, &CPU::HandleSingleDataTransfer },
    { 0x0FFFFFF0, 0x012FFF10, &CPU::HandleBranchAndExchange },
    { 0x0FB00FF0, 0x01000090, &CPU::HandleSingleDataSwap },
    { 0x0FC000F0, 0x00000090, &CPU::HandleMultiply },
    { 0x0F8000F0, 0x00800090, &CPU::HandleMultiplyLong },
    { 0x0E400F90, 0x00000090, &CPU::HandleHalfwordDataTransferRegister },
    { 0x0E400090, 0x00400090, &CPU::HandleHalfwordDataTransferImm },
    { 0x0E000000, 0x02000000, &CPU::HandleDataProcessing },
  }};
};
