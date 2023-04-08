#ifndef LLVM_IR_COMPILER_SNESInstructionClearFlag_H
#define LLVM_IR_COMPILER_SNESInstructionClearFlag_H

#include "SNESInstruction.h"

namespace {
    class SNESInstructionClearFlag : public SNESAssembly::SNESInstruction {
        public:
            SNESInstructionClearFlag() {}
            virtual std::string getName() const { return "SNES Instruction: ClearFlag"; }
            virtual std::string getStringRepresentation() const { throw std::runtime_error("Not Implemented: SNESInstructionClearFlag::getStringRepresentation()"); }
    };
}
namespace SNESAssembly {

    // Think this actually works a bit better than using a template, at least for this.
    class SNESInstructionClearCarryFlag : public SNESInstructionClearFlag {
        public: 
            virtual std::string getStringRepresentation() const override { return "CLC"; }
    };

    class SNESInstructionClearDecimalFlag : public SNESInstructionClearFlag {
        public:
            virtual std::string getStringRepresentation() const override { return "CLD";}
    };

    class SNESInstructionClearInterruptFlag : public SNESInstructionClearFlag {
        public: virtual std::string getStringRepresentation() const override { return "CLI";}
    };

    class SNESInstructionClearOverflowFlag : public SNESInstructionClearFlag {
        public: virtual std::string getStringRepresentation() const override { return "CLV";}
    };
}

#endif // LLVM_IR_COMPILER_SNESInstructionClearFlag_H
