#ifndef LLVM_IR_COMPILER_TOKENLIBRARY_H
#define LLVM_IR_COMPILER_TOKENLIBRARY_H

#include "Token.h"

namespace TokenLibrary {
    // TODO Add a "TokenValidate" which just tries to access the current character ((*fileData)[startPos]) and throws an error if it can't.
    //      Basically, validates that the starting position is valid.
    extern std::unique_ptr<Tokens::TokenConstructor> tokenConstructors[];
    extern const int nTokens;
}

#endif // LLVM_IR_COMPILER_TOKENLIBRARY_H