#ifndef LLVM_IR_COMPILER_TOKENBaseUnnamedIdentifier_H
#define LLVM_IR_COMPILER_TOKENBaseUnnamedIdentifier_H

#include <memory>
#include <iostream>

#include "Token.h"

namespace Token {
    class TokenBaseUnnamedIdentifier : public Token::Token {
        public:
            const int ID;
            explicit TokenBaseUnnamedIdentifier(int IDArg, int srcPosArg) : ID(IDArg), Token(srcPosArg) {}
            std::string getName() const {return "Generic Unnamed Identifier Token"; }
            // Maybe store the token's minimum/maximum lengths, as well as the valid starting character(s)?
            //  To perhaps optimize how the tokenizer iterates through them.
    };

    class TokenBaseUnnamedIdentifierConstructor : public TokenConstructor { // TODO: Should this be a singleton?
        public:
            TokenizeResult tokenizeHelper(BasicArray::BasicCharArray* fileData, int startPos, char identiferStartChar,
                                        std::shared_ptr<Token> (*tokenCtor)(int ID, int srcPos));
    };
}

#endif // LLVM_IR_COMPILER_TOKENBaseUnnamedIdentifier_H
