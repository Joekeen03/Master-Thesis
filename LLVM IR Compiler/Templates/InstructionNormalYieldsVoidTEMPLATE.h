#ifndef LLVM_IR_COMPILER_INSTRUCTIONTEMPLATE_H
#define LLVM_IR_COMPILER_INSTRUCTIONTEMPLATE_H

#include <string>

#include "InstructionNonTerminatorYieldsVoid.h"

namespace Instructions {
    class InstructionTEMPLATE : public InstructionNonTerminatorYieldsVoid {
        public:
            InstructionTEMPLATE() {}
            std::string getName() const { return "InstructionTEMPLATE"; }
    };
}

#endif // LLVM_IR_COMPILER_INSTRUCTIONTEMPLATE_H