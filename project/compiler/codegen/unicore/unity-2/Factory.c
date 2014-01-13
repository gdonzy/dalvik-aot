
static int coreTemps[] = {r0, r1, r2, r3, r4, r4PC, r7};

static UnicoreLIR *loadConstantNoClobber(CompilationUnit *cUnit, int rDest,
                                     int value)
{
    UnicoreLIR *res;
    //chenglin change
    //int tDest = LOWREG(rDest) ? rDest : dvmCompilerAllocTemp(cUnit);
    int tDest = rDest;
    /* See if the value can be constructed cheaply */
    if ((value >= 0) && (value <= 511)) {
        //res = newLIR2(cUnit, kThumbMovImm, tDest, value);
        res = newLIR2(cUnit, kUnicoreMovImm, tDest, value);
        //if (rDest != tDest) {
        //   opRegReg(cUnit, kOpMov, rDest, tDest);
        //   dvmCompilerFreeTemp(cUnit, tDest);
        //}  
        return res; 
    } else if ((value & 0xFFFFFe00) == 0xFFFFFe00) {
        res = newLIR2(cUnit, kUnicoreMovImm, tDest, ~value);
        //newLIR2(cUnit, kThumbMvn, tDest, tDest);
        newLIR2(cUnit, kUnicoreNotRR, tDest, tDest);
        //if (rDest != tDest) {
        //   opRegReg(cUnit, kOpMov, rDest, tDest);
        //   dvmCompilerFreeTemp(cUnit, tDest);
        //}  
        return res; 
    }    
    /* No shortcut - go ahead and use literal pool */
    //chenglin change
    //ArmLIR *dataTarget = scanLiteralPool(cUnit, value, 255);
    UnicoreLIR *dataTarget = scanLiteralPool(cUnit, value, 511);
    if (dataTarget == NULL) {
        dataTarget = addWordData(cUnit, value, false);
    }    
    UnicoreLIR *loadPcRel = dvmCompilerNew(sizeof(UnicoreLIR), true);
    //chenglin change
    //loadPcRel->opCode = kThumbLdrPcRel;
    loadPcRel->opCode = kUnicoreLdwPcRel;
    loadPcRel->generic.target = (LIR *) dataTarget;
    loadPcRel->operands[0] = tDest;
    setupResourceMasks(loadPcRel);
    /*   
     * Special case for literal loads with a link register target.
     * Self-cosim mode will insert calls prior to heap references
     * after optimization, and those will destroy r14.  The easy
     * workaround is to treat literal loads into r14 as heap references
     * to prevent them from being hoisted.  Use of r14 in this manner
     * is currently rare.  Revist if that changes.
     */
    if (rDest != rlr)
        setMemRefType(loadPcRel, true, kLiteral);
    loadPcRel->aliasInfo = dataTarget->operands[0];
    res = loadPcRel;
    dvmCompilerAppendLIR(cUnit, (LIR *) loadPcRel);

    /*
     * To save space in the constant pool, we use the ADD_RRI8 instruction to
     * add up to 255 to an existing constant value.
     */
    //chenglin change
    if (dataTarget->operands[0] != value) {
        //newLIR2(cUnit, kThumbAddRI8, tDest, value - dataTarget->operands[0]);
        newLIR3(cUnit, kUnicoreAddRRI9, tDest,tDest, value - dataTarget->operands[0]);
    }
    //if (rDest != tDest) {
    //    //chenglin change
    //   opRegReg(cUnit, kOpMov, rDest, tDest);
    //   dvmCompilerFreeTemp(cUnit, tDest);
    //}
    return res;
}

/*
 * Load an immediate value into a fixed or temp register.  Target
 * register is clobbered, and marked inUse.
 */
static UnicoreLIR *loadConstant(CompilationUnit *cUnit, int rDest, int value)                                                                                             
{   
    if (dvmCompilerIsTemp(cUnit, rDest)) {
        dvmCompilerClobber(cUnit, rDest);                                                                                                                                                  
        dvmCompilerMarkInUse(cUnit, rDest);
    }
    return loadConstantNoClobber(cUnit, rDest, value);
}    

