#ifndef LLVM_IR_COMPILER_INSTRUCTIONALLOCA_H
#define LLVM_IR_COMPILER_INSTRUCTIONALLOCA_H

#include <memory>

#include "InstructionYieldsValue.h"
#include "../../Types/TypeSized.h"

namespace Instructions {
    class InstructionAlloca : public InstructionYieldsValue {
        public:
            const bool inalloca;
            const std::shared_ptr<const Types::TypeSized> allocationType;
            const int alignment;
            InstructionAlloca(std::shared_ptr<const Expressions::ExpressionIdentifier> assigneeArg, bool inallocaArg,
                              std::shared_ptr<const Types::TypeSized> allocationTypeArg, int alignmentArg)
                                  : inalloca(inallocaArg), allocationType(allocationTypeArg), alignment(alignmentArg), InstructionYieldsValue(assigneeArg) {}
    };
}

#endif // LLVM_IR_COMPILER_INSTRUCTIONALLOCA_H