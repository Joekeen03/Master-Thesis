#ifndef LLVM_IR_COMPILER_INSTRUCTIONNONTERMINATORYIELDSVALUETEMPLATE_H
#define LLVM_IR_COMPILER_INSTRUCTIONNONTERMINATORYIELDSVALUETEMPLATE_H

#include <string>

#include "InstructionNonTerminatorYieldsValue.h"

namespace Instructions {
    class InstructionTEMPLATE : public InstructionNonTerminatorYieldsValue {
        public:
            InstructionTEMPLATE(std::shared_ptr<const Expressions::ExpressionLocalIdentifier> assigneeArg) : InstructionNonTerminatorYieldsValue(assigneeArg) {}
            std::string getName() const override { return "InstructionTEMPLATE"; }
            virtual std::shared_ptr<const Types::Type> getYieldedType() const override { throw std::runtime_error("InstructionTEMPLATE::getYieldedType was not\
                                                                                     implemented.");}
            virtual std::string getStringRepresentation() const override {
                throw std::runtime_error("InstructionTEMPLATE::getStringRepresentation was not implemented.");
            }
    };
}

#endif // LLVM_IR_COMPILER_INSTRUCTIONNONTERMINATORYIELDSVALUETEMPLATE_H