#ifndef LLVM_IR_COMPILER_PARSER_H
#define LLVM_IR_COMPILER_PARSER_H

#include <memory>
#include <utility>
#include <set>

#include "ParseExpressions/Expression.h"
#include "ParseExpressions/ExpressionFunctionDefinition.h"
#include "ParseExpressions/ExpressionReturnType.h"
#include "ParseExpressions/ExpressionSourceFile.h"
#include "ParseExpressions/ExpressionTargetTriple.h"
#include "ParseExpressions/ExpressionFunctionHeaderPreName.h"
#include "ParseExpressions/ExpressionFunctionHeaderPostName.h"
#include "ParseExpressions/ExpressionArgumentList.h"
#include "ParseExpressions/ExpressionFile.h"
#include "ParseExpressions/ExpressionDataLayout.h"
#include "ParseExpressions/ExpressionFunctionCodeBlock.h"

#include "Tokenizer.h"
#include "AttributeIDProcessor.h"
#include "EnumRegistry.h"

namespace Parser {
    template <typename T>
    struct ParsingResult;
    struct CodeBlockParsingResult;

    using Tokenizer::tokensArrayPointer;
    using stringExtractResult = std::pair<std::string, bool>;
    using tokenPointer = std::shared_ptr<const Token::Token>;
    using expressionPointer = std::shared_ptr<const Expression::Expression>;

    class Parser {
        private:
            const Tokenizer::tokensArrayPointer tokens;
            const std::shared_ptr<std::vector<int>> mappings;
            const std::shared_ptr<const std::vector<const Tokenizer::tokensArray>> attributeGroups;
            // FIXME Should I make this description more generic - remove mentions of checking if 'token' is
            //      a <T> type token?
            //      E.g. '<T> type token' -> '<T> type'

            // Returns whether or not the provided token is a <T> type token for the provided reserved string.
            // Returns false if '*token' is not a <T> type token, or the token's registryItem (reserved word, operator, etc.)
            // isn't the same as the provided RegistryItem; otherwise returns true.
            //
            // Note: Generically, this checks that '*token' can be dynamically cast to type <T>,
            //      and that (T)(*token).registryItem == reserved
            //      So, objects of type T must have a RegistryItem registryItem member.
            template<typename T>
            bool checkReserved(int pos, EnumRegistry::RegistryItem reserved);

            // Checks if the provided token is a <T> type identifier token.
            // throws a ParsingException if *token is not a <T> type token.
            // Note: <T> must be a sub-type of TokenBaseIdentifier.
            template<typename T>
            std::string extractNamedIdentifier(int pos);

            // Attempts to extract a string from the provided token, returns a Pair<string, bool> with the result.
            // The bool indicates if the extraction was successful, and the string will contain the token's string if successful,
            //      an emtpy string otherwise.
            stringExtractResult attemptExtractString(int pos);
            
            // Extracts a string from the token at the specified position in 'tokens', or throws a ParseException if it can't.
            //  Specifically, if the token at 'pos' is a TokenString, it returns the string from it; otherwise it throws a ParseException.
            //  For cases where a string is syntactically required (are there any cases where it isn't?).
            std::string extractString(int pos);

            // Checks that the specified token is a <T> type token, and that it is a left-side token
            // Returns true if both of those are true, and false otherwise.
            // E.g. if T=TokenParenthesis, returns whether that the specified token is a TokenParenthesis, and is a left parenthesis: '('
            // The specified type <T> must have a boolean 'left' member.
            template<typename T>
            bool isLeftToken(int pos);

            // Checks that the specified token is a <T> type token, and that it is a right-side token
            // Returns true if both of those are true, and false otherwise.
            // E.g. if T=TokenParenthesis, returns whether that the specified token is a TokenParenthesis, and is a right parenthesis: ')'
            // The specified type <T> must have a boolean 'left' member.
            template<typename T>
            bool isRightToken(int pos);
        public:
            ParsingResult<Expression::ExpressionSourceFile> parseSourceFile(int startPos);
            ParsingResult<Expression::ExpressionDataLayout> parseDataLayout(int startPos);
            ParsingResult<Expression::ExpressionTargetTriple> parseTargetTriple(int startPos);
            ParsingResult<Expression::ExpressionFunctionHeaderPreName> parseFunctionHeaderPreName(int startPos);
            ParsingResult<Expression::ExpressionReturnType> parseFunctionReturnType(int startPos);
            ParsingResult<Expression::ExpressionArgumentList> parseFunctionArgumentList(int startPos);
            ParsingResult<Expression::ExpressionFunctionHeaderPostName> parseFunctionHeaderPostName(int startPos);
            CodeBlockParsingResult parseFunctionCodeBlock(int startPos, int startUnnamedLocal, std::shared_ptr<std::set<std::string>> localNameSet);
            ParsingResult<std::vector<std::shared_ptr<Expression::ExpressionFunctionCodeBlock>>> parseFunctionCodeBlocks(int startPos);
            ParsingResult<Expression::ExpressionFunctionDefinition> parseFunctionDefinition(int startPos);

            std::shared_ptr<const Expression::ExpressionFile> parse();
            Parser(AttributeIDProcessor::SubstitutedTokens substitutedTokens)
                : tokens(substitutedTokens.tokens), mappings(substitutedTokens.mappings), attributeGroups(substitutedTokens.attributeGroups) {}
    };

    using newTokenPos = int;

    template <typename T>
    struct ParsingResult {
        const std::shared_ptr<T> expression;
        const newTokenPos newPos;
        const bool success;
        ParsingResult() : expression(), newPos(-1), success(false) {}
        ParsingResult(std::shared_ptr<T> expressionArg, newTokenPos newPosArg)
                        : expression(expressionArg), newPos(newPosArg), success(true) {}
    };

    struct CodeBlockParsingResult : public ParsingResult<Expression::ExpressionFunctionCodeBlock> {
        const int nextUnnamedLocal;
        const std::shared_ptr<std::set<std::string>> localNameSet;
        CodeBlockParsingResult() : nextUnnamedLocal(-1), ParsingResult() {}
        CodeBlockParsingResult(std::shared_ptr<Expression::ExpressionFunctionCodeBlock> expressionArg, newTokenPos newPosArg,
                                int nextUnnamedLocalArg, std::shared_ptr<std::set<std::string>> localNameSetArg)
                                : nextUnnamedLocal(nextUnnamedLocalArg), localNameSet(localNameSetArg), ParsingResult(expressionArg, newPosArg) {}
    };

    // Indicates if the parser has encountered a issue and cannot progress.
    class ParsingException : public std::runtime_error {
        public:
            const int tokenPos;
            ParsingException(std::string msg, int tokenPosArg) : tokenPos(tokenPosArg), runtime_error(msg.c_str()) {}
    };
}

#endif // LLVM_IR_COMPILER_PARSER_H