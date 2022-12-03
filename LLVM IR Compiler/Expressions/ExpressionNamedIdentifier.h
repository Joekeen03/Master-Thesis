#ifndef LLVM_IR_COMPILER_EXPRESSIONNamedIdentifier_H
#define LLVM_IR_COMPILER_EXPRESSIONNamedIdentifier_H

#include <string>

#include "Expression.h"

namespace Expressions {
    class ExpressionNamedIdentifier : public Expression {
        public:
            const std::string identifier;
            ExpressionNamedIdentifier(std::string identifierArg) : identifier(identifierArg) {}
            std::string getName() { return "ExpressionNamedIdentifier"; }
    };
}

#endif // LLVM_IR_COMPILER_EXPRESSIONNamedIdentifier_H
