extern unsigned char *instrFormatTable;
extern inline unsigned char dexGetInstrFormat(const unsigned char* fmts, OpCode opCode);
extern char* dexGetOpcodeName(OpCode op);

static bool genArithOpInt(CompilationUnit *cUnit, MIR *mir,  
						  RegLocation rlDest, RegLocation rlSrc1, 
						  RegLocation rlSrc2)
{
	OpKind op = kOpBkpt;
    bool callOut = false;
    bool checkZero = false;
    bool unary = false;
    int retReg = r0;
    void *callTgt;
    RegLocation rlResult;
    bool shiftOp = false;

    switch (mir->dalvikInsn.opCode) {
        case OP_NEG_INT:
            op = kOpNeg;
            unary = true;
            break;
        case OP_NOT_INT:
            op = kOpMvn;
            unary = true;
            break;
        case OP_ADD_INT:
        case OP_ADD_INT_2ADDR:
            op = kOpAdd;
            break;
        case OP_SUB_INT:
        case OP_SUB_INT_2ADDR:
            op = kOpSub;
            break;
        case OP_MUL_INT:
        case OP_MUL_INT_2ADDR:
            op = kOpMul;
            break;
        case OP_DIV_INT:
        //eric
	/*	case OP_DIV_INT_2ADDR:
            callOut = true;
            checkZero = true;
            callTgt = __aeabi_idiv;
            retReg = r0;
            break;*/
        /* NOTE: returns in r1 */
        case OP_REM_INT:
//eric
/*
        case OP_REM_INT_2ADDR:
            callOut = true;
            checkZero = true;
            callTgt = __aeabi_idivmod;
            retReg = r1;
            break;
  */
	      case OP_AND_INT:
        case OP_AND_INT_2ADDR:
            op = kOpAnd;
            break;
        case OP_OR_INT:
        case OP_OR_INT_2ADDR:
            op = kOpOr;
            break;
        case OP_XOR_INT:
        case OP_XOR_INT_2ADDR:
            op = kOpXor;
            break;
        case OP_SHL_INT:
        case OP_SHL_INT_2ADDR:
            shiftOp = true;
            op = kOpLsl;
            break;
        case OP_SHR_INT:
        case OP_SHR_INT_2ADDR:
            shiftOp = true;
            op = kOpAsr;
            break;
        case OP_USHR_INT:
        case OP_USHR_INT_2ADDR:
            shiftOp = true;
            op = kOpLsr;
            break;
        default:
            printf("Invalid word arith op: 0x%x(%d)",
                  mir->dalvikInsn.opCode, mir->dalvikInsn.opCode);
            //dvmCompilerAbort(cUnit);
			//exit(-1);
    }
    if (!callOut) {
        rlSrc1 = loadValue(cUnit, rlSrc1, kCoreReg);
        if (unary) {
            rlResult = dvmCompilerEvalLoc(cUnit, rlDest, kCoreReg, true);
            opRegReg(cUnit, op, rlResult.lowReg,
                     rlSrc1.lowReg);
        } else {
            rlSrc2 = loadValue(cUnit, rlSrc2, kCoreReg);
            if (shiftOp) {
                int tReg = dvmCompilerAllocTemp(cUnit);
                opRegRegImm(cUnit, kOpAnd, tReg, rlSrc2.lowReg, 31);
                rlResult = dvmCompilerEvalLoc(cUnit, rlDest, kCoreReg, true);
                opRegRegReg(cUnit, op, rlResult.lowReg,
                            rlSrc1.lowReg, tReg);
                dvmCompilerFreeTemp(cUnit, tReg);
            } else {
                rlResult = dvmCompilerEvalLoc(cUnit, rlDest, kCoreReg, true);
                opRegRegReg(cUnit, op, rlResult.lowReg,
                            rlSrc1.lowReg, rlSrc2.lowReg);
            }
        }
        storeValue(cUnit, rlDest, rlResult);
    } 
	/*
	else {
        RegLocation rlResult;
        dvmCompilerFlushAllRegs(cUnit);   
	*/ /*
        loadValueDirectFixed(cUnit, rlSrc2, r1);
        LOAD_FUNC_ADDR(cUnit, r2, (int) callTgt);
        loadValueDirectFixed(cUnit, rlSrc1, r0);
        if (checkZero) {
            genNullCheck(cUnit, rlSrc2.sRegLow, r1, mir->offset, NULL);
        }
        opReg(cUnit, kOpBlx, r2);
        dvmCompilerClobberCallRegs(cUnit);
        if (retReg == r0)
            rlResult = dvmCompilerGetReturn(cUnit);
        else
            rlResult = dvmCompilerGetReturnAlt(cUnit);
        storeValue(cUnit, rlDest, rlResult);
    }*/
    return false;	
}

