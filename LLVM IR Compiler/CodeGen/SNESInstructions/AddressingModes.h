#ifndef LLVM_IR_COMPILER_AddressingModes_H
#define LLVM_IR_COMPILER_AddressingModes_H

#include <string>

namespace {
    // Represents SNES 65c816 addressing modes
    // General format of the string representation is: prefix + to_string(operand<<stringBitShift) + suffix
    // maxBitLength is the max permitted size of the supplied operand. This will throw an error if the operand is too big.
    template <const char* prefix, const char* suffix, unsigned int maxBitLength, unsigned int stringBitShift=0>
    class AddressingMode {
        public:
            const unsigned int operand;
            AddressingMode(unsigned int operandArg) : operand(operandArg) {
                // FIXME Is it a reasonable to check the value a const member was initialized to in the constructor
                //  (i.e. after the const member was initialized), or should I have a validation method that passes
                //  the const member's value through to the const member if it's valid, or throws an error if it's not?
                //  So: AddressingMode(int operandArg) : operand(validateOperand(operandArg)) {}
                if (!isValidOperand(operandArg)) {

                    throw std::runtime_error(std::string("AddressingMode<")+prefix+std::string(", ")+suffix+std::string(",")+std::to_string(maxBitLength)
                                                +std::string("> received an operand larger than its max bit length. Operand: ")
                                                +std::to_string(operandArg));
                }
            }
            std::string getStringRepresentation() const {
                return prefix+std::to_string(operand<<stringBitShift)+suffix;
            }
            static bool isValidOperand(int operand) {
                return !(bool)(operand>>maxBitLength);
            }
    };
}

namespace AddressingModes {
    const char empty[] = "";
    const char stackRelativeSuffix[] = ", s";
    const char immediateModePrefix[] = "#";
    const char leftParen[] = "(";
    const char rightParen[] = ")";
    const char stackRelativeIndirectIndexedSuffix[] = ", s), y";

    using DirectPageIndirect = AddressingMode<leftParen, rightParen, 8>;
    using AbsoluteLong = AddressingMode<empty, empty, 24>;
    using Absolute = AddressingMode<empty, empty, 16>;
    using StackRelative = AddressingMode<empty, stackRelativeSuffix, 8>;
    using StackRelativeIndirectIndexed = AddressingMode<leftParen, stackRelativeIndirectIndexedSuffix, 8>;
    // FIXME Need an 8-bit immediate mode, for cases where an instruction only has an 8-bit immediate operand, such as
    //  STA in 8-bit accumulator mode.
    using Immediate = AddressingMode<immediateModePrefix, empty, 16>;
    using ImmediateForced8Bit = AddressingMode<immediateModePrefix, empty, 8>;
    using BlockMove = AddressingMode<empty, empty, 8, 16>;

    // Note:
    //  Direct Indirect Indexed: (*(*address))[index]
    //  Direct Indexed Indirect: *((*address)[index])
}

namespace AddressingModeGroups {
    // FIXME Is there a way to prevent specializations from being added outside this file? And is that
    //  necessary/good practice?

    // Address modes used for LDA, STA, ADC, EOR, and other commonly used instructions
    // Modes: 
    template <typename AddressMode>
    struct LDA_AddressingModes { static const bool valid = false; };
    template <>
    struct LDA_AddressingModes<AddressingModes::AbsoluteLong> {static const bool valid = true; };
    template <>
    struct LDA_AddressingModes<AddressingModes::Absolute> {static const bool valid = true; };
    template <>
    struct LDA_AddressingModes<AddressingModes::StackRelative> {static const bool valid = true; };
    template <>
    struct LDA_AddressingModes<AddressingModes::StackRelativeIndirectIndexed> {static const bool valid = true; };
    template <>
    struct LDA_AddressingModes<AddressingModes::Immediate> {static const bool valid = true; };
    template <>
    struct LDA_AddressingModes<AddressingModes::DirectPageIndirect> {static const bool valid = true; };

    // Address modes used for LDX, and possibly other instructions.
    //  Modes: Immediate; Direct Page; Absolute; DP Indexed,Y; Absolute Indexed,Y
    template <typename AddressingMode>
    struct LDX_AddressingModes { static const bool valid = false; };
    
    template <>
    struct LDX_AddressingModes<AddressingModes::Absolute> { static const bool valid = true; };
    template <>
    struct LDX_AddressingModes<AddressingModes::Immediate> { static const bool valid = true; };

    // Address modes used for LDY, and possibly other instructions.
    //  Modes: Immediate; Direct Page; Absolute; DP Indexed,X; Absolute Indexed,X
    template <typename AddressingMode>
    struct LDY_AddressingModes { static const bool valid = false; };
    
    template <>
    struct LDY_AddressingModes<AddressingModes::Absolute> { static const bool valid = true; };
    template <>
    struct LDY_AddressingModes<AddressingModes::Immediate> { static const bool valid = true; };

    // Address modes used for STZ, and possibly other instructions.
    //  Modes: Direct Page; Absolute; DP Indexed,Y; Absolute Indexed,Y
    template <typename AddressingMode>
    struct STZ_AddressingModes { static const bool valid = false; };
    
    template <>
    struct STZ_AddressingModes<AddressingModes::Absolute> { static const bool valid = true; };
}

#endif // LLVM_IR_COMPILER_AddressingModes_H
