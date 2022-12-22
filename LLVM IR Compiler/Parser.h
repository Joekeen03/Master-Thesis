#ifndef LLVM_IR_COMPILER_PARSER_H
#define LLVM_IR_COMPILER_PARSER_H

#include <memory>
#include <utility>
#include <set>

#include "Expressions/Expression.h"
#include "Expressions/ExpressionFunctionDefinition.h"
#include "Expressions/ExpressionReturnType.h"
#include "Expressions/ExpressionSourceFile.h"
#include "Expressions/ExpressionTargetTriple.h"
#include "Expressions/ExpressionFunctionHeaderPreName.h"
#include "Expressions/ExpressionFunctionHeaderPostName.h"
#include "Expressions/ExpressionArgumentList.h"
#include "Expressions/ExpressionFile.h"
#include "Expressions/ExpressionDataLayout.h"
#include "Expressions/ExpressionFunctionCodeBlock.h"
#include "Expressions/ExpressionIdentifier.h"
#include "Expressions/ExpressionLocalIdentifier.h"
#include "Expressions/ExpressionOperand.h"
#include "Expressions/ExpressionNamedMetadataDefinition.h"
#include "Expressions/ExpressionUnnamedMetadataDefinition.h"
#include "Expressions/ExpressionNamedMetadataIdentifier.h"
#include "Expressions/ExpressionUnnamedMetadataIdentifier.h"
#include "Expressions/ExpressionValue.h"

#include "Expressions/Instructions/Instruction.h"

#include "Types/TypeSized.h"

#include "Tokenizer.h"
#include "AttributeIDProcessor.h"
#include "EnumRegistry.h"
#include "Types/Type.h"
#include "Lib/Result.h"

#define DEFAULT_ALIGNMENT 1

namespace Parser {
    struct CodeBlockParsingResult;

    using Tokenizer::tokensArrayPointer;
    using stringExtractResult = std::pair<std::string, bool>;
    using numberExtractResult = std::pair<int, bool>;
    using tokenPointer = std::shared_ptr<const Tokens::Token>;
    using expressionPointer = std::shared_ptr<const Expressions::Expression>;
    using newTokenPos = int;

    template <typename T>
    using ParsingResult = Lib::ResultPointer<T>;

    class Parser {
        private:
            const Tokenizer::tokensArrayPointer tokens;
            const std::shared_ptr<std::vector<int>> mappings;
            const std::shared_ptr<const std::vector<const Tokenizer::tokensArray>> attributeGroups;

            inline tokenPointer getToken(int pos) const { return (*tokens)[pos]; }
            inline void outputToken(int pos) const { std::cout << getToken(pos)->getNameAndPos() << '\n'; }

            // FIXME Should I make this description more generic - remove mentions of checking if 'token' is
            //      a <T> type token?
            //      E.g. '<T> type token' -> '<T> type'

            // Returns whether or not the token at 'pos' is a <T> type token for the provided reserved string.
            // Returns false if '*getToken(pos)' is not a <T> type token, or the token's registryItem (reserved word, operator, etc.)
            // isn't the same as the provided RegistryItem; otherwise returns true.
            //
            // Note: Generically, this checks that the token at 'pos' can be dynamically cast to type <T>,
            //      and that (T)(*token).registryItem == reserved
            //      So, objects of type T must have a RegistryItem registryItem member.
            template<typename T>
            bool checkReserved(int pos, EnumRegistry::RegistryItem reserved);

            // Checks if the token at 'pos' is a <T> type identifier token.
            // throws a ParsingException if that token is not a <T> type token.
            // Note: <T> must have an "std::string name" member.
            template<typename T>
            std::string extractNamedIdentifier(int pos);

            // Attempts to extract a string from the token at the specified position, returns a Pair<string, bool> with the result.
            // The bool indicates if the extraction was successful, and the string will contain the token's string if successful,
            //      an emtpy string otherwise.
            stringExtractResult attemptExtractString(int pos);

            // Attempts to extract an integer from the token at the specified position, returns a Pair<integer, bool> with the result.
            // The bool indicates if the extraction was successful, and the int will contain the token's integer if successful,
            //      -1 otherwise.
            numberExtractResult attemptExtractInteger(int pos);
            
            // Extracts a string from the TokenString at the specified position in 'tokens', or throws a ParseException if it can't.
            //  Specifically, if the token at 'pos' is a TokenString, it returns the string from it; otherwise it throws a ParseException.
            //  For cases where a string is syntactically required (are there any cases where it isn't?).
            std::string extractString(int pos);
            
            // Extracts an integer from the TokenNumberLiteral at the specified position in 'tokens', or throws a ParseException if it can't.
            //  Specifically, if the token at 'pos' is a TokenInteger, it returns the integer from it; otherwise it throws a ParseException.
            //  For cases where a integer is syntactically required (are there any cases where it isn't?).
            int extractInteger(int pos);

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
            // Parsing for instructions that yield void

