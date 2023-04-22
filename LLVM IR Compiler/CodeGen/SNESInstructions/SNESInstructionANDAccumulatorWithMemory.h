#ifndef LLVM_IR_COMPILER_SNESInstructionANDAccumulatorWithMemory_H
#define LLVM_IR_COMPILER_SNESInstructionANDAccumulatorWithMemory_H

#include "SNESInstruction.h"
#include "AddressingModes.h"

namespace SNESAssembly {
    template <typename AddressingMode>
    class SNESInstructionANDAccumulatorWithMemory : public SNESInstruction {
        static_assert(AddressingModeGroups::LDA_AddressingModes<AddressingMode>::valid, "Invalid address mode used for SNESInstructionANDAccumulatorWithMemory.");

        const AddressingMode operand;
        public:
            SNESInstructionANDAccumulatorWithMemory(unsigned int operandArg) : operand(operandArg) {}
            virtual std::string getName() const override { return "SNES Instruction: ANDAccumulatorWithMemory"; }
            virtual std::string getStringRepresentation() const override { return "AND "+operand.getStringRepresentation(); }
    };
}

#endif // LLVM_IR_COMPILER_SNESInstructionANDAccumulatorWithMemory_H
