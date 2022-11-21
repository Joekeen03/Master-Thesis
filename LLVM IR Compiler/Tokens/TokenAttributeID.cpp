#include "TokenAttributeID.h"


namespace Token {
    TokenizeResult TokenAttributeIDConstructor::tokenize(BasicArray::BasicCharArray* fileData, int startPos) {
        int currPos = startPos;
        bool success = false;
        std::string attributeIDRaw = "";
        try {
            if ((*fileData)[currPos++] == '#') {
                bool buildID = true;
                do {
                    char currChar = (*fileData)[currPos++];
                    // FIXME Can attribute ID's have other characters? Do they need to be whitespace separated from other tokens (e.g. global identifiers)
                    if (std::isdigit(currChar)) {
                        success = true; // FIXME better way to handle this?
                        attributeIDRaw += currChar;
                    } else {
                        buildID = false;
                    }
                } while (buildID);
            }
        } catch (...) { // Out-of-bounds on fileData

        }
        if (success) {
            int attributeID = std::stoi(attributeIDRaw);
            return TokenizeResult(std::make_shared<TokenAttributeID>(attributeID), currPos);
        } else {
            return TokenizeResult();
        }
    }
}
