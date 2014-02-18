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
	//	case OP_DIV_INT_2ADDR:
    //        callOut = true;
    //        checkZero = true;
    //        callTgt = __divsi3;
    //        retReg = r0;
    //        break;
        /* NOTE: returns in r1 */
/*        case OP_REM_INT:


        case OP_REM_INT_2ADDR:
            callOut = true;
            checkZero = true;
            callTgt = __modsi3;
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
	else {
//        RegLocation rlResult;
//        dvmCompilerFlushAllRegs(cUnit);   
//        loadValueDirectFixed(cUnit, rlSrc2, r1);
//        LOAD_FUNC_ADDR(cUnit, r2, (int) callTgt);
//        loadValueDirectFixed(cUnit, rlSrc1, r0);
//        if (checkZero) {
//            genNullCheck(cUnit, rlSrc2.sRegLow, r1, mir->offset, NULL);
//        }
//        opReg(cUnit, kOpBlx, r2);
//        dvmCompilerClobberCallRegs(cUnit);
//        if (retReg == r0)
//            rlResult = dvmCompilerGetReturn(cUnit);
//        else
//            rlResult = dvmCompilerGetReturnAlt(cUnit);
//        storeValue(cUnit, rlDest, rlResult);
    }
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

static void genIGet(CompilationUnit *cUnit, MIR *mir, OpSize size,
                    int fieldOffset, bool isVolatile)
{
    RegLocation rlResult;
    RegisterClass regClass = dvmCompilerRegClassBySize(size);
    RegLocation rlObj = dvmCompilerGetSrc(cUnit, mir, 0);
    RegLocation rlDest = dvmCompilerGetDest(cUnit, mir, 0);
    rlObj = loadValue(cUnit, rlObj, kCoreReg);
    rlResult = dvmCompilerEvalLoc(cUnit, rlDest, regClass, true);
//    genNullCheck(cUnit, rlObj.sRegLow, rlObj.lowReg, mir->offset,
//                 NULL);/* null object? */

//    HEAP_ACCESS_SHADOW(true);
    loadBaseDisp(cUnit, mir, rlObj.lowReg, fieldOffset, rlResult.lowReg,
                 size, rlObj.sRegLow);
//    HEAP_ACCESS_SHADOW(false);
//    if (isVolatile) {
//        dvmCompilerGenMemBarrier(cUnit);
//    }

    storeValue(cUnit, rlDest, rlResult);
}

static void genIPut(CompilationUnit *cUnit, MIR *mir, OpSize size,
                    int fieldOffset, bool isObject, bool isVolatile)
{
    RegisterClass regClass = dvmCompilerRegClassBySize(size);
    RegLocation rlSrc = dvmCompilerGetSrc(cUnit, mir, 0);
    RegLocation rlObj = dvmCompilerGetSrc(cUnit, mir, 1);
    rlObj = loadValue(cUnit, rlObj, kCoreReg);
    rlSrc = loadValue(cUnit, rlSrc, regClass);
//    genNullCheck(cUnit, rlObj.sRegLow, rlObj.lowReg, mir->offset,
//                 NULL);/* null object? */

//    if (isVolatile) {
//        dvmCompilerGenMemBarrier(cUnit);
//    }
//    HEAP_ACCESS_SHADOW(true);
    storeBaseDisp(cUnit, rlObj.lowReg, fieldOffset, rlSrc.lowReg, size);
//    HEAP_ACCESS_SHADOW(false);
    if (isObject) {
        /* NOTE: marking card based on object head */
//        markCard(cUnit, rlSrc.lowReg, rlObj.lowReg);
    }
}

