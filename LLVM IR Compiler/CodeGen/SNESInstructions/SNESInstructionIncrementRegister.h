#ifndef LLVM_IR_COMPILER_SNESInstructionIncrementRegister_H
#define LLVM_IR_COMPILER_SNESInstructionIncrementRegister_H

#include "SNESInstruction.h"

namespace {
    class SNESInstructionIncrementRegister : public SNESAssembly::SNESInstruction {
        public:
            SNESInstructionIncrementRegister() {}
            virtual std::string getName() const { return "SNES Instruction: IncrementRegister"; }
            virtual std::string getStringRepresentation() const { throw std::runtime_error("Not Implemented: SNESInstructionIncrementRegister::getStringRepresentation()"); }
    };
}

namespace SNESAssembly {
    class SNESInstructionIncrementAccumulator : public SNESInstructionIncrementRegister {
        public: virtual std::string getStringRepresentation() const override { return "INC A"; }
    };
    class SNESInstructionIncrementIndexRegisterX : public SNESInstructionIncrementRegister {
        public: virtual std::string getStringRepresentation() const override { return "INX"; }
    };
    class SNESInstructionIncrementIndexRegisterY : public SNESInstructionIncrementRegister {
        public: virtual std::string getStringRepresentation() const override { return "INY"; }
    };
}

#endif // LLVM_IR_COMPILER_SNESInstructionIncrementRegister_H
