#ifndef LLVM_IR_COMPILER_INSTRUCTION_H
#define LLVM_IR_COMPILER_INSTRUCTION_H

#include <string>

#include "../Expression.h"
#include "../../Lib/Result.h"

namespace Instructions {
    class Instruction : public Expressions::Expression {
        public:
            virtual std::string getName() const { return "Base Instruction"; }
    };
}

#endif // LLVM_IR_COMPILER_INSTRUCTION_H