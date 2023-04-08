#ifndef LLVM_IR_COMPILER_SNESInstructionExchangeCarryWithEmulation_H
#define LLVM_IR_COMPILER_SNESInstructionExchangeCarryWithEmulation_H

#include "SNESInstruction.h"

namespace SNESAssembly {
    class SNESInstructionExchangeCarryWithEmulation : public SNESInstruction {
        public:
            SNESInstructionExchangeCarryWithEmulation() {}
            virtual std::string getName() const { return "SNES Instruction: ExchangeCarryWithEmulation"; }
            virtual std::string getStringRepresentation() const { return "XCE"; }
    };
}

#endif // LLVM_IR_COMPILER_SNESInstructionExchangeCarryWithEmulation_H
