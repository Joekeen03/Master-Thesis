#ifndef LLVM_IR_COMPILER_INSTRUCTIONNonTerminator_H
#define LLVM_IR_COMPILER_INSTRUCTIONNonTerminator_H

#include <string>

#include "Instruction.h"

namespace Instructions {
    class InstructionNonTerminator : public Instruction {
        public:
            InstructionNonTerminator() {}
            std::string getName() const { return "InstructionNonTerminator"; }
    };
}

#endif // LLVM_IR_COMPILER_INSTRUCTIONNonTerminator_H
