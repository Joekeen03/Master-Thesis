#ifndef LLVM_IR_COMPILER_YIELDSVALUE_H
#define LLVM_IR_COMPILER_YIELDSVALUE_H

#include "../ExpressionIdentifier.h"

namespace Instructions {
    class YieldsValue {
        // Is it possible to have one of these instructions and not assign the yielded value to an identifier?
        //      I.e. ignore the yielded value.
        public:
            const std::shared_ptr<const Expressions::ExpressionIdentifier> assignee;
            YieldsValue(std::shared_ptr<const Expressions::ExpressionIdentifier> assigneeArg) : assignee(assigneeArg) {}
    };
}

#endif // LLVM_IR_COMPILER_YIELDSVALUE_H