static bool genArithOp(CompilationUnit *cUnit, MIR *mir)
{
    OpCode opCode = mir->dalvikInsn.opCode;

    RegLocation rlDest;
    RegLocation rlSrc1;
    RegLocation rlSrc2;
    /* Deduce sizes of operands */
    if (mir->ssaRep->numUses == 2) { 
        rlSrc1 = dvmCompilerGetSrc(cUnit, mir, 0);
        rlSrc2 = dvmCompilerGetSrc(cUnit, mir, 1);
    } else if (mir->ssaRep->numUses == 3) { 
        rlSrc1 = dvmCompilerGetSrcWide(cUnit, mir, 0, 1);
        rlSrc2 = dvmCompilerGetSrc(cUnit, mir, 2);
    } else {
        rlSrc1 = dvmCompilerGetSrcWide(cUnit, mir, 0, 1);
        rlSrc2 = dvmCompilerGetSrcWide(cUnit, mir, 2, 3);
        assert(mir->ssaRep->numUses == 4);
    }    
    if (mir->ssaRep->numDefs == 1) { 
        rlDest = dvmCompilerGetDest(cUnit, mir, 0);
    } else {
        assert(mir->ssaRep->numDefs == 2);
        rlDest = dvmCompilerGetDestWide(cUnit, mir, 0, 1);
    }    
/*
    if ((opCode >= OP_ADD_LONG_2ADDR) && (opCode <= OP_XOR_LONG_2ADDR)) {
        return genArithOpLong(cUnit,mir, rlDest, rlSrc1, rlSrc2);
    }    
    if ((opCode >= OP_ADD_LONG) && (opCode <= OP_XOR_LONG)) {
        return genArithOpLong(cUnit,mir, rlDest, rlSrc1, rlSrc2);
    }    
    if ((opCode >= OP_SHL_LONG_2ADDR) && (opCode <= OP_USHR_LONG_2ADDR)) {
        return genShiftOpLong(cUnit,mir, rlDest, rlSrc1, rlSrc2);
    }    
    if ((opCode >= OP_SHL_LONG) && (opCode <= OP_USHR_LONG)) {
        return genShiftOpLong(cUnit,mir, rlDest, rlSrc1, rlSrc2);
    }    */
    if ((opCode >= OP_ADD_INT_2ADDR) && (opCode <= OP_USHR_INT_2ADDR)) {
        return genArithOpInt(cUnit,mir, rlDest, rlSrc1, rlSrc2);
    }    
/*
    if ((opCode >= OP_ADD_INT) && (opCode <= OP_USHR_INT)) {
        return genArithOpInt(cUnit,mir, rlDest, rlSrc1, rlSrc2);
    }                                                                               
    if ((opCode >= OP_ADD_FLOAT_2ADDR) && (opCode <= OP_REM_FLOAT_2ADDR)) {
        return genArithOpFloat(cUnit,mir, rlDest, rlSrc1, rlSrc2);
    }
    if ((opCode >= OP_ADD_FLOAT) && (opCode <= OP_REM_FLOAT)) {
        return genArithOpFloat(cUnit, mir, rlDest, rlSrc1, rlSrc2);
    }
    if ((opCode >= OP_ADD_DOUBLE_2ADDR) && (opCode <= OP_REM_DOUBLE_2ADDR)) {
        return genArithOpDouble(cUnit,mir, rlDest, rlSrc1, rlSrc2);
    }
    if ((opCode >= OP_ADD_DOUBLE) && (opCode <= OP_REM_DOUBLE)) {
        return genArithOpDouble(cUnit,mir, rlDest, rlSrc1, rlSrc2);
    }
*/
    return true;
}

