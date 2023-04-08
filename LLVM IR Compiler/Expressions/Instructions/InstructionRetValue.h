#ifndef LLVM_IR_COMPILER_INSTRUCTIONRetValue_H
#define LLVM_IR_COMPILER_INSTRUCTIONRetValue_H

#include <string>

#include "InstructionTerminatorYieldsVoid.h"
#include "../../Types/Type.h"
#include "../ExpressionOperand.h"

namespace Instructions {
    class InstructionRetValue : public InstructionTerminatorYieldsVoid {
        public:
            const std::shared_ptr<const Types::Type> returnType; // FIXME Should only allow valid return types
            const std::shared_ptr<const Expressions::ExpressionOperand> returnOperand; // FIXME Should only allow valid return values (?)
            InstructionRetValue(std::shared_ptr<const Types::Type> returnTypeArg, std::shared_ptr<const Expressions::ExpressionOperand> returnValueArg)
                                : returnType(returnTypeArg), returnOperand(returnValueArg) {}
            std::string getName() const { return "InstructionRetValue"; }
    };
}

#endif // LLVM_IR_COMPILER_INSTRUCTIONRetValue_H
