/*根据物理寄存器的个数分配内存*/
void dvmCompilerInitializeRegAlloc(CompilationUnit *cUnit)
{
      int numTemps = sizeof(coreTemps)/sizeof(int);
      RegisterPool *pool = dvmCompilerNew(sizeof(*pool), true);

      if(NULL == pool) {
	  	printf("error:malloc RegisterPool\n");	
      }
      cUnit->regPool = pool;
      pool->numCoreTemps = numTemps;
      pool->coreTemps =
              dvmCompilerNew(numTemps * sizeof(*pool->coreTemps), true);
      if(NULL == pool->coreTemps){
	  	printf("error:malloc coreTemps\n");
      }
      pool->numFPTemps = 0;
      pool->FPTemps = NULL;
      pool->numCoreRegs = 0;
      pool->coreRegs = NULL;
      pool->numFPRegs = 0;
      pool->FPRegs = NULL;

      dvmCompilerInitPool(pool->coreTemps, coreTemps, pool->numCoreTemps);
      dvmCompilerInitPool(pool->FPTemps, NULL, 0);
      dvmCompilerInitPool(pool->coreRegs, NULL, 0);
      dvmCompilerInitPool(pool->FPRegs, NULL, 0);
//      pool->nullCheckedRegs =
//        dvmCompilerAllocBitVector(cUnit->numSSARegs, false);
}

static void genMultiplyByTwoBitMultiplier(CompilationUnit *cUnit,
        RegLocation rlSrc, RegLocation rlResult, int lit,
        int firstBit, int secondBit)
{
    // We can't implement "add src, src, src, lsl#shift" on Thumb, so we have
    // to do a regular multiply.
    opRegRegImm(cUnit, kOpMul, rlResult.lowReg, rlSrc.lowReg, lit);
}             
