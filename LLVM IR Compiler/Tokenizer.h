#ifndef LLVM_IR_COMPILER_TOKENIZER_H
#define LLVM_IR_COMPILER_TOKENIZER_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <exception>
#include <array>
#include <memory>

#include "Token.h"
#include "TokenLibrary.h"
#include "BasicCharArray.h"

namespace Tokenizer {
    class Tokenizer {
        public:
            Tokenizer() : fileData(nullptr), fileLength(-1) {};
            std::vector<Token::Token> Tokenize(std::string fileName);
        private:
            void readFileData(std::string fileName);
            BasicArray::BasicCharArray* fileData;
            int fileLength;
    };

    class TokenizationException : public std::exception {
        public:
            TokenizationException(const char * msg) : exception(msg) {}
    };
}

#endif // LLVM_IR_COMPILER_TOKENIZER_H