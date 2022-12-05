#ifndef LLVM_IR_COMPILER_INSTRUCTIONNonTerminatorYieldsValue_H
#define LLVM_IR_COMPILER_INSTRUCTIONNonTerminatorYieldsValue_H

#include <string>

#include "InstructionNonTerminator.h"
#include "YieldsValue.h"

namespace Instructions {
    class InstructionNonTerminatorYieldsValue : public InstructionNonTerminator, public YieldsValue {
        public:
            InstructionNonTerminatorYieldsValue(std::shared_ptr<const Expressions::ExpressionIdentifier> assigneeArg) : YieldsValue(assigneeArg) {}
            std::string getName() const { return "InstructionNonTerminatorYieldsValue"; }
    };
}

#endif // LLVM_IR_COMPILER_INSTRUCTIONNonTerminatorYieldsValue_H
