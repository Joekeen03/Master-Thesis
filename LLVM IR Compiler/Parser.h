#ifndef LLVM_IR_COMPILER_PARSER_H
#define LLVM_IR_COMPILER_PARSER_H

#include <memory>
#include <utility>

#include "ParseExpressions/Expression.h"
#include "Tokenizer.h"
#include "EnumRegistry.h"

namespace Parser {
    struct ParsingResult;

    using tokensArrayPointer = std::shared_ptr<Tokenizer::tokensArray>;
    using stringExtractResult = std::pair<std::string, bool>;
    using tokenPointer = std::shared_ptr<const Token::Token>;

    class Parser {
        private:
            // FIXME: Combine these methods into one templated method

            // Returns whether or not the provided token is a TokenKeyword for the provided keyword.
            // Returns false if '*token' is not a TokenKeyword, or the token's keyword (registryItem) isn't the same as
            // the provided keyword; otherwise returns true.
            static bool checkKeyword(tokenPointer token, EnumRegistry::RegistryItem keyword);

            // Returns whether or not the provided token is a TokenOperator for the provided operator.
            // Returns false if '*token' is not a TokenOperator, or the token's operator (registryItem) isn't the same as
            // the provided operator; otherwise returns true.
            static bool checkOperand(tokenPointer token, EnumRegistry::RegistryItem op);

            static stringExtractResult attemptExtractString(tokenPointer token);
        public:
            ParsingResult parseSourceFile(tokensArrayPointer tokens, int startPos);
            ParsingResult parseDataLayout(tokensArrayPointer tokens, int startPos);
    };

    using newTokenPos = int;
    struct ParsingResult {
        const std::shared_ptr<const Expression::Expression> expression;
        const newTokenPos newPos;
        const bool success;
        ParsingResult() : expression(), newPos(-1), success(false) {}
        ParsingResult(std::shared_ptr<const Expression::Expression> expressionArg, newTokenPos newPosArg)
                        : expression(expressionArg), newPos(newPosArg), success(true) {}
    };
}

#endif // LLVM_IR_COMPILER_PARSER_H