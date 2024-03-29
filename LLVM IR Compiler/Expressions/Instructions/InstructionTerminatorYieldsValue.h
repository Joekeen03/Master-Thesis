#ifndef LLVM_IR_COMPILER_INSTRUCTIONTerminatorYieldsValue_H
#define LLVM_IR_COMPILER_INSTRUCTIONTerminatorYieldsValue_H

#include <string>

#include "InstructionTerminator.h"
#include "YieldsValue.h"

namespace Instructions {
    class InstructionTerminatorYieldsValue : public InstructionTerminator, public YieldsValue {
        public:
            InstructionTerminatorYieldsValue(std::shared_ptr<const Expressions::ExpressionLocalIdentifier> assigneeArg) : YieldsValue(assigneeArg) {}
            std::string getName() const { return "InstructionTerminatorYieldsValue"; }
    };
}

#endif // LLVM_IR_COMPILER_INSTRUCTIONTerminatorYieldsValue_H
