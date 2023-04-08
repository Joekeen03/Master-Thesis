#ifndef LLVM_IR_COMPILER_SNESAssemblyLine_H
#define LLVM_IR_COMPILER_SNESAssemblyLine_H


#include <string>

namespace SNESAssembly {
    // Represents a single line of SNES-compatible 65c816 ASM
    class SNESAssemblyLine {
        protected:
            SNESAssemblyLine() {};
        public:
            virtual std::string getStringRepresentation() const { throw std::runtime_error("SNESAssemblyLine::getStringRepresentation was not overridden in a subclass."); }
    };
}

#endif // LLVM_IR_COMPILER_SNESAssemblyLine_H
