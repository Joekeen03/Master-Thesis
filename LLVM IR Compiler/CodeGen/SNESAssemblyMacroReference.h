#ifndef LLVM_IR_COMPILER_SNESAssemblyMacroReference_H
#define LLVM_IR_COMPILER_SNESAssemblyMacroReference_H

#include <string>

namespace SNESAssembly {
    class SNESAssemblyMacroReference {
        public:
            const std::string macroName;
            SNESAssemblyMacroReference(std::string macroNameArg) : macroName(macroNameArg) {}
            inline std::string getStringRepresentation() const { return macroName; }
    };
}

#endif // LLVM_IR_COMPILER_SNESAssemblyMacroReference_H
