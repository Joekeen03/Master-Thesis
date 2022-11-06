#include "SNESTargetMachine.h"
#include "TargetInfo/SNESTargetInfo.h"

using namespace llvm;

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeSNESTarget() {
  RegisterTargetMachine<SNESTargetMachine> X(getTheSNESTarget());
}

static StringRef getDataLayout() {
  return "e-p:8:8-i:8:8-i:16:8";
}

SNESTargetMachine::SNESTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                     StringRef FS, const TargetOptions &Options,
                     Optional<Reloc::Model> RM, Optional<CodeModel::Model> CM,
                     CodeGenOpt::Level OL, bool JIT) : SNESTargetMachine(T, getDataLayout(), TT, CPU, FS, Options) {
  
}