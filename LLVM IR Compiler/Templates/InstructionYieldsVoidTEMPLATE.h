#ifndef LLVM_IR_COMPILER_INSTRUCTIONTEMPLATE_H
#define LLVM_IR_COMPILER_INSTRUCTIONTEMPLATE_H

#include <string>

#include "InstructionYieldsVoid.h"

namespace Instructions {
    class InstructionTEMPLATE : public InstructionYieldsVoid {
        public:
            InstructionTEMPLATE() {}
            std::string getName() { return "InstructionTEMPLATE"; }
    };
}

#endif // LLVM_IR_COMPILER_INSTRUCTIONTEMPLATE_H