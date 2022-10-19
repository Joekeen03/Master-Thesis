
// FIXME: Should I instead have the target machine be 65816c, with SNES as a subtarget?

#include "SNESTargetInfo.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

Target &llvm::getTheSNESTarget() {
  static Target TheSNESTarget;
  return TheSNESTarget;
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeSNESTargetInfo() {
  RegisterTarget<Triple::UnknownArch, /*HasJIT=*/false> X(
      getTheSNESTarget(), "snes", "SNES", "SNES");
}