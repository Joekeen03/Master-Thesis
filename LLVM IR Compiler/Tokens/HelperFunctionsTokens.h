#ifndef LLVM_IR_COMPILER_HELPERFUNCTIONSTOKENS_H
#define LLVM_IR_COMPILER_HELPERFUNCTIONSTOKENS_H

#include <utility>

#include "../BasicCharArray.h"

namespace TokenLib{
    using nextPosition = int;
    using numberLiteral = int;
    using numberParseResult = std::pair<numberLiteral, nextPosition>;
    using stringParseResult = std::pair<std::string, nextPosition>;

    extern const numberParseResult numberParseFAILED;
    extern const stringParseResult stringParseFAILED;

    bool isNamedIdentifierStartChar(char c);

    // Attempts to extract a number literal from fileData, starting at startPos.
    // Returns the number literal and the position immediately after it if successful; returns -1 for the position otherwise.
    // Only fails if there was no number starting at the specified position.
    numberParseResult extractNumber(BasicArray::BasicCharArray *fileData, int startPos);
    
    // Attempts to extract a double-quoted string from fileData, starting at startPos.
    // Returns the string and the position immediately after it if successful; returns -1 for the position otherwise.
    // Fails if there was no string starting at the specified position, or if the string was improperly
    //   terminated (no '"', or a newline before the ending '"')
    stringParseResult extractQuotedString(BasicArray::BasicCharArray *fileData, int startPos);
}

#endif // LLVM_IR_COMPILER_HELPERFUNCTIONSTOKENS_H