#ifndef LLVM_IR_COMPILER_EXPRESSIONFunctionHeaderPostName_H
#define LLVM_IR_COMPILER_EXPRESSIONFunctionHeaderPostName_H

#include <string>
#include <map>

#include "Expression.h"
#include "../Tokens/TokenKeyword.h"

namespace Expressions {
    class ExpressionFunctionHeaderPostName : public Expression {
        public:
            const std::shared_ptr<std::vector<std::shared_ptr<const Tokens::TokenKeyword>>> keywords;
            const std::shared_ptr<std::map<std::string, std::string>> strKeywords;
            ExpressionFunctionHeaderPostName(std::shared_ptr<std::vector<std::shared_ptr<const Tokens::TokenKeyword>>> keywordsArg,
                                             std::shared_ptr<std::map<std::string, std::string>> strKeywordsArg)
                                             : keywords(keywordsArg), strKeywords(strKeywordsArg) {}
            std::string getName() const { return "ExpressionFunctionHeaderPostName"; }
    };
}

#endif // LLVM_IR_COMPILER_EXPRESSIONFunctionHeaderPostName_H
