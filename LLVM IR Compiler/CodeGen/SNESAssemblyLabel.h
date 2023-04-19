#ifndef LLVM_IR_COMPILER_SNESAssemblyLabel_H
#define LLVM_IR_COMPILER_SNESAssemblyLabel_H

#include <string>

namespace SNESAssembly {
    // Represents an 65c816 ASM label.
    // Example: SNESAssemblyLabel("Vegetable") -> 'Vegetable:'
    class SNESAssemblyLabel {
        public:
            const std::string rawLabel;
            SNESAssemblyLabel(std::string rawLabelArg) : rawLabel(rawLabelArg) {}
            inline std::string getStringRepresentation() const { return convertToLabelString(rawLabel); }
            static inline std::string convertToLabelString(std::string rawLabel) { return rawLabel+":"; }
    };
    // FIXME Fix the name of this class.
}

#endif // LLVM_IR_COMPILER_SNESAssemblyLabel_H
