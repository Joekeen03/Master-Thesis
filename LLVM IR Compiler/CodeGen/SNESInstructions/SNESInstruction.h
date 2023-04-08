#ifndef LLVM_IR_COMPILER_SNESAssemblyInstruction_H
#define LLVM_IR_COMPILER_SNESAssemblyInstruction_H

#include <string>

namespace SNESAssembly {
    // Represents a single SNES-compatible 65c816 ASM instruction
    class SNESInstruction {
        protected:
            SNESInstruction() {}
        public:
            virtual ~SNESInstruction() {}
            virtual std::string getName() const { return "Default Instruction"; };
            virtual std::string getStringRepresentation() const {
                throw std::runtime_error("Subclass of SNESInstruction did not provide its own implementation of \
                getStringRepresentation(). getName() result: "+getName());
            }
    };
}

#endif // LLVM_IR_COMPILER_SNESAssemblyInstruction_H
