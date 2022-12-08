#ifndef LLVM_IR_COMPILER_TOKEN_H
#define LLVM_IR_COMPILER_TOKEN_H

#include <memory>
#include <iostream>

#include "../BasicCharArray.h" // FIXME Properly set up include path
#include "../lib/Result.h"

namespace Tokens {
    class Token;
    using TokenizeResult = Lib::ResultPointer<Token>;

    class Token {
        public:
            const int srcPos;
            explicit Token(int srcPosArg) : srcPos(srcPosArg) {}
            virtual std::string getName() const { return "Default Token"; }
            std::string getNameAndPos() const { return getName() + " @ " + std::to_string(srcPos); }
        // Maybe store the token's minimum/maximum lengths, as well as the valid starting character(s)?
        //  To perhaps optimize how the tokenizer iterates through them.
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