#ifndef LLVM_IR_COMPILER_SNESAssemblyLineLabel_H
#define LLVM_IR_COMPILER_SNESAssemblyLineLabel_H

#include "SNESAssemblyLine.h"
#include "SNESAssemblyLabel.h"

namespace SNESAssembly {
    // Represents a line of SNES-compatible 65c816 ASM which holds a label.
    // Basically a SNESAssemblyLine wrapper for a label.
    class SNESAssemblyLineLabel : public SNESAssemblyLine {
        public:
            const SNESAssemblyLabel label;
            SNESAssemblyLineLabel(SNESAssemblyLabel labelArg) : label(labelArg) {}
            SNESAssemblyLineLabel(std::string rawLabel) : label(rawLabel) {}
            virtual inline std::string getStringRepresentation() const { return label.getStringRepresentation(); }
    };

    // FIXME Is this wrapper actually necessary?
}

#endif // LLVM_IR_COMPILER_SNESAssemblyLineLabel_H
