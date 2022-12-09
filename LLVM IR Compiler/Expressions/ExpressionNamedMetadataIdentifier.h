#ifndef LLVM_IR_COMPILER_EXPRESSIONNamedMetadataIdentifier_H
#define LLVM_IR_COMPILER_EXPRESSIONNamedMetadataIdentifier_H

#include <string>

#include "Expression.h"

namespace Expressions {
    class ExpressionNamedMetadataIdentifier : public Expression {
        public:
            const std::string name;
            ExpressionNamedMetadataIdentifier(std::string nameArg) : name(nameArg) {}
            std::string getName() const { return "ExpressionNamedMetadataIdentifier"; }
    };
}

#endif // LLVM_IR_COMPILER_EXPRESSIONNamedMetadataIdentifier_H
