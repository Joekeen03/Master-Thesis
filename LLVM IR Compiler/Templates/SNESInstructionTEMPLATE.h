#ifndef LLVM_IR_COMPILER_SNESInstructionTEMPLATE_H
#define LLVM_IR_COMPILER_SNESInstructionTEMPLATE_H

#include "SNESInstruction.h"

namespace SNESAssembly {
    class SNESInstructionTEMPLATE : public SNESInstruction {
        public:
            SNESInstructionTEMPLATE() {}
            virtual std::string getName() const override { return "SNES Instruction: TEMPLATE"; }
            virtual std::string getStringRepresentation() const override { throw std::runtime_error("Not Implemented: SNESInstructionTEMPLATE::getStringRepresentation()"); }
    };
}

#endif // LLVM_IR_COMPILER_SNESInstructionTEMPLATE_H