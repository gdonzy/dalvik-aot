//static void genMulLong(CompilationUnit *cUnit, RegLocation rlDest,
//                       RegLocation rlSrc1, RegLocation rlSrc2)
//{   
//    RegLocation rlResult;
//    loadValueDirectWideFixed(cUnit, rlSrc1, r0, r1);
//    loadValueDirectWideFixed(cUnit, rlSrc2, r2, r3);
//    genDispatchToHandler(cUnit, TEMPLATE_MUL_LONG);
//    rlResult = dvmCompilerGetReturnWide(cUnit);
//    storeValueWide(cUnit, rlDest, rlResult);
//}

static bool partialOverlap(int sreg1, int sreg2)                                           
{                                                                                             
    return abs(sreg1 - sreg2) == 1;                                                        
}           

static void genLong3Addr(CompilationUnit *cUnit, MIR *mir, OpKind firstOp,
                         OpKind secondOp, RegLocation rlDest,
                         RegLocation rlSrc1, RegLocation rlSrc2)
{
    RegLocation rlResult;
    if (partialOverlap(rlSrc1.sRegLow,rlSrc2.sRegLow) ||
        partialOverlap(rlSrc1.sRegLow,rlDest.sRegLow) ||
        partialOverlap(rlSrc2.sRegLow,rlDest.sRegLow)) {
        // Rare case - not enough registers to properly handle
		//eric for simple 
      //  genInterpSingleStep(cUnit, mir);
		LOG("the function is %s\n, the genInterpSingleStep function is not compilted\n", __func__);
    } else if (rlDest.sRegLow == rlSrc1.sRegLow) {
        // Already 2-operand
        rlResult = loadValueWide(cUnit, rlDest, kCoreReg);
        rlSrc2 = loadValueWide(cUnit, rlSrc2, kCoreReg);
        opRegReg(cUnit, firstOp, rlResult.lowReg, rlSrc2.lowReg);
        opRegReg(cUnit, secondOp, rlResult.highReg, rlSrc2.highReg);
        storeValueWide(cUnit, rlDest, rlResult);
    } else if (rlDest.sRegLow == rlSrc2.sRegLow) {
        // Bad case - must use/clobber Src1 and reassign Dest
        rlSrc1 = loadValueWide(cUnit, rlSrc1, kCoreReg);
        rlResult = loadValueWide(cUnit, rlDest, kCoreReg);
        opRegReg(cUnit, firstOp, rlSrc1.lowReg, rlResult.lowReg);
        opRegReg(cUnit, secondOp, rlSrc1.highReg, rlResult.highReg);
        // Old reg assignments are now invalid
        dvmCompilerClobber(cUnit, rlResult.lowReg);
        dvmCompilerClobber(cUnit, rlResult.highReg);
        dvmCompilerClobber(cUnit, rlSrc1.lowReg);
        dvmCompilerClobber(cUnit, rlSrc1.highReg);
        rlDest.location = kLocDalvikFrame;
        assert(rlSrc1.location == kLocPhysReg);
        // Reassign registers - rlDest will now get rlSrc1's old regs
        storeValueWide(cUnit, rlDest, rlSrc1);
    } else {
        // Copy Src1 to Dest
        rlSrc2 = loadValueWide(cUnit, rlSrc2, kCoreReg);
        rlResult = dvmCompilerEvalLoc(cUnit, rlDest, kCoreReg, false);
        loadValueDirectWide(cUnit, rlSrc1, rlResult.lowReg,
                            rlResult.highReg);
        rlResult.location = kLocPhysReg;
        opRegReg(cUnit, firstOp, rlResult.lowReg, rlSrc2.lowReg);
        opRegReg(cUnit, secondOp, rlResult.highReg, rlSrc2.highReg);
        storeValueWide(cUnit, rlDest, rlResult);
    }
}                                                                 

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
