#include "SNESAssemblyFunction.h"

#include "../lib/General.h"

namespace SNESAssembly {
    const char* functionPrefix = "Function_";

    std::shared_ptr<const std::vector<const std::string>> SNESAssemblyFunction::getASMLines() const {
        auto asmStrings = std::make_shared<std::vector<const std::string>>();
        std::string fullName = functionPrefix+name;
        asmStrings->push_back(SNESAssembly::SNESAssemblyLineLabel(fullName).getStringRepresentation());
        bool firstBlock = true;
        for (auto &&codeBlock : *codeBlocks) {
            // FIXME better way to do this?
            if (firstBlock) {
                firstBlock = false;
            } else {
                asmStrings->push_back("\t");
            }
            auto codeBlockASM = codeBlock->getASMLines(name);
            for (auto &&line : *codeBlockASM)
            {
                asmStrings->push_back('\t'+line);
            }
        }
        asmStrings->push_back(SNESAssembly::SNESAssemblyLineComment(fullName).getStringRepresentation());
        return Lib::makePointerToConst(asmStrings);
    }
}