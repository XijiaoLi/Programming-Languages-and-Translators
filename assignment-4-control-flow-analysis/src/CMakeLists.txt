add_llvm_library( LLVMcfg MODULE BUILDTREE_ONLY
  hw4-cfg.cpp

  DEPENDS
  intrinsics_gen
  PLUGIN_TOOL
  opt
)
