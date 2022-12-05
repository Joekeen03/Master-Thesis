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
            const std::shared_ptr<const Expressions::ExpressionOperand> value; // FIXME Should only allow valid return values (?)
            InstructionRetValue(std::shared_ptr<const Types::Type> returnTypeArg, std::shared_ptr<const Expressions::ExpressionOperand> valueArg)
                                : returnType(returnTypeArg), value(valueArg) {}
            std::string getName() const { return "InstructionRetValue"; }
    };
}

#endif // LLVM_IR_COMPILER_INSTRUCTIONRetValue_H
