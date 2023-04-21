#include "HelperFunctionsTokens.h"

#include <cctype>
#include <iostream>

namespace TokenLib {
    const unsignedIntegerParseResult unsignedIntegerParseFAILED = unsignedIntegerParseResult();
    const signedIntegerParseResult signedIntegerParseFAILED = signedIntegerParseResult();
    const stringParseResult stringParseFAILED = stringParseResult();

    bool isNamedIdentifierStartChar(char c) {
        return std::isalpha(c) || c == '-' || c == '$' || c == '.' || c == '_';
    }

    unsignedIntegerParseResult extractUnsignedInteger(BasicArray::BasicCharArray *fileData, int startPos) {
        // FIXME is it safe to assume it's a valid token regardless of what character terminated the tokenization? I.e. would encountering
        //  an 'a' immediately after the identifier not invalidate the tokenization itself?\
        result = TokenizeResult(TokenGlobalIdentifier(identifier), currPos);
        std::string numberRaw = "";
        bool success = false;
        int currPos = startPos;
        unsigned int firstCharAfterNumber = currPos;
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
        return success ? unsignedIntegerParseResult((unsigned int)std::stoi(numberRaw), firstCharAfterNumber)
                            : unsignedIntegerParseFAILED;
    }

    signedIntegerParseResult extractSignedInteger(BasicArray::BasicCharArray *fileData, int startPos) {
        // FIXME is it safe to assume it's a valid token regardless of what character terminated the tokenization? I.e. would encountering
        //  an 'a' immediately after the identifier not invalidate the tokenization itself?\
        result = TokenizeResult(TokenGlobalIdentifier(identifier), currPos);
        bool success = false;
        int currPos = startPos;
        bool sign = 1;
        auto unsignedResult = unsignedIntegerParseFAILED;
        try {
            char currChar = (*fileData)[currPos];
            if (currChar == '-') {
                sign = -1;
                currPos++;
            }
            unsignedResult = extractUnsignedInteger(fileData, currPos);
            success = unsignedResult.success;
        } catch (...) { // Out of bounds on array.

        }
        return success ? signedIntegerParseResult{sign*((int)unsignedResult.result), unsignedResult.newPosition}
                            : signedIntegerParseFAILED;
    }

    unsignedIntegerParseResult extractPrefixedNumber(BasicArray::BasicCharArray *fileData, int startPos, char prefix) {
        unsignedIntegerParseResult result = unsignedIntegerParseFAILED;
        if (fileData->positionInBounds(startPos) && (*fileData)[startPos] == prefix) {
            result = TokenLib::extractUnsignedInteger(fileData, startPos+1);
        }
        return result;
    }

    stringParseResult extractQuotedString(BasicArray::BasicCharArray *fileData, int startPos) {
        unsigned int currPos = startPos;
        unsigned int nextPosAfterString;
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
        return success ? stringParseResult(str, (int)nextPosAfterString) : stringParseFAILED;
    }

    Tokens::TokenizeResult lexUnnamedIdentifier(BasicArray::BasicCharArray* fileData, int startPos, char identiferStartChar,
                                                        std::shared_ptr<Tokens::Token> (*tokenCtor)(int ID, int srcPos)) {
        auto unsignedIntegerResult = extractPrefixedNumber(fileData, startPos, identiferStartChar);
        return unsignedIntegerResult.newPosition != -1 ? Tokens::TokenizeResult(tokenCtor(unsignedIntegerResult.result, startPos), unsignedIntegerResult.newPosition)
                                                        : Tokens::TokenizeResult();
    }

    Tokens::TokenizeResult lexNamedIdentifier(BasicArray::BasicCharArray* fileData, int startPos, char identiferStartChar,
                                        std::shared_ptr<Tokens::Token> (*tokenCtor)(std::string, int)) {
        int currPos = startPos;
        int firstCharAfterToken = currPos;
        bool succeeded = false;
        std::string identifier = "";
        /**
         * FIXME: What I'd like to do is have a "result" variable which is initialized to some "failed" value (e.g. TokenizeResult result;),
         *          and then if parsing succeeds, assign a "succcesful" value to it, like I would in Python or Java - except, I don't want to
         *          use pointers for this. Unfortunately, it seems that b/c TokenizeResult has const members (and an abstract data type for one
         *          of them), it doesn't work?
        */
        try {
            if ((*fileData)[currPos] == identiferStartChar) {
                currPos++;
                if ((*fileData)[currPos] == '"') { // Double-quoted named identifier
                    TokenLib::stringParseResult stringResult = TokenLib::extractQuotedString(fileData, currPos);
                    if (stringResult.newPosition != -1) {
                        identifier = stringResult.result;
                        firstCharAfterToken = stringResult.newPosition;
                        succeeded = true;
                    }
                } else { // Unquoted identifier
                    char currChar = (*fileData)[currPos];
                    if (TokenLib::isNamedIdentifierStartChar(currChar)) { // Named identifier?
                        identifier += currChar;
                        currPos++;
                        firstCharAfterToken = currPos;
                        bool tokenize = true;
                        while (tokenize) {
                            currChar = (*fileData)[currPos];
                            if (TokenLib::isNamedIdentifierStartChar(currChar) || std::isdigit(currChar)) {
                                identifier += currChar;
                                currPos++;
                                /** FIXME: Better way to handle this than selectively incrementing currPos, depending on
                                 *          whether or not the most recent character was part of the string? Just, the main reason
                                 *          I'm doing this is so, when I return a successful token, currPos is the location of the
                                 *          first character after the keyword string. But, this feels a bit unclear.
                                */
                            } else {
                                tokenize = false;
                                firstCharAfterToken = currPos;
                            }
                        }
                        succeeded = true;
                    }
                }
            }
        } catch (...) { // Went out of bounds on the array
        
        }
        return succeeded ? Tokens::TokenizeResult(tokenCtor(identifier, startPos), firstCharAfterToken) : Tokens::TokenizeResult();
    }

}