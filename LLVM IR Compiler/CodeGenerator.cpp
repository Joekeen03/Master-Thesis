#include "CodeGenerator.h"

#include "Lib/General.h"
#include "CodeGen/SNESAssemblyModule.h"
#include "CodeGen/SNESAssemblyFunction.h"
#include "CodeGen/SNESAssemblyCodeBlock.h"
#include "CodeGen/SNESAssemblyLineInstruction.h"
#include "CodeGen/SNESAssemblySegmentInstructionChunk.h"

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

namespace CodeGen {
    const std::string MAIN = "main";
    const unsigned int ALLOCA_STACK_POINTER_ADDRESS = 0;

    // CodeGenerator::convertNonTerminatorInstruction specializations
    
    template <>
    std::shared_ptr<SNESAssembly::SNESAssemblySegmentInstructionChunk> CodeGenerator::convertNonTerminatorInstruction (
        std::monostate nonInstruction, std::shared_ptr<LocalsMap> localsMap
    ) {
        throw std::runtime_error("CodeGenerator::convertNonTerminator received a monostate instance.");
    }

    template <>
    std::shared_ptr<SNESAssembly::SNESAssemblySegmentInstructionChunk> CodeGenerator::convertNonTerminatorInstruction (
        Instructions::InstructionAlloca instruction, std::shared_ptr<LocalsMap> localsMap
    ) {
        
        auto SNESInstructions = std::vector<const std::shared_ptr<const SNESAssembly::SNESInstruction>>();
        
        // Load alloca pointer to accumulator
        SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionLoadAccumulatorFromMemory<AddressingModes::Absolute>>(ALLOCA_STACK_POINTER_ADDRESS));

