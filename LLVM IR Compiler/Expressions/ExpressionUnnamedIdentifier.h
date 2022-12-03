#ifndef LLVM_IR_COMPILER_EXPRESSIONUnnamedIdentifier_H
#define LLVM_IR_COMPILER_EXPRESSIONUnnamedIdentifier_H

#include <string>

#include "Expression.h"

namespace Expressions {
    class ExpressionUnnamedIdentifier : public Expression {
        public:
            const int ID;
            ExpressionUnnamedIdentifier(int IDArg) : ID(IDArg) {}
            std::string getName() { return "ExpressionUnnamedIdentifier"; }
    };
}

#endif // LLVM_IR_COMPILER_EXPRESSIONUnnamedIdentifier_H