static UnicoreLIR *opRegRegReg(CompilationUnit *cUnit, OpKind op, int rDest,                                                                                              
                           int rSrc1, int rSrc2)                                                                                                                      
{   
    UnicoreOpCode opCode = kUnicoreBkpt;                                                                                                                                  
    switch (op) {
        case kOpAdd:
            //opCode = kThumbAddRRR;                                                                                                                                  
            opCode = kUnicoreAddRRR;                                                                                                                                  
            break;
        case kOpSub:
            //opCode = kThumbSubRRR;                                                                                                                                  
        opCode = kUnicoreSubRRR;                                                                                                                                      
            break;                                                                                                                                                    
        default:
            if (rDest == rSrc1) {
                return opRegReg(cUnit, op, rDest, rSrc2);
            } else if (rDest == rSrc2) {
                assert(dvmCompilerIsTemp(cUnit, rSrc1));
                dvmCompilerClobber(cUnit, rSrc1);
                opRegReg(cUnit, op, rSrc1, rSrc2);
                return opRegReg(cUnit, kOpMov, rDest, rSrc1);
            } else {
                opRegReg(cUnit, kOpMov, rDest, rSrc1);
                return opRegReg(cUnit, op, rDest, rSrc2);
            }
            break;
    }
    return newLIR3(cUnit, opCode, rDest, rSrc1, rSrc2);
}

static UnicoreLIR *opRegRegImm(CompilationUnit *cUnit, OpKind op, int rDest,
                           int rSrc1, int value)
{
    UnicoreLIR *res;
    bool neg = (value < 0);
    int absValue = (neg) ? -value : value;
    UnicoreOpCode opCode = kUnicoreBkpt;
    bool shortForm = (absValue & 0x1ff) == absValue;
    //bool shortForm = (absValue & 7) == absValue;
    switch(op) {
        case kOpAdd:
            //if (rDest == rSrc1)
            //    return opRegImm(cUnit, op, rDest, value);
            //if ((rSrc1 == 13) && (value <= 1020)) { /* sp */
            //    assert((value & 0x3) == 0);
            //    shortForm = true;
            //    opCode = kThumbAddSpRel;
            //    value >>= 2;
            //} else if ((rSrc1 == 15) && (value <= 1020)) { /* pc */
            //    assert((value & 0x3) == 0);
            //    shortForm = true;
            //    opCode = kThumbAddPcRel;
            //    value >>= 2;
            //} else if (shortForm) {
            //    opCode = (neg) ? kThumbSubRRI3 : kThumbAddRRI3;
            //} else if ((absValue > 0) && (absValue <= (255 + 7))) {
            //    /* Two shots - 1st handle the 7 */
            //    opCode = (neg) ? kThumbSubRRI3 : kThumbAddRRI3;
            //    res = newLIR3(cUnit, opCode, rDest, rSrc1, 7);
            //    opCode = (neg) ? kThumbSubRI8 : kThumbAddRI8;
            //    newLIR2(cUnit, opCode, rDest, absValue - 7);
            //    return res;
            //} else
            //    opCode = kThumbAddRRR;
        if (rDest == rSrc1)
                return opRegImm(cUnit, op, rDest, value);
            if ((rSrc1 == 29) && (value <= 508)) { /* sp */
                assert((value & 0x3) == 0);
                shortForm = true;
               // opCode = kThumbAddSpRel;
		        opCode = kUnicoreAddSpRel;
//                value >>= 2;
            } else if ((rSrc1 == 31) && (value <= 508)) { /* pc */
                assert((value & 0x3) == 0);
                shortForm = true;
                //opCode = kThumbAddPcRel;
                opCode = kUnicoreAddPcRel;
             //   value >>= 2;
            } else if (shortForm) {
                opCode = (neg) ? kUnicoreSubRRI9 : kUnicoreAddRRI9;
            } else{
                opCode = kUnicoreAddRRR;
        }
            break;

        case kOpSub:
            if (rDest == rSrc1)
                return opRegImm(cUnit, op, rDest, value);
            if (shortForm) {
                opCode = (neg) ? kUnicoreAddRRI9 : kUnicoreSubRRI9;
            } else {
                opCode = kUnicoreSubRRR;
        }
            break;
        case kOpLsl:
                shortForm = (!neg && value <= 31);
        //opCode = kThumbLslRRI5;
                opCode = kUnicoreLslRRI5;
                break;
        case kOpLsr:
                shortForm = (!neg && value <= 31);
                opCode = kUnicoreLsrRRI5;
                break;
        case kOpAsr:
                shortForm = (!neg && value <= 31);
                opCode = kUnicoreAsrRRI5;
                break;
        case kOpMul:
        case kOpAnd:
        case kOpOr:
        case kOpXor:
                if (rDest == rSrc1) {
                    int rScratch = dvmCompilerAllocTemp(cUnit);
                    res = loadConstant(cUnit, rScratch, value);
                    opRegReg(cUnit, op, rDest, rScratch);
                } else {
                    res = loadConstant(cUnit, rDest, value);
                    opRegReg(cUnit, op, rDest, rSrc1);
                }
                return res;
        default:
            printf("Jit: bad case in opRegRegImm");
            //dvmCompilerAbort(cUnit);
            break;
    }
    if (shortForm)
        res = newLIR3(cUnit, opCode, rDest, rSrc1, absValue);
    else {
        if (rDest != rSrc1) {
            res = loadConstant(cUnit, rDest, value);
            newLIR3(cUnit, cUnit->debugBB,opCode, rDest, rSrc1, rDest);
        } else {
            int rScratch = dvmCompilerAllocTemp(cUnit);
            res = loadConstant(cUnit, rScratch, value);
            newLIR3(cUnit, cUnit->debugBB,opCode, rDest, rSrc1, rScratch);
        }
    }
    return res;
}

