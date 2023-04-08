#ifndef LLVM_IR_COMPILER_SNESAssemblyLabel_H
#define LLVM_IR_COMPILER_SNESAssemblyLabel_H

#include <string>

namespace SNESAssembly {
    // Represents an 65c816 ASM label.
    // Example: SNESAssemblyLabel("Vegetable") -> 'Vegetable:'
    class SNESAssemblyLabel {
        public:
            const std::string label;
            SNESAssemblyLabel(std::string labelArg) : label(labelArg) {}
            inline std::string getStringRepresentation() const { return label+":"; }
    };
}

#endif // LLVM_IR_COMPILER_SNESAssemblyLabel_H
