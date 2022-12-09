#ifndef LLVM_IR_COMPILER_EXPRESSIONUnnamedMetadataIdentifier_H
#define LLVM_IR_COMPILER_EXPRESSIONUnnamedMetadataIdentifier_H

#include <string>

#include "Expression.h"

namespace Expressions {
    class ExpressionUnnamedMetadataIdentifier : public Expression {
        public:
            const int ID;
            ExpressionUnnamedMetadataIdentifier(int IDArg) : ID(IDArg) {}
            std::string getName() const { return "ExpressionUnnamedMetadataIdentifier"; }
    };
}

#endif // LLVM_IR_COMPILER_EXPRESSIONUnnamedMetadataIdentifier_H
