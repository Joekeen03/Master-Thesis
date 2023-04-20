#ifndef LLVM_IR_COMPILER_SNESInstructionSubtractWithBorrow_H
#define LLVM_IR_COMPILER_SNESInstructionSubtractWithBorrow_H

#include "SNESInstruction.h"
#include "AddressingModes.h"

namespace SNESAssembly {
    template <typename AddressingMode>
    class SNESInstructionSubtractWithBorrow : public SNESInstruction {
        static_assert(AddressingModeGroups::LDA_AddressingModes<AddressingMode>::valid, "Invalid address mode used for SNESInstructionSubtractWithBorrow.");

        const AddressingMode operand;
        public:
            SNESInstructionSubtractWithBorrow(unsigned int operandArg) : operand(operandArg) {}
            virtual std::string getName() const { return "SNES Instruction: SubtractWithBorrow"; }
            virtual std::string getStringRepresentation() const { return "SBC "+operand.getStringRepresentation(); }
    };
}

#endif // LLVM_IR_COMPILER_SNESInstructionSubtractWithBorrow_H
