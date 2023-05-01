#include "CodeGenerator.h"

#include <functional>

#include "Lib/General.h"

#include "CodeGen/SNESAssemblyModule.h"
#include "CodeGen/SNESAssemblyFunction.h"
#include "CodeGen/SNESAssemblyCodeBlock.h"
#include "CodeGen/SNESAssemblyLineInstruction.h"
#include "CodeGen/SNESAssemblySegmentInstructionChunk.h"
#include "CodeGen/NameMangling.h"
#include "CodeGen/FunctionInfo.h"
#include "CodeGen/CodeBlockMap.h"
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
#include "CodeGen/SNESInstructions/SNESInstructionANDAccumulatorWithMemory.h"
#include "CodeGen/SNESInstructions/SNESInstructionBranchConditional.h"
#include "CodeGen/SNESInstructions/SNESInstructionBranchAlways.h"

using rawInstructionVector = std::vector<const std::shared_ptr<const SNESAssembly::SNESInstruction>>;

namespace {
    void emptyProcessor (rawInstructionVector &destinationVector, unsigned int bytesTransferred) {}

    // Template to generate series of instructions for this logic:
    //  Take in a source and destination local
    //  For every two bytes in the source:
    //      Load them into the accumulator
    //      Do something with them
    //      If these are the last two bytes of the padded value, and there are padding bits (9-15 bits
    //          of actual data), AND the computed value to set the padding bits to zero
    //      Store the result into the destination.
    //  If there is one left over byte, set the accumulator to 8-bit mode, do the same stuff as above
    //      with the one byte, and set the accumulator back to 16-bit mode.
    // generateProcessingInstructions should take the following parameters:
    //      (std::vector<const std::shared_ptr<const SNESAssembly::SNESInstruction>>, unsigned int)
    //  and should return void.
    template<typename Functor>
    void generateLoadSR_Process_StoreSR_Instructions(rawInstructionVector &destinationVector, Functor generateProcessingInstructions,
                                                     std::shared_ptr<const Types::Type> yieldedType, unsigned int sourceOffset,
                                                     unsigned int destinationOffset) {
        unsigned int bytesToTransfer = yieldedType->getSizeBytesCeil();
        unsigned int bytesTransferred = 0;
        for (; bytesTransferred < (bytesToTransfer-1); bytesTransferred+=2) {
            destinationVector.push_back(std::make_shared<SNESAssembly::SNESInstructionLoadAccumulatorFromMemory<AddressingModes::StackRelative>>(sourceOffset+bytesTransferred));
            generateProcessingInstructions(destinationVector, bytesTransferred);
            unsigned int bitsRemaining = yieldedType->getSizeBits()-bytesTransferred*8;
            if (bitsRemaining < 16) {
                // Bitmask to ignore bits that shouldn't be part of the final value.
                const unsigned int bitMask = (1<<bitsRemaining)-1;
                destinationVector.push_back(std::make_shared<SNESAssembly::SNESInstructionANDAccumulatorWithMemory<AddressingModes::Immediate>>(bitMask));
            }
            destinationVector.push_back(std::make_shared<SNESAssembly::SNESInstructionStoreAccumulatorToMemory<AddressingModes::StackRelative>>(destinationOffset+bytesTransferred));
        }
        if (bytesToTransfer%2) {
            destinationVector.push_back(std::make_shared<SNESAssembly::SNESInstructionSetProcessorStatusBits>(CodeGen::accumulatorModeFlag));
            destinationVector.push_back(std::make_shared<SNESAssembly::SNESInstructionLoadAccumulatorFromMemory<AddressingModes::StackRelative>>(sourceOffset+bytesTransferred));
            generateProcessingInstructions(destinationVector, bytesTransferred);
            unsigned int bitsRemaining = yieldedType->getSizeBits()-bytesTransferred*8;
            if (bitsRemaining < 8) {
                // Bitmask to ignore bits that shouldn't be part of the final value.
                const unsigned int bitMask = (1<<bitsRemaining)-1;
                destinationVector.push_back(std::make_shared<SNESAssembly::SNESInstructionANDAccumulatorWithMemory<AddressingModes::Immediate>>(bitMask));
            }
            destinationVector.push_back(std::make_shared<SNESAssembly::SNESInstructionStoreAccumulatorToMemory<AddressingModes::StackRelative>>(destinationOffset+bytesTransferred));
            destinationVector.push_back(std::make_shared<SNESAssembly::SNESInstructionStoreAccumulatorToMemory<AddressingModes::StackRelative>>(destinationOffset+bytesTransferred));
            destinationVector.push_back(std::make_shared<SNESAssembly::SNESInstructionResetProcessorStatusBits>(CodeGen::accumulatorModeFlag));
        }
    }

