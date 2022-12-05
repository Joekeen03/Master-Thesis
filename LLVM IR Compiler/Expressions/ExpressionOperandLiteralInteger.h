#ifndef LLVM_IR_COMPILER_EXPRESSIONOperandLiteralInteger_H
#define LLVM_IR_COMPILER_EXPRESSIONOperandLiteralInteger_H

#include <string>

#include "ExpressionOperand.h"

namespace Expressions {
    class ExpressionOperandLiteralInteger : public ExpressionOperand {
        public:
            const int val;
            ExpressionOperandLiteralInteger(int valArg) : val(valArg) {}
            std::string getName() const { return "ExpressionOperandLiteralInteger"; }
    };
}

#endif // LLVM_IR_COMPILER_EXPRESSIONOperandLiteralInteger_H
