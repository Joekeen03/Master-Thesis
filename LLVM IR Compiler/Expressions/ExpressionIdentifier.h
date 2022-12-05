#ifndef LLVM_IR_COMPILER_EXPRESSIONIdentifier_H
#define LLVM_IR_COMPILER_EXPRESSIONIdentifier_H

#include <string>

#include "Expression.h"

namespace Expressions {
    class ExpressionIdentifier : public Expression {
        public:
            ExpressionIdentifier() {}
            std::string getName() const { return "ExpressionIdentifier"; }
    };
}

#endif // LLVM_IR_COMPILER_EXPRESSIONIdentifier_H
