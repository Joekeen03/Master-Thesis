#ifndef LLVM_IR_COMPILER_INSTRUCTIONTEMPLATE_H
#define LLVM_IR_COMPILER_INSTRUCTIONTEMPLATE_H

#include <string>

#include "InstructionYieldsValue.h"

namespace Instructions {
    class InstructionTEMPLATE : public InstructionYieldsValue {
        public:
            InstructionTEMPLATE(std::shared_ptr<const Expressions::ExpressionIdentifier> assigneeArg) : InstructionYieldsValue(assigneeArg) {}
            std::string getName() { return "InstructionTEMPLATE"; }
    };
}

#endif // LLVM_IR_COMPILER_INSTRUCTIONTEMPLATE_H