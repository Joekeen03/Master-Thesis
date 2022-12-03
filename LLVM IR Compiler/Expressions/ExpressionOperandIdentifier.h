#ifndef LLVM_IR_COMPILER_EXPRESSIONOperandIdentifier_H
#define LLVM_IR_COMPILER_EXPRESSIONOperandIdentifier_H

#include <string>

#include "ExpressionOperand.h"
#include "ExpressionIdentifier.h"

namespace Expressions {
    class ExpressionOperandIdentifier : public ExpressionOperand {
        public:
            const std::shared_ptr<const ExpressionIdentifier> identifier;
            ExpressionOperandIdentifier(std::shared_ptr<const ExpressionIdentifier> identifierArg)
                                        : identifier(identifierArg) {}
            std::string getName() { return "ExpressionOperandIdentifier"; }
    };
}

#endif // LLVM_IR_COMPILER_EXPRESSIONOperandIdentifier_H
