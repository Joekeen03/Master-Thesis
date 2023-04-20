#ifndef LLVM_IR_COMPILER_INSTRUCTIONNONTERMINATORYIELDSVALUEAdd_H
#define LLVM_IR_COMPILER_INSTRUCTIONNONTERMINATORYIELDSVALUEAdd_H

#include <string>

#include "InstructionNonTerminatorYieldsValue.h"
#include "../../Types/TypeSized.h"
#include "../ExpressionOperand.h"

namespace Instructions {
    class InstructionAdd : public InstructionNonTerminatorYieldsValue {
        public:
            const bool nsw;
            const bool nuw;
            const std::shared_ptr<const Types::TypeSized> yieldedType;
            const std::shared_ptr<const Expressions::ExpressionOperand> leftOperand;
            const std::shared_ptr<const Expressions::ExpressionOperand> rightOperand;
            
            InstructionAdd(std::shared_ptr<const Expressions::ExpressionLocalIdentifier> assigneeArg, bool nswArg,
                            bool nuwArg, std::shared_ptr<const Types::TypeSized> yieldedTypeArg,
                            const std::shared_ptr<const Expressions::ExpressionOperand> leftOperandArg,
                            const std::shared_ptr<const Expressions::ExpressionOperand> rightOperandArg)
                            : InstructionNonTerminatorYieldsValue(assigneeArg), nsw(nswArg), nuw(nuwArg),
                                yieldedType(yieldedTypeArg), leftOperand(leftOperandArg), rightOperand(rightOperandArg) {}
            std::string getName() const override { return "InstructionAdd"; }
            virtual std::shared_ptr<const Types::Type> getYieldedType() const override { return yieldedType;}
    };
}

#endif // LLVM_IR_COMPILER_INSTRUCTIONNONTERMINATORYIELDSVALUEAdd_H
