#ifndef LLVM_IR_COMPILER_EXPRESSION_H
#define LLVM_IR_COMPILER_EXPRESSION_H

#include <string>

namespace Expressions {
    class Expression {
        public:
            virtual std::string getName() { return "Expression"; }
    };
}

#endif // LLVM_IR_COMPILER_EXPRESSION_H