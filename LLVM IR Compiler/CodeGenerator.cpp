#include "CodeGenerator.h"

#include "Lib/General.h"

#include "CodeGen/SNESAssemblyModule.h"
#include "CodeGen/SNESAssemblyFunction.h"
#include "CodeGen/SNESAssemblyCodeBlock.h"
#include "CodeGen/SNESAssemblyLineInstruction.h"
#include "CodeGen/SNESAssemblySegmentInstructionChunk.h"
#include "CodeGen/NameMangling.h"
#include "CodeGen/FunctionInfo.h"
#include "CodeGen/ProcessorFlags.h"
#include "CodeGen/VariousConstants.h"

#include "Expressions/Instructions/InstructionAlloca.h"
#include "Expressions/Instructions/InstructionStore.h"
#include "Expressions/Instructions/InstructionRetValue.h"
#include "Expressions/ExpressionIdentifier.h"
#include "Expressions/ExpressionLocalIdentifier.h"
#include "Expressions/ExpressionLocalNamedIdentifier.h"
#include "Expressions/ExpressionLocalUnnamedIdentifier.h"
#include "Expressions/Instructions/YieldsValue.h"

#include "CodeGen/SNESInstructions/AddressingModes.h"
#include "CodeGen/SNESInstructions/SNESInstructionPushRegister.h"
#include "CodeGen/SNESInstructions/SNESInstructionPopRegister.h"
#include "CodeGen/SNESInstructions/SNESInstructionPushEffectiveIndirectAddress.h"
#include "CodeGen/SNESInstructions/SNESInstructionAddWithCarry.h"
#include "CodeGen/SNESInstructions/SNESInstructionLoadAccumulatorFromMemory.h"
#include "CodeGen/SNESInstructions/SNESInstructionSubtractWithBorrow.h"
#include "CodeGen/SNESInstructions/SNESInstructionStoreAccumulatorToMemory.h"
#include "CodeGen/SNESInstructions/SNESInstructionLoadXRegisterFromMemory.h"
#include "CodeGen/SNESInstructions/SNESInstructionLoadYRegisterFromMemory.h"
#include "CodeGen/SNESInstructions/SNESInstructionTransferRegister.h"
#include "CodeGen/SNESInstructions/SNESInstructionClearFlag.h"
#include "CodeGen/SNESInstructions/SNESInstructionBlockMove.h"
#include "CodeGen/SNESInstructions/SNESInstructionIncrementRegister.h"
#include "CodeGen/SNESInstructions/SNESInstructionSetFlag.h"
#include "CodeGen/SNESInstructions/SNESInstructionExchangeCarryWithEmulation.h"
#include "CodeGen/SNESInstructions/SNESInstructionChangeProcessorStatus.h"
#include "CodeGen/SNESInstructions/SNESInstructionReturn.h"
#include "CodeGen/SNESInstructions/SNESInstructionJump.h"
#include "CodeGen/SNESInstructions/SNESInstructionJumpToSubroutine.h"

namespace {
    void generateCopyIntegerToLocalInstructions(std::vector<const std::shared_ptr<const SNESAssembly::SNESInstruction>> &destinationVector,
                                                const int literal, const int destinationLocalOffset, const int bytesToTransfer) {
        unsigned int bytesTransferred = 0;
        for (; bytesTransferred < (bytesToTransfer-1); bytesTransferred+=2) {
            int nextBytes = (literal>>(bytesTransferred*8))&0xFFFF;
            destinationVector.push_back(std::make_shared<const SNESAssembly::SNESInstructionLoadAccumulatorFromMemory<AddressingModes::Immediate>>(nextBytes));
            destinationVector.push_back(std::make_shared<const SNESAssembly::SNESInstructionStoreAccumulatorToMemory<AddressingModes::StackRelative>>(destinationLocalOffset+bytesTransferred));
        }
        if (bytesToTransfer%1) {
            // Switch to 8-bit accumulator
            destinationVector.push_back(std::make_shared<SNESAssembly::SNESInstructionSetProcessorStatusBits>(CodeGen::accumulatorModeFlag));

            // Transfer 1 byte.
            int nextByte = (literal>>(bytesTransferred*8))&0xFF;
            destinationVector.push_back(std::make_shared<const SNESAssembly::SNESInstructionLoadAccumulatorFromMemory<AddressingModes::Immediate>>(nextByte));
            destinationVector.push_back(std::make_shared<const SNESAssembly::SNESInstructionStoreAccumulatorToMemory<AddressingModes::StackRelative>>(destinationLocalOffset+bytesTransferred));

            // Switch to 16-bit accumulator
            destinationVector.push_back(std::make_shared<SNESAssembly::SNESInstructionResetProcessorStatusBits>(CodeGen::accumulatorModeFlag));
        }
    }

