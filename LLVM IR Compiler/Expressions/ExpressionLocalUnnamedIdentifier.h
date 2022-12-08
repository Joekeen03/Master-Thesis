#ifndef LLVM_IR_COMPILER_EXPRESSIONLocalUnnamedIdentifier_H
#define LLVM_IR_COMPILER_EXPRESSIONLocalUnnamedIdentifier_H

#include <string>

#include "ExpressionLocalIdentifier.h"

namespace Expressions {
    class ExpressionLocalUnnamedIdentifier : public ExpressionLocalIdentifier {
        public:
            const int ID;
            ExpressionLocalUnnamedIdentifier(int IDArg) : ID(IDArg) {}
            std::string getName() const { return "ExpressionLocalUnnamedIdentifier"; }
    };
}

#endif // LLVM_IR_COMPILER_EXPRESSIONLocalUnnamedIdentifier_H
