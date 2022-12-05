#ifndef LLVM_IR_COMPILER_INSTRUCTIONTerminator_H
#define LLVM_IR_COMPILER_INSTRUCTIONTerminator_H

#include <string>

#include "Instruction.h"

namespace Instructions {
    class InstructionTerminator : public Instruction {
        public:
            InstructionTerminator() {}
            std::string getName() const { return "InstructionTerminator"; }
    };
}

#endif // LLVM_IR_COMPILER_INSTRUCTIONTerminator_H
