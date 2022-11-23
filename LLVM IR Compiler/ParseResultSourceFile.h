#ifndef LLVM_IR_COMPILER_PARSERESULTSOURCEFILE_H
#define LLVM_IR_COMPILER_PARSERESULTSOURCEFILE_H

#include <string>
#include <utility>

#include "Tokenizer.h"

namespace ParseResult {
    struct ParsingResult;

    class ParseResultSourceFile {
        public:
            const std::string fileName;
            ParseResultSourceFile(std::string fileNameArg) : fileName(fileNameArg) {};
            static ParsingResult attemptToParse(std::shared_ptr<Tokenizer::tokensArray> tokens, int startPos);
    };
    
    using newTokenPos = int;
    struct ParsingResult {
        const std::shared_ptr<ParseResultSourceFile> result;
        const newTokenPos newPos;
        const bool success;
        ParsingResult() : result(), newPos(-1), success(false) {}
        ParsingResult(std::shared_ptr<ParseResultSourceFile> resultArg, newTokenPos newPosArg)
                        : result(resultArg), newPos(newPosArg), success(true) {}
    };
}

#endif // LLVM_IR_COMPILER_PARSERESULTSOURCEFILE_H