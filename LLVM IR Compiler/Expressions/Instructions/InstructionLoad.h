#ifndef LLVM_IR_COMPILER_INSTRUCTIONNONTERMINATORYIELDSVALUELoad_H
#define LLVM_IR_COMPILER_INSTRUCTIONNONTERMINATORYIELDSVALUELoad_H

#include <string>

#include "InstructionNonTerminatorYieldsValue.h"
#include "../ExpressionIdentifier.h"
#include "../../Types/TypeSized.h"

namespace Instructions {
    class InstructionLoad : public InstructionNonTerminatorYieldsValue {
        public:
            const std::shared_ptr<const Types::TypeSized> yieldedType;
            const std::shared_ptr<const Expressions::ExpressionIdentifier> pointerOperand;
            const int alignment;
            InstructionLoad(std::shared_ptr<const Expressions::ExpressionLocalIdentifier> assigneeArg,
                            std::shared_ptr<const Types::TypeSized> yieldedTypeArg,
                            std::shared_ptr<const Expressions::ExpressionIdentifier> pointerOperandArg, int alignmentArg)
                            : InstructionNonTerminatorYieldsValue(assigneeArg), yieldedType(yieldedTypeArg),
                                pointerOperand(pointerOperandArg), alignment(alignmentArg) {}
            std::string getName() const override { return "InstructionLoad"; }
            virtual std::shared_ptr<const Types::Type> getYieldedType() const override { return yieldedType; }
    };
}

#endif // LLVM_IR_COMPILER_INSTRUCTIONNONTERMINATORYIELDSVALUELoad_H
