#ifndef LLVM_IR_COMPILER_SNESInstructionStoreZeroToMemory_H
#define LLVM_IR_COMPILER_SNESInstructionStoreZeroToMemory_H

#include "SNESInstruction.h"

namespace SNESAssembly {
    template <typename AddressingMode>
    class SNESInstructionStoreZeroToMemory : public SNESInstruction {
        static_assert(AddressingModeGroups::STZ_AddressingModes<AddressingMode>::valid, "Invalid address mode used for SNESInstructionStoreZeroToMemory.");

        const AddressingMode operand;
        public:
            SNESInstructionStoreZeroToMemory(unsigned int operandArg) : operand(operandArg) {}
            virtual std::string getName() const override { return "SNES Instruction: StoreZeroToMemory"; }
            virtual std::string getStringRepresentation() const override { return "STZ " +operand.getStringRepresentation(); }
    };
}

#endif // LLVM_IR_COMPILER_SNESInstructionStoreZeroToMemory_H
