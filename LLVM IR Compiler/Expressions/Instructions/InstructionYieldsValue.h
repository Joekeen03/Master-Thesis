#ifndef LLVM_IR_COMPILER_INSTRUCTIONYIELDSVALUE_H
#define LLVM_IR_COMPILER_INSTRUCTIONYIELDSVALUE_H

#include "Instruction.h"
#include "../ExpressionIdentifier.h"
#include "../../Lib/Result.h"

namespace Instructions {
    class InstructionYieldsValue : public Instruction {
        // Is it possible to have one of these instructions and not assign the yielded value to an identifier?
        //      I.e. ignore the yielded value.
        public:
            const std::shared_ptr<const Expressions::ExpressionIdentifier> assignee;
            InstructionYieldsValue(std::shared_ptr<const Expressions::ExpressionIdentifier> assigneeArg) : assignee(assigneeArg) {}
    };
}

#endif // LLVM_IR_COMPILER_INSTRUCTIONYIELDSVALUE_H