#ifndef LLVM_IR_COMPILER_EXPRESSIONReturnType_H
#define LLVM_IR_COMPILER_EXPRESSIONReturnType_H

#include <string>

#include "Expression.h"
#include "../Tokens/TokenKeyword.h"

namespace Expression {
    class ExpressionReturnType : public Expression {
        using keywordsVector = const std::shared_ptr<std::vector<std::shared_ptr<const Token::TokenKeyword>>>;
        using keywordPointer = const std::shared_ptr<const Token::TokenKeyword>;
        public:
            keywordsVector attributes;
            keywordPointer returnType;
            ExpressionReturnType(keywordsVector attributesArg, keywordPointer returnTypeArg) : attributes(attributesArg), returnType(returnTypeArg) {}
            std::string getName() { return "ExpressionReturnType"; }
    };
}

#endif // LLVM_IR_COMPILER_EXPRESSIONReturnType_H