            Instructions::InstructionParseResult parseInstructionStore(int startPos);

            // Parsing for instructions that yield a value

            Instructions::InstructionParseResult parseInstructionAlloca(int startPos, std::shared_ptr<const Expressions::ExpressionLocalIdentifier> assignee);

            // Parsing for terminator instructions

            Instructions::InstructionParseResult parseInstructinRetValue(int startPos);

            // General parsing

            ParsingResult<Expressions::ExpressionIdentifier> parseIdentifier(int startPos);
            ParsingResult<Expressions::ExpressionLocalIdentifier> parseLocalIdentifier(int startPos);
            ParsingResult<Types::Type> parseType(int startPos);
            ParsingResult<Types::TypeSized> parseSizedType(int startPos);
            ParsingResult<Types::TypeSized> parseFirstClassKnownSizeType(int startPos);
            ParsingResult<Expressions::ExpressionOperand> parseOperand(int startPos);

            ParsingResult<Expressions::ExpressionSourceFile> parseSourceFile(int startPos);
            ParsingResult<Expressions::ExpressionDataLayout> parseDataLayout(int startPos);
            ParsingResult<Expressions::ExpressionTargetTriple> parseTargetTriple(int startPos);

            // Function Parsing Methods

            ParsingResult<Expressions::ExpressionFunctionHeaderPreName> parseFunctionHeaderPreName(int startPos);
            ParsingResult<Expressions::ExpressionReturnType> parseFunctionReturnType(int startPos);
            ParsingResult<Expressions::ExpressionArgumentList> parseFunctionArgumentList(int startPos);
            ParsingResult<Expressions::ExpressionFunctionHeaderPostName> parseFunctionHeaderPostName(int startPos);
            Instructions::InstructionParseResult parseInstruction(int startPos);
            CodeBlockParsingResult parseFunctionCodeBlock(int startPos, int startUnnamedLocal, std::shared_ptr<std::set<std::string>> localNameSet);
            ParsingResult<std::vector<const std::shared_ptr<const Expressions::ExpressionFunctionCodeBlock>>> parseFunctionCodeBlocks(int startPos);
            ParsingResult<Expressions::ExpressionFunctionDefinition> parseFunctionDefinition(int startPos);

            // Metadata Parsing Methods
            ParsingResult<Expressions::ExpressionNamedMetadataIdentifier> parseNamedMetadataIdentifier(int startPos);
            ParsingResult<Expressions::ExpressionUnnamedMetadataIdentifier> parseUnnamedMetadataIdentifier(int startPos);
            ParsingResult<Expressions::ExpressionNamedMetadataDefinition> parseNamedMetdataDefinition(int startPos);
            ParsingResult<Expressions::ExpressionValue> parseValue(int startPos);
            ParsingResult<Expressions::ExpressionUnnamedMetadataDefinition> parseUnnamedMetdataDefinition(int startPos);

            std::shared_ptr<const Expressions::ExpressionFile> parse();
            Parser(AttributeIDProcessor::SubstitutedTokens substitutedTokens)
                : tokens(substitutedTokens.tokens), mappings(substitutedTokens.mappings), attributeGroups(substitutedTokens.attributeGroups) {}
    };

    struct CodeBlockParsingResult : public ParsingResult<Expressions::ExpressionFunctionCodeBlock> {
        const int nextUnnamedLocal;
        const std::shared_ptr<std::set<std::string>> localNameSet;
        CodeBlockParsingResult() : nextUnnamedLocal(-1), Result() {}
        CodeBlockParsingResult(std::shared_ptr<Expressions::ExpressionFunctionCodeBlock> expressionArg, newTokenPos newPosArg,
                                int nextUnnamedLocalArg, std::shared_ptr<std::set<std::string>> localNameSetArg)
                                : nextUnnamedLocal(nextUnnamedLocalArg), localNameSet(localNameSetArg), Result(expressionArg, newPosArg) {}
    };

    // Indicates if the parser has encountered a issue and cannot progress.
    class ParsingException : public std::runtime_error {
        public:
            const int tokenPos;
            ParsingException(std::string msg, int tokenPosArg) : tokenPos(tokenPosArg), runtime_error(msg.c_str()) {}
    };

    using instructionYieldsVoidParser = Instructions::InstructionParseResult(Parser::*)(int);
    using instructionYieldsValueParser = Instructions::InstructionParseResult(Parser::*)(int, std::shared_ptr<const Expressions::ExpressionLocalIdentifier>);
    extern const instructionYieldsVoidParser instructionsYieldVoid[];
    extern const int nInstructionsYieldVoid;
    extern const instructionYieldsValueParser instructionsYieldValue[];
    extern const int nInstructionsYieldValue;
}

#endif // LLVM_IR_COMPILER_PARSER_H