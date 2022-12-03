#include "HelperFunctionsTokens.h"

#include <cctype>
#include <iostream>

namespace TokenLib {
    const numberParseResult numberParseFAILED(0, -1);
    const stringParseResult stringParseFAILED("", -1);

    bool isNamedIdentifierStartChar(char c) {
        return std::isalpha(c) || c == '-' || c == '$' || c == '.' || c == '_';
    }

    numberParseResult extractNumber(BasicArray::BasicCharArray *fileData, int startPos) {
        // FIXME is it safe to assume it's a valid token regardless of what character terminated the tokenization? I.e. would encountering
        //  an 'a' immediately after the identifier not invalidate the tokenization itself?\
        result = TokenizeResult(TokenGlobalIdentifier(identifier), currPos);
        std::string numberRaw = "";
        bool success = false;
        int currPos = startPos;
        int firstCharAfterNumber = currPos;
        try {
            bool tokenize = true;
            char currChar;
            do {
                currChar = (*fileData)[currPos];
                if (std::isdigit(currChar)){
                    numberRaw += currChar;
                    currPos++;
                } else {
                    tokenize = false;
                    firstCharAfterNumber = currPos;
                }
            } while (tokenize);
            success = numberRaw.size() > 0;
        } catch (...) { // Out of bounds on array.

        }
        return success ? numberParseResult((numberLiteral)std::stoi(numberRaw), (nextPosition)firstCharAfterNumber)
                            : numberParseFAILED;
    }

    numberParseResult extractPrefixedNumber(BasicArray::BasicCharArray *fileData, int startPos, char prefix) {
        numberParseResult result = numberParseFAILED;
        if (fileData->positionInBounds(startPos) && (*fileData)[startPos] == prefix) {
            result = TokenLib::extractNumber(fileData, startPos+1);
        }
        return result;
    }

    stringParseResult extractQuotedString(BasicArray::BasicCharArray *fileData, int startPos) {
        int currPos = startPos;
        int nextPosAfterString;
        bool success = false;
        std::string str = "";
        try {
            if ((*fileData)[currPos++] == '"') { // Start of a string - I think all strings are double-quoted?
                bool buildString = true;
                do {
                    char currChar = (*fileData)[currPos];
                    if (currChar == '\n') { // Strings can't span multiple lines
                        buildString = false;
                    } else if (currChar == '"') { // String end
                        nextPosAfterString = currPos+1;
                        buildString = false;
                        success = true;
                    } else {
                        str += currChar;
                        currPos++;
                    }
                } while (buildString);
            }
        } catch (...) { // Out of bounds on array

        }
        return success ? stringParseResult(str, nextPosAfterString) : stringParseFAILED;
    }
}