

#ifndef SNES_65816C_TARGETMACHINE_H
#define SNES_65816C_TARGETMACHINE_H

#include "llvm/Target/TargetMachine.h"
#include "llvm/MC/TargetRegistry.h"

namespace llvm {
    class SNESTargetMachine : public TargetMachine {
        public:
            SNESTargetMachine(const Target &T, StringRef DataLayoutString,
                const Triple &TargetTriple, StringRef CPU, StringRef FS,
                const TargetOptions &Options) : TargetMachine(T, DataLayoutString, TargetTriple, CPU, FS, Options) { }
    };
}


#endif // SNES_65816C_TARGETMACHINE_H