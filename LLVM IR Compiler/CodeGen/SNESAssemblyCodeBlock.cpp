#include "SNESAssemblyCodeBlock.h"

#include <iostream>

#include "../lib/General.h"

namespace SNESAssembly {
    const char* codeBlockPrefix = "CodeBlock_";
    
    std::shared_ptr<const std::vector<const std::string>> SNESAssemblyCodeBlock::getASMLines(std::string functionName) const {
        auto asmStrings = std::make_shared<std::vector<const std::string>>();
        std::string instructionIndent = "";
        // FIXME should this be done in the main code generator.
        auto labelPrefix = codeBlockPrefix+functionName+'_';
        if (comment) {
            asmStrings->push_back(comment->getStringRepresentation());
        }
        if (label) {
            instructionIndent = "\t";
            asmStrings->push_back(labelPrefix+label->getStringRepresentation());
        }

        int lastType = -1;
        for (auto &&instruction : *instructions) {
            if (lastType != -1 && instruction->getBlockID()==lastType) { // Add a newline only between joinable segments of different types.
                asmStrings->push_back(instructionIndent);
            }
            auto lines = instruction->getASMLines();
            for (auto &&line : *lines) {
                asmStrings->push_back(instructionIndent+line);
            }
            lastType = instruction->getBlockID();
        }

        if (label) {
            asmStrings->push_back(SNESAssemblyComment(labelPrefix+label->label.label+" END").getStringRepresentation()); // FIXME label->label.label
        }
        return asmStrings;
    }
}