    void generateCopyIntegerToLocalInstructions(rawInstructionVector &destinationVector, const int literal,
                                                const int destinationLocalOffset, const int bytesToTransfer) {
        unsigned int bytesTransferred = 0;
        for (; bytesTransferred < (bytesToTransfer-1); bytesTransferred+=2) {
            int nextBytes = (literal>>(bytesTransferred*8))&0xFFFF;
            destinationVector.push_back(std::make_shared<const SNESAssembly::SNESInstructionLoadAccumulatorFromMemory<AddressingModes::Immediate>>(nextBytes));
            destinationVector.push_back(std::make_shared<const SNESAssembly::SNESInstructionStoreAccumulatorToMemory<AddressingModes::StackRelative>>(destinationLocalOffset+bytesTransferred));
        }
        if (bytesToTransfer%2) {
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

    void generateAddIntegertoLocalInstructions(rawInstructionVector &destinationVector,
                                                const int literal, const int sourceLocalOffset, const int destinationLocalOffset, const std::shared_ptr<const Types::Type> additionType) {
        destinationVector.push_back(std::make_shared<const SNESAssembly::SNESInstructionClearCarryFlag>());
        auto processor = [literal](rawInstructionVector &destinationVector, unsigned int bytesTransferred){
            int nextImmediateBytes = (literal>>(bytesTransferred*8))&0xFFFF;
            destinationVector.push_back(std::make_shared<SNESAssembly::SNESInstructionAddWithCarry<AddressingModes::Immediate>>(nextImmediateBytes));
        };
        generateLoadSR_Process_StoreSR_Instructions(destinationVector, processor, additionType, sourceLocalOffset, destinationLocalOffset);
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
        
        auto SNESInstructions = rawInstructionVector();
        
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
                if (bytesToTransfer%2) {
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
                    if (bytesToTransfer%2) {
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
            if (bytesToTransfer%2) {
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
        // FIXME need to ensure the result fits w/in the specific integer bit-width (e.g. if it's an i31, need
        //  to AND the high byte with 0b01111111)

        // If the type we're working with has padding bits (bitwidth isn't a multiple of 8), need to set those padding
        //  bits to zero
        
        unsigned int paddingBits = instruction.yieldedType->getSizeBytesCeil()*8-instruction.yieldedType->getSizeBits();
        if (paddingBits > 0) {
            throw std::runtime_error("NYI: convertNonTerminatorInstruction<AddWithCarry> for integers with padding.");
        }

        std::visit([definedFunctionInfo, &SNESInstructions, instruction](auto&& leftOperand){
            auto yieldedType = instruction.getYieldedType();
            auto assigneeOffset = definedFunctionInfo->localsMap->getLocalEntry(instruction.assignee).second;
            if constexpr (std::is_base_of_v<Expressions::ExpressionLiteral, std::remove_reference_t<decltype(leftOperand)>>) {
                std::visit([definedFunctionInfo, &SNESInstructions, leftOperand, yieldedType, assigneeOffset](auto&& rightOperand){
                    if constexpr (std::is_base_of_v<Expressions::ExpressionLiteral, std::remove_reference_t<decltype(rightOperand)>>) {
                        int sum = leftOperand.literal+rightOperand.literal;
                        generateCopyIntegerToLocalInstructions(SNESInstructions, sum, assigneeOffset, yieldedType->getSizeBytesCeil());
                    } else if constexpr (std::is_base_of_v<Expressions::ExpressionLocalIdentifier, std::remove_reference_t<decltype(rightOperand)>>) {
                        const int literal = leftOperand.literal;
                        const int rightOperandOffset = definedFunctionInfo->localsMap->getLocalEntry(rightOperand).second;
                        generateAddIntegertoLocalInstructions(SNESInstructions, literal, rightOperandOffset, assigneeOffset, yieldedType);
                    } else {
                        throw std::runtime_error("Unhandled operand variant.");
                    }
                }, *(instruction.rightOperand->operand));
            } else if constexpr (std::is_base_of_v<Expressions::ExpressionLocalIdentifier, std::remove_reference_t<decltype(leftOperand)>>) {
                std::visit([definedFunctionInfo, &SNESInstructions, leftOperand, yieldedType, assigneeOffset](auto&& rightOperand){
                    const int leftOperandOffset = definedFunctionInfo->localsMap->getLocalEntry(leftOperand).second;
                    if constexpr (std::is_base_of_v<Expressions::ExpressionLiteral, std::remove_reference_t<decltype(rightOperand)>>) {
                        const int literal = rightOperand.literal;
                        generateAddIntegertoLocalInstructions(SNESInstructions, literal, leftOperandOffset, assigneeOffset, yieldedType);
                    } else if constexpr (std::is_base_of_v<Expressions::ExpressionLocalIdentifier, std::remove_reference_t<decltype(rightOperand)>>) {
                        // Stack grows downwards, so bigger offsets refer to higher addresses
                        // I believe that in 16-bit register loads/stores, the high byte is the upper address (effective address
                        //  plus one); i.e. high byte of a 16-bit integer is at the upper address.
                        // Going to treat larger integers as also working this way - the more significant the byte, the
                        //  higher the address it's stored at.
                        // So, bascially work through the integer in two-byte chunks, with the carry being kept between chunks.
                        const int rightOperandOffset = definedFunctionInfo->localsMap->getLocalEntry(rightOperand).second;
                        unsigned int bytesTransferred = 0;
                        SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionClearCarryFlag>());
                        auto processor = [rightOperandOffset](rawInstructionVector &instructionVector, unsigned int bytesTransferred) {
                            instructionVector.push_back(std::make_shared<SNESAssembly::SNESInstructionAddWithCarry<AddressingModes::StackRelative>>(rightOperandOffset+bytesTransferred));
                        };
                        generateLoadSR_Process_StoreSR_Instructions(SNESInstructions, processor, yieldedType, leftOperandOffset, assigneeOffset);
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
    } // convertNonTerminatorInstruction<Instructions::InstructionAdd>
    
    template <>
    std::shared_ptr<SNESAssembly::SNESAssemblySegmentInstructionChunk> CodeGenerator::convertNonTerminatorInstruction (
        Instructions::InstructionTruncate instruction, std::shared_ptr<const DefinedFunctionInfo> definedFunctionInfo
    ) {
        // NOTE - Ignoring vectors for now.
        auto SNESInstructions = std::vector<const std::shared_ptr<const SNESAssembly::SNESInstruction>>();
        std::visit([instruction, definedFunctionInfo, &SNESInstructions](auto&& operand){
            const unsigned int destinationOffset = definedFunctionInfo->localsMap->getLocalEntry(instruction.assignee).second;
            if constexpr (std::is_base_of_v<Expressions::ExpressionLiteralInteger, std::remove_reference_t<decltype(operand)>>) {
                const int bitMask = (((unsigned long)2)<<instruction.yieldedType->getSizeBits())-1;
                const int truncatedValue = operand.literal&bitMask;
                const unsigned int bytesToTransfer = instruction.yieldedType->getSizeBytesCeil();
                unsigned int bytesTransferred = 0;
                for(; bytesTransferred < (bytesToTransfer-1); bytesTransferred += 2) {
                    int nextTwoBytes = (truncatedValue>>(bytesTransferred*8))&0xFFFF;
                    SNESInstructions.push_back(std::make_shared<SNESAssembly::SNESInstructionLoadAccumulatorFromMemory<AddressingModes::Immediate>>(nextTwoBytes));
                    SNESInstructions.push_back(std::make_shared<SNESAssembly::SNESInstructionStoreAccumulatorToMemory<AddressingModes::StackRelative>>(destinationOffset+bytesTransferred));
                }
                if (bytesToTransfer%2) {
                    SNESInstructions.push_back(std::make_shared<SNESAssembly::SNESInstructionSetProcessorStatusBits>(CodeGen::accumulatorModeFlag));
                    int nextByte = (truncatedValue>>(bytesTransferred*8))&0xFF;
                    SNESInstructions.push_back(std::make_shared<SNESAssembly::SNESInstructionLoadAccumulatorFromMemory<AddressingModes::Immediate>>(nextByte));
                    SNESInstructions.push_back(std::make_shared<SNESAssembly::SNESInstructionStoreAccumulatorToMemory<AddressingModes::StackRelative>>(destinationOffset+bytesTransferred));
                    SNESInstructions.push_back(std::make_shared<SNESAssembly::SNESInstructionResetProcessorStatusBits>(CodeGen::accumulatorModeFlag));
                }

            } else if constexpr (std::is_base_of_v<Expressions::ExpressionLocalIdentifier, std::remove_reference_t<decltype(operand)>>) {
                const unsigned int sourceOffset = definedFunctionInfo->localsMap->getLocalEntry(operand).second;
                generateLoadSR_Process_StoreSR_Instructions(SNESInstructions, emptyProcessor, instruction.yieldedType, sourceOffset, destinationOffset);
            } else {
                throw std::runtime_error("convertNonTerminatorInstruction<InstructionTruncate>: Unsupported operand type received.");
            }
        }, *(instruction.operand->operand));

        auto instructionLines = std::make_shared<std::vector<const std::shared_ptr<const SNESAssembly::SNESAssemblyLineInstruction>>>();
        for (auto &&instruction : SNESInstructions) {
            instructionLines->push_back(std::make_shared<SNESAssembly::SNESAssemblyLineInstruction>(instruction));
        }
        return std::make_shared<SNESAssembly::SNESAssemblySegmentInstructionChunk>("Truncate instruction", instructionLines);
    } // convertNonTerminatorInstruction<InstructionTruncate>

    template <>
    std::shared_ptr<SNESAssembly::SNESAssemblySegmentInstructionChunk> CodeGenerator::convertNonTerminatorInstruction (
        Instructions::InstructionCallReturnValue instruction, std::shared_ptr<const DefinedFunctionInfo> definedFunctionInfo
    ) {
        // NOTE - Ignoring vectors for now.
        throw std::runtime_error("[ERROR] NYI: CodeGenerator::convertNonTerminatorInstruction<InstructionCallReturnValue>");
        auto SNESInstructions = std::vector<const std::shared_ptr<const SNESAssembly::SNESInstruction>>();

        auto instructionLines = std::make_shared<std::vector<const std::shared_ptr<const SNESAssembly::SNESAssemblyLineInstruction>>>();
        for (auto &&instruction : SNESInstructions) {
            instructionLines->push_back(std::make_shared<SNESAssembly::SNESAssemblyLineInstruction>(instruction));
        }
        return std::make_shared<SNESAssembly::SNESAssemblySegmentInstructionChunk>("Call yields value instruction", instructionLines);
    } // convertNonTerminatorInstruction<InstructionCallReturnValue>


    // CodeGenerator::convertTerminatorInstruction specializations

    template <>
    std::shared_ptr<SNESAssembly::SNESAssemblySegmentInstructionChunk> CodeGenerator::convertTerminatorInstruction (
        std::monostate nonInstruction, std::shared_ptr<const DefinedFunctionInfo> definedFunctionInfo
    ) {
        throw std::runtime_error("CodeGenerator::convertTerminatorInstruction received a monostate instance.");
    } // convertTerminatorInstruction<std::monostate>

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
                    const auto processor = [](std::vector<const std::shared_ptr<const SNESAssembly::SNESInstruction>> &destinationVector, unsigned int bytesTransferred){};
                    generateLoadSR_Process_StoreSR_Instructions(SNESInstructions, processor, instruction.returnType, sourceOffset, destinationOffset);
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
    } // convertTerminatorInstruction<Instructions::InstructionRetValue>

    template <>
    std::shared_ptr<SNESAssembly::SNESAssemblySegmentInstructionChunk> CodeGenerator::convertTerminatorInstruction (
        Instructions::InstructionBranchConditional instruction, std::shared_ptr<const DefinedFunctionInfo> definedFunctionInfo
    ) {
        // throw std::runtime_error("[ERROR] NYI: CodeGenerator::convertTerminatorInstruction<Instructions::InstructionBranchConditional>");
        auto SNESInstructions = std::vector<const std::shared_ptr<const SNESAssembly::SNESInstruction>>();
        const unsigned int operandOffset = definedFunctionInfo->localsMap->getLocalEntry(instruction.condition).second;

        SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionLoadAccumulatorFromMemory<AddressingModes::StackRelative>>(operandOffset));

        // Boolean is a single byte (1 bit padded to 8 bits), but we're loading accumulator in 16-bit, so it
        //  loads 2 bytes; need to ensure the high byte loaded is non-zero.
        SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionANDAccumulatorWithMemory<AddressingModes::Immediate>>(1));
        // AND sets zero flag if the result is zero, otherwise clears it; as we're working with a boolean (i1),
        //  the zero flag is effectively the opposite of the boolean (i1=0=false -> z=1=true, and vice versa)

        // Branch if the zero flag is clear - i.e. if the value we loaded is true (one).
        auto ifTrueCodeBlockInfo = definedFunctionInfo->codeBlocksMap->getCodeblockInfo(instruction.ifTrueLabel);
        auto ifTrueMangledCodeBlockLabel = mangleCodeBlockLabel(definedFunctionInfo, ifTrueCodeBlockInfo);
        SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionBranchIfNotEqual>(ifTrueMangledCodeBlockLabel));
        auto ifFalseCodeBlockInfo = definedFunctionInfo->codeBlocksMap->getCodeblockInfo(instruction.ifFalseLabel);
        auto ifFalseMangledCodeBlockLabel = mangleCodeBlockLabel(definedFunctionInfo, ifFalseCodeBlockInfo);
        SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionJumpAbsolute>(ifFalseMangledCodeBlockLabel));
        
        auto instructionLines = std::make_shared<std::vector<const std::shared_ptr<const SNESAssembly::SNESAssemblyLineInstruction>>>();
        for (auto &&instruction : SNESInstructions) {
            instructionLines->push_back(std::make_shared<SNESAssembly::SNESAssemblyLineInstruction>(instruction));
        }

        return std::make_shared<SNESAssembly::SNESAssemblySegmentInstructionChunk>("Conditional Branch instruction", instructionLines);
    } // convertTerminatorInstruction<Instructions::InstructionBranchConditional>

    template <>
    std::shared_ptr<SNESAssembly::SNESAssemblySegmentInstructionChunk> CodeGenerator::convertTerminatorInstruction (
        Instructions::InstructionBranchUnconditional instruction, std::shared_ptr<const DefinedFunctionInfo> definedFunctionInfo
    ) {
        // throw std::runtime_error("[ERROR] NYI: CodeGenerator::convertTerminatorInstruction<Instructions::InstructionBranchUnconditional>");
        auto SNESInstructions = std::vector<const std::shared_ptr<const SNESAssembly::SNESInstruction>>();

        auto targetLabel = mangleCodeBlockLabel(definedFunctionInfo, definedFunctionInfo->codeBlocksMap->getCodeblockInfo(instruction.destination));
        SNESInstructions.push_back(std::make_shared<SNESAssembly::SNESInstructionJumpAbsolute>(targetLabel));
        
        auto instructionLines = std::make_shared<std::vector<const std::shared_ptr<const SNESAssembly::SNESAssemblyLineInstruction>>>();
        for (auto &&instruction : SNESInstructions) {
            instructionLines->push_back(std::make_shared<SNESAssembly::SNESAssemblyLineInstruction>(instruction));
        }

        return std::make_shared<SNESAssembly::SNESAssemblySegmentInstructionChunk>("Unconditional Branch instruction", instructionLines);
    } // convertTerminatorInstruction<Instructions::InstructionBranchUnConditional>

    std::shared_ptr<const std::vector<const std::string>> CodeGenerator::generateCode() {
        auto generatedCode = std::make_shared<std::vector<std::string>>();
        auto functionDefinitions = this->moduleFile->functionDefinitions;
        // Compute locals/return value/parameter sizes and locations (store size/location in object)
        auto definedFunctionInfoMap = std::make_shared<std::map<std::string, std::shared_ptr<const DefinedFunctionInfo>>>();
        std::optional<std::shared_ptr<const FunctionInfo>> mainFunctionInfo;
        for (auto &&functionDefinition : *(functionDefinitions)) {
            auto localsMap = computeVariableLocations(functionDefinition);
            auto codeBlockMap = generateCodeBlockMap(functionDefinition);
            auto functionInfo = std::make_shared<const DefinedFunctionInfo>(functionDefinition->name, localsMap, codeBlockMap);
            definedFunctionInfoMap->insert_or_assign(localsMap->functionName, functionInfo);
            if (functionInfo->functionName == MAIN) {
                mainFunctionInfo = std::optional<std::shared_ptr<const FunctionInfo>>(functionInfo);
            }
        }

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
                codeBlocks->push_back(std::make_shared<SNESAssembly::SNESAssemblyCodeBlock>(mangleSetupCodeBlockLabel(definedFunctionInfo), std::string("Preserve registers"), setupChunks));
            }

            for (auto &&codeBlock : *(functionDefinition->codeBlocks)) {
                auto segments = std::make_shared<std::vector<const std::shared_ptr<const SNESAssembly::SNESAssemblySegment>>>();
                for (auto &&instruction : *(codeBlock->instructions)) {
                    auto instructionChunk = std::visit([this, definedFunctionInfo](auto&& arg){return convertNonTerminatorInstruction(arg, definedFunctionInfo);}, *instruction);
                    segments->push_back(instructionChunk);
                }
                auto instructionChunk = std::visit([this, definedFunctionInfo](auto&& arg){return convertTerminatorInstruction(arg, definedFunctionInfo);}, *(codeBlock->terminator));
                segments->push_back(instructionChunk);
                auto mangledCodeBlockLabel = mangleCodeBlockLabel(definedFunctionInfo, definedFunctionInfo->codeBlocksMap->getCodeblockInfo(codeBlock->label));
                codeBlocks->push_back(std::make_shared<SNESAssembly::SNESAssemblyCodeBlock>(mangledCodeBlockLabel, segments));
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

    std::shared_ptr<LocalsMap> CodeGenerator::computeVariableLocations(std::shared_ptr<const Expressions::ExpressionFunctionDefinition> functionDefinition) {
        
        auto returnType = functionDefinition->returnType->returnType;
        // TODO handle parameters
        auto paramLocals = std::make_shared<LocalsTypeList>();
        std::cout << "[WARNING] NYI: function parameters are not included in the locals memory map." << '\n';

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
        for (auto &&codeBlock : *(functionDefinition->codeBlocks))
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
        return LocalsMap::createLocalsMap(functionDefinition->name, paramLocals, instructionLocals, returnType);
    } // computeVariableLocations

    std::shared_ptr<const CodeBlockMap> CodeGenerator::generateCodeBlockMap(std::shared_ptr<const Expressions::ExpressionFunctionDefinition> functionDefinition) {
        auto codeBlockMapRaw = std::make_shared<std::map<std::string, std::shared_ptr<const CodeBlockInfo>>>();
        for (auto &&codeBlock : *(functionDefinition->codeBlocks)) {
            codeBlockMapRaw->insert_or_assign(codeBlock->label, std::make_shared<CodeBlockInfo>(codeBlock->label));
        }
        return std::make_shared<const CodeBlockMap>(codeBlockMapRaw);
    }
}