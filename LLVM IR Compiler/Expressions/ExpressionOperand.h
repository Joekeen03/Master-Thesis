#ifndef LLVM_IR_COMPILER_EXPRESSIONOperand_H
#define LLVM_IR_COMPILER_EXPRESSIONOperand_H

#include <string>

#include "Expression.h"

namespace Expressions {
    class ExpressionOperand : public Expression {
        public:
            ExpressionOperand() {}
            std::string getName() const { return "ExpressionOperand"; }
    };
}

#endif // LLVM_IR_COMPILER_EXPRESSIONOperand_H