/* 
 * The following are the first-level codegen routines that analyze the format
 * of each bytecode.
 */
static bool handleFmt11n_Fmt31i(CompilationUnit *cUnit, MIR *mir)
{
    RegLocation rlDest;
    RegLocation rlResult;
    if (mir->ssaRep->numDefs == 2) {
        rlDest = dvmCompilerGetDestWide(cUnit, mir, 0, 1);
    } else {
        rlDest = dvmCompilerGetDest(cUnit, mir, 0);
	LOG(">>>>>>>>>>>>>rlDest.sRegLow is %d<<<<<<<<<<<<\n", rlDest.sRegLow);
	LOG(">>>>>>>>>>>>>rlDest.location is %d<<<<<<<<<<<<\n", rlDest.location);
    }

    switch (mir->dalvikInsn.opCode) {
        case OP_CONST:
        case OP_CONST_4: {
            rlResult = dvmCompilerEvalLoc(cUnit, rlDest, kAnyReg, true);
            loadConstantNoClobber(cUnit, rlResult.lowReg, mir->dalvikInsn.vB);
            storeValue(cUnit, rlDest, rlResult);
            break;
        }
/*
        case OP_CONST_WIDE_32: {
            //TUNING: single routine to load constant pair for support doubles
            //TUNING: load 0/-1 separately to avoid load dependency
            rlResult = dvmCompilerEvalLoc(cUnit, rlDest, kCoreReg, true);
            loadConstantNoClobber(cUnit, rlResult.lowReg, mir->dalvikInsn.vB);
            opRegRegImm(cUnit, kOpAsr, rlResult.highReg,
                        rlResult.lowReg, 31);
            storeValueWide(cUnit, rlDest, rlResult);
            break;
        }
*/
        default:
            return true;
    }
    return false;
}

