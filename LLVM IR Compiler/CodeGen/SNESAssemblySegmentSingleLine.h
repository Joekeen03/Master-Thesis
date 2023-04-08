#ifndef LLVM_IR_COMPILER_SNESAssemblySegmentSingleLine_H
#define LLVM_IR_COMPILER_SNESAssemblySegmentSingleLine_H

#include <variant>

#include "SNESAssemblySegment.h"
#include "SNESAssemblyLine.h"
#include "SNESAssemblyLineComment.h"
#include "SNESAssemblyLineInstruction.h"

namespace SNESAssembly {
    using SegmentLine = std::variant<SNESAssemblyLineComment, SNESAssemblyLineInstruction>;
    // Segment wrapper for comment and instruction lines. Specifically, wraps SNESAssemblyLineComment & SNESAssemblyLineInstruction
    class SNESAssemblySegmentSingleLine : public SNESAssemblySegment {
        public:
            const SegmentLine line;
            SNESAssemblySegmentSingleLine(SegmentLine lineArg) : line(lineArg) {}
            virtual std::shared_ptr<const std::vector<const std::string>> getASMLines() const override {
                auto vec = std::vector<const std::string>({std::visit(&SNESAssemblyLine::getStringRepresentation, line)});
                return std::make_shared<const std::vector<const std::string>>(vec);
            }

            virtual int getBlockID() const override { return 1; }
    };
}


#endif // LLVM_IR_COMPILER_SNESAssemblySegmentSingleLine_H
