#ifndef LLVM_IR_COMPILER_BASICCHARARRAY_H
#define LLVM_IR_COMPILER_BASICCHARARRAY_H

#include <stdexcept>
#include <stdio.h>
#include <string>

namespace BasicArray {
    class BasicCharArray {
        public:
            BasicCharArray(char* arr, int size) : array(arr), size(size) {}
            const char& operator[](int pos) {
                if (!positionInBounds(pos)) {
                    throw new std::runtime_error("Index out of bounds for BasicCharArray.");
                }
                return array[pos];
            }
            inline bool positionInBounds(int pos) {
                return (pos >= 0 && pos < size);
            }
            // Compares the provided literal to a subsection of this BasicCharArray; subsection starts at startPos, and is the same size as the literal.
            bool compareSubsectionToLiteral(int startPos, std::string literal);
        private:
            char* array;
            const int size;
    };
}
#endif // LLVM_IR_COMPILER_BASICCHARARRAY_H