static UnicoreLIR *opRegReg(CompilationUnit *cUnit, OpKind op, int rDestSrc1,
                        int rSrc2)
{
    UnicoreLIR *res;
    UnicoreOpCode opCode = kUnicoreBkpt;
    switch (op) {
        case kOpAdc:
            //opCode = kThumbAdcRR;
            opCode = kUnicoreAdcRRR;
            break;
        case kOpAnd:
            //opCode = kThumbAndRR;
            opCode = kUnicoreAndRRR;
            break;
        case kOpBic:
            //opCode = kThumbBicRR;
            opCode = kUnicoreAndNRRR;
            break;
        case kOpCmn:
            //opCode = kThumbCmnRR;
            opCode = kUnicoreCmpAddRR;
        return newLIR2(cUnit,cUnit->debugBB, opCode, rDestSrc1, rSrc2);
            break;
        case kOpCmp:
            //opCode = kThumbCmpRR;
            opCode = kUnicoreCmpSubRR;
        return newLIR2(cUnit,cUnit->debugBB, opCode, rDestSrc1, rSrc2);
            break;
        case kOpXor:
            //opCode = kThumbEorRR;
            opCode = kUnicoreXorRRR;
            break;
        case kOpMov:
            //chenglin change
            //if (LOWREG(rDestSrc1) && LOWREG(rSrc2))
            //    opCode = kThumbMovRR;
            //else if (!LOWREG(rDestSrc1) && !LOWREG(rSrc2))
            //    opCode = kThumbMovRR_H2H;
            //else if (LOWREG(rDestSrc1))
            //    opCode = kThumbMovRR_H2L;
            //else
            //    opCode = kThumbMovRR_L2H;
            //break;
                opCode = kUnicoreMovRR; //319
        return newLIR2(cUnit,cUnit->debugBB, opCode, rDestSrc1, rSrc2);
            break;
        case kOpMul:
            //opCode = kThumbMul;
            opCode = kUnicoreMulRRR;
            break;
        case kOpMvn:
            //opCode = kThumbMvn;
            opCode = kUnicoreNotRR;
        return newLIR2(cUnit,cUnit->debugBB ,opCode, rDestSrc1, rSrc2);
            break;
        case kOpNeg:
            //opCode = kThumbNeg;
        opCode = kUnicoreRsubRR;
        return newLIR2(cUnit,cUnit->debugBB, opCode, rDestSrc1, rSrc2);
            break;
        case kOpOr:
            //opCode = kThumbOrr;
            opCode = kUnicoreOrrRRR;
            break;
        case kOpSbc:
            //opCode = kThumbSbc;
            opCode = kUnicoreSubcRRR;
            break;
        case kOpTst:
            //opCode = kThumbTst;  //bug
            opCode = kUnicoreCmpAndRR;
        return newLIR2(cUnit,cUnit->debugBB, opCode, rDestSrc1, rSrc2);
            break;
        case kOpLsl:
            //opCode = kThumbLslRR;
        opCode = kUnicoreLslRRR;
            break;
        case kOpLsr:                                                                                                                                                             
            //opCode = kThumbLsrRR;
            opCode = kUnicoreLsrRRR;
            break;
        case kOpAsr:
            //opCode = kThumbAsrRR;
            opCode = kUnicoreAsrRRR;
            break;
        case kOpRor:
            //opCode = kThumbRorRR;
            opCode = kUnicoreRorRRR;
        case kOpAdd:
        case kOpSub:
            return opRegRegReg(cUnit, op, rDestSrc1, rDestSrc1, rSrc2);
        case kOp2Byte:
             res = opRegRegImm(cUnit, kOpLsl, rDestSrc1, rSrc2, 24);
             opRegRegImm(cUnit, kOpAsr, rDestSrc1, rDestSrc1, 24);
             return res;
        case kOp2Short:
             res = opRegRegImm(cUnit, kOpLsl, rDestSrc1, rSrc2, 16);
             opRegRegImm(cUnit, kOpAsr, rDestSrc1, rDestSrc1, 16);
             return res;
        case kOp2Char:
             res = opRegRegImm(cUnit, kOpLsl, rDestSrc1, rSrc2, 16);
             opRegRegImm(cUnit, kOpLsr, rDestSrc1, rDestSrc1, 16);
             return res;
        default:
            printf("Jit: bad case in opRegReg");
            // dvmCompilerAbort(cUnit);
            break;
    }
    return newLIR3(cUnit,cUnit->debugBB, opCode, rDestSrc1,rDestSrc1, rSrc2);
}

