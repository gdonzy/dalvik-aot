extern UnicoreLIR* dvmCompilerRegCopy(CompilationUnit *cUnit, int rDest, int rSrc); 

extern void dvmCompilerFlushRegImpl(CompilationUnit *cUnit, int rBase, int displacement, int rSrc, OpSize size);

extern void dvmCompilerFlushRegWideImpl(CompilationUnit *cUnit, int rBase, int displacement, int rSrcLo, int rSrcHi);

extern int dvmCompilerAllocTypedTemp(CompilationUnit *cUnit, bool fpHint, int regClass);
