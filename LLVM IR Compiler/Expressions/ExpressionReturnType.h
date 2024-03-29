#ifndef LLVM_IR_COMPILER_EXPRESSIONReturnType_H
#define LLVM_IR_COMPILER_EXPRESSIONReturnType_H

#include <string>

#include "Expression.h"
#include "../Tokens/TokenKeyword.h"
#include "../Types/Type.h"

namespace Expressions {
    class ExpressionReturnType : public Expression {
        using keywordsVector = const std::shared_ptr<std::vector<std::shared_ptr<const Tokens::TokenKeyword>>>;
        public:
            const keywordsVector attributes;
            const std::shared_ptr<const Types::Type> returnType;
            ExpressionReturnType(keywordsVector attributesArg, const std::shared_ptr<const Types::Type> returnTypeArg) : attributes(attributesArg), returnType(returnTypeArg) {}
            std::string getName() const { return "ExpressionReturnType"; }
    };
}

#endif // LLVM_IR_COMPILER_EXPRESSIONReturnType_H
