#ifndef LLVM_IR_COMPILER_SNESInstructionPushEffectiveIndirectAddress_H
#define LLVM_IR_COMPILER_SNESInstructionPushEffectiveIndirectAddress_H

#include "SNESInstruction.h"

namespace SNESAssembly {
    class SNESInstructionPushEffectiveIndirectAddress : public SNESInstruction {
        public:
            const int address;
            SNESInstructionPushEffectiveIndirectAddress(int addressArg) : address(addressArg) {}
            virtual std::string getName() const { return "SNES Instruction: PushEffectiveIndirectAddress"; }
            virtual std::string getStringRepresentation() const { return "PEI ("+std::to_string(address)+")"; }
    };
}

#endif // LLVM_IR_COMPILER_SNESInstructionPushEffectiveIndirectAddress_H
