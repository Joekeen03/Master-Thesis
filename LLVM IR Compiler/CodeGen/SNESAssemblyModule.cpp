#include "SNESAssemblyModule.h"

#include "NameMangling.h"
#include "SNESAssemblyMacroReference.h"
#include "VariousConstants.h"
#include "ProcessorFlags.h"

#include "SNESInstructions/SNESInstructionJumpToSubroutine.h"
#include "SNESInstructions/SNESInstructionJump.h"
#include "SNESInstructions/SNESInstructionTransferRegister.h"
#include "SNESInstructions/SNESInstructionAddWithCarry.h"
#include "SNESInstructions/SNESInstructionLoadAccumulatorFromMemory.h"
#include "SNESInstructions/SNESInstructionStoreAccumulatorToMemory.h"
#include "SNESInstructions/SNESInstructionClearFlag.h"
#include "SNESInstructions/SNESInstructionPushRegister.h"
#include "SNESInstructions/SNESInstructionPopRegister.h"
#include "SNESInstructions/SNESInstructionStopProcessor.h"
#include "SNESInstructions/SNESInstructionChangeProcessorStatus.h"

namespace SNESAssembly {
    constexpr auto headerInclude = ".INCLUDE \"header.inc\"";
    constexpr auto SNESInitInclude = ".INCLUDE \"SNES_Init.asm\"";
    constexpr auto memoryDirectives = ".INCLUDE \"SNES_Init.asm\"";
    const auto startInterruptLabel = SNESAssemblyLabel("Start");
    const auto initSNESMacroReference = SNESAssemblyMacroReference("InitSNES");
    constexpr auto functionSectionHeader = "FUNCTIONS";
    constexpr auto sectionHeaderBars = "=========";

    std::shared_ptr<const std::vector<const std::string>> SNESAssemblyModule::getASMLines() const {
        auto lines = std::make_shared<std::vector<const std::string>>();
        if (mainFunction) {
            lines->push_back(headerInclude);
            lines->push_back(SNESInitInclude);
            lines->push_back("");
            lines->push_back(SNESAssemblyComment::convertToCommentString(sectionHeaderBars));
            lines->push_back(SNESAssemblyComment::convertToCommentString("INTERRUPTS"));
            lines->push_back(SNESAssemblyComment::convertToCommentString(sectionHeaderBars));
            lines->push_back("");
            // FIXME    This maybe should be generated in the Code Generator?
            //          Should definitely pull from the main function's info, once the calling convention stuff
            //              is implemented there.
            //          Also, should maybe put these instructions into a SNESAssemblyFunction, and use its
            //              getStringRepresentation, along w/ a for-range loop.
            lines->push_back(startInterruptLabel.getStringRepresentation());
            lines->push_back('\t'+initSNESMacroReference.getStringRepresentation());
            // Ensure 16-bit accumulator/index registers
            lines->push_back('\t'+SNESInstructionResetProcessorStatusBits(CodeGen::accumulatorModeFlag).getStringRepresentation());
            lines->push_back('\t'+SNESInstructionResetProcessorStatusBits(CodeGen::indexRegisterModeFlag).getStringRepresentation());
            // Initialize alloca stack pointer
            lines->push_back('\t'+SNESInstructionLoadAccumulatorFromMemory<AddressingModes::Immediate>(2).getStringRepresentation());
            lines->push_back('\t'+SNESInstructionStoreAccumulatorToMemory<AddressingModes::Absolute>(CodeGen::allocaStackPointerAddress).getStringRepresentation());
            // Allocate space for main's return value.
            lines->push_back('\t'+SNESInstructionLoadAccumulatorFromMemory<AddressingModes::Immediate>(0).getStringRepresentation());
            lines->push_back('\t'+SNESInstructionPushAccumulator().getStringRepresentation());
            lines->push_back('\t'+SNESInstructionPushAccumulator().getStringRepresentation());
            // Call main
            lines->push_back('\t'+SNESInstructionJumpSubroutineAbsolute(mainFunction.value()).getStringRepresentation());
            // Deallocate space for main's return value.
            lines->push_back('\t'+SNESInstructionPopAccumulator().getStringRepresentation());
            lines->push_back('\t'+SNESInstructionPopAccumulator().getStringRepresentation());
            lines->push_back('\t'+SNESAssemblyLabel::convertToLabelString("Infinite"));
            lines->push_back("\t\t"+SNESInstructionJumpAbsolute(CodeGen::MangledCodeBlockLabel{"Infinite"}).getStringRepresentation());
            lines->push_back(SNESAssemblyComment::convertToCommentString(startInterruptLabel.rawLabel+" END"));
            lines->push_back("");
        }
        lines->push_back(SNESAssemblyLineComment(sectionHeaderBars).getStringRepresentation());
        lines->push_back(SNESAssemblyLineComment(functionSectionHeader).getStringRepresentation());
        lines->push_back(SNESAssemblyLineComment(sectionHeaderBars).getStringRepresentation());
        lines->push_back("");
        for (auto &&function : *functions) {
            // FIXME lots of temporary vectors being created in the contained function/codeBlock objects - any way to
            //  trim that down?
            auto functionLines = function->getASMLines();
            for (auto &&line : *functionLines) {
                lines->push_back(line);
            }
        }
        return lines;
    }
}