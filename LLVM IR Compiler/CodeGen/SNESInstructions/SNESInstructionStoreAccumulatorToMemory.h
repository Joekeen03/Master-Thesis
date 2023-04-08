#ifndef LLVM_IR_COMPILER_SNESInstructionStoreAccumulatorToMemory_H
#define LLVM_IR_COMPILER_SNESInstructionStoreAccumulatorToMemory_H

#include "SNESInstruction.h"
#include "AddressingModes.h"

namespace SNESAssembly {
    template <typename AddressingMode>
    class SNESInstructionStoreAccumulatorToMemory : public SNESInstruction {
        static_assert(AddressingModeGroups::LDA_AddressingModes<AddressingMode>::valid, "Invalid address mode used for SNESInstructionStoreAccumulatorToMemory.");

        const AddressingMode operand;
        public:
            SNESInstructionStoreAccumulatorToMemory(unsigned int operandArg) : operand(operandArg) {}
            virtual std::string getName() const { return "SNES Instruction: StoreAccumulatorToMemory"; }
            virtual std::string getStringRepresentation() const { return "STA " +operand.getStringRepresentation(); }
    };
}

#endif // LLVM_IR_COMPILER_SNESInstructionStoreAccumulatorToMemory_H
