add_llvm_component_library(LLVMSNESDesc
  SNESMCTargetDesc.cpp

  LINK_COMPONENTS
  MC
  Support

  ADD_TO_COMPONENT
  SNES
)