static bool handleFmt12x(CompilationUnit *cUnit, MIR *mir)
{
	OpCode opCode = mir->dalvikInsn.opCode;

	RegLocation rlDest;
	RegLocation rlSrc;
	RegLocation rlResult;

#ifdef DEBUG 
	printf(">>>>>>>>>>>>>>>The function is %s<<<<<<<<<<<<<<<<<\n", __func__);	
#endif
    if ( (opCode >= OP_ADD_INT_2ADDR) && (opCode <= OP_REM_DOUBLE_2ADDR)) {
        return genArithOp( cUnit, mir );
    }

    if (mir->ssaRep->numUses == 2)
        rlSrc = dvmCompilerGetSrcWide(cUnit, mir, 0, 1);
    else
        rlSrc = dvmCompilerGetSrc(cUnit, mir, 0);       
    if (mir->ssaRep->numDefs == 2)
        rlDest = dvmCompilerGetDestWide(cUnit, mir, 0, 1);
    else
        rlDest = dvmCompilerGetDest(cUnit, mir, 0);

    switch (opCode) {
/*
        case OP_DOUBLE_TO_INT:
        case OP_INT_TO_FLOAT:
        case OP_FLOAT_TO_INT:
        case OP_DOUBLE_TO_FLOAT:
        case OP_FLOAT_TO_DOUBLE:
        case OP_INT_TO_DOUBLE:
        case OP_FLOAT_TO_LONG:
        case OP_LONG_TO_FLOAT:
        case OP_DOUBLE_TO_LONG:
        case OP_LONG_TO_DOUBLE:
            return genConversion(cUnit, mir);
        case OP_NEG_INT:
        case OP_NOT_INT:
            return genArithOpInt(cUnit, mir, rlDest, rlSrc, rlSrc);
        case OP_NEG_LONG:
        case OP_NOT_LONG:
            return genArithOpLong(cUnit, mir, rlDest, rlSrc, rlSrc);
        case OP_NEG_FLOAT:
            return genArithOpFloat(cUnit, mir, rlDest, rlSrc, rlSrc);
        case OP_NEG_DOUBLE:
            return genArithOpDouble(cUnit, mir, rlDest, rlSrc, rlSrc);
        case OP_MOVE_WIDE:
            storeValueWide(cUnit, rlDest, rlSrc);
            break;
        case OP_INT_TO_LONG:
            rlSrc = dvmCompilerUpdateLoc(cUnit, rlSrc);
            rlResult = dvmCompilerEvalLoc(cUnit, rlDest, kCoreReg, true);
            //TUNING: shouldn't loadValueDirect already check for phys reg?
            if (rlSrc.location == kLocPhysReg) {
                genRegCopy(cUnit, rlResult.lowReg, rlSrc.lowReg);
            } else {
                loadValueDirect(cUnit, rlSrc, rlResult.lowReg);
            }
            opRegRegImm(cUnit, kOpAsr, rlResult.highReg,
                        rlResult.lowReg, 31);
            storeValueWide(cUnit, rlDest, rlResult);
            break;
        case OP_LONG_TO_INT:
            rlSrc = dvmCompilerUpdateLocWide(cUnit, rlSrc);
            rlSrc = dvmCompilerWideToNarrow(cUnit, rlSrc);
            // Intentional fallthrough
*/
        case OP_MOVE:
        case OP_MOVE_OBJECT:           
            storeValue(cUnit, rlDest, rlSrc);
            break;
/*
        case OP_INT_TO_BYTE:
            rlSrc = loadValue(cUnit, rlSrc, kCoreReg);
            rlResult = dvmCompilerEvalLoc(cUnit, rlDest, kCoreReg, true);
            opRegReg(cUnit, kOp2Byte, rlResult.lowReg, rlSrc.lowReg);
            storeValue(cUnit, rlDest, rlResult);
            break;
        case OP_INT_TO_SHORT:
            rlSrc = loadValue(cUnit, rlSrc, kCoreReg);
            rlResult = dvmCompilerEvalLoc(cUnit, rlDest, kCoreReg, true);
            opRegReg(cUnit, kOp2Short, rlResult.lowReg, rlSrc.lowReg);
            storeValue(cUnit, rlDest, rlResult);
            break;
        case OP_INT_TO_CHAR:
            rlSrc = loadValue(cUnit, rlSrc, kCoreReg);
            rlResult = dvmCompilerEvalLoc(cUnit, rlDest, kCoreReg, true);
            opRegReg(cUnit, kOp2Char, rlResult.lowReg, rlSrc.lowReg);
            storeValue(cUnit, rlDest, rlResult);
            break;
        case OP_ARRAY_LENGTH: {
            int lenOffset = offsetof(ArrayObject, length);
            rlSrc = loadValue(cUnit, rlSrc, kCoreReg);
            genNullCheck(cUnit, rlSrc.sRegLow, rlSrc.lowReg,
                         mir->offset, NULL);
            rlResult = dvmCompilerEvalLoc(cUnit, rlDest, kCoreReg, true);
            loadWordDisp(cUnit, rlSrc.lowReg, lenOffset,
                         rlResult.lowReg);
            storeValue(cUnit, rlDest, rlResult);
            break;
        }
*/
        default:
            return true;
		
    }
    return false;
}