static UnicoreLIR *loadBaseDispBody(CompilationUnit *cUnit, MIR *mir, int rBase,
                                int displacement, int rDest, int rDestHi,
                                OpSize size, int sReg)
/*
 * Load value from base + displacement.  Optionally perform null check
 * on base (which must have an associated sReg and MIR).  If not
 * performing null check, incoming MIR can be null. IMPORTANT: this
 * code must not allocate any new temps.  If a new register is needed
 * and base and dest are the same, spill some other register to
 * rlp and then restore.
 */
{
    UnicoreLIR *res;
    UnicoreLIR *load = NULL;
    UnicoreLIR *load2 = NULL;
    UnicoreOpCode opCode = kUnicoreBkpt;
    bool shortForm = false;
    int encodedDisp = displacement;
    bool pair = false;

    switch (size) {
        case kLong:
        case kDouble:
            //pair = true;
            //if ((displacement < 124) && (displacement >= 0)) {
            //    assert((displacement & 0x3) == 0);
            //    shortForm = true;
            //    encodedDisp >>= 2;
            //    opCode = kThumbLdrRRI5;
            //} else {
            //    opCode = kThumbLdrRRR;
            //}
        	pair = true;
            if ((displacement < 16384) && (displacement >= 0)) {
                assert((displacement & 0x3) == 0);
                shortForm = true;
                //encodedDisp >>= 2;
				//ignore newLIR4，use newLIR3
                opCode = kUnicoreLdwRRI14;
            } else {
                opCode = kUnicoreLdwRRR;
            }
            break;
        case kWord:                                                                   
        //chenglin change
            //if (LOWREG(rDest) && (rBase == rpc) &&
            //    (displacement <= 1020) && (displacement >= 0)) {
            //    shortForm = true;
            //    encodedDisp >>= 2;
            //    opCode = kThumbLdrPcRel;
            //} else if (LOWREG(rDest) && (rBase == r13) &&
            //          (displacement <= 1020) && (displacement >= 0)) {
            //    shortForm = true;
            //    encodedDisp >>= 2;
            //    opCode = kThumbLdrSpRel;
            //} else if (displacement < 128 && displacement >= 0) {
            //    assert((displacement & 0x3) == 0);
            //    shortForm = true;
            //    encodedDisp >>= 2;
            //    opCode = kThumbLdrRRI5;
            //} else {
            //    opCode = kThumbLdrRRR;
            //}
            //break;
        if ((rBase == rpc) &&
                (displacement <= 16380) && (displacement >= 0)) {
                shortForm = true;
               // encodedDisp >>= 2;
                opCode = kUnicoreLdwPcRel; //311p
            } else if ((rBase == r29) &&
                      (displacement <= 16380) && (displacement >= 0)) {
                shortForm = true;
               // encodedDisp >>= 2;
                opCode = kUnicoreLdwSpRel; //311p
            } else if (displacement < 16384 && displacement >= 0) {
                assert((displacement & 0x3) == 0);
                shortForm = true;
               // encodedDisp >>= 2;
                opCode = kUnicoreLdwRRI14; //311p
            } else {
                opCode = kUnicoreLdwRRR; //279p
            }
            break;
        case kUnsignedHalf:
           // if (displacement < 64 && displacement >= 0) {
           //     assert((displacement & 0x1) == 0);
           //     shortForm = true;
           //     encodedDisp >>= 1;
           //     opCode = kThumbLdrhRRI5;
           // } else {
           //     opCode = kThumbLdrhRRR;
           // }
       if (displacement < 1024 && displacement >= 0) {
               assert((displacement & 0x1) == 0);
               shortForm = true;
              // encodedDisp >>= 1;
               opCode = kUnicoreLdhRRI5I5;  //newLIR4
           } else {
               opCode = kUnicoreLdhRRR;
           }
            break;
        case kSignedHalf:
            opCode = kUnicoreLdshRRR;
            break;
        case kUnsignedByte:
            if (displacement < 16384 && displacement >= 0) {
                shortForm = true;
                opCode = kUnicoreLdbRRI14;
            } else {
                opCode = kUnicoreLdbRRR;
            }
            break;
        case kSignedByte:
            opCode = kUnicoreLdsbRRR;
            break;
        default:
            printf("Jit: bad case in loadBaseIndexedBody");
            //dvmCompilerAbort(cUnit);
    }
    if (shortForm) {
        if(opCode == kUnicoreLdhRRI5I5){ //chenglin change and only armv7 define
            load = res = newLIR4(cUnit,cUnit->debugBB,opCode,rDest,rBase,(encodedDisp&0x3e0)>>5,encodedDisp&0x1f);
        }else{
            load = res = newLIR3(cUnit, cUnit->debugBB,opCode, rDest, rBase, encodedDisp);
        }
        if (pair) { //pass for unicore
            load2 = newLIR3(cUnit,cUnit->debugBB, opCode, rDestHi, rBase, encodedDisp+4);
        }
    } else {
        if (pair) { //pass for unicore
            int rTmp = dvmCompilerAllocFreeTemp(cUnit);
            res = opRegRegImm(cUnit,cUnit->debugBB ,kOpAdd, rTmp, rBase, displacement);
            //load = newLIR3(cUnit, kThumbLdrRRI5, rDest, rTmp, 0);
            load = newLIR3(cUnit,cUnit->debugBB ,kUnicoreLdwRRI14, rDest, rTmp, 0);
            //load2 = newLIR3(cUnit, kThumbLdrRRI5, rDestHi, rTmp, 1);
            load2 = newLIR3(cUnit,cUnit->debugBB, kUnicoreLdwRRI14, rDestHi, rTmp, 4);
            dvmCompilerFreeTemp(cUnit, rTmp);
        } else {
            int rTmp = (rBase == rDest) ? dvmCompilerAllocFreeTemp(cUnit)
                                        : rDest;
            res = loadConstant(cUnit, rTmp, displacement);
            load = newLIR3(cUnit, opCode, rDest, rBase, rTmp);
            if (rBase == rFP)
                annotateDalvikRegAccess(load, displacement >> 2,
                                        true /* isLoad */);
            if (rTmp != rDest)
                dvmCompilerFreeTemp(cUnit, rTmp);
        }
    }
    if (rBase == rFP) {
        if (load != NULL)
            annotateDalvikRegAccess(load, displacement >> 2,
                                    true /* isLoad */);
        if (load2 != NULL)
            annotateDalvikRegAccess(load2, (displacement >> 2) + 1,
                                    true /* isLoad */);
    }
	return res;
}

