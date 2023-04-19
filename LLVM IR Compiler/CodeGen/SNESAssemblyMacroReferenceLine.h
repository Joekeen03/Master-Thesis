#ifndef LLVM_IR_COMPILER_SNESAssemblyMacroReferenceLine_H
#define LLVM_IR_COMPILER_SNESAssemblyMacroReferenceLine_H

#include <memory>
#include <optional>

#include "SNESAssemblyLine.h"
#include "SNESAssemblyMacroReference.h"
#include "SNESAssemblyComment.h"

namespace SNESAssembly {
    class SNESAssemblyMacroReferenceLine : public SNESAssemblyLine{
        public:
            const std::shared_ptr<const SNESAssemblyMacroReference> macroReference;
            const std::optional<const SNESAssemblyComment> comment;
            SNESAssemblyMacroReferenceLine(std::shared_ptr<const SNESAssemblyMacroReference> macroReferenceArg) : macroReference(macroReferenceArg) {}
            SNESAssemblyMacroReferenceLine(std::shared_ptr<const SNESAssemblyMacroReference> macroReferenceArg, std::string rawComment)
                                        : macroReference(macroReferenceArg), comment(SNESAssemblyComment(rawComment)) {}
            virtual std::string getStringRepresentation() const override {
                auto line = macroReference->getStringRepresentation();
                if (comment) {
                    line += " "+comment->getStringRepresentation();
                }
                return line;
            }
    };
}

#endif // LLVM_IR_COMPILER_SNESAssemblyMacroReferenceLine_H
