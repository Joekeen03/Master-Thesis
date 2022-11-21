#ifndef LLVM_IR_COMPILER_TOKENIZER_H
#define LLVM_IR_COMPILER_TOKENIZER_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <array>
#include <memory>

#include "Tokens/Token.h"
#include "Tokens/LibraryTokens.h"
#include "BasicCharArray.h"

namespace Tokenizer {
    class Tokenizer {
        public:
            Tokenizer() : fileData(nullptr), fileLength(-1) {};
            std::vector<std::shared_ptr<const Token::Token>> Tokenize(std::string fileName);
        private:
            void readFileData(std::string fileName);
            BasicArray::BasicCharArray* fileData;
            int fileLength;
    };

    class TokenizationException : public std::runtime_error {
        public:
            TokenizationException(const char * msg) : runtime_error(msg) {}
    };
}

#endif // LLVM_IR_COMPILER_TOKENIZER_H