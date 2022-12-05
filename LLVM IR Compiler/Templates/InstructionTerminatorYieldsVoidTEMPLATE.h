#ifndef LLVM_IR_COMPILER_INSTRUCTIONTEMPLATE_H
#define LLVM_IR_COMPILER_INSTRUCTIONTEMPLATE_H

#include <string>

#include "InstructionTerminatorYieldsVoid.h"

namespace Instructions {
    class InstructionTEMPLATE : public InstructionTerminatorYieldsVoid {
        public:
            InstructionTEMPLATE() {}
            std::string getName() const { return "InstructionTEMPLATE"; }
    };
}

#endif // LLVM_IR_COMPILER_INSTRUCTIONTEMPLATE_H