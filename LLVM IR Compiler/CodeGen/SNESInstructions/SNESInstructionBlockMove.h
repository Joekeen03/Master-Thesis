#ifndef LLVM_IR_COMPILER_SNESInstructionBlockMove_H
#define LLVM_IR_COMPILER_SNESInstructionBlockMove_H

#include "SNESInstruction.h"
#include "AddressingModes.h"

namespace {
    template <const char* opcode, const char* name>
    class SNESInstructionBlockMove : public SNESAssembly::SNESInstruction {
        public:
            const AddressingModes::BlockMove sourceBank, destBank;
            SNESInstructionBlockMove(int sourceBankArg, int destBankArg) : sourceBank(sourceBankArg), destBank(destBankArg) {}
            virtual std::string getStringRepresentation() const {
                return getOpcode()+" "+sourceBank.getStringRepresentation()+","+destBank.getStringRepresentation();
            }

            virtual std::string getOpcode() const { throw std::runtime_error("No opcode specified."); }
    };
}

namespace {
    constexpr char MVP_Name[] = "BlockMovePositive";
    constexpr char MVP_Opcode[] = "MVP";
    constexpr char MVN_Name[] = "BlockMoveNegative";
    constexpr char MVN_Opcode[] = "MVN";
}

namespace SNESAssembly {
    using SNESInstructionBlockMovePositive = SNESInstructionBlockMove<MVP_Name, MVP_Opcode>;
    using SNESInstructionBlockMoveNegative = SNESInstructionBlockMove<MVN_Name, MVN_Opcode>;
}

#endif // LLVM_IR_COMPILER_SNESInstructionBlockMove_H
