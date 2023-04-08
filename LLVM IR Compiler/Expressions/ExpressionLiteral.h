#ifndef LLVM_IR_COMPILER_EXPRESSIONLiteral_H
#define LLVM_IR_COMPILER_EXPRESSIONLiteral_H

#include <string>

#include "Expression.h"

namespace Expressions {
    class ExpressionLiteral : public Expression {
        public:
            ExpressionLiteral() {}
            std::string getName() const { return "ExpressionLiteral"; }
    };
}

#endif // LLVM_IR_COMPILER_EXPRESSIONLiteral_H