static bool handleFmt22b_Fmt22s(CompilationUnit *cUnit, MIR *mir)
{
    OpCode dalvikOpCode = mir->dalvikInsn.opCode;
    RegLocation rlSrc = dvmCompilerGetSrc(cUnit, mir, 0);
    RegLocation rlDest = dvmCompilerGetDest(cUnit, mir, 0);
    RegLocation rlResult;
    int lit = mir->dalvikInsn.vC;
    OpKind op = 0;      /* Make gcc happy */
    int shiftOp = false;
    bool isDiv = false;

    switch (dalvikOpCode) {
//        case OP_RSUB_INT_LIT8:
//        case OP_RSUB_INT: {
//            int tReg;
//            //TUNING: add support for use of Arm rsub op
//            rlSrc = loadValue(cUnit, rlSrc, kCoreReg);
//            tReg = dvmCompilerAllocTemp(cUnit);
//            loadConstant(cUnit, tReg, lit);
//            rlResult = dvmCompilerEvalLoc(cUnit, rlDest, kCoreReg, true);
//            opRegRegReg(cUnit, kOpSub, rlResult.lowReg,
//                        tReg, rlSrc.lowReg);
//            storeValue(cUnit, rlDest, rlResult);
//            return false;
//            break;
//        }

        case OP_ADD_INT_LIT8:
        case OP_ADD_INT_LIT16:
            op = kOpAdd;
            break;
//        case OP_MUL_INT_LIT8:
//        case OP_MUL_INT_LIT16: {
//            if (handleEasyMultiply(cUnit, rlSrc, rlDest, lit)) {
//                return false;
//            }
//            op = kOpMul;
//            break;
//        }
//        case OP_AND_INT_LIT8:
//        case OP_AND_INT_LIT16:
//            op = kOpAnd;
//            break;
//        case OP_OR_INT_LIT8:
//        case OP_OR_INT_LIT16:
//            op = kOpOr;
//            break;
//        case OP_XOR_INT_LIT8:
//        case OP_XOR_INT_LIT16:
//            op = kOpXor;
//            break;
//        case OP_SHL_INT_LIT8:
//            lit &= 31;
//            shiftOp = true;
//            op = kOpLsl;
//            break;
//        case OP_SHR_INT_LIT8:
//            lit &= 31;
//            shiftOp = true;
//            op = kOpAsr;
//            break;
//        case OP_USHR_INT_LIT8:
//            lit &= 31;
//            shiftOp = true;
//            op = kOpLsr;
//            break;
//
//        case OP_DIV_INT_LIT8:
//        case OP_DIV_INT_LIT16:
//        case OP_REM_INT_LIT8:
//        case OP_REM_INT_LIT16:
//            if (lit == 0) {
//                /* Let the interpreter deal with div by 0 */
//                genInterpSingleStep(cUnit, mir);
//                return false;
//            }
//            if (handleEasyDivide(cUnit, dalvikOpCode, rlSrc, rlDest, lit)) {
//                return false;
//            }   
//            dvmCompilerFlushAllRegs(cUnit);   /* Everything to home location */
//            loadValueDirectFixed(cUnit, rlSrc, r0);
//            dvmCompilerClobber(cUnit, r0);
//            if ((dalvikOpCode == OP_DIV_INT_LIT8) ||
//                (dalvikOpCode == OP_DIV_INT_LIT16)) {
//                LOAD_FUNC_ADDR(cUnit, r2, (int)__aeabi_idiv);
//                isDiv = true;
//            } else {
//                LOAD_FUNC_ADDR(cUnit, r2, (int)__aeabi_idivmod);
//                isDiv = false;
//            }
//            loadConstant(cUnit, r1, lit);
//            opReg(cUnit, kOpBlx, r2);
//            dvmCompilerClobberCallRegs(cUnit);
//            if (isDiv)
//                rlResult = dvmCompilerGetReturn(cUnit);
//            else
//                rlResult = dvmCompilerGetReturnAlt(cUnit);
//            storeValue(cUnit, rlDest, rlResult);
//            return false;
//            break;
        default:
            return true;
    }
    rlSrc = loadValue(cUnit, rlSrc, kCoreReg);
    rlResult = dvmCompilerEvalLoc(cUnit, rlDest, kCoreReg, true);
    // Avoid shifts by literal 0 - no support in Thumb.  Change to copy
    if (shiftOp && (lit == 0)) {
        genRegCopy(cUnit, rlResult.lowReg, rlSrc.lowReg);
    } else {
        opRegRegImm(cUnit, op, rlResult.lowReg, rlSrc.lowReg, lit);
    }
    storeValue(cUnit, rlDest, rlResult);
    return false;
}