static UnicoreLIR *loadBaseDisp(CompilationUnit *cUnit, MIR *mir, int rBase,
                            int displacement, int rDest, OpSize size,
                            int sReg)
{
    return loadBaseDispBody(cUnit, mir, rBase, displacement, rDest, -1,
                            size, sReg);
}                                          

static UnicoreLIR *storeBaseDispBody(CompilationUnit *cUnit, int rBase,
                                 int displacement, int rSrc, int rSrcHi,
                                 OpSize size)
{
    ArmLIR *res;
    ArmLIR *store = NULL;
    ArmLIR *store2 = NULL;
    ArmOpCode opCode = kUnicoreBkpt;
    bool shortForm = false;
    int encodedDisp = displacement;
    bool pair = false;

    switch (size) {
        case kLong:
        case kDouble:
            //pair = true;                                                                                                                                                                           
            //if ((displacement < 124) && (displacement >= 0)) {
            //    assert((displacement & 0x3) == 0);
            //    pair = true;
            //    shortForm = true;
            //    encodedDisp >>= 2;
            //    opCode = kThumbStrRRI5;
            //} else {
            //    opCode = kThumbStrRRR;
            //}
        pair = true;
            if ((displacement < 16384) && (displacement >= 0)) {
                assert((displacement & 0x3) == 0);
                pair = true;
                shortForm = true;
                //encodedDisp >>= 2;
                opCode = kUnicoreStwRRI14;
            } else {
                opCode = kUnicoreStwRRR;
            }
            break;
        case kWord:
            //if (displacement < 128 && displacement >= 0) {
            //    assert((displacement & 0x3) == 0);
            //    shortForm = true;
            //    encodedDisp >>= 2;
            //    opCode = kThumbStrRRI5;
            //} else {
            //    opCode = kThumbStrRRR;
            //}
            //break;
            //chenglin change
            if (displacement < 16384 && displacement >= 0) {
                assert((displacement & 0x3) == 0);
                shortForm = true;
                //encodedDisp >>= 2;
                opCode = kUnicoreStwRRI14;  //p569
            } else {
                opCode = kUnicoreStwRRR;    //p537
            }
            break;
        case kUnsignedHalf:
        case kSignedHalf:
            //if (displacement < 64 && displacement >= 0) {
            //    assert((displacement & 0x1) == 0);
            //    shortForm = true;
            //    encodedDisp >>= 1;
            //    opCode = kThumbStrhRRI5;
            //} else {
            //    opCode = kThumbStrhRRR;
            //}
        if (displacement < 1024 && displacement >= 0) {
                assert((displacement & 0x1) == 0);
                shortForm = true;
            //    encodedDisp >>= 1;
                opCode = kUnicoreSthRRI5I5;   //newLIR4
            } else {
                opCode = kUnicoreSthRRR;
            }
            break;
        case kUnsignedByte:
        case kSignedByte:
            //if (displacement < 32 && displacement >= 0) {
            //    shortForm = true;
            //    opCode = kThumbStrbRRI5;
            //} else {
            //    opCode = kThumbStrbRRR;
            //}
        if (displacement < 16384 && displacement >= 0) {
                shortForm = true;
                opCode = kUnicoreStbRRI14;
            } else {
                opCode = kUnicoreStbRRR;
            }

            break;
        default:
            LOGE("Jit: bad case in storeBaseIndexedBody");
            dvmCompilerAbort(cUnit);
    }
    if (shortForm) {
    if(opCode == kUnicoreSthRRI5I5){//chenglin change
        store = res = newLIR4(cUnit,cUnit->debugBB,opCode,rSrc,rBase,(encodedDisp&0x3e0)>>5,encodedDisp&0x1f);
    }else{
        store = res = newLIR3(cUnit,cUnit->debugBB, opCode, rSrc, rBase, encodedDisp);
    }
        if (pair) {  //pass for unicore
           // store2 = newLIR3(cUnit, opCode, rSrcHi, rBase, encodedDisp + 1);
            store2 = newLIR3(cUnit,cUnit->debugBB, opCode, rSrcHi, rBase, encodedDisp + 4);
        }
    } else {
        int rScratch = dvmCompilerAllocTemp(cUnit);
        if (pair) { //pass for unicore
            res = opRegRegImm(cUnit, kOpAdd, rScratch, rBase, displacement);
            //store =  newLIR3(cUnit, kThumbStrRRI5, rSrc, rScratch, 0);
            //store2 = newLIR3(cUnit, kThumbStrRRI5, rSrcHi, rScratch, 1);
        store =  newLIR3(cUnit,debugBB, kUnicoreStwRRI14, rSrc, rScratch, 0);
            store2 = newLIR3(cUnit,cUnit->debugBB, kUnicoreStwRRI14, rSrcHi, rScratch, 4);

        } else {
            res = loadConstant(cUnit, rScratch, displacement);
            store = newLIR3(cUnit,cUnit->debugBB,opCode, rSrc, rBase, rScratch);
        }
        dvmCompilerFreeTemp(cUnit, rScratch);
    }
    if (rBase == rFP) {
        if (store != NULL)
            annotateDalvikRegAccess(store, displacement >> 2,
                                    false /* isLoad */);
        if (store2 != NULL)
            annotateDalvikRegAccess(store2, (displacement >> 2) + 1,
                                    false /* isLoad */);
    }
#if defined(WITH_SELF_VERIFICATION)
    if (store != NULL && cUnit->heapMemOp)
        store->branchInsertSV = true;
    if (store2 != NULL && cUnit->heapMemOp)
        store2->branchInsertSV = true;
#endif
    return res;
}

