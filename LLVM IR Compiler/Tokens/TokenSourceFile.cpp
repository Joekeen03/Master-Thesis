#include "TokenSourceFile.h"

namespace {
    std::string sourceFileLiteral = "source_filename";
}

namespace Token {
    TokenizeResult TokenSourceFileConstructor::tokenize(BasicArray::BasicCharArray* fileData, int startPos) {
        return fileData->compareSubsectionToLiteral(startPos, sourceFileLiteral)
                ? TokenizeResult(TokenSourceFile(), startPos+sourceFileLiteral.size()) : TokenizeResult();
    }
}
