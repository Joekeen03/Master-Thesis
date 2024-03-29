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
    using tokensArray = std::vector<std::shared_ptr<const Tokens::Token>>;
    using tokensArrayPointer = std::shared_ptr<tokensArray>;

    class Tokenizer {
        public:
            Tokenizer() : fileData(nullptr), fileLength(-1) {};
            tokensArrayPointer tokenize(std::string fileName);
        private:
            void readFileData(std::string fileName);
            BasicArray::BasicCharArray* fileData;
            int fileLength;
    };

    class TokenizationException : public std::runtime_error {
        public:
            TokenizationException(std::string msg) : runtime_error(msg.c_str()) {}
    };
}

#endif // LLVM_IR_COMPILER_TOKENIZER_H