static enicoreLIR *storeBaseDisp(CompilationUnit *cUnit, int rBase,     
							 int displacement, int rSrc, OpSize size)       
{
    return storeBaseDispBody(cUnit, rBase, displacement, rSrc, -1, size);
}

static UnicoreLIR* genRegCopyNoInsert(CompilationUnit *cUnit, int rDest, int rSrc)
{
    UnicoreLIR* res;
    UnicoreOpCode opCode;
    res = dvmCompilerNew(sizeof(UnicoreLIR), true);
	/*
    if (LOWREG(rDest) && LOWREG(rSrc))
        opCode = kThumbMovRR;
    else if (!LOWREG(rDest) && !LOWREG(rSrc))
         opCode = kThumbMovRR_H2H;
    else if (LOWREG(rDest))
         opCode = kThumbMovRR_H2L;
    else
         opCode = kThumbMovRR_L2H;
	*/
	opCode = kUnicoreMovRR;

    res->operands[0] = rDest;
    res->operands[1] = rSrc;
    res->opCode = opCode;
    setupResourceMasks(res);
    if (rDest == rSrc) {
        res->isNop = true;
    }   
    return res;
}

static UnicoreLIR* genRegCopy(CompilationUnit *cUnit, int rDest, int rSrc)                    
{
    UnicoreLIR *res = genRegCopyNoInsert(cUnit, rDest, rSrc);
    dvmCompilerAppendLIR(cUnit, (LIR*)res);
    return res;
}

