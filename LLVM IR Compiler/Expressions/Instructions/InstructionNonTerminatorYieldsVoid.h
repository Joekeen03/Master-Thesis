#ifndef LLVM_IR_COMPILER_INSTRUCTIONNonTerminatorYieldsVoid_H
#define LLVM_IR_COMPILER_INSTRUCTIONNonTerminatorYieldsVoid_H

#include <string>

#include "InstructionNonTerminator.h"
#include "YieldsVoid.h"

namespace Instructions {
    class InstructionNonTerminatorYieldsVoid : public InstructionNonTerminator, public YieldsVoid {
        public:
            InstructionNonTerminatorYieldsVoid() {}
            std::string getName() { return "InstructionNonTerminatorYieldsVoid"; }
    };
}

#endif // LLVM_IR_COMPILER_INSTRUCTIONNonTerminatorYieldsVoid_H