static void genIGetWide(CompilationUnit *cUnit, MIR *mir, int fieldOffset)
{
    RegLocation rlObj = dvmCompilerGetSrc(cUnit, mir, 0);
    RegLocation rlDest = dvmCompilerGetDestWide(cUnit, mir, 0, 1);
    RegLocation rlResult;
    rlObj = loadValue(cUnit, rlObj, kCoreReg);
    int regPtr = dvmCompilerAllocTemp(cUnit);

    assert(rlDest.wide);

//    genNullCheck(cUnit, rlObj.sRegLow, rlObj.lowReg, mir->offset,
//                 NULL);/* null object? */
    opRegRegImm(cUnit, kOpAdd, regPtr, rlObj.lowReg, fieldOffset);
    rlResult = dvmCompilerEvalLoc(cUnit, rlDest, kAnyReg, true);

//    HEAP_ACCESS_SHADOW(true);
    loadPair(cUnit, regPtr, rlResult.lowReg, rlResult.highReg);
//    HEAP_ACCESS_SHADOW(false);

    dvmCompilerFreeTemp(cUnit, regPtr);
    storeValueWide(cUnit, rlDest, rlResult);
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

static bool handleFmt21h(CompilationUnit *cUnit, MIR *mir)
{
    RegLocation rlDest;
    RegLocation rlResult;
    if (mir->ssaRep->numDefs == 2) { 
        rlDest = dvmCompilerGetDestWide(cUnit, mir, 0, 1);
    } else {
        rlDest = dvmCompilerGetDest(cUnit, mir, 0);
    }    
    rlResult = dvmCompilerEvalLoc(cUnit, rlDest, kAnyReg, true);

    switch (mir->dalvikInsn.opCode) {
        case OP_CONST_HIGH16: {
			LOG("THE function is %s\n", __func__);
            loadConstantNoClobber(cUnit, rlResult.lowReg,
                                  mir->dalvikInsn.vB << 16); 
            storeValue(cUnit, rlDest, rlResult);
            break;
        }    
        case OP_CONST_WIDE_HIGH16: {
            loadConstantValueWide(cUnit, rlResult.lowReg, rlResult.highReg,
                                  0, mir->dalvikInsn.vB << 16); 
            storeValueWide(cUnit, rlDest, rlResult);
            break;
        }    
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

	LOG(">>>>>>>>>>>>>>>The function is %s<<<<<<<<<<<<<<<<<\n", __func__);	

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
            break; */
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
/*        case OP_LONG_TO_INT:
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

static bool handleFmt21c_Fmt31c(CompilationUnit *cUnit, MIR *mir)
{
    RegLocation rlResult;
    RegLocation rlDest;
    RegLocation rlSrc;

    switch (mir->dalvikInsn.opCode) {
//        case OP_CONST_STRING_JUMBO:
//        case OP_CONST_STRING: {
//            void *strPtr = (void*)
//              (cUnit->method->clazz->pDvmDex->pResStrings[mir->dalvikInsn.vB]);
//
//            if (strPtr == NULL) {
//                LOGE("Unexpected null string");
//                dvmAbort();
//            }
//
//            rlDest = dvmCompilerGetDest(cUnit, mir, 0);
//            rlResult = dvmCompilerEvalLoc(cUnit, rlDest, kCoreReg, true);
//            loadConstantNoClobber(cUnit, rlResult.lowReg, (int) strPtr );
//            storeValue(cUnit, rlDest, rlResult);
//            break;
//        }
//        case OP_CONST_CLASS: {
//            void *classPtr = (void*)
//              (cUnit->method->clazz->pDvmDex->pResClasses[mir->dalvikInsn.vB]);
//
//            if (classPtr == NULL) {
//                LOGE("Unexpected null class");
//                dvmAbort();
//            }
//
//            rlDest = dvmCompilerGetDest(cUnit, mir, 0);
//            rlResult = dvmCompilerEvalLoc(cUnit, rlDest, kCoreReg, true);
//            loadConstantNoClobber(cUnit, rlResult.lowReg, (int) classPtr );
//            storeValue(cUnit, rlDest, rlResult);
//            break;
//        }
//        case OP_SGET_VOLATILE:
//        case OP_SGET_OBJECT_VOLATILE:
//        case OP_SGET_OBJECT:
//        case OP_SGET_BOOLEAN:
//        case OP_SGET_CHAR:
//        case OP_SGET_BYTE:
//        case OP_SGET_SHORT:
//        case OP_SGET: {
//            int valOffset = offsetof(StaticField, value);
//            int tReg = dvmCompilerAllocTemp(cUnit);
//            bool isVolatile;
//            const Method *method = (mir->OptimizationFlags & MIR_CALLEE) ?
//                mir->meta.calleeMethod : cUnit->method;
//            void *fieldPtr = (void*)
//              (method->clazz->pDvmDex->pResFields[mir->dalvikInsn.vB]);
//
//            if (fieldPtr == NULL) {
//                LOGE("Unexpected null static field");
//                dvmAbort();
//            }
//
//            isVolatile = (mir->dalvikInsn.opCode == OP_SGET_VOLATILE) ||
//                         (mir->dalvikInsn.opCode == OP_SGET_OBJECT_VOLATILE) ||
//                         dvmIsVolatileField(fieldPtr);
//
//            rlDest = dvmCompilerGetDest(cUnit, mir, 0);
//            rlResult = dvmCompilerEvalLoc(cUnit, rlDest, kAnyReg, true);
//            loadConstant(cUnit, tReg,  (int) fieldPtr + valOffset);
//
//            if (isVolatile) {
//                dvmCompilerGenMemBarrier(cUnit);
//            }
//            HEAP_ACCESS_SHADOW(true);
//            loadWordDisp(cUnit, tReg, 0, rlResult.lowReg);
//            HEAP_ACCESS_SHADOW(false);
//
//            storeValue(cUnit, rlDest, rlResult);
//            break;
//        }
//        case OP_SGET_WIDE: {
//            int valOffset = offsetof(StaticField, value);
//            const Method *method = (mir->OptimizationFlags & MIR_CALLEE) ?
//                mir->meta.calleeMethod : cUnit->method;
//            void *fieldPtr = (void*)
//              (method->clazz->pDvmDex->pResFields[mir->dalvikInsn.vB]);
//
//            if (fieldPtr == NULL) {
//                LOGE("Unexpected null static field");
//                dvmAbort();
//            }
//
//            int tReg = dvmCompilerAllocTemp(cUnit);
//            rlDest = dvmCompilerGetDestWide(cUnit, mir, 0, 1);
//            rlResult = dvmCompilerEvalLoc(cUnit, rlDest, kAnyReg, true);
//            loadConstant(cUnit, tReg,  (int) fieldPtr + valOffset);
//
//            HEAP_ACCESS_SHADOW(true);
//            loadPair(cUnit, tReg, rlResult.lowReg, rlResult.highReg);
//            HEAP_ACCESS_SHADOW(false);
//
//            storeValueWide(cUnit, rlDest, rlResult);
//            break;
//        }
//        case OP_SPUT_OBJECT:
//        case OP_SPUT_OBJECT_VOLATILE:
//        case OP_SPUT_VOLATILE:
//        case OP_SPUT_BOOLEAN:
//        case OP_SPUT_CHAR:
//        case OP_SPUT_BYTE:
//        case OP_SPUT_SHORT:
//        case OP_SPUT: {
//            int valOffset = offsetof(StaticField, value);
//            int tReg = dvmCompilerAllocTemp(cUnit);
//            int objHead;
//            bool isVolatile;
//            bool isSputObject;
//            const Method *method = (mir->OptimizationFlags & MIR_CALLEE) ?
//                mir->meta.calleeMethod : cUnit->method;
//            void *fieldPtr = (void*)
//              (method->clazz->pDvmDex->pResFields[mir->dalvikInsn.vB]);
//
//            isVolatile = (mir->dalvikInsn.opCode == OP_SPUT_VOLATILE) ||
//                         (mir->dalvikInsn.opCode == OP_SPUT_OBJECT_VOLATILE) ||
//                         dvmIsVolatileField(fieldPtr);
//
//            isSputObject = (mir->dalvikInsn.opCode == OP_SPUT_OBJECT) ||
//                           (mir->dalvikInsn.opCode == OP_SPUT_OBJECT_VOLATILE);
//
//            if (fieldPtr == NULL) {
//                LOGE("Unexpected null static field");
//                dvmAbort();
//            }
//
//            rlSrc = dvmCompilerGetSrc(cUnit, mir, 0);
//            rlSrc = loadValue(cUnit, rlSrc, kAnyReg);
//            loadConstant(cUnit, tReg,  (int) fieldPtr);
//            if (isSputObject) {
//                objHead = dvmCompilerAllocTemp(cUnit);
//                loadWordDisp(cUnit, tReg, offsetof(Field, clazz), objHead);
//            }
//            HEAP_ACCESS_SHADOW(true);
//            storeWordDisp(cUnit, tReg, valOffset ,rlSrc.lowReg);
//            dvmCompilerFreeTemp(cUnit, tReg);
//            HEAP_ACCESS_SHADOW(false);
//            if (isVolatile) {
//                dvmCompilerGenMemBarrier(cUnit);
//            }
//            if (isSputObject) {
//                /* NOTE: marking card based sfield->clazz */
//                markCard(cUnit, rlSrc.lowReg, objHead);
//                dvmCompilerFreeTemp(cUnit, objHead);
//            }
//
//            break;
//        }
//        case OP_SPUT_WIDE: {
//            int tReg = dvmCompilerAllocTemp(cUnit);
//            int valOffset = offsetof(StaticField, value);
//            const Method *method = (mir->OptimizationFlags & MIR_CALLEE) ?
//                mir->meta.calleeMethod : cUnit->method;
//            void *fieldPtr = (void*)
//              (method->clazz->pDvmDex->pResFields[mir->dalvikInsn.vB]);
//
//            if (fieldPtr == NULL) {
//                LOGE("Unexpected null static field");
//                dvmAbort();
//            }
//
//            rlSrc = dvmCompilerGetSrcWide(cUnit, mir, 0, 1);
//            rlSrc = loadValueWide(cUnit, rlSrc, kAnyReg);
//            loadConstant(cUnit, tReg,  (int) fieldPtr + valOffset);
//
//            HEAP_ACCESS_SHADOW(true);
//            storePair(cUnit, tReg, rlSrc.lowReg, rlSrc.highReg);
//            HEAP_ACCESS_SHADOW(false);
//            break;
//        }
//        case OP_NEW_INSTANCE: {
//            /*
//             * Obey the calling convention and don't mess with the register
//             * usage.
//             */
//            ClassObject *classPtr = (void*)
//              (cUnit->method->clazz->pDvmDex->pResClasses[mir->dalvikInsn.vB]);
//
//            if (classPtr == NULL) {
//                LOGE("Unexpected null class");
//                dvmAbort();
//            }
//            /*
//             * If it is going to throw, it should not make to the trace to begin
//             * with.  However, Alloc might throw, so we need to genExportPC()
//             */
//            assert((classPtr->accessFlags & (ACC_INTERFACE|ACC_ABSTRACT)) == 0);
//            dvmCompilerFlushAllRegs(cUnit);   /* Everything to home location */
//            genExportPC(cUnit, mir);
//            LOAD_FUNC_ADDR(cUnit, r2, (int)dvmAllocObject);
//            loadConstant(cUnit, r0, (int) classPtr);
//            loadConstant(cUnit, r1, ALLOC_DONT_TRACK);
//            opReg(cUnit, kOpBlx, r2);
//            dvmCompilerClobberCallRegs(cUnit);
//            /* generate a branch over if allocation is successful */
//            opRegImm(cUnit, kOpCmp, r0, 0); /* NULL? */
//            ArmLIR *branchOver = opCondBranch(cUnit, kArmCondNe);
//            /*
//             * OOM exception needs to be thrown here and cannot re-execute
//             */
//            loadConstant(cUnit, r0,
//                         (int) (cUnit->method->insns + mir->offset));
//            genDispatchToHandler(cUnit, TEMPLATE_THROW_EXCEPTION_COMMON);
//            /* noreturn */
//
//            ArmLIR *target = newLIR0(cUnit, kArmPseudoTargetLabel);
//            target->defMask = ENCODE_ALL;
//            branchOver->generic.target = (LIR *) target;
//            rlDest = dvmCompilerGetDest(cUnit, mir, 0);
//            rlResult = dvmCompilerGetReturn(cUnit);
//            storeValue(cUnit, rlDest, rlResult);
//            break;
//        }
//        case OP_CHECK_CAST: {
//            /*
//             * Obey the calling convention and don't mess with the register
//             * usage.
//             */
//            ClassObject *classPtr =
//              (cUnit->method->clazz->pDvmDex->pResClasses[mir->dalvikInsn.vB]);
//            /*
//             * Note: It is possible that classPtr is NULL at this point,
//             * even though this instruction has been successfully interpreted.
//             * If the previous interpretation had a null source, the
//             * interpreter would not have bothered to resolve the clazz.
//             * Bail out to the interpreter in this case, and log it
//             * so that we can tell if it happens frequently.
//             */
//            if (classPtr == NULL) {
//                 LOGVV("null clazz in OP_CHECK_CAST, single-stepping");
//                 genInterpSingleStep(cUnit, mir);
//                 return false;
//            }
//            dvmCompilerFlushAllRegs(cUnit);   /* Everything to home location */
//            loadConstant(cUnit, r1, (int) classPtr );
//            rlSrc = dvmCompilerGetSrc(cUnit, mir, 0);
//            rlSrc = loadValue(cUnit, rlSrc, kCoreReg);
//            opRegImm(cUnit, kOpCmp, rlSrc.lowReg, 0);   /* Null? */
//            ArmLIR *branch1 = opCondBranch(cUnit, kArmCondEq);
//            /*
//             *  rlSrc.lowReg now contains object->clazz.  Note that
//             *  it could have been allocated r0, but we're okay so long
//             *  as we don't do anything desctructive until r0 is loaded
//             *  with clazz.
//             */
//            /* r0 now contains object->clazz */
//            loadWordDisp(cUnit, rlSrc.lowReg, offsetof(Object, clazz), r0);
//            LOAD_FUNC_ADDR(cUnit, r2, (int)dvmInstanceofNonTrivial);
//            opRegReg(cUnit, kOpCmp, r0, r1);
//            ArmLIR *branch2 = opCondBranch(cUnit, kArmCondEq);
//            opReg(cUnit, kOpBlx, r2);
//            dvmCompilerClobberCallRegs(cUnit);
//            /*
//             * If null, check cast failed - punt to the interpreter.  Because
//             * interpreter will be the one throwing, we don't need to
//             * genExportPC() here.
//             */
//            genZeroCheck(cUnit, r0, mir->offset, NULL);
//            /* check cast passed - branch target here */
//            ArmLIR *target = newLIR0(cUnit, kArmPseudoTargetLabel);
//            target->defMask = ENCODE_ALL;
//            branch1->generic.target = (LIR *)target;
//            branch2->generic.target = (LIR *)target;
//            break;
//        }
//        case OP_SGET_WIDE_VOLATILE:
//        case OP_SPUT_WIDE_VOLATILE:
//            genInterpSingleStep(cUnit, mir);
//            break;
        default:
            return true;
    }
    return false;
}

static bool handleFmt21s(CompilationUnit *cUnit, MIR *mir)                                 
{
    OpCode dalvikOpCode = mir->dalvikInsn.opCode;
    RegLocation rlDest;
    RegLocation rlResult;
    int BBBB = mir->dalvikInsn.vB;
	
		LOG("the function is %s, !!!!!!!!!!!!!!haha\n", __func__);
    if (dalvikOpCode == OP_CONST_WIDE_16) {
        rlDest = dvmCompilerGetDestWide(cUnit, mir, 0, 1);
        rlResult = dvmCompilerEvalLoc(cUnit, rlDest, kCoreReg, true);
        loadConstantNoClobber(cUnit, rlResult.lowReg, BBBB);
        //TUNING: do high separately to avoid load dependency
        opRegRegImm(cUnit, kOpAsr, rlResult.highReg, rlResult.lowReg, 31);
        storeValueWide(cUnit, rlDest, rlResult);
    } else if (dalvikOpCode == OP_CONST_16) {
        rlDest = dvmCompilerGetDest(cUnit, mir, 0);
        rlResult = dvmCompilerEvalLoc(cUnit, rlDest, kAnyReg, true);
        loadConstantNoClobber(cUnit, rlResult.lowReg, BBBB);
        storeValue(cUnit, rlDest, rlResult);
    } else
        return true;
    return false;
}

static bool isPowerOfTwo(int x)                                                           
{   
    return (x & (x - 1)) == 0;                                                               
}            

// Returns true if no more than two bits are set in 'x'.                                
static bool isPopCountLE2(unsigned int x)                                                
{   
    x &= x - 1; 
    return (x & (x - 1)) == 0;                                                             
}                             

// Returns the index of the lowest set bit in 'x'.                                       
static int lowestSetBit(unsigned int x) {                                                
	int bit_posn = 0;
    while ((x & 0xf) == 0) {                                                             
	    bit_posn += 4;                                                                   
	    x >>= 4;                                                                          
	}
    while ((x & 1) == 0) {                                                              
		bit_posn++;                                                                     
 	   	x >>= 1;                                                                             
	}
    return bit_posn;                                                                         
}                     

// Returns true if it added instructions to 'cUnit' to multiply 'rlSrc' by 'lit'
// and store the result in 'rlDest'.
static bool handleEasyMultiply(CompilationUnit *cUnit,
                               RegLocation rlSrc, RegLocation rlDest, int lit)
{
    // Can we simplify this multiplication?
    bool powerOfTwo = false;
    bool popCountLE2 = false;
    bool powerOfTwoMinusOne = false;

    if (lit < 2) {
        // Avoid special cases.
        return false;
    } else if (isPowerOfTwo(lit)) {
        powerOfTwo = true;
    } else if (isPopCountLE2(lit)) {
        popCountLE2 = true;
    } else if (isPowerOfTwo(lit + 1)) {
        powerOfTwoMinusOne = true;
    } else {
        return false;
    }
    rlSrc = loadValue(cUnit, rlSrc, kCoreReg);
    RegLocation rlResult = dvmCompilerEvalLoc(cUnit, rlDest, kCoreReg, true);
    if (powerOfTwo) {
        // Shift.
        opRegRegImm(cUnit, kOpLsl, rlResult.lowReg, rlSrc.lowReg,
                    lowestSetBit(lit));
    } else if (popCountLE2) {
        // Shift and add and shift.
        int firstBit = lowestSetBit(lit);
        int secondBit = lowestSetBit(lit ^ (1 << firstBit));
        genMultiplyByTwoBitMultiplier(cUnit, rlSrc, rlResult, lit,
                                      firstBit, secondBit);
    } else {
        // Reverse subtract: (src << (shift + 1)) - src.
        assert(powerOfTwoMinusOne);
        // TODO: rsb dst, src, src lsl#lowestSetBit(lit + 1)
        int tReg = dvmCompilerAllocTemp(cUnit);
        opRegRegImm(cUnit, kOpLsl, tReg, rlSrc.lowReg, lowestSetBit(lit + 1));
        opRegRegReg(cUnit, kOpSub, rlResult.lowReg, tReg, rlSrc.lowReg);
    }
    storeValue(cUnit, rlDest, rlResult);
    return true;
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
        case OP_MUL_INT_LIT8:
        case OP_MUL_INT_LIT16: {
            if (handleEasyMultiply(cUnit, rlSrc, rlDest, lit)) {
                return false;
            }
            op = kOpMul;
            break;
        }
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

	//donzy2fieldOffset
int findFieldOffset(MIR *mir);

static bool handleFmt22c(CompilationUnit *cUnit, MIR *mir)
{
    OpCode dalvikOpCode = mir->dalvikInsn.opCode;
    int fieldOffset = -1;
    bool isVolatile = false;
    switch (dalvikOpCode) {
        /*
         * Wide volatiles currently handled via single step.
         * Add them here if generating in-line code.
         *     case OP_IGET_WIDE_VOLATILE:
         *     case OP_IPUT_WIDE_VOLATILE:
         */
        case OP_IGET:
//        case OP_IGET_VOLATILE:
        case OP_IGET_WIDE:
        case OP_IGET_OBJECT:
//        case OP_IGET_OBJECT_VOLATILE:
        case OP_IGET_BOOLEAN:
        case OP_IGET_BYTE:
        case OP_IGET_CHAR:
        case OP_IGET_SHORT:
        case OP_IPUT:
//        case OP_IPUT_VOLATILE:
        case OP_IPUT_WIDE:
        case OP_IPUT_OBJECT:
        case OP_IPUT_OBJECT_VOLATILE:
        case OP_IPUT_BOOLEAN:
        case OP_IPUT_BYTE:
        case OP_IPUT_CHAR:
        case OP_IPUT_SHORT: {
//            const Method *method = (mir->OptimizationFlags & MIR_CALLEE) ?
//                mir->meta.calleeMethod : cUnit->method;
//            Field *fieldPtr =
//                method->clazz->pDvmDex->pResFields[mir->dalvikInsn.vC];
//
//            if (fieldPtr == NULL) {
//                LOGE("Unexpected null instance field");
//                dvmAbort();
//            }
//            isVolatile = dvmIsVolatileField(fieldPtr);
//            fieldOffset = ((InstField *)fieldPtr)->byteOffset;
	//donzy2fieldOffset
	fieldOffset = findFieldOffset(mir);
            break;
        }
        default:
            break;
    }

    switch (dalvikOpCode) {
////        case OP_NEW_ARRAY: {
////            // Generates a call - use explicit registers
////            RegLocation rlSrc = dvmCompilerGetSrc(cUnit, mir, 0);
////            RegLocation rlDest = dvmCompilerGetDest(cUnit, mir, 0);
////            RegLocation rlResult;
////            void *classPtr = (void*)
////              (cUnit->method->clazz->pDvmDex->pResClasses[mir->dalvikInsn.vC]);
////
////            if (classPtr == NULL) {
////                LOGE("Unexpected null class");
////                dvmAbort();
////            }
////
////            dvmCompilerFlushAllRegs(cUnit);   /* Everything to home location */
////            genExportPC(cUnit, mir);
////            loadValueDirectFixed(cUnit, rlSrc, r1);   /* Len */
////            loadConstant(cUnit, r0, (int) classPtr );
////            LOAD_FUNC_ADDR(cUnit, r3, (int)dvmAllocArrayByClass);
////            /*
////             * "len < 0": bail to the interpreter to re-execute the
////             * instruction
////             */
////            genRegImmCheck(cUnit, kArmCondMi, r1, 0, mir->offset, NULL);
////            loadConstant(cUnit, r2, ALLOC_DONT_TRACK);
////            opReg(cUnit, kOpBlx, r3);
////            dvmCompilerClobberCallRegs(cUnit);
////            /* generate a branch over if allocation is successful */
////            opRegImm(cUnit, kOpCmp, r0, 0); /* NULL? */
////            ArmLIR *branchOver = opCondBranch(cUnit, kArmCondNe);
////            /*
////             * OOM exception needs to be thrown here and cannot re-execute
////             */
////            loadConstant(cUnit, r0,
////                         (int) (cUnit->method->insns + mir->offset));
////            genDispatchToHandler(cUnit, TEMPLATE_THROW_EXCEPTION_COMMON);
////            /* noreturn */
////
////            ArmLIR *target = newLIR0(cUnit, kArmPseudoTargetLabel);
////            target->defMask = ENCODE_ALL;
////            branchOver->generic.target = (LIR *) target;
////            rlResult = dvmCompilerGetReturn(cUnit);
////            storeValue(cUnit, rlDest, rlResult);
////            break;
////        }
////        case OP_INSTANCE_OF: {
////            // May generate a call - use explicit registers
////            RegLocation rlSrc = dvmCompilerGetSrc(cUnit, mir, 0);
////            RegLocation rlDest = dvmCompilerGetDest(cUnit, mir, 0);
////            RegLocation rlResult;
////            ClassObject *classPtr =
////              (cUnit->method->clazz->pDvmDex->pResClasses[mir->dalvikInsn.vC]);
////            /*
////             * Note: It is possible that classPtr is NULL at this point,
////             * even though this instruction has been successfully interpreted.
////             * If the previous interpretation had a null source, the
////             * interpreter would not have bothered to resolve the clazz.
////             * Bail out to the interpreter in this case, and log it
////             * so that we can tell if it happens frequently.
////             */
////            if (classPtr == NULL) {
////                LOGD("null clazz in OP_INSTANCE_OF, single-stepping");
////                genInterpSingleStep(cUnit, mir);
////                break;
////            }
////            dvmCompilerFlushAllRegs(cUnit);   /* Everything to home location */
////            loadValueDirectFixed(cUnit, rlSrc, r0);  /* Ref */
////            loadConstant(cUnit, r2, (int) classPtr );
//////TUNING: compare to 0 primative to allow use of CB[N]Z
////            opRegImm(cUnit, kOpCmp, r0, 0); /* NULL? */
////            /* When taken r0 has NULL which can be used for store directly */
////            ArmLIR *branch1 = opCondBranch(cUnit, kArmCondEq);
////            /* r1 now contains object->clazz */
////            loadWordDisp(cUnit, r0, offsetof(Object, clazz), r1);
////            /* r1 now contains object->clazz */
////            LOAD_FUNC_ADDR(cUnit, r3, (int)dvmInstanceofNonTrivial);
////            loadConstant(cUnit, r0, 1);                /* Assume true */
////            opRegReg(cUnit, kOpCmp, r1, r2);
////            ArmLIR *branch2 = opCondBranch(cUnit, kArmCondEq);
////            genRegCopy(cUnit, r0, r1);
////            genRegCopy(cUnit, r1, r2);
////            opReg(cUnit, kOpBlx, r3);
////            dvmCompilerClobberCallRegs(cUnit);
////            /* branch target here */
////            ArmLIR *target = newLIR0(cUnit, kArmPseudoTargetLabel);
////            target->defMask = ENCODE_ALL;
////            rlResult = dvmCompilerGetReturn(cUnit);
////            storeValue(cUnit, rlDest, rlResult);
////            branch1->generic.target = (LIR *)target;
////            branch2->generic.target = (LIR *)target;
////            break;
////        }
        case OP_IGET_WIDE:
            genIGetWide(cUnit, mir, fieldOffset);
            break;
//        case OP_IGET_VOLATILE:
////        case OP_IGET_OBJECT_VOLATILE:
////            isVolatile = true;
//            // NOTE: intentional fallthrough
        case OP_IGET:
        case OP_IGET_OBJECT:
        case OP_IGET_BOOLEAN:
        case OP_IGET_BYTE:
        case OP_IGET_CHAR:
        case OP_IGET_SHORT:
            genIGet(cUnit, mir, kWord, fieldOffset, isVolatile);
            break;
////        case OP_IPUT_WIDE:
////            genIPutWide(cUnit, mir, fieldOffset);
////            break;
        case OP_IPUT:
        case OP_IPUT_SHORT:
        case OP_IPUT_CHAR:
        case OP_IPUT_BYTE:
        case OP_IPUT_BOOLEAN:
            genIPut(cUnit, mir, kWord, fieldOffset, false, isVolatile);
            break;
////        case OP_IPUT_VOLATILE:
////        case OP_IPUT_OBJECT_VOLATILE:
////            isVolatile = true;
////            // NOTE: intentional fallthrough
////        case OP_IPUT_OBJECT:
////            genIPut(cUnit, mir, kWord, fieldOffset, true, isVolatile);
////            break;
////        case OP_IGET_WIDE_VOLATILE:
////        case OP_IPUT_WIDE_VOLATILE:
////            genInterpSingleStep(cUnit, mir);
////            break;
        default:
            return true;
    }
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
	                    case kFmt21c:
    	                case kFmt31c:
        	                notHandled = handleFmt21c_Fmt31c(cUnit, mir);                
 			        	    break;
						case kFmt21h:
	                        notHandled = handleFmt21h(cUnit, mir);                 
	                        break;
                    	case kFmt21s:
                        	notHandled = handleFmt21s(cUnit, mir);
                        	break;             
						case kFmt22b:
							LOG("The function is %s: the MIR opcode is %d\n", __func__, mir->dalvikInsn.opCode);
							notHandled = handleFmt22b_Fmt22s(cUnit, mir);
							break;
                    				case kFmt22c:
                        				notHandled = handleFmt22c(cUnit, mir);
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

/* Needed by the register allocator */ 
void dvmCompilerRegCopyWide(CompilationUnit *cUnit, int destLo, int destHi,                 
                            int srcLo, int srcHi)
{           
    genRegCopyWide(cUnit, destLo, destHi, srcLo, srcHi);
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

