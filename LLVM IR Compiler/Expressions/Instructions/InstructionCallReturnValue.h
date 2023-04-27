#ifndef LLVM_IR_COMPILER_INSTRUCTIONNONTERMINATORYIELDSVALUECallReturnValue_H
#define LLVM_IR_COMPILER_INSTRUCTIONNONTERMINATORYIELDSVALUECallReturnValue_H

#include <string>

#include "InstructionNonTerminatorYieldsValue.h"
#include "../ExpressionReturnType.h"

namespace Instructions {
    class InstructionCallReturnValue : public InstructionNonTerminatorYieldsValue {
        public:
            const std::string name;
            const std::shared_ptr<const Expressions::ExpressionReturnType> returnType;
            InstructionCallReturnValue(std::shared_ptr<const Expressions::ExpressionLocalIdentifier> assigneeArg,
                                       std::string nameArg, std::shared_ptr<const Expressions::ExpressionReturnType> returnTypeArg)
                                           : InstructionNonTerminatorYieldsValue(assigneeArg), name(nameArg),
                                             returnType(returnTypeArg) {}
            std::string getName() const override { return "InstructionCallReturnValue"; }
            virtual std::shared_ptr<const Types::Type> getYieldedType() const override { throw std::runtime_error("InstructionCallReturnValue::getYieldedType was not\
                                                                                     implemented.");}
            virtual std::string getStringRepresentation() const override {
                throw std::runtime_error("InstructionCallReturnValue::getStringRepresentation was not implemented.");
            }
    };
}

#endif // LLVM_IR_COMPILER_INSTRUCTIONNONTERMINATORYIELDSVALUECallReturnValue_H
