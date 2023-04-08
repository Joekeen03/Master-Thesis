#include "SNESAssemblySegmentInstructionChunk.h"

#include <iostream>

namespace SNESAssembly {
    std::shared_ptr<const std::vector<const std::string>> SNESAssemblySegmentInstructionChunk::getASMLines() const{
        auto vec = std::make_shared<std::vector<const std::string>>();
        if (leadingComment) vec->push_back(leadingComment.value().getStringRepresentation());
        
        for (auto &&instruction : *instructions) {
            vec->push_back(instruction->getStringRepresentation());
        }
        return vec;
    }
}