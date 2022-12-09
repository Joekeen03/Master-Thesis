#ifndef LLVM_IR_COMPILER_EXPRESSIONValue_H
#define LLVM_IR_COMPILER_EXPRESSIONValue_H

#include <string>

#include "Expression.h"

namespace Expressions {
    class ExpressionValue : public Expression {
        public:
            ExpressionValue() {}
            std::string getName() const { return "ExpressionValue"; }
    };
}

#endif // LLVM_IR_COMPILER_EXPRESSIONValue_H
