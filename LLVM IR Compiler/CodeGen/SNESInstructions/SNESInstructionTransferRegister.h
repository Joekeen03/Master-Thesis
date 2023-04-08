#ifndef LLVM_IR_COMPILER_SNESInstructionTransferRegister_H
#define LLVM_IR_COMPILER_SNESInstructionTransferRegister_H

#include "SNESInstruction.h"

namespace SNESAssembly {

    
}

namespace {
    constexpr char opcodeTransferAccToX[] = "TAX";
    constexpr char opcodeTransferAccToY[] = "TAY";
    constexpr char opcodeTransferAccToDP[] = "TCD";
    constexpr char opcodeTransferAccToStack[] = "TCS";
    constexpr char opcodeTransferDPToAcc[] = "TDC";
    constexpr char opcodeTransferStackToAcc[] = "TSC";
    constexpr char opcodeTransferStackToX[] = "TSX";
    constexpr char opcodeTransferXToAcc[] = "TXA";
    constexpr char opcodeTransferXToStack[] = "TXS";
    constexpr char opcodeTransferXToY[] = "TXY";
    constexpr char opcodeTransferYToAcc[] = "TYA";
    constexpr char opcodeTransferYToX[] = "TYX";
}

namespace SNESAssembly {
    template <const char* opcode>
    class SNESInstructionTransferRegister : public SNESInstruction {
        public:
            SNESInstructionTransferRegister() {}
            virtual std::string getName() const { return "SNES Instruction: TransferRegister<"+std::string(opcode)+">"; }
            virtual std::string getStringRepresentation() const { return opcode; }
    };
    
    using SNESInstructionTransferAccumulatorToX = SNESInstructionTransferRegister<opcodeTransferAccToX>;
    using SNESInstructionTransferAccumulatorToY = SNESInstructionTransferRegister<opcodeTransferAccToY>;
    using SNESInstructionTransferAccumulatorToDirectPage = SNESInstructionTransferRegister<opcodeTransferAccToDP>;
    using SNESInstructionTransferAccumulatorToStack = SNESInstructionTransferRegister<opcodeTransferAccToStack>;
    using SNESInstructionTransferDirectPageToAccumulator = SNESInstructionTransferRegister<opcodeTransferDPToAcc>;
    using SNESInstructionTransferStackToAccumulator = SNESInstructionTransferRegister<opcodeTransferStackToAcc>;
    using SNESInstructionTransferStackToX = SNESInstructionTransferRegister<opcodeTransferStackToX>;
    using SNESInstructionTransferXToAccumulator = SNESInstructionTransferRegister<opcodeTransferXToAcc>;
    using SNESInstructionTransferXToStack = SNESInstructionTransferRegister<opcodeTransferXToStack>;
    using SNESInstructionTransferXToY = SNESInstructionTransferRegister<opcodeTransferXToY>;
    using SNESInstructionTransferYToAccumulator = SNESInstructionTransferRegister<opcodeTransferYToAcc>;
    using SNESInstructionTransferYToX = SNESInstructionTransferRegister<opcodeTransferYToX>;
}

#endif // LLVM_IR_COMPILER_SNESInstructionTransferRegister_H