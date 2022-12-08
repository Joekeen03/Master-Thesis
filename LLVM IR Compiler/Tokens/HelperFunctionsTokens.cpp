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

    Token::TokenizeResult lexUnnamedIdentifier(BasicArray::BasicCharArray* fileData, int startPos, char identiferStartChar,
                                                        std::shared_ptr<Token::Token> (*tokenCtor)(int ID, int srcPos)) {
        numberParseResult result = extractPrefixedNumber(fileData, startPos, identiferStartChar);
        return result != TokenLib::numberParseFAILED ? Token::TokenizeResult(tokenCtor(result.first, startPos), result.second)
                                                        : Token::TokenizeResult();
    }

    Token::TokenizeResult lexNamedIdentifier(BasicArray::BasicCharArray* fileData, int startPos, char identiferStartChar,
                                        std::shared_ptr<Token::Token> (*tokenCtor)(std::string, int)) {
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
                    TokenLib::stringParseResult result = TokenLib::extractQuotedString(fileData, currPos);
                    if (result.second != -1) {
                        identifier = result.first;
                        firstCharAfterToken = result.second;
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
        return succeeded ? Token::TokenizeResult(tokenCtor(identifier, startPos), firstCharAfterToken) : Token::TokenizeResult();
    }

}