#include "TokenEOF.h"


namespace Tokens {
    std::shared_ptr<TokenEOF> TokenEOFConstructor::createEOFToken(BasicArray::BasicCharArray* fileData, int finalPos) const {
        if (finalPos != fileData->size) {
            throw new std::runtime_error("EOF token position must be at the end of the source file.");
        }
        return std::make_shared<TokenEOF>(finalPos);
    }
}
