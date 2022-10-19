#include "SNESMCTargetDesc.h"
#include "llvm/MC/TargetRegistry.h"
#include "TargetInfo/SNESTargetInfo.h"

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeSNESTargetMC() {
  // for (Target *T : {&getTheSNESTarget()}) {
  //   TargetRegistry::RegisterMCAsmInfo(*T, createRISCVMCAsmInfo);
  //   TargetRegistry::RegisterMCObjectFileInfo(*T, createRISCVMCObjectFileInfo);
  //   TargetRegistry::RegisterMCInstrInfo(*T, createRISCVMCInstrInfo);
  //   TargetRegistry::RegisterMCRegInfo(*T, createRISCVMCRegisterInfo);
  //   TargetRegistry::RegisterMCAsmBackend(*T, createRISCVAsmBackend);
  //   TargetRegistry::RegisterMCCodeEmitter(*T, createRISCVMCCodeEmitter);
  //   TargetRegistry::RegisterMCInstPrinter(*T, createRISCVMCInstPrinter);
  //   TargetRegistry::RegisterMCSubtargetInfo(*T, createRISCVMCSubtargetInfo);
  //   TargetRegistry::RegisterELFStreamer(*T, createRISCVELFStreamer);
  //   TargetRegistry::RegisterObjectTargetStreamer(
  //       *T, createRISCVObjectTargetStreamer);
  //   TargetRegistry::RegisterMCInstrAnalysis(*T, createRISCVInstrAnalysis);

  //   // Register the asm target streamer.
  //   TargetRegistry::RegisterAsmTargetStreamer(*T, createRISCVAsmTargetStreamer);
  //   // Register the null target streamer.
  //   TargetRegistry::RegisterNullTargetStreamer(*T,
  //                                              createRISCVNullTargetStreamer);
  // }
}
