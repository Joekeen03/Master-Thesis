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
            SNESTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                     StringRef FS, const TargetOptions &Options,
                     Optional<Reloc::Model> RM, Optional<CodeModel::Model> CM,
                     CodeGenOpt::Level OL, bool JIT);
    };
}


#endif // SNES_65816C_TARGETMACHINE_H