void dvmCompilerMIR2LIR(CompilationUnit *cUnit)
{
	BasicBlock *curBB;
	MIR *mir;
	bool notHandled = false;	
	
	dvmCompilerResetRegPool(cUnit); 
	dvmCompilerClobberAllRegs(cUnit); 
	
	//for(curBB = cUnit->firstBB ; curBB != NULL ; curBB = curBB->next) {
//debug
		curBB = cUnit->debugBB;
		
		//eric: 暂时没有用处
		//dvmCompilerResetNullCheck(cUnit);  

	//	if(curBB->startOffset == 0x24fc) {	
		/*LIR insn is designed to insert debugBB in newLIRxxx functions, so we need to initialize debugBB*/
	//		cUnit->debugBB = curBB;
			for(mir = curBB->firstMIRInsn; mir; mir = mir->next) {
				dvmCompilerResetRegPool(cUnit);	
				dvmCompilerResetDefTracking(cUnit);
			
				OpCode dalvikOpCode = mir->dalvikInsn.opCode;
				InstructionFormat dalvikFormat = dexGetInstrFormat(instrFormatTable, dalvikOpCode);
	//			if(mir->dalvikInsn.opCode == 18 || mir->dalvikInsn.opCode == 1 || mir->dalvikInsn.opCode == 0xd8){
					switch(dalvikFormat) {
						case kFmt11n:
						case kFmt31i:
							LOG("The function is %s: the MIR opcode is %d\n", __func__, mir->dalvikInsn.opCode);	
							notHandled = handleFmt11n_Fmt31i(cUnit, mir);
							break;
						case kFmt12x:
							LOG("The function is %s: the MIR opcode is %d\n", __func__, mir->dalvikInsn.opCode);
							notHandled = handleFmt12x(cUnit, mir);	
							break;
						case kFmt22b:
							LOG("The function is %s: the MIR opcode is %d\n", __func__, mir->dalvikInsn.opCode);
							notHandled = handleFmt22b_Fmt22s(cUnit, mir);
							break;
						default:
							notHandled = true;
							break;			
					}
			
					if(notHandled) {
					printf("%#06x: Opcode 0x%x (%s) / Fmt %d not handled\n",
						mir->offset,
						dalvikOpCode, dexGetOpcodeName(dalvikOpCode),
						dalvikFormat);
					//exit(1);
					break;
					}
	//			} else continue;
			}
	//	}
	//}
}

UnicoreLIR *dvmCompilerRegCopy(CompilationUnit *cUnit, int rDest, int rSrc)
{
	return genRegCopy(cUnit, rDest, rSrc);
}

//通过rBase和displacement找到虚拟寄存器，rSrc是物理寄存器号
void dvmCompilerFlushRegImpl(CompilationUnit *cUnit, int rBase,
                             int displacement, int rSrc, OpSize size)                       
{
    storeBaseDisp(cUnit, rBase, displacement, rSrc, size);
}

void dvmCompilerFlushRegWideImpl(CompilationUnit *cUnit, int rBase,
								int displacement, int rSrcLo, int rSrcHi)
{
    storeBaseDispWide(cUnit, rBase, displacement, rSrcLo, rSrcHi);
}

