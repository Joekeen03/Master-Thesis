#ifndef LLVM_IR_COMPILER_EXPRESSIONFunctionHeaderPreName_H
#define LLVM_IR_COMPILER_EXPRESSIONFunctionHeaderPreName_H

#include <string>

#include "Expression.h"
#include "../Tokens/TokenKeyword.h"

namespace Expressions {
    // Temporary expression
    class ExpressionFunctionHeaderPreName : public Expression {
        public:
            const std::shared_ptr<std::vector<std::shared_ptr<const Token::TokenKeyword>>> keywords;
            ExpressionFunctionHeaderPreName(std::shared_ptr<std::vector<std::shared_ptr<const Token::TokenKeyword>>> keywordsArg) : keywords(keywordsArg) {}
            std::string getName() const { return "ExpressionFunctionHeaderPreName"; }
    };
}

#endif // LLVM_IR_COMPILER_EXPRESSIONFunctionHeaderPreName_H
