#ifndef LLVM_IR_COMPILER_EXPRESSIONLocalUnnamedIdentifier_H
#define LLVM_IR_COMPILER_EXPRESSIONLocalUnnamedIdentifier_H

#include <string>

#include "ExpressionLocalIdentifier.h"
#include "ExpressionUnnamedIdentifier.h"

namespace Expressions {
    class ExpressionLocalUnnamedIdentifier : public ExpressionLocalIdentifier, public ExpressionUnnamedIdentifier {
        public:
            ExpressionLocalUnnamedIdentifier(int IDArg) : ExpressionUnnamedIdentifier(IDArg) {}
            std::string getName() const { return "ExpressionLocalUnnamedIdentifier"; }
    };
}

#endif // LLVM_IR_COMPILER_EXPRESSIONLocalUnnamedIdentifier_H