    void generateAddIntegertoLocalInstructions(std::vector<const std::shared_ptr<const SNESAssembly::SNESInstruction>> &destinationVector,
                                                const int literal, const int sourceLocalOffset, const int destinationLocalOffset, const int bytesToTransfer) {
        unsigned int bytesTransferred = 0;
        destinationVector.push_back(std::make_shared<const SNESAssembly::SNESInstructionClearCarryFlag>());
        for (; bytesTransferred < (bytesToTransfer-1); bytesTransferred+=2) {
            int nextImmediateBytes = (literal>>(bytesTransferred*8))&0xFFFF;
            destinationVector.push_back(std::make_shared<SNESAssembly::SNESInstructionLoadAccumulatorFromMemory<AddressingModes::StackRelative>>(sourceLocalOffset+bytesTransferred));
            destinationVector.push_back(std::make_shared<SNESAssembly::SNESInstructionAddWithCarry<AddressingModes::Immediate>>(nextImmediateBytes));
            destinationVector.push_back(std::make_shared<SNESAssembly::SNESInstructionStoreAccumulatorToMemory<AddressingModes::StackRelative>>(destinationLocalOffset+bytesTransferred));
        }
        if (bytesTransferred%1) {
            int nextImmediateByte = (literal>>(bytesTransferred*8))&0xFFFF;
            destinationVector.push_back(std::make_shared<SNESAssembly::SNESInstructionSetProcessorStatusBits>(CodeGen::accumulatorModeFlag));
            destinationVector.push_back(std::make_shared<SNESAssembly::SNESInstructionLoadAccumulatorFromMemory<AddressingModes::StackRelative>>(sourceLocalOffset+bytesTransferred));
            destinationVector.push_back(std::make_shared<SNESAssembly::SNESInstructionAddWithCarry<AddressingModes::Immediate>>(nextImmediateByte));
            destinationVector.push_back(std::make_shared<SNESAssembly::SNESInstructionStoreAccumulatorToMemory<AddressingModes::StackRelative>>(destinationLocalOffset+bytesTransferred));
            destinationVector.push_back(std::make_shared<SNESAssembly::SNESInstructionResetProcessorStatusBits>(CodeGen::accumulatorModeFlag));
        }
    }
}

namespace CodeGen {
    const std::string MAIN = "main";

    // CodeGenerator::convertNonTerminatorInstruction specializations
    
    template <>
    std::shared_ptr<SNESAssembly::SNESAssemblySegmentInstructionChunk> CodeGenerator::convertNonTerminatorInstruction (
        std::monostate nonInstruction, std::shared_ptr<const DefinedFunctionInfo> definedFunctionInfo
    ) {
        throw std::runtime_error("CodeGenerator::convertNonTerminator received a monostate instance.");
    }

    template <>
    std::shared_ptr<SNESAssembly::SNESAssemblySegmentInstructionChunk> CodeGenerator::convertNonTerminatorInstruction (
        Instructions::InstructionAlloca instruction, std::shared_ptr<const DefinedFunctionInfo> definedFunctionInfo
    ) {
        
        auto SNESInstructions = std::vector<const std::shared_ptr<const SNESAssembly::SNESInstruction>>();
        
        // Load alloca pointer to accumulator
        SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionLoadAccumulatorFromMemory<AddressingModes::Absolute>>(allocaStackPointerAddress));

