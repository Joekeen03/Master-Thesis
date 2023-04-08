#ifndef LLVM_IR_COMPILER_EXPRESSIONLiteralInteger_H
#define LLVM_IR_COMPILER_EXPRESSIONLiteralInteger_H

#include <string>

#include "ExpressionLiteral.h"

namespace Expressions {
    // Represents a generic integer (bit-size it should fit into is unknown).
    class ExpressionLiteralInteger : public ExpressionLiteral {
        public:
            const int literal;
            ExpressionLiteralInteger(int literalArg) : literal(literalArg) {}
            std::string getName() const { return "ExpressionLiteralInteger"; }
    };
}

#endif // LLVM_IR_COMPILER_EXPRESSIONLiteralInteger_H
