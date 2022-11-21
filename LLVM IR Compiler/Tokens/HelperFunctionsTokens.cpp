#include "HelperFunctionsTokens.h"

#include <cctype>

namespace TokenLib {
    bool isNamedIdentifierStartChar(char c) {
        return std::isalpha(c) || c == '-' || c == '$' || c == '.' || c == '_';
    }
}