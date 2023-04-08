#ifndef LLVM_IR_COMPILER_TypeInteger_H
#define LLVM_IR_COMPILER_TypeInteger_H

#include "TypeSized.h"

namespace Types {
    class TypeInteger : public TypeSized {
        public:
            const int bits;
            TypeInteger(int bitsArg) : bits(bitsArg) {}
            virtual int getSizeBits() const override { return bits; }
    };
}

#endif // LLVM_IR_COMPILER_TypeInteger_H
