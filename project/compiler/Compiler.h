void dvmInitializeSSAConversion(struct CompilationUnit *cUnit);   
void dvmCompilerNonLoopAnalysis(struct CompilationUnit *cUnit); 
void dvmCompilerDoSSAConversion(struct CompilationUnit *cUnit, struct BasicBlock *bb);
