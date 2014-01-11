

void dvmCompilerInitializeRegAlloc(CompilationUnit *cUnit)
{
      int numTemps = sizeof(coreTemps)/sizeof(int);
      RegisterPool *pool = dvmCompilerNew(sizeof(*pool), true);
      if(NULL == pool){
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
