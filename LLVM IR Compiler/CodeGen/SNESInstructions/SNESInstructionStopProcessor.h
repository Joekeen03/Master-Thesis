#ifndef LLVM_IR_COMPILER_SNESInstructionStopProcessor_H
#define LLVM_IR_COMPILER_SNESInstructionStopProcessor_H

#include "SNESInstruction.h"

namespace SNESAssembly {
    class SNESInstructionStopProcessor : public SNESInstruction {
        public:
            SNESInstructionStopProcessor() {}
            virtual std::string getName() const override { return "SNES Instruction: StopProcessor"; }
            virtual std::string getStringRepresentation() const override { return "STP"; }
    };
}

#endif // LLVM_IR_COMPILER_SNESInstructionStopProcessor_H
