#ifndef LLVM_IR_COMPILER_TOKEN_H
#define LLVM_IR_COMPILER_TOKEN_H

#include <memory>
#include <iostream>

#include "../BasicCharArray.h" // FIXME Properly set up include path

namespace Token {
    class Token {
        public:
            const int srcPos;
            explicit Token(int srcPosArg) : srcPos(srcPosArg) {}
            virtual std::string getName() const { return "Default Token"; }
            std::string getNameAndPos() const { return getName() + " @ " + std::to_string(srcPos); }
        // Maybe store the token's minimum/maximum lengths, as well as the valid starting character(s)?
        //  To perhaps optimize how the tokenizer iterates through them.
    };

    struct TokenizeResult {
        public:
            TokenizeResult() : token(), newPos(-1), success(false) {}
            TokenizeResult(std::shared_ptr<const Token> t, int p) : token(t), newPos(p), success(true) {}
            const std::shared_ptr<const Token> token; // FIXME Is this actually equivalent to Token * const p (pointer that can't point to anything else)?
            const int newPos;
            const bool success;

    };

    class TokenConstructor { // TODO: Should this be a singleton?
        public:
            virtual TokenizeResult tokenize(BasicArray::BasicCharArray* fileData, int startPos) {
                std::cout << "Default tokenize method called.\n";
                return TokenizeResult();
            }
    };
}

#endif // LLVM_IR_COMPILER_TOKEN_H