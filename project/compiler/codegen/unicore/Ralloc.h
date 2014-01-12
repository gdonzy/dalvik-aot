static inline int dvmCompilerSSASrc(MIR *mir, int num)
{
	return mir->ssaRep->uses[num];
}

extern void dvmCompilerInitPool(RegisterInfo *regs, int *regNums, int num);
extern RegLocation dvmCompilerGetSrcWide(CompilationUnit *cUnit, MIR *mir, int low, int high);
extern RegLocation dvmCompilerGetDestWide(CompilationUnit *cUnit, MIR *mir, int low, int high);
extern RegLocation dvmCompilerGetSrc(CompilationUnit *cUnit, MIR *mir, int num); 
extern RegLocation dvmCompilerGetDest(CompilationUnit *cUnit, MIR *mir, int num);
