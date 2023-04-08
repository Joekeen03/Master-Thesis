#ifndef LLVM_IR_COMPILER_SNESInstructionPushEffectiveAddress_H
#define LLVM_IR_COMPILER_SNESInstructionPushEffectiveAddress_H

#include "SNESInstruction.h"

namespace SNESAssembly {
    // Actually just pushes an immediate value, but that is what the Famicom wiki calls this instruction (and the manual calls
    //  it Push Effective Absolute Address).
    class SNESInstructionPushEffectiveAddress : public SNESInstruction {
        public:
            const int immediate;
            SNESInstructionPushEffectiveAddress(int immediateArg) : immediate(immediateArg) {}
            virtual std::string getName() const { return "SNES Instruction: PushEffectiveAddress"; }
            
            // FIXME easy way to convert # to hex string?
            // FIXME Move the '('+to_string(...)+')' into a separate function, as this is part of the addressing mode, not
            //  instruction-specific.
            virtual std::string getStringRepresentation() const { return "PEA "+std::to_string(immediate); }
    };
}

#endif // LLVM_IR_COMPILER_SNESInstructionPushEffectiveAddress_H
