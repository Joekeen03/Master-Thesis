#ifndef LLVM_IR_COMPILER_EXPRESSIONValueInt_H
#define LLVM_IR_COMPILER_EXPRESSIONValueInt_H

#include <string>

#include "ExpressionValue.h"
#include "../Types/TypeInteger.h"

namespace Expressions {
    class ExpressionValueInteger : public ExpressionValue {
        public:
            const std::shared_ptr<const Types::TypeInteger> intType;
            const int val;
            ExpressionValueInteger(std::shared_ptr<const Types::TypeInteger> intTypeArg, int valArg) : intType(intTypeArg), val(valArg) {}
            std::string getName() const { return "ExpressionValueInt"; }
    };
}

#endif // LLVM_IR_COMPILER_EXPRESSIONValueInt_H
