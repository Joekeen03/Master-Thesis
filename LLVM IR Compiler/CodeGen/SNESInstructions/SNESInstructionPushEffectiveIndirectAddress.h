#ifndef LLVM_IR_COMPILER_SNESInstructionPushEffectiveIndirectAddress_H
#define LLVM_IR_COMPILER_SNESInstructionPushEffectiveIndirectAddress_H

#include "SNESInstruction.h"
#include "AddressingModes.h"

namespace SNESAssembly {
    class SNESInstructionPushEffectiveIndirectAddress : public SNESInstruction {
        public:
            const AddressingModes::DirectPageIndirect operand;
            SNESInstructionPushEffectiveIndirectAddress(int operandArg) : operand(operandArg) {}
            virtual std::string getName() const { return "SNES Instruction: PushEffectiveIndirectAddress"; }
            virtual std::string getStringRepresentation() const { return "PEI "+operand.getStringRepresentation(); }
    };
}

#endif // LLVM_IR_COMPILER_SNESInstructionPushEffectiveIndirectAddress_H
