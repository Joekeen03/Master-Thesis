#ifndef LLVM_IR_COMPILER_BASICCHARARRAY_H
#define LLVM_IR_COMPILER_BASICCHARARRAY_H

#include <exception>
#include <stdio.h>

namespace BasicArray {
    struct BasicCharArray {
        public:
            BasicCharArray(char* arr, int size) : array(arr), size(size) {}
            const char& operator[](int pos) {
                if (pos < 0 || pos > size) {
                    throw new std::exception("Index out of bounds for BasicCharArray.");
                }
                return array[pos];
            }
        private:
            char* array;
            const int size;
    };
}
#endif // LLVM_IR_COMPILER_BASICCHARARRAY_H