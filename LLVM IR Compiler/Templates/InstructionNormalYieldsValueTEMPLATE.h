#ifndef LLVM_IR_COMPILER_INSTRUCTIONTEMPLATE_H
#define LLVM_IR_COMPILER_INSTRUCTIONTEMPLATE_H

#include <string>

#include "InstructionNonTerminatorYieldsValue.h"

namespace Instructions {
    class InstructionTEMPLATE : public InstructionNonTerminatorYieldsValue {
        public:
            InstructionTEMPLATE(std::shared_ptr<const Expressions::ExpressionIdentifier> assigneeArg) : InstructionNonTerminatorYieldsValue(assigneeArg) {}
            std::string getName() const { return "InstructionTEMPLATE"; }
    };
}

#endif // LLVM_IR_COMPILER_INSTRUCTIONTEMPLATE_H