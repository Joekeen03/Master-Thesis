#include "SNESAssemblyFunction.h"

#include "../lib/General.h"
#include "NameMangling.h"

namespace SNESAssembly {
    const char* functionPrefix = "Function_";

    std::shared_ptr<const std::vector<const std::string>> SNESAssemblyFunction::getASMLines() const {
        auto asmStrings = std::make_shared<std::vector<const std::string>>();
        asmStrings->push_back(SNESAssembly::SNESAssemblyLineLabel(label.mangledLabel).getStringRepresentation());
        bool firstBlock = true;
        for (auto &&codeBlock : *codeBlocks) {
            // FIXME better way to do this?
            if (firstBlock) {
                firstBlock = false;
            } else {
                asmStrings->push_back("\t");
            }
            auto codeBlockASM = codeBlock->getASMLines();
            for (auto &&line : *codeBlockASM)
            {
                asmStrings->push_back('\t'+line);
            }
        }
        asmStrings->push_back(SNESAssembly::SNESAssemblyLineComment(label.mangledLabel).getStringRepresentation());
        return Lib::makePointerToConst(asmStrings);
    }
}