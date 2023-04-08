#ifndef LLVM_IR_COMPILER_SNESInstructionReturn_H
#define LLVM_IR_COMPILER_SNESInstructionReturn_H

#include "SNESInstruction.h"

namespace {
    class SNESInstructionReturn : public SNESAssembly::SNESInstruction {
        public:
            SNESInstructionReturn() {}
            virtual std::string getName() const override { return "SNES Instruction: Return"; }
            virtual std::string getStringRepresentation() const override { throw std::runtime_error("Not Implemented: SNESInstructionReturn::getStringRepresentation()"); }
    };
}

namespace SNESAssembly {
    class SNESInstructionReturnFromInterrupt : public SNESInstructionReturn {
        public:
            virtual std::string getName() const override { return "SNES Instruction: ReturnFromInterrupt"; }
            virtual std::string getStringRepresentation() const override { return "RTI"; };
    };
    class SNESInstructionReturnFromSubroutine : public SNESInstructionReturn {
        public:
            virtual std::string getName() const override { return "SNES Instruction: ReturnFromSubroutine"; }
            virtual std::string getStringRepresentation() const override { return "RTS"; };
    };
    class SNESInstructionReturnFromSubroutineLong : public SNESInstructionReturn {
        public:
            virtual std::string getName() const override { return "SNES Instruction: ReturnFromSubroutineLong"; }
            virtual std::string getStringRepresentation() const override { return "RTL"; };
    };
}

#endif // LLVM_IR_COMPILER_SNESInstructionReturn_H