#ifndef LLVM_IR_COMPILER_EXPRESSIONLocalNamedIdentifier_H
#define LLVM_IR_COMPILER_EXPRESSIONLocalNamedIdentifier_H

#include <string>

#include "ExpressionLocalIdentifier.h"

namespace Expressions {
    class ExpressionLocalNamedIdentifier : public ExpressionLocalIdentifier {
        public:
            const std::string name;
            ExpressionLocalNamedIdentifier(std::string nameArg) : name(nameArg) {}
            std::string getName() const { return "ExpressionLocalNamedIdentifier"; }
    };
}

#endif // LLVM_IR_COMPILER_EXPRESSIONLocalNamedIdentifier_H
