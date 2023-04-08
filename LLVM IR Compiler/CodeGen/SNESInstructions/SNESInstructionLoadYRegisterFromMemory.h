#ifndef LLVM_IR_COMPILER_SNESInstructionLoadYRegisterFromMemory_H
#define LLVM_IR_COMPILER_SNESInstructionLoadYRegisterFromMemory_H

#include "SNESInstruction.h"
#include "AddressingModes.h"

namespace SNESAssembly {
    template <typename AddressingMode>
    class SNESInstructionLoadYRegisterFromMemory : public SNESInstruction {
        static_assert(AddressingModeGroups::LDY_AddressingModes<AddressingMode>::valid,
                        "Invalid addressing mode used for SNESInstructionLoadYRegisterFromMemory.");

        const AddressingMode operand;
        public:
            SNESInstructionLoadYRegisterFromMemory(unsigned int operandArg) : operand(operandArg) {}
            virtual std::string getName() const { return "SNES Instruction: LoadYRegisterFromMemory"; }
            virtual std::string getStringRepresentation() const { return "LDY "+operand.getStringRepresentation(); }
    };
}

#endif // LLVM_IR_COMPILER_SNESInstructionLoadYRegisterFromMemory_H