        // Store accumulator (pointer to allocated object) to yielded variable - what if this instruction sequence is interrupted?
        // I believe better practice would be to save the allocated object's pointer to the yielded variable after the alloca stack
        //  pointer has been updated (space on the stack has actually been allocated), but given our programs are single-
        //  threaded, I think it's acceptable to not do so here, especially since it would take
        //  extra instructions to juggle around the allocated object's pointer.
        auto stackOffset = definedFunctionInfo->localsMap->getLocalEntry(instruction.assignee).second;
        SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionStoreAccumulatorToMemory<AddressingModes::StackRelative>>(stackOffset));

        SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionClearCarryFlag>());
        // Add sizeof(type) to accumulator
        auto typeSize = instruction.allocationType->getSizeBytesCeil(); // TODO handle cases where alloca allocates multiple of
        //                                                              // the specified type (NumElements > 1).
        SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionAddWithCarry<AddressingModes::Immediate>>(typeSize));

        // Store accumulator to alloca pointer (only a 16-bit bank-local address)
        SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionStoreAccumulatorToMemory<AddressingModes::Absolute>>(allocaStackPointerAddress));

        // Convert instructions over to lines.
        // Should really be done as I create instructions, so I can add trailing comments to each.
        auto instructionLines = std::make_shared<std::vector<const std::shared_ptr<const SNESAssembly::SNESAssemblyLineInstruction>>>();
        for (auto &&instruction : SNESInstructions) {
            instructionLines->push_back(std::make_shared<SNESAssembly::SNESAssemblyLineInstruction>(instruction));
        }
        

        // auto x = SNESAssembly::SNESAssemblySegmentInstructionChunk("Alloca instruction", instructionLines);
        // TODO add getStringRepresentation() to alloca and other instructions; use that for the leading comment on the
        //  instruction chunk.
        return std::make_shared<SNESAssembly::SNESAssemblySegmentInstructionChunk>("Alloca instruction", instructionLines);
    }

    template <>
    std::shared_ptr<SNESAssembly::SNESAssemblySegmentInstructionChunk> CodeGenerator::convertNonTerminatorInstruction (
        Instructions::InstructionStore instruction, std::shared_ptr<const DefinedFunctionInfo> definedFunctionInfo
    ) {
        
        auto SNESInstructions = std::vector<const std::shared_ptr<const SNESAssembly::SNESInstruction>>();
        // Logic:
        //  -If the value to store is a literal value (not stored in a local/gloabl variable in the LLVM IR), load its
        //      bytes directly into the target location.
        //  -Otherwise...
        //      If the value to store is less than 8 bytes, copy it from the source location to the target location
        //          via the accumulator (LDA & STA)
        //      Otherwise, copy it directly from the source location to the target location via a block move

        std::visit([instruction, definedFunctionInfo, &SNESInstructions](auto&& operand) {
            auto destinationOffset = definedFunctionInfo->localsMap->getLocalEntry(instruction.pointerOperand).second;
            auto bytesToTransfer = instruction.valueType->getSizeBytesCeil();
            if constexpr (std::is_base_of_v<Expressions::ExpressionLiteral, std::remove_reference_t<decltype(operand)>>) { // Value is a literal
                Expressions::ExpressionLiteralInteger literalInteger = operand;
                auto val = literalInteger.literal;
                // Initialize index register to zero
                SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionLoadYRegisterFromMemory<AddressingModes::Immediate>>(0));

                // Transfer all bytes like so, except for the last one if the # of bytes is odd.
                // FIXME Have larger literal values stored in the ROM somewhere, and use a block move to copy them instead.
                int bytesTransferred = 0;
                for (; bytesTransferred < (bytesToTransfer-1); bytesTransferred+=2) {
                    // Load first two bytes of value to store (stack relative, as the stack holds the source memory).
                    // Integers are laid out in little-endian.
                    // I'm assuming that if you load an immediate value, the high byte of the immediate goes into the high
                    //  byte of the accumulator.
                    auto immediateBytes = (val >> (8*bytesTransferred)) & 0xFFFF;
                    SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionLoadAccumulatorFromMemory<AddressingModes::Immediate>>(immediateBytes));

                    // Store first two bytes of value to store (stack relative indirect indexed, X, as the stack holds a
                    //  pointer to the destination memory)
                    SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionStoreAccumulatorToMemory<AddressingModes::StackRelativeIndirectIndexed>>(destinationOffset));
                    
                    // Increment index register twice, if we're moving more data (even if it's just one byte).
                    if ((bytesTransferred+2) < (bytesToTransfer)) {
                        SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionIncrementIndexRegisterY>());
                        SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionIncrementIndexRegisterY>());
                    }
                } // Repeat
                
                // If there are an odd number of bytes to transfer, transfer the last byte
                if (bytesToTransfer%1) {
                    // TODO maybe move this mode swapping into a function, which "wraps" the provided instructions with the
                    //  mode swapping instructions - so it injects the instructions to switch to an 8-bit accumulator
                    //  before, and the instructions to switch to a 16-bit accumulator afterwards.
                    //  Or maybe have each instruction track what mode each register should be in, and the program
                    //  automatically generates the appropriate mode-switching instructions?

                    // Switch to 8-bit accumulator
                    SNESInstructions.push_back(std::make_shared<SNESAssembly::SNESInstructionSetProcessorStatusBits>(CodeGen::accumulatorModeFlag));

                    // Transfer 1 byte.
                    unsigned int immediateByte = (val >> (8*(bytesTransferred))) & 0xFF;
                    SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionLoadAccumulatorFromMemory<AddressingModes::Immediate>>(immediateByte));
                    SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionStoreAccumulatorToMemory<AddressingModes::StackRelativeIndirectIndexed>>(destinationOffset));

                    // Switch to 16-bit accumulator
                    SNESInstructions.push_back(std::make_shared<SNESAssembly::SNESInstructionResetProcessorStatusBits>(CodeGen::accumulatorModeFlag));
                }
                
            } else { // operand is an identifier.
                auto sourceOffset = definedFunctionInfo->localsMap->getLocalEntry(operand).second;
                if (bytesToTransfer > 8) { // Transfer via move block instructions
                    
                    // Load address of source value into X register:
                    //  Transfer stack pointer to accumulator
                    SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionTransferStackToAccumulator>());
                    //  Reset carry bit
                    SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionClearCarryFlag>());
                    //  Add source address stack offset (stack grows downwards, so top of stack is at lowest address)
                    SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionAddWithCarry<AddressingModes::Immediate>>(sourceOffset));
                    //  Transfer accumulator to X register
                    SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionTransferAccumulatorToX>());

                    // Load destination address into Y register:
                    //  Load destination variable into accumulator
                    SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionLoadAccumulatorFromMemory<AddressingModes::StackRelative>>(destinationOffset));
                    //  Transfer accumulator into Y register
                    SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionTransferAccumulatorToY>());

                    // Load # bytes to transfer into accumulator
                    SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionLoadAccumulatorFromMemory<AddressingModes::Immediate>>(bytesToTransfer-1));

                    // Block move instruction
                    unsigned char sourceBank = 0x7E;
                    unsigned char destinationBank = 0x7E;
                    SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionBlockMoveNegative>(sourceBank, destinationBank));
                } else { // Transfer via LDA & STA
                    // Initialize index register to zero
                    SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionLoadYRegisterFromMemory<AddressingModes::Immediate>>(0));

                    // Transfer all bytes like so, except for the last one if the # of bytes is odd.
                    int bytesTransferred = 0;
                    for (; bytesTransferred < (bytesToTransfer-1); bytesTransferred+=2) {
                        // Load first two bytes of value to store (stack relative, as the stack holds the source memory).
                        SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionLoadAccumulatorFromMemory<AddressingModes::StackRelative>>(sourceOffset+bytesTransferred));

                        // Store first two bytes of value to store (stack relative indirect indexed, X, as the stack holds a
                        //  pointer to the destination memory)
                        SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionStoreAccumulatorToMemory<AddressingModes::StackRelativeIndirectIndexed>>(destinationOffset));
                        
                        // Increment index register twice, if we're moving more data (even if it's just one byte).
                        if ((bytesTransferred+2) < (bytesToTransfer)) {
                            SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionIncrementIndexRegisterY>());
                            SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionIncrementIndexRegisterY>());
                        }
                    } // Repeat
                    // If we have an odd number of bytes to transfer, we have to do the last one separately, as (assuming we're
                    //  in 16-bit mode) all loads/stores transfer 16 bits.
                    if (bytesToTransfer%1) {
                        // Switch to 8-bit accumulator
                        SNESInstructions.push_back(std::make_shared<SNESAssembly::SNESInstructionSetProcessorStatusBits>(CodeGen::accumulatorModeFlag));

                        // Transfer 1 byte.
                        SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionLoadAccumulatorFromMemory<AddressingModes::StackRelative>>(sourceOffset+bytesTransferred));
                        SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionStoreAccumulatorToMemory<AddressingModes::StackRelativeIndirectIndexed>>(destinationOffset));

                        // Switch to 16-bit accumulator
                        SNESInstructions.push_back(std::make_shared<SNESAssembly::SNESInstructionResetProcessorStatusBits>(CodeGen::accumulatorModeFlag));
                    }
                }
            }
        }, *(instruction.valueOperand->operand));

        auto instructionLines = std::make_shared<std::vector<const std::shared_ptr<const SNESAssembly::SNESAssemblyLineInstruction>>>();
        for (auto &&instruction : SNESInstructions) {
            instructionLines->push_back(std::make_shared<SNESAssembly::SNESAssemblyLineInstruction>(instruction));
        }

        return std::make_shared<SNESAssembly::SNESAssemblySegmentInstructionChunk>("Store instruction", instructionLines);
    }

    template <>
    std::shared_ptr<SNESAssembly::SNESAssemblySegmentInstructionChunk> CodeGenerator::convertNonTerminatorInstruction (
        Instructions::InstructionLoad instruction, std::shared_ptr<const DefinedFunctionInfo> definedFunctionInfo
    ) {
        
        auto SNESInstructions = std::vector<const std::shared_ptr<const SNESAssembly::SNESInstruction>>();

        auto destinationOffset = definedFunctionInfo->localsMap->getLocalEntry(instruction.assignee).second;
        auto bytesToTransfer = instruction.yieldedType->getSizeBytesCeil();
        
        auto sourceOffset = definedFunctionInfo->localsMap->getLocalEntry(instruction.pointerOperand).second;
        if (bytesToTransfer > 8) { // Transfer via move block instructions
            
            // Load address of source value into X register:
            //  Load source variable into accumulator
            SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionLoadAccumulatorFromMemory<AddressingModes::StackRelative>>(sourceOffset));
            //  Transfer accumulator into Y register
            SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionTransferAccumulatorToY>());

            // Load destination address into Y register:
            //  Transfer stack pointer to accumulator
            SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionTransferStackToAccumulator>());
            //  Reset carry bit
            SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionClearCarryFlag>());
            //  Add source address stack offset (stack grows downwards, so top of stack is at lowest address)
            SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionAddWithCarry<AddressingModes::Immediate>>(destinationOffset));
            //  Transfer accumulator to X register
            SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionTransferAccumulatorToX>());

            // Load # bytes to transfer into accumulator
            SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionLoadAccumulatorFromMemory<AddressingModes::Immediate>>(bytesToTransfer-1));

            // Block move instruction
            unsigned char sourceBank = 0x7E;
            unsigned char destinationBank = 0x7E;
            SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionBlockMoveNegative>(sourceBank, destinationBank));
        } else { // Transfer via LDA & STA
            // Initialize index register to zero
            SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionLoadYRegisterFromMemory<AddressingModes::Immediate>>(0));

            int bytesTransferred = 0;
            // Transfer all bytes like so, except for the last one if the # of bytes is odd.
            for (; bytesTransferred < (bytesToTransfer-1); bytesTransferred+=2) {
                // Load first two bytes of value to store (stack relative indirect indexed, Y), as the stack holds a
                //  pointer to the source memory)
                SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionLoadAccumulatorFromMemory<AddressingModes::StackRelativeIndirectIndexed>>(sourceOffset));

                // Store first two bytes of value to store (stack relative, as the stack holds the destination memory).
                SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionStoreAccumulatorToMemory<AddressingModes::StackRelative>>(destinationOffset+bytesTransferred));
                
                // Increment index register twice, if we're moving more data (even if it's just one byte).
                if ((bytesTransferred+2) < (bytesToTransfer)) {
                    SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionIncrementIndexRegisterY>());
                    SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionIncrementIndexRegisterY>());
                }
            } // Repeat
            // If we have an odd number of bytes to transfer, we have to do the last one separately, as (assuming we're
            //  in 16-bit mode) all loads/stores transfer 16 bits.
            if (bytesToTransfer%1) {
                // Switch to 8-bit accumulator
                SNESInstructions.push_back(std::make_shared<SNESAssembly::SNESInstructionSetProcessorStatusBits>(CodeGen::accumulatorModeFlag));

                // Transfer 1 byte.
                SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionLoadAccumulatorFromMemory<AddressingModes::StackRelativeIndirectIndexed>>(sourceOffset));
                SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionStoreAccumulatorToMemory<AddressingModes::StackRelative>>(destinationOffset+bytesTransferred));

                // Switch to 16-bit accumulator
                SNESInstructions.push_back(std::make_shared<SNESAssembly::SNESInstructionResetProcessorStatusBits>(CodeGen::accumulatorModeFlag));
            }
        }

        auto instructionLines = std::make_shared<std::vector<const std::shared_ptr<const SNESAssembly::SNESAssemblyLineInstruction>>>();
        for (auto &&instruction : SNESInstructions) {
            instructionLines->push_back(std::make_shared<SNESAssembly::SNESAssemblyLineInstruction>(instruction));
        }

        return std::make_shared<SNESAssembly::SNESAssemblySegmentInstructionChunk>("Load instruction", instructionLines);
    };

    template <>
    std::shared_ptr<SNESAssembly::SNESAssemblySegmentInstructionChunk> CodeGenerator::convertNonTerminatorInstruction (
        Instructions::InstructionAdd instruction, std::shared_ptr<const DefinedFunctionInfo> definedFunctionInfo
    ) {
        // NOTE - Ignoring vectors for now.
        auto SNESInstructions = std::vector<const std::shared_ptr<const SNESAssembly::SNESInstruction>>();

        std::visit([definedFunctionInfo, &SNESInstructions, instruction](auto&& leftOperand){
            auto bytesToTransfer = instruction.getYieldedType()->getSizeBytesCeil();
            auto assigneeOffset = definedFunctionInfo->localsMap->getLocalEntry(instruction.assignee).second;
            if constexpr (std::is_base_of_v<Expressions::ExpressionLiteral, std::remove_reference_t<decltype(leftOperand)>>) {
                std::visit([definedFunctionInfo, &SNESInstructions, leftOperand, bytesToTransfer, assigneeOffset](auto&& rightOperand){
                    if constexpr (std::is_base_of_v<Expressions::ExpressionLiteral, std::remove_reference_t<decltype(rightOperand)>>) {
                        int sum = leftOperand.literal+rightOperand.literal;
                        generateCopyIntegerToLocalInstructions(SNESInstructions, sum, assigneeOffset, bytesToTransfer);
                    } else if constexpr (std::is_base_of_v<Expressions::ExpressionLocalIdentifier, std::remove_reference_t<decltype(rightOperand)>>) {
                        const int literal = leftOperand.literal;
                        const int rightOperandOffset = definedFunctionInfo->localsMap->getLocalEntry(rightOperand).second;
                        generateAddIntegertoLocalInstructions(SNESInstructions, literal, rightOperandOffset, assigneeOffset, bytesToTransfer);
                    } else {
                        throw std::runtime_error("Unhandled operand variant.");
                    }
                }, *(instruction.rightOperand->operand));
            } else if constexpr (std::is_base_of_v<Expressions::ExpressionLocalIdentifier, std::remove_reference_t<decltype(leftOperand)>>) {
                std::visit([definedFunctionInfo, &SNESInstructions, leftOperand, bytesToTransfer, assigneeOffset](auto&& rightOperand){
                    const int leftOperandOffset = definedFunctionInfo->localsMap->getLocalEntry(leftOperand).second;
                    if constexpr (std::is_base_of_v<Expressions::ExpressionLiteral, std::remove_reference_t<decltype(rightOperand)>>) {
                        const int literal = rightOperand.literal;
                        generateAddIntegertoLocalInstructions(SNESInstructions, literal, leftOperandOffset, assigneeOffset, bytesToTransfer);
                    } else if constexpr (std::is_base_of_v<Expressions::ExpressionLocalIdentifier, std::remove_reference_t<decltype(rightOperand)>>) {
                        const int rightOperandOffset = definedFunctionInfo->localsMap->getLocalEntry(rightOperand).second;
                        unsigned int bytesTransferred = 0;
                        SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionClearCarryFlag>());
                        // Stack grows downwards, so bigger offsets refer to higher addresses
                        // I believe that in 16-bit register loads/stores, the high byte is the upper address (effective address
                        //  plus one); i.e. high byte of a 16-bit integer is at the upper address.
                        // Going to treat larger integers as also working this way - the more significant the byte, the
                        //  higher the address it's stored at.
                        // So, bascially work through the integer in two-byte chunks, with the carry being kept between chunks.
                        for (; bytesTransferred < (bytesToTransfer-1); bytesTransferred+=2) {
                            SNESInstructions.push_back(std::make_shared<SNESAssembly::SNESInstructionLoadAccumulatorFromMemory<AddressingModes::StackRelative>>(leftOperandOffset+bytesTransferred));
                            SNESInstructions.push_back(std::make_shared<SNESAssembly::SNESInstructionAddWithCarry<AddressingModes::StackRelative>>(rightOperandOffset+bytesTransferred));
                            SNESInstructions.push_back(std::make_shared<SNESAssembly::SNESInstructionStoreAccumulatorToMemory<AddressingModes::StackRelative>>(assigneeOffset+bytesTransferred));
                        }
                        if (bytesTransferred%1) {
                            SNESInstructions.push_back(std::make_shared<SNESAssembly::SNESInstructionSetProcessorStatusBits>(CodeGen::accumulatorModeFlag));
                            SNESInstructions.push_back(std::make_shared<SNESAssembly::SNESInstructionLoadAccumulatorFromMemory<AddressingModes::StackRelative>>(leftOperandOffset+bytesTransferred));
                            SNESInstructions.push_back(std::make_shared<SNESAssembly::SNESInstructionAddWithCarry<AddressingModes::StackRelative>>(rightOperandOffset+bytesTransferred));
                            SNESInstructions.push_back(std::make_shared<SNESAssembly::SNESInstructionStoreAccumulatorToMemory<AddressingModes::StackRelative>>(assigneeOffset+bytesTransferred));
                            SNESInstructions.push_back(std::make_shared<SNESAssembly::SNESInstructionResetProcessorStatusBits>(CodeGen::accumulatorModeFlag));
                        }
                    } else {
                        throw std::runtime_error("Unhandled operand variant.");
                    }
                }, *(instruction.rightOperand->operand));
            } else {
                throw std::runtime_error("Unhandled operand variant.");
            }
        }, *(instruction.leftOperand->operand));

        // FIXME handle poison values, such as from nsw flag

        auto instructionLines = std::make_shared<std::vector<const std::shared_ptr<const SNESAssembly::SNESAssemblyLineInstruction>>>();
        for (auto &&instruction : SNESInstructions) {
            instructionLines->push_back(std::make_shared<SNESAssembly::SNESAssemblyLineInstruction>(instruction));
        }

        return std::make_shared<SNESAssembly::SNESAssemblySegmentInstructionChunk>("Add with carry instruction", instructionLines);
    };

    // CodeGenerator::convertTerminatorInstruction specializations

    template <>
    std::shared_ptr<SNESAssembly::SNESAssemblySegmentInstructionChunk> CodeGenerator::convertTerminatorInstruction (
        std::monostate nonInstruction, std::shared_ptr<const DefinedFunctionInfo> definedFunctionInfo
    ) {
        throw std::runtime_error("CodeGenerator::convertTerminatorInstruction received a monostate instance.");
    }

    template <>
    std::shared_ptr<SNESAssembly::SNESAssemblySegmentInstructionChunk> CodeGenerator::convertTerminatorInstruction (
        Instructions::InstructionRetValue instruction, std::shared_ptr<const DefinedFunctionInfo> definedFunctionInfo
    ) {
        
        auto SNESInstructions = std::vector<const std::shared_ptr<const SNESAssembly::SNESInstruction>>();
        std::visit([instruction, definedFunctionInfo, &SNESInstructions](auto&& operand){
            // Similar to the operand visitor lambda in convertNonTerminatorInstruction<InstructionStore>(...)
            auto destinationOffset = definedFunctionInfo->localsMap->returnEntry.second;
            auto bytesToTransfer = instruction.returnType->getSizeBytesCeil();
            if constexpr (std::is_base_of_v<Expressions::ExpressionLiteral, std::remove_reference_t<decltype(operand)>>) {
                int literal = operand.literal;
                generateCopyIntegerToLocalInstructions(SNESInstructions, literal, destinationOffset, bytesToTransfer);
                // for (int bytesTransferred = 0; bytesTransferred < (bytesToTransfer-1); bytesTransferred+=2) {
                //     auto immediateBytes = (literal >> (8*bytesTransferred)) & 0xFFFF; // Next two bytes to store. Should this be unsigned?
                //     SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionLoadAccumulatorFromMemory<AddressingModes::Immediate>>(immediateBytes));
                //     SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionStoreAccumulatorToMemory<AddressingModes::StackRelative>>(destinationOffset+bytesTransferred));
                // }
                // if (bytesToTransfer%1) {
                //     // Switch to 8-bit accumulator
                //     // FIXME magic number should be a defined constant. Maybe a constant that holds the REP/SEP instructions
                //     //  to change accumulator modes?
                //     SNESInstructions.push_back(std::make_shared<SNESAssembly::SNESInstructionSetProcessorStatusBits>(CodeGen::accumulatorModeFlag));
                //     unsigned int immediateByte = (literal >> (8*(bytesToTransfer-1))) & 0xFF;
                //     SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionLoadAccumulatorFromMemory<AddressingModes::Immediate>>(immediateByte));
                //     SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionStoreAccumulatorToMemory<AddressingModes::StackRelative>>(destinationOffset+bytesToTransfer-1));

                //     // Switch to 16-bit accumulator
                //     SNESInstructions.push_back(std::make_shared<SNESAssembly::SNESInstructionResetProcessorStatusBits>(CodeGen::accumulatorModeFlag));
                // }
            } else {
                auto sourceOffset = definedFunctionInfo->localsMap->getLocalEntry(operand).second;
                // FIXME see if I can merge parts of this with the duplicates in
                //  convertNonTerminatorInstruction<InstructionStore>(...)
                if (bytesToTransfer > 8) { // FIXME Magic number, should be moved to a constant
                    // Load address of source value into X register:
                    //  Transfer stack pointer to accumulator
                    SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionTransferStackToAccumulator>());
                    //  Reset carry bit
                    SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionClearCarryFlag>());
                    //  Add source address stack offset (stack grows downwards, so top of stack is at lowest address)
                    SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionAddWithCarry<AddressingModes::Immediate>>(sourceOffset));
                    //  Transfer accumulator to X register
                    SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionTransferAccumulatorToX>());

                    // Load destination address (stack-relative location) into Y register:
                    //  Transfer stack pointer to accumulator
                    SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionTransferStackToAccumulator>());
                    //  Reset carry bit - could maybe be dropped?
                    SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionClearCarryFlag>());
                    //  Add source address stack offset (stack grows downwards, so top of stack is at lowest address)
                    SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionAddWithCarry<AddressingModes::Immediate>>(destinationOffset));
                    //  Transfer accumulator to X register
                    SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionTransferAccumulatorToY>());

                    // Load # bytes-1 to transfer into accumulator (block move moves accumulator+1 bytes)
                    SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionLoadAccumulatorFromMemory<AddressingModes::Immediate>>(bytesToTransfer-1));

                    // Block move instruction
                    unsigned char sourceBank = 0x7E;
                    unsigned char destinationBank = 0x7E;
                    SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionBlockMoveNegative>(sourceBank, destinationBank));
                } else {
                    int bytesTransferred = 0;
                    for (; bytesTransferred < (bytesToTransfer-1); bytesTransferred+=2) {
                        SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionLoadAccumulatorFromMemory<AddressingModes::StackRelative>>(sourceOffset+bytesTransferred));
                        SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionStoreAccumulatorToMemory<AddressingModes::StackRelative>>(destinationOffset+bytesTransferred));
                    }
                    if (bytesToTransfer%1) {
                        // Switch to 8-bit accumulator
                        // FIXME magic number should be a defined constant. Maybe a constant that holds the REP/SEP instructions
                        //  to change accumulator modes?
                        SNESInstructions.push_back(std::make_shared<SNESAssembly::SNESInstructionSetProcessorStatusBits>(CodeGen::accumulatorModeFlag));
                        SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionLoadAccumulatorFromMemory<AddressingModes::StackRelative>>(sourceOffset+bytesTransferred-1));
                        SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionStoreAccumulatorToMemory<AddressingModes::StackRelative>>(destinationOffset+bytesTransferred-1));

                        // Switch to 16-bit accumulator
                        SNESInstructions.push_back(std::make_shared<SNESAssembly::SNESInstructionResetProcessorStatusBits>(CodeGen::accumulatorModeFlag));
                    }
                }
            }
        }, *((instruction.returnOperand)->operand));
        SNESInstructions.push_back(std::make_shared<SNESAssembly::SNESInstructionJumpAbsolute>(mangleCleanupCodeBlockLabel(definedFunctionInfo)));
        // FIXME Add a jump to the cleanup label
        // int x = "fixme";
        
        auto instructionLines = std::make_shared<std::vector<const std::shared_ptr<const SNESAssembly::SNESAssemblyLineInstruction>>>();
        for (auto &&instruction : SNESInstructions) {
            instructionLines->push_back(std::make_shared<SNESAssembly::SNESAssemblyLineInstruction>(instruction));
        }

        return std::make_shared<SNESAssembly::SNESAssemblySegmentInstructionChunk>("Return value instruction", instructionLines);
    }

    std::shared_ptr<const std::vector<const std::string>> CodeGenerator::generateCode() {
        auto generatedCode = std::make_shared<std::vector<std::string>>();
        auto functionDefinitions = this->moduleFile->functionDefinitions;
        // Compute locals/return value/parameter sizes and locations (store size/location in object)
        auto definedFunctionInfoMap = std::make_shared<std::map<std::string, std::shared_ptr<const DefinedFunctionInfo>>>();
        std::optional<std::shared_ptr<const FunctionInfo>> mainFunctionInfo;
        for (auto &&functionDefinition : *(functionDefinitions)) {
            auto localsMap = computeVariableLocations(functionDefinition);
            auto functionInfo = std::make_shared<const DefinedFunctionInfo>(functionDefinition->name, localsMap);
            definedFunctionInfoMap->insert_or_assign(localsMap->functionName, functionInfo);
            if (functionInfo->functionName == MAIN) {
                mainFunctionInfo = std::optional<std::shared_ptr<const FunctionInfo>>(functionInfo);
            }
        }

        // TODO: Generate SNES 65c816 instructions.
        auto generatedNormalFunctions = std::make_shared<std::vector<const std::shared_ptr<const SNESAssembly::SNESAssemblyFunction>>>();
        // TODO separate vector for special functions, like interrupt handlers & main.
        for (auto &&functionDefinition : *(functionDefinitions)) {
            auto functionName = functionDefinition->name;
            auto definedFunctionInfo = definedFunctionInfoMap->at(functionName);
            auto codeBlocks = std::make_shared<std::vector<const std::shared_ptr<const SNESAssembly::SNESAssemblyCodeBlock>>>();
            
            // Generate setup instructions
            {
                auto setupInstructions = std::make_shared<std::vector<const std::shared_ptr<const SNESAssembly::SNESInstruction>>>();
                setupInstructions->push_back(std::make_shared<const SNESAssembly::SNESInstructionPushAccumulator>());
                setupInstructions->push_back(std::make_shared<const SNESAssembly::SNESInstructionPushIndexRegisterX>());
                setupInstructions->push_back(std::make_shared<const SNESAssembly::SNESInstructionPushIndexRegisterY>());
                setupInstructions->push_back(std::make_shared<const SNESAssembly::SNESInstructionPushDirectPage>());
                // Save caller's alloca stack pointer - maybe need to ensure the DP register points to page zero?
                setupInstructions->push_back(std::make_shared<const SNESAssembly::SNESInstructionPushEffectiveIndirectAddress>(allocaStackPointerAddress));
                // Set up locals section of stack frame
                setupInstructions->push_back(std::make_shared<const SNESAssembly::SNESInstructionSetCarryFlag>());
                setupInstructions->push_back(std::make_shared<const SNESAssembly::SNESInstructionTransferStackToAccumulator>());
                setupInstructions->push_back(std::make_shared<const SNESAssembly::SNESInstructionSubtractWithBorrow<AddressingModes::Immediate>>(definedFunctionInfo->localsMap->localsSectionSize));
                setupInstructions->push_back(std::make_shared<const SNESAssembly::SNESInstructionTransferAccumulatorToStack>());
                auto setupLines = std::make_shared<std::vector<const std::shared_ptr<const SNESAssembly::SNESAssemblyLineInstruction>>>();
                for (auto &&instruction : *setupInstructions) {
                    setupLines->push_back(std::make_shared<const SNESAssembly::SNESAssemblyLineInstruction>(instruction));
                }
                auto setupChunks = std::make_shared<std::vector<const std::shared_ptr<const SNESAssembly::SNESAssemblySegment>>>();
                setupChunks->push_back(std::make_shared<SNESAssembly::SNESAssemblySegmentInstructionChunk>(setupLines));
                
                // FIXME function block should prepend its name to all code block labels w/in it automatically. Or something.
                //  Also, it should do likewise for any intra-function jumps to other code blocks.
                codeBlocks->push_back(std::make_shared<SNESAssembly::SNESAssemblyCodeBlock>(mangleSetupCodeBlockLabel(definedFunctionInfo), std::string("Preserve registers"), setupChunks));
            }

            // TODO: Convert LLVM IR instructions to SNES 65c816 instructions
            for (auto &&codeBlock : *(functionDefinition->codeBlocks)) {
                auto segments = std::make_shared<std::vector<const std::shared_ptr<const SNESAssembly::SNESAssemblySegment>>>();
                for (auto &&instruction : *(codeBlock->instructions)) {
                    auto instructionChunk = std::visit([this, definedFunctionInfo](auto&& arg){return convertNonTerminatorInstruction(arg, definedFunctionInfo);}, *instruction);
                    segments->push_back(instructionChunk);
                }
                auto instructionChunk = std::visit([this, definedFunctionInfo](auto&& arg){return convertTerminatorInstruction(arg, definedFunctionInfo);}, *(codeBlock->terminator));
                segments->push_back(instructionChunk);
                codeBlocks->push_back(std::make_shared<SNESAssembly::SNESAssemblyCodeBlock>(mangleCodeBlockLabel(definedFunctionInfo, codeBlock), segments));
            }
            
            {
                // Generate cleanup instructions
                auto cleanupInstructions = std::make_shared<std::vector<const std::shared_ptr<const SNESAssembly::SNESInstruction>>>();
                
                // Clean up locals section of stack frame
                cleanupInstructions->push_back(std::make_shared<const SNESAssembly::SNESInstructionClearCarryFlag>());
                cleanupInstructions->push_back(std::make_shared<const SNESAssembly::SNESInstructionTransferStackToAccumulator>());
                cleanupInstructions->push_back(std::make_shared<const SNESAssembly::SNESInstructionAddWithCarry<AddressingModes::Immediate>>(definedFunctionInfo->localsMap->localsSectionSize));
                cleanupInstructions->push_back(std::make_shared<const SNESAssembly::SNESInstructionTransferAccumulatorToStack>());
                // Restore caller's alloca stack pointer
                // WARNING: assume accumulator is in 16-bit mode; assume active data bank (?) is the one with the alloca stack.
                cleanupInstructions->push_back(std::make_shared<const SNESAssembly::SNESInstructionPopAccumulator>());
                cleanupInstructions->push_back(std::make_shared<const SNESAssembly::SNESInstructionStoreAccumulatorToMemory<AddressingModes::Absolute>>(allocaStackPointerAddress)); // Should be absolute long for safety?
                cleanupInstructions->push_back(std::make_shared<const SNESAssembly::SNESInstructionPopDirectPage>());
                cleanupInstructions->push_back(std::make_shared<const SNESAssembly::SNESInstructionPopIndexRegisterY>());
                cleanupInstructions->push_back(std::make_shared<const SNESAssembly::SNESInstructionPopIndexRegisterX>());
                cleanupInstructions->push_back(std::make_shared<const SNESAssembly::SNESInstructionPopAccumulator>());
                cleanupInstructions->push_back(std::make_shared<SNESAssembly::SNESInstructionReturnFromSubroutine>());

                auto cleanupLines = std::make_shared<std::vector<const std::shared_ptr<const SNESAssembly::SNESAssemblyLineInstruction>>>();
                for (auto &&instruction : *cleanupInstructions) {
                    cleanupLines->push_back(std::make_shared<const SNESAssembly::SNESAssemblyLineInstruction>(instruction));
                }
                auto cleanupChunks = std::make_shared<std::vector<const std::shared_ptr<const SNESAssembly::SNESAssemblySegment>>>();
                cleanupChunks->push_back(std::make_shared<SNESAssembly::SNESAssemblySegmentInstructionChunk>(cleanupLines));
                
                codeBlocks->push_back(std::make_shared<SNESAssembly::SNESAssemblyCodeBlock>(mangleCleanupCodeBlockLabel(definedFunctionInfo), std::string("Restore registers"), cleanupChunks));
                // FIXME missing JSR instruction. Also, might eventually need to determine if a subroutine will be called across
                //  banks (JSL), or only w/in its own bank (JSR).
            }
            auto function = std::make_shared<SNESAssembly::SNESAssemblyFunction>(mangleFunctionLabel(definedFunctionInfo), codeBlocks);
            generatedNormalFunctions->push_back(function);
        }
        auto generatedFunctions = std::make_shared<std::vector<const std::shared_ptr<const SNESAssembly::SNESAssemblyFunction>>>(*generatedNormalFunctions);
        auto module = mainFunctionInfo ? SNESAssembly::SNESAssemblyModule(generatedFunctions, mangleFunctionLabel(mainFunctionInfo.value())) : SNESAssembly::SNESAssemblyModule(generatedFunctions);
        
        std::cout << "Generated code:\n";
        auto lines = module.getASMLines();
        for (auto &&asmLine : *(lines)) {
            std::cout << asmLine << '\n';
        }

        return module.getASMLines();
    } // generateCode

    std::shared_ptr<LocalsMap> CodeGenerator::computeVariableLocations(std::shared_ptr<const Expressions::ExpressionFunctionDefinition> function) {
        
        auto returnType = function->returnType->returnType;
        // TODO handle parameters
        auto paramLocals = std::make_shared<LocalsTypeList>();
        std::cout << "NYI: function parameters are not included in the locals memory map." << '\n';

        // FIXME would handling parameters and locals be easier if they were "resolved"? Meaning, you locate all identifier objects
        //  which are equivalent (i.e. they refer to the same variable), and make them all point to the same object? So instead of
        //  one identifier object for each place a given variable appears, you would have one identifier object for that variable,
        //  and each place that variable appears it would point to that one object. Maybe even develop a central map for
        //  identifiers. This step could also handle unnamed identifiers that aren't explicitly declared, such as parameters that
        //  specify the type, but not the parameter name.
        // FIXME How do I determine variable types? Should I make a typechecker that determines that? Or would adding a
        //  "getYieldedType" function to YieldsValue be enough, with each instruction that yields a value implementing the function
        //  correctly? Considering it's a function-specific thing, it probably would make sense. And doing so wouldn't exclude
        //  later making a type-checker. It'd probably help, if anything.
        // FIXME Have I handled correctly computing unspecified, unnamed locals (e.g. %0, %1, %2, ...)

        // Determine size of each local.
        auto instructionLocals = std::make_shared<LocalsTypeList>();
        for (auto &&codeBlock : *(function->codeBlocks))
        {
            for (auto &&instruction : *(codeBlock->instructions))
            {
                std::visit([instructionLocals](auto&& instruction) {
                    if constexpr(std::is_base_of_v<Instructions::YieldsValue, std::remove_reference_t<decltype(instruction)>>) {
                        instructionLocals->emplace_back(instruction.assignee, instruction.getYieldedType());
                    }
                }, *(instruction));
            }
        }
        // FIXME move the code that computes each variable's location on the stack into this function.

        // FIXME What about global variables?
        return LocalsMap::createLocalsMap(function->name, paramLocals, instructionLocals, returnType);
    } // computeVariableLocations
}