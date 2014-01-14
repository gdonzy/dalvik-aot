
void dvmCompilerMIR2LIR(CompilationUnit *cUnit);
void dvmCompilerInitializeRegAlloc(CompilationUnit *cUnit);

/* Implemented in codegen/<target>/Ralloc.c */
void dvmCompilerRegAlloc(CompilationUnit *cUnit);  

void debugDvmCompilerAssembleLIR(BasicBlock *bb);
