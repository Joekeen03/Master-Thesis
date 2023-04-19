#include "SNESAssemblyModule.h"

#include "NameMangling.h"
#include "SNESAssemblyMacroReference.h"
#include "SNESInstructions/SNESInstructionJumpToSubroutine.h"

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
            lines->push_back(startInterruptLabel.getStringRepresentation());
            lines->push_back('\t'+initSNESMacroReference.getStringRepresentation());
            lines->push_back('\t'+SNESInstructionJumpSubroutineAbsolute(mainFunction.value()).getStringRepresentation());
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