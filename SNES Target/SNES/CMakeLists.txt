add_llvm_component_group(SNES)

set(LLVM_TARGET_DEFINITIONS SNES.td)

add_llvm_target(SNESCodeGen
  SNESTargetMachine.cpp

  LINK_COMPONENTS
  Analysis
  AsmPrinter
  Core
  IPO
  CodeGen
  MC
  RISCVDesc
  RISCVInfo
  SelectionDAG
  Support
  Target
  TransformUtils
  GlobalISel

  ADD_TO_COMPONENT
  SNES
)

add_subdirectory(TargetInfo)
add_subdirectory(MCTargetDesc)