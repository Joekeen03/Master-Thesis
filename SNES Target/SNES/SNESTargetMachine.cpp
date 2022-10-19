#include "SNESTargetMachine.h"
#include "TargetInfo/SNESTargetInfo.h"

using namespace llvm;

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeSNESTarget() {
  //RegisterTargetMachine<SNESTargetMachine> X(getTheSNESTarget());
}