#ifndef LLVM_IR_COMPILER_TypePointer_H
#define LLVM_IR_COMPILER_TypePointer_H

#include "TypeSized.h"

namespace Types {
    enum class LayoutAddressSpace {
        program,
        globals,
        alloca,
        other
    };

    // Are pointer types this simple, or is there more to them?
    class TypePointer : public TypeSized {
        public:
            const LayoutAddressSpace layoutAddressSpace;
            const int otherAddressSpace;
            TypePointer(int otherAddressSpaceArg) : layoutAddressSpace(LayoutAddressSpace::other), otherAddressSpace(otherAddressSpaceArg) {}
            TypePointer(LayoutAddressSpace addressSpace) : layoutAddressSpace(addressSpace), otherAddressSpace(-1) {}
            virtual int getSizeBits() const override { return 16; } // FIXME Is this correct? This would only work w/in a bank.
    };
}

#endif // LLVM_IR_COMPILER_TypePointer_H