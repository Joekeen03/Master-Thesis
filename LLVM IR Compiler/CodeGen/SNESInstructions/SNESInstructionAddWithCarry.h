#ifndef LLVM_IR_COMPILER_SNESInstructionAddWithCarry_H
#define LLVM_IR_COMPILER_SNESInstructionAddWithCarry_H

#include "SNESInstruction.h"
#include "AddressingModes.h"

namespace SNESAssembly {
    // FIXME Should merge this, LDA, STA, STX, STY, and other similar instructions into one meta-template. Something like:
    //  template <typename AddressingMode, const char* instructionName, typename AddressingModeGroup, const char* opcode>
    //  class SNESInstructionSingleOperand : public SNESInstruction {...}
    //  template <typename AddressingMode>
    //  using SNESInstructionLoadAccumulatorFromMemory = SNESInstructionOperand<AddressingMode, "LoadAccumulatorFromMemory",
    //                                                                          LAddressingModes::LDA_AddressingModes, opcode>
    //  ...or something like that. Probably not valid code, but gets the idea across.
    template <typename AddressingMode>
    class SNESInstructionAddWithCarry : public SNESInstruction {
        static_assert(AddressingModeGroups::LDA_AddressingModes<AddressingMode>::valid, "Invalid address mode used for SNESInstructionAddWithCarry.");

        const AddressingMode operand;
        public:
            SNESInstructionAddWithCarry(unsigned int operandArg) : operand(operandArg) {}
            virtual std::string getName() const { return "SNES Instruction: AddWithCarry"; }
            virtual std::string getStringRepresentation() const { return "ADC "+operand.getStringRepresentation(); }
    };
}

#endif // LLVM_IR_COMPILER_SNESInstructionAddWithCarry_H
