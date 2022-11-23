#include "ParseResultSourceFile.h"

#include <typeinfo>

#include "Tokens/TokenKeyword.h"
#include "Tokens/TokenOperator.h"
#include "Tokens/TokenString.h"

namespace ParseResult {
    ParsingResult ParseResultSourceFile::attemptToParse(std::shared_ptr<Tokenizer::tokensArray> tokens, int startPos) {
        int currPos = startPos;
        if (typeid(*(*tokens)[currPos]) == typeid(Token::TokenKeyword)) {
            std::shared_ptr<const Token::TokenKeyword> keyword = std::dynamic_pointer_cast<const Token::TokenKeyword>((*tokens)[currPos]);
            currPos++;
            if (keyword->registryItem == ReservedWords::source_filename) {
                if (typeid(*(*tokens)[currPos]) == typeid(Token::TokenOperator)) {
                    std::shared_ptr<const Token::TokenOperator> op = std::dynamic_pointer_cast<const Token::TokenOperator>((*tokens)[currPos]);
                    currPos++;
                    if (op->registryItem == Operators::equals) {
                        if (typeid(*(*tokens)[currPos]) == typeid(Token::TokenString)) {
                            std::string str = std::dynamic_pointer_cast<const Token::TokenString>((*tokens)[currPos])->str;
                            return ParsingResult(std::make_shared<ParseResultSourceFile>(str), currPos);
                        }
                    }
                }
            }
        }
        return ParsingResult();
    }
}