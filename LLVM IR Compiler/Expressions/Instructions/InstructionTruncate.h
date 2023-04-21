#ifndef LLVM_IR_COMPILER_INSTRUCTIONNONTERMINATORYIELDSVALUETruncate_H
#define LLVM_IR_COMPILER_INSTRUCTIONNONTERMINATORYIELDSVALUETruncate_H

#include <string>
#include <memory>

#include "InstructionNonTerminatorYieldsValue.h"
#include "../ExpressionOperand.h"
#include "../../Types/TypeSized.h"

namespace Instructions {
    class InstructionTruncate : public InstructionNonTerminatorYieldsValue {
        public:
            const std::shared_ptr<const Types::TypeSized> operandType;
            const std::shared_ptr<const Expressions::ExpressionOperand> operand;
            const std::shared_ptr<const Types::TypeSized> yieldedType;
            InstructionTruncate(std::shared_ptr<const Expressions::ExpressionLocalIdentifier> assigneeArg,
                                std::shared_ptr<const Types::TypeSized> operandTypeArg,
                                std::shared_ptr<const Expressions::ExpressionOperand> operandArg,
                                std::shared_ptr<const Types::TypeSized> yieldedTypeArg)
                                    : InstructionNonTerminatorYieldsValue(assigneeArg), operandType(operandTypeArg),
                                        operand(operandArg), yieldedType(yieldedTypeArg) {}
            std::string getName() const override { return "InstructionTruncate"; }
            virtual std::shared_ptr<const Types::Type> getYieldedType() const override { return yieldedType; }
    };
}

#endif // LLVM_IR_COMPILER_INSTRUCTIONNONTERMINATORYIELDSVALUETruncate_H