        // Store accumulator (pointer to allocated object) to yielded variable - what if this instruction sequence is interrupted?
        // I believe better practice would be to save the allocated object's pointer to the yielded variable after the alloca stack
        //  pointer has been updated (space on the stack has actually been allocated), but given our programs are single-
        //  threaded, and just how the SNES works, I think it's acceptable to not do so here, especially since it would take
        //  extra instructions to juggle around the allocated object's pointer.
        auto stackOffset = localsMap->getLocalEntry(instruction.assignee).second;
        SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionStoreAccumulatorToMemory<AddressingModes::StackRelative>>(stackOffset));

        SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionClearCarryFlag>());
        // Add sizeof(type) to accumulator
        auto typeSize = instruction.allocationType->getSizeBytesCeil(); // TODO handle cases where alloca allocates multiple of
        //                                                              // the specified type (NumElements > 1).
        SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionAddWithCarry<AddressingModes::Immediate>>(typeSize));

        // Store accumulator to alloca pointer (only a 16-bit bank-local address)
        SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionStoreAccumulatorToMemory<AddressingModes::Absolute>>(ALLOCA_STACK_POINTER_ADDRESS));

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
        Instructions::InstructionStore instruction, std::shared_ptr<LocalsMap> localsMap
    ) {
        
        auto SNESInstructions = std::vector<const std::shared_ptr<const SNESAssembly::SNESInstruction>>();
        // Logic:
        //  -If the value to store is a literal value (not stored in a local/gloabl variable in the LLVM IR), load its
        //      bytes directly into the target location.
        //  -Otherwise...
        //      If the value to store is less than 8 bytes, copy it from the source location to the target location
        //          via the accumulator (LDA & STA)
        //      Otherwise, copy it directly from the source location to the target location via a block move

        std::visit([instruction, localsMap, &SNESInstructions](auto&& operand) {
            auto destinationOffset = localsMap->getLocalEntry(instruction.pointerOperand).second;
            auto bytesToTransfer = instruction.valueType->getSizeBytesCeil();
            if constexpr (std::is_base_of_v<Expressions::ExpressionLiteral, std::remove_reference_t<decltype(operand)>>) { // Value is a literal
                Expressions::ExpressionLiteralInteger literalInteger = operand;
                auto val = literalInteger.literal;
                // Initialize index register to zero
                SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionLoadYRegisterFromMemory<AddressingModes::Immediate>>(0));

                // Transfer all bytes like so, except for the last one if the # of bytes is odd.
                // FIXME Have larger literal values stored in the ROM somewhere, and use a block move to copy them instead.
                for (int bytesTransferred = 0; bytesTransferred < (bytesToTransfer-1); bytesTransferred+=2) {
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
                    SNESInstructions.push_back(std::make_shared<SNESAssembly::SNESInstructionSetProcessorStatusBits>(0x20));

                    // Transfer 1 byte.
                    unsigned int immediateByte = (val >> (8*(bytesToTransfer-1))) & 0xFF;
                    SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionLoadAccumulatorFromMemory<AddressingModes::Immediate>>(immediateByte));
                    SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionStoreAccumulatorToMemory<AddressingModes::StackRelativeIndirectIndexed>>(destinationOffset));

                    // Switch to 16-bit accumulator
                    SNESInstructions.push_back(std::make_shared<SNESAssembly::SNESInstructionResetProcessorStatusBits>(0x20));
                }
                
            } else { // operand is an identifier.
                auto sourceOffset = localsMap->getLocalEntry(operand).second;
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
                    for (int bytesTransferred = 0; bytesTransferred < (bytesToTransfer-1); bytesTransferred+=2) {
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
                        SNESInstructions.push_back(std::make_shared<SNESAssembly::SNESInstructionSetProcessorStatusBits>(0x20));

                        // Transfer 1 byte.
                        SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionLoadAccumulatorFromMemory<AddressingModes::StackRelative>>(sourceOffset));
                        SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionStoreAccumulatorToMemory<AddressingModes::StackRelativeIndirectIndexed>>(destinationOffset));

                        // Switch to 16-bit accumulator
                        SNESInstructions.push_back(std::make_shared<SNESAssembly::SNESInstructionResetProcessorStatusBits>(0x20));
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

    // CodeGenerator::convertTerminatorInstruction specializations

    template <>
    std::shared_ptr<SNESAssembly::SNESAssemblySegmentInstructionChunk> CodeGenerator::convertTerminatorInstruction (
        std::monostate nonInstruction, std::shared_ptr<LocalsMap> localsMap
    ) {
        throw std::runtime_error("CodeGenerator::convertTerminatorInstruction received a monostate instance.");
    }

    template <>
    std::shared_ptr<SNESAssembly::SNESAssemblySegmentInstructionChunk> CodeGenerator::convertTerminatorInstruction (
        Instructions::InstructionRetValue instruction, std::shared_ptr<LocalsMap> localsMap
    ) {
        
        auto SNESInstructions = std::vector<const std::shared_ptr<const SNESAssembly::SNESInstruction>>();
        std::visit([instruction, localsMap, &SNESInstructions](auto&& operand){
            // Similar to the operand visitor lambda in convertNonTerminatorInstruction<InstructionStore>(...)
            auto destinationOffset = localsMap->returnEntry.second;
            auto bytesToTransfer = instruction.returnType->getSizeBytesCeil();
            if constexpr (std::is_base_of_v<Expressions::ExpressionLiteral, std::remove_reference_t<decltype(operand)>>) {
                int literal = operand.literal;
                for (int bytesTransferred = 0; bytesTransferred < (bytesToTransfer-1); bytesTransferred+=2) {
                    auto immediateBytes = (literal >> (8*bytesTransferred)) & 0xFFFF; // Next two bytes to store. Should this be unsigned?
                    SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionLoadAccumulatorFromMemory<AddressingModes::Immediate>>(immediateBytes));
                    SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionStoreAccumulatorToMemory<AddressingModes::StackRelative>>(destinationOffset+bytesTransferred));
                }
                if (bytesToTransfer%1) {
                    // Switch to 8-bit accumulator
                    // FIXME magic number should be a defined constant. Maybe a constant that holds the REP/SEP instructions
                    //  to change accumulator modes?
                    SNESInstructions.push_back(std::make_shared<SNESAssembly::SNESInstructionSetProcessorStatusBits>(0x20));
                    unsigned int immediateByte = (literal >> (8*(bytesToTransfer-1))) & 0xFF;
                    SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionLoadAccumulatorFromMemory<AddressingModes::Immediate>>(immediateByte));
                    SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionStoreAccumulatorToMemory<AddressingModes::StackRelative>>(destinationOffset+bytesToTransfer-1));

                    // Switch to 16-bit accumulator
                    SNESInstructions.push_back(std::make_shared<SNESAssembly::SNESInstructionResetProcessorStatusBits>(0x20));
                }
            } else {
                auto sourceOffset = localsMap->getLocalEntry(operand).second;
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
                    for (int bytesTransferred = 0; bytesTransferred < (bytesToTransfer-1); bytesTransferred+=2) {
                        SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionLoadAccumulatorFromMemory<AddressingModes::StackRelative>>(sourceOffset+bytesTransferred));
                        SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionStoreAccumulatorToMemory<AddressingModes::StackRelative>>(destinationOffset+bytesTransferred));
                    }
                    if (bytesToTransfer%1) {
                        // Switch to 8-bit accumulator
                        // FIXME magic number should be a defined constant. Maybe a constant that holds the REP/SEP instructions
                        //  to change accumulator modes?
                        SNESInstructions.push_back(std::make_shared<SNESAssembly::SNESInstructionSetProcessorStatusBits>(0x20));
                        SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionLoadAccumulatorFromMemory<AddressingModes::StackRelative>>(sourceOffset+bytesToTransfer-1));
                        SNESInstructions.push_back(std::make_shared<const SNESAssembly::SNESInstructionStoreAccumulatorToMemory<AddressingModes::StackRelative>>(destinationOffset+bytesToTransfer-1));

                        // Switch to 16-bit accumulator
                        SNESInstructions.push_back(std::make_shared<SNESAssembly::SNESInstructionResetProcessorStatusBits>(0x20));
                    }
                }
            }
        }, *((instruction.returnOperand)->operand));
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
        auto localsMaps = std::make_shared<std::map<std::string, std::shared_ptr<LocalsMap>>>();
        for (auto &&functionDefinition : *(functionDefinitions)) {
            auto localsMap = computeVariableLocations(functionDefinition);
            localsMaps->insert_or_assign(localsMap->functionName, localsMap);
        }

        // TODO: Generate SNES 65c816 instructions.
        auto generatedNormalFunctions = std::make_shared<std::vector<const std::shared_ptr<const SNESAssembly::SNESAssemblyFunction>>>();
        // TODO separate vector for special functions, like interrupt handlers & main.
        for (auto &&functionDefinition : *(functionDefinitions)) {
            auto functionName = functionDefinition->name;
            auto localsMap = localsMaps->at(functionName);
            auto codeBlocks = std::make_shared<std::vector<const std::shared_ptr<const SNESAssembly::SNESAssemblyCodeBlock>>>();
            
            // Generate setup instructions
            {
                auto setupInstructions = std::make_shared<std::vector<const std::shared_ptr<const SNESAssembly::SNESInstruction>>>();
                setupInstructions->push_back(std::make_shared<const SNESAssembly::SNESInstructionPushAccumulator>());
                setupInstructions->push_back(std::make_shared<const SNESAssembly::SNESInstructionPushIndexRegisterX>());
                setupInstructions->push_back(std::make_shared<const SNESAssembly::SNESInstructionPushIndexRegisterY>());
                setupInstructions->push_back(std::make_shared<const SNESAssembly::SNESInstructionPushDirectPage>());
                // Save caller's alloca stack pointer
                setupInstructions->push_back(std::make_shared<const SNESAssembly::SNESInstructionPushEffectiveIndirectAddress>(ALLOCA_STACK_POINTER_ADDRESS));
                auto setupLines = std::make_shared<std::vector<const std::shared_ptr<const SNESAssembly::SNESAssemblyLineInstruction>>>();
                for (auto &&instruction : *setupInstructions) {
                    setupLines->push_back(std::make_shared<const SNESAssembly::SNESAssemblyLineInstruction>(instruction));
                }
                auto setupChunks = std::make_shared<std::vector<const std::shared_ptr<const SNESAssembly::SNESAssemblySegment>>>();
                setupChunks->push_back(std::make_shared<SNESAssembly::SNESAssemblySegmentInstructionChunk>(setupLines));
                
                // FIXME function block should prepend its name to all code block labels w/in it automatically. Or something.
                //  Also, it should do likewise for any intra-function jumps to other code blocks.
                codeBlocks->push_back(std::make_shared<SNESAssembly::SNESAssemblyCodeBlock>(std::string("setup"), std::string("Preserve registers"), setupChunks));
            }

            // TODO: Convert LLVM IR instructions to SNES 65c816 instructions
            for (auto &&codeBlock : *(functionDefinition->codeBlocks)) {
                auto segments = std::make_shared<std::vector<const std::shared_ptr<const SNESAssembly::SNESAssemblySegment>>>();
                for (auto &&instruction : *(codeBlock->instructions)) {
                    auto instructionChunk = std::visit([this, localsMap](auto&& arg){return convertNonTerminatorInstruction(arg, localsMap);}, *instruction);
                    segments->push_back(instructionChunk);
                }
                auto instructionChunk = std::visit([this, localsMap](auto&& arg){return convertTerminatorInstruction(arg, localsMap);}, *(codeBlock->terminator));
                segments->push_back(instructionChunk);
                codeBlocks->push_back(std::make_shared<SNESAssembly::SNESAssemblyCodeBlock>(codeBlock->label, segments));
            }
            
            {
                // Generate cleanup instructions
                auto cleanupInstructions = std::make_shared<std::vector<const std::shared_ptr<const SNESAssembly::SNESInstruction>>>();
                // Restore caller's alloca stack pointer
                // WARNING: assume accumulator is in 16-bit mode; assume active data bank (?) is the one with the alloca stack.
                cleanupInstructions->push_back(std::make_shared<const SNESAssembly::SNESInstructionPopAccumulator>());
                cleanupInstructions->push_back(std::make_shared<const SNESAssembly::SNESInstructionStoreAccumulatorToMemory<AddressingModes::Absolute>>(ALLOCA_STACK_POINTER_ADDRESS)); // Should be absolute long for safety?
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
                
                codeBlocks->push_back(std::make_shared<SNESAssembly::SNESAssemblyCodeBlock>(std::string("cleanup"), std::string("Restore registers"), cleanupChunks));
                // FIXME missing JSR instruction. Also, might eventually need to determine if a subroutine will be called across
                //  banks (JSL), or only w/in its own bank (JSR).
            }
            auto function = std::make_shared<SNESAssembly::SNESAssemblyFunction>(functionDefinition->name, codeBlocks);
            generatedNormalFunctions->push_back(function);
        }
        auto generatedFunctions = std::make_shared<std::vector<const std::shared_ptr<const SNESAssembly::SNESAssemblyFunction>>>(*generatedNormalFunctions);
        auto module = SNESAssembly::SNESAssemblyModule(generatedFunctions, true);
            
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