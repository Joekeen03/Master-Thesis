#ifndef LLVM_IR_COMPILER_SNESInstructionLoadXRegisterFromMemory_H
#define LLVM_IR_COMPILER_SNESInstructionLoadXRegisterFromMemory_H

#include "SNESInstruction.h"
#include "AddressingModes.h"

namespace SNESAssembly {
    template <typename AddressingMode>
    class SNESInstructionLoadXRegisterFromMemory : public SNESInstruction {
        static_assert(AddressingModeGroups::LDX_AddressingModes<AddressingMode>::valid,
                        "Invalid addressing mode used for SNESInstructionLoadXRegisterFromMemory.");

        const AddressingMode operand;
        public:
            SNESInstructionLoadXRegisterFromMemory(unsigned int operandArg) : operand(operandArg) {}
            virtual std::string getName() const { return "SNES Instruction: LoadXRegisterFromMemory"; }
            virtual std::string getStringRepresentation() const { return "LDX "+operand.getStringRepresentation(); }
    };
}

#endif // LLVM_IR_COMPILER_SNESInstructionLoadXRegisterFromMemory_H
