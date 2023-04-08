#include "SNESAssemblyModule.h"

namespace SNESAssembly {
    constexpr auto SNESInitInclude = ".INCLUDE \"SNES_Init.asm\"";
    constexpr auto functionSectionHeader = "FUNCTIONS";
    constexpr auto sectionHeaderBars = "=========";

    std::shared_ptr<const std::vector<const std::string>> SNESAssemblyModule::getASMLines() const {
        auto lines = std::make_shared<std::vector<const std::string>>();
        if (isMainModule) {
            lines->push_back(SNESInitInclude);
            lines->push_back("");
            lines->push_back("");
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