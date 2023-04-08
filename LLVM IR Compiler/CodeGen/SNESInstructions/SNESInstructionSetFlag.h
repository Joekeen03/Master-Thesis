#ifndef LLVM_IR_COMPILER_SNESInstructionSetFlag_H
#define LLVM_IR_COMPILER_SNESInstructionSetFlag_H

#include "SNESInstruction.h"

namespace {
    class SNESInstructionSetFlag : public SNESAssembly::SNESInstruction {
        public:
            SNESInstructionSetFlag() {}
            virtual std::string getName() const { return "SNES Instruction: SetFlag"; }
            virtual std::string getStringRepresentation() const { throw std::runtime_error("Not Implemented: SNESInstructionSetFlag::getStringRepresentation()"); }
    };
}

namespace SNESAssembly {
    class SNESInstructionSetCarryFlag : public SNESInstructionSetFlag {
        public: virtual std::string getStringRepresentation() const override { return "SEC"; }
    };

    class SNESInstructionSetDecimalFlag : public SNESInstructionSetCarryFlag {
        public: virtual std::string getStringRepresentation() const override { return "SED"; }
    };

    class SNESInstructionSetInterruptFlag : public SNESInstructionSetFlag {
        public: virtual std::string getStringRepresentation() const override { return "SEI"; }
    };
}

#endif // LLVM_IR_COMPILER_SNESInstructionSetFlag_H
