#ifndef LLVM_IR_COMPILER_INSTRUCTIONNONTERMINATORYIELDSVOIDTEMPLATE_H
#define LLVM_IR_COMPILER_INSTRUCTIONNONTERMINATORYIELDSVOIDTEMPLATE_H

#include <string>

#include "InstructionNonTerminatorYieldsVoid.h"

namespace Instructions {
    class InstructionTEMPLATE : public InstructionNonTerminatorYieldsVoid {
        public:
            InstructionTEMPLATE() {}
            std::string getName() const override { return "InstructionTEMPLATE"; }
    };
}

#endif // LLVM_IR_COMPILER_INSTRUCTIONNONTERMINATORYIELDSVOIDTEMPLATE_H