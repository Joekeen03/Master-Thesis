#ifndef LLVM_IR_COMPILER_INSTRUCTIONStore_H
#define LLVM_IR_COMPILER_INSTRUCTIONStore_H

#include <string>

#include "InstructionNonTerminatorYieldsVoid.h"
#include "../../Types/TypeSized.h"

namespace Instructions {
    class InstructionStore : public InstructionNonTerminatorYieldsVoid {
        public:
            const std::shared_ptr<const Types::TypeSized> valueType;
            const std::shared_ptr<const Expressions::ExpressionOperand> valueOperand;
            const std::shared_ptr<const Expressions::ExpressionIdentifier> pointerOperand;
            const int alignment;
            InstructionStore(std::shared_ptr<const Types::TypeSized> valueTypeArg,
                                std::shared_ptr<const Expressions::ExpressionOperand> valueOperandArg,
                                std::shared_ptr<const Expressions::ExpressionIdentifier> pointerOperandArg, int alignmentArg)
                                : valueType(valueTypeArg), valueOperand(valueOperandArg),
                                    pointerOperand(pointerOperandArg), alignment(alignmentArg) {}
            std::string getName() { return "InstructionStore"; }
    };
}

#endif // LLVM_IR_COMPILER_INSTRUCTIONStore_H
