#ifndef LLVM_IR_COMPILER_YIELDSVALUE_H
#define LLVM_IR_COMPILER_YIELDSVALUE_H

#include "../ExpressionIdentifier.h"
#include "../ExpressionLocalIdentifier.h"
#include "../../Types/Type.h"

namespace Instructions {
    class YieldsValue {
        // Is it possible to have one of these instructions and not assign the yielded value to an identifier?
        //      I.e. ignore the yielded value.
        public:
            // Assume you can only assign to a local variable.
            const std::shared_ptr<const Expressions::ExpressionLocalIdentifier> assignee;
            YieldsValue(std::shared_ptr<const Expressions::ExpressionLocalIdentifier> assigneeArg) : assignee(assigneeArg) {}
            virtual std::shared_ptr<const Types::Type> getYieldedType() const { throw std::runtime_error("YieldsValue::getYieldedType was not\
                                                                                     overridden in a subclass");}
    };
}

#endif // LLVM_IR_COMPILER_YIELDSVALUE_H