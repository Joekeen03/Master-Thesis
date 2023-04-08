#ifndef LLVM_IR_COMPILER_SNESAssemblyCodeBlock_H
#define LLVM_IR_COMPILER_SNESAssemblyCodeBlock_H

#include <optional>
#include <memory>
#include <vector>
#include <string>

#include "SNESAssemblySegment.h"
#include "SNESAssemblyLineComment.h"
#include "SNESAssemblyLineLabel.h"

namespace SNESAssembly {
    using segmentVectorPtr = std::shared_ptr<const std::vector<const std::shared_ptr<const SNESAssemblySegment>>>;
    class SNESAssemblyCodeBlock {
        public:
            const std::optional<SNESAssemblyLineComment> comment;
            const std::optional<SNESAssemblyLineLabel> label;
            const segmentVectorPtr instructions;
            // FIXME Are these constructors clear and unambiguous? I believe it can also take strings or the corresponding
            //  label/comment objects as arguments, given the converison constructors in the LineLabel/LineComment objects

            SNESAssemblyCodeBlock(SNESAssemblyLineLabel labelArg, segmentVectorPtr instructionsArg)
                                    : label(labelArg), instructions(instructionsArg) {}
            SNESAssemblyCodeBlock(std::string labelArg, segmentVectorPtr instructionsArg)
                                    : label(labelArg), instructions(instructionsArg) {}
            SNESAssemblyCodeBlock(SNESAssemblyLineLabel labelArg, SNESAssemblyLineComment commentArg, segmentVectorPtr instructionsArg)
                                    : comment(commentArg), label(labelArg), instructions(instructionsArg) {}
            std::shared_ptr<const std::vector<const std::string>> getASMLines(std::string functionName) const;
    };
}

#endif // LLVM_IR_COMPILER_SNESAssemblyCodeBlock_H
