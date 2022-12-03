#ifndef LLVM_IR_COMPILER_INSTRUCTIONTerminatorYieldsVoid_H
#define LLVM_IR_COMPILER_INSTRUCTIONTerminatorYieldsVoid_H

#include <string>

#include "InstructionTerminator.h"
#include "YieldsVoid.h"

namespace Instructions {
    class InstructionTerminatorYieldsVoid : public InstructionTerminator, public YieldsVoid {
        public:
            InstructionTerminatorYieldsVoid() {}
            std::string getName() { return "InstructionTerminatorYieldsVoid"; }
    };
}

#endif // LLVM_IR_COMPILER_INSTRUCTIONTerminatorYieldsVoid_H
