#ifndef LLVM_IR_COMPILER_INSTRUCTIONTerminator_H
#define LLVM_IR_COMPILER_INSTRUCTIONTerminator_H

#include <string>

#include "Instruction.h"

namespace Instructions {
    class InstructionTerminator : public Instruction {
        public:
            InstructionTerminator() {}
            std::string getName() { return "InstructionTerminator"; }
    };
}

#endif // LLVM_IR_COMPILER_INSTRUCTIONTerminator_H
