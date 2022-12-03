#ifndef LLVM_IR_COMPILER_EXPRESSIONLocalNamedIdentifier_H
#define LLVM_IR_COMPILER_EXPRESSIONLocalNamedIdentifier_H

#include <string>

#include "ExpressionLocalIdentifier.h"
#include "ExpressionNamedIdentifier.h"

namespace Expressions {
    class ExpressionLocalNamedIdentifier : public ExpressionLocalIdentifier, public ExpressionNamedIdentifier {
        public:
            ExpressionLocalNamedIdentifier(std::string identifierArg) : ExpressionNamedIdentifier(identifierArg) {}
            std::string getName() { return "ExpressionLocalNamedIdentifier"; }
    };
}

#endif // LLVM_IR_COMPILER_EXPRESSIONLocalNamedIdentifier_H
