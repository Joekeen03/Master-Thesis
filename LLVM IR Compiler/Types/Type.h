#ifndef LLVM_IR_COMPILER_TYPE_H
#define LLVM_IR_COMPILER_TYPE_H

namespace Types {
    class Type {
        public:
            // FIXME Should these getSize methods be moved into a separate, target-specific Type class? I.e. use this class and
            //  sub-classes for general parsing, then transform them to the target-specific Type classes for code generation.
            //  Mostly not sure if some more complex types (e.g. structs) can vary in their size, depending on the target (how
            //  bits are packed).

            // Exact size of the type in bits.
            virtual int getSizeBits() const { throw std::runtime_error("Type::getSizeBits was not implemented in a subclass."); };

            // Size of the type in bytes, rounded up to the nearest byte. 
            virtual int getSizeBytesCeil() const {
                int bits = getSizeBits();
                // This is equivalent to ceil(bits/8.0), but w/o intermediate floating-point arithmetic.
                //  Works b/c bit-shifting to the right 3 places is equivalent to dividing by 8, rounding down (converting to
                //  bytes). Adding 7 (0b111) means anything between two multiples of 8 (a byte) will be brought up over the
                //  higher multiple multiple of 8, and thereby rounded down; anything which is exactly of a multiple of 8 will
                //  still be less than the next multiple of 8, and therefore rounded down to that same multiple of 8.
                return (bits+0b111)>>3;
            };
    };
}

#endif // LLVM_IR_COMPILER_TYPE_H
