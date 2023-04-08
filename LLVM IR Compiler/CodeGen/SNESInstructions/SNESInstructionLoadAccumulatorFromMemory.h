#ifndef LLVM_IR_COMPILER_SNESInstructionLoadAccumulatorFromMemory_H
#define LLVM_IR_COMPILER_SNESInstructionLoadAccumulatorFromMemory_H

#include "SNESInstruction.h"
#include "AddressingModes.h"

namespace SNESAssembly {
    template <typename AddressingMode>
    class SNESInstructionLoadAccumulatorFromMemory : public SNESInstruction {
        static_assert(AddressingModeGroups::LDA_AddressingModes<AddressingMode>::valid, "Invalid address mode used for SNESInstructionLoadAccumulatorFromMemory.");

        const AddressingMode operand;
        public:
            SNESInstructionLoadAccumulatorFromMemory(unsigned int operandArg) : operand(operandArg) {}
            virtual std::string getName() const { return "SNES Instruction: LoadAccumulatorFromMemory"; }
            virtual std::string getStringRepresentation() const { return "LDA "+operand.getStringRepresentation(); }
    };
}

#endif // LLVM_IR_COMPILER_SNESInstructionLoadAccumulatorFromMemory_H
