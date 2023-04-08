#ifndef LLVM_IR_COMPILER_EXPRESSIONOperand_H
#define LLVM_IR_COMPILER_EXPRESSIONOperand_H

#include <string>
#include <variant>

#include "Expression.h"
#include "ExpressionLocalUnnamedIdentifier.h"
#include "ExpressionLocalNamedIdentifier.h"
#include "ExpressionLiteralInteger.h"

namespace Expressions {
    // TEMP
    using operandVariant = std::variant<ExpressionLocalNamedIdentifier, ExpressionLocalUnnamedIdentifier, ExpressionLiteralInteger>;
    class ExpressionOperand : public Expression {
        public:
            const std::shared_ptr<const operandVariant> operand;
            ExpressionOperand(std::shared_ptr<const operandVariant> operandArg) : operand(operandArg) {}
            std::string getName() const { return "ExpressionOperand"; }
    };
}

#endif // LLVM_IR_COMPILER_EXPRESSIONOperand_H
