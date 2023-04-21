#ifndef LLVM_IR_COMPILER_LocalsParsingInfo_H
#define LLVM_IR_COMPILER_LocalsParsingInfo_H

#include <set>
#include <memory>

namespace Parser {
    class LocalsParsingInfo {
        unsigned int nextUnnamedLocalIdentifier;
        
        public:
            LocalsParsingInfo() : nextUnnamedLocalIdentifier(0) {}
            // Fetches the next unnamed local identifier in the sequence, and advances the unnamed local
            //  identifier counter
            unsigned int getNewUnnamedLocalIdentifier() {
                return nextUnnamedLocalIdentifier++;
            }
            // Fetches the next unnamed local identifier in the sequence, and does not advance the unnamed
            //  local identifier counter
            unsigned int peekNewUnnamedLocalIdentifier() {
                return nextUnnamedLocalIdentifier;
            }
            // Checks that the provided unnamed local identifier is the next in the sequence; if it is, increments the
            //  unnamed local identifier counter and returns true; otherwise, returns false;
            bool validateAndAddNewUnnamedLocalIdentifier(unsigned int newLocalIdentifier) {
                if (newLocalIdentifier == nextUnnamedLocalIdentifier) {
                    nextUnnamedLocalIdentifier++;
                    return true;
                } else {
                    return false;
                }
            }
    };
}

#endif // LLVM_IR_COMPILER_LocalsParsingInfo_H
