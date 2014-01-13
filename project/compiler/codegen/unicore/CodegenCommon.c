
static inline void setupRegMask(u8 *mask, int reg)
{
    u8 seed;
    int shift;
    int regId = reg & 0x1f;

    /*
     * Each double register is equal to a pair of single-precision FP registers
     */
    seed = DOUBLEREG(reg) ? 3 : 1;
    /* FP register starts at bit position 16 */
    shift = FPREG(reg) ? kFPReg0 : 0;
    /* Expand the double register id into single offset */
    //LOGI("chenglin funtion setupRegMask seed = %d  +++++++++++++++++++++",seed);
    //LOGI("chenglin funtion setupRegMask shift = %d  +++++++++++++++++++++",shift);
    shift += regId;
    *mask |= seed << shift;
}

static void setMemRefType(UnicoreLIR *lir, bool isLoad, int memType)
{
    u8 *maskPtr;
    u8 mask;
//    assert( EncodingMap[lir->opCode].flags & (IS_LOAD | IS_STORE));
    if (isLoad) {
        maskPtr = &lir->useMask;
        mask = ENCODE_MEM_USE;
    } else {
        maskPtr = &lir->defMask;
        mask = ENCODE_MEM_DEF;
    }
    /* Clear out the memref flags */
    *maskPtr &= ~mask;
    /* ..and then add back the one we need */
    switch(memType) {
        case kLiteral:
 //           assert(isLoad);
            *maskPtr |= (ENCODE_LITERAL | ENCODE_LITPOOL_REF);
            break;
        case kDalvikReg:
            *maskPtr |= (ENCODE_DALVIK_REG | ENCODE_FRAME_REF);
            break;
        case kHeapRef:
            *maskPtr |= ENCODE_HEAP_REF;
            break;
        default:
//            LOGE("Jit: invalid memref kind - %d", memType);
//            assert(0);  // Bail if debug build, set worst-case in the field
            *maskPtr |= ENCODE_ALL;
    }
}

/*
 * Mark load/store instructions that access Dalvik registers through rFP +
 * offset.
 */
static void annotateDalvikRegAccess(UnicoreLIR *lir, int regId, bool isLoad)                    
{
    setMemRefType(lir, isLoad, kDalvikReg);

    /*  
     * Store the Dalvik register id in aliasInfo. Mark he MSB if it is a 64-bit
     * access.
     */
    lir->aliasInfo = regId;
    if (DOUBLEREG(lir->operands[0])) {
        lir->aliasInfo |= 0x80000000;
    }   
}

static void setupResourceMasks(UnicoreLIR *lir)
{
    int opCode = lir->opCode;
    int flags;

    if (opCode <= 0) {
        lir->useMask = lir->defMask = 0;
        return;
    }

    flags = EncodingMap[lir->opCode].flags;

    /* Set up the mask for resources that are updated */
    if (flags & (IS_LOAD | IS_STORE)) {
        /* Default to heap - will catch specialized classes later */
        setMemRefType(lir, flags & IS_LOAD, kHeapRef);
    }

    if (flags & IS_BRANCH) {
        lir->defMask |= ENCODE_REG_PC;
        lir->useMask |= ENCODE_REG_PC;
    }

    if (flags & REG_DEF0) {
        setupRegMask(&lir->defMask, lir->operands[0]);
    }

    if (flags & REG_DEF1) {
        setupRegMask(&lir->defMask, lir->operands[1]);
    }

    if (flags & REG_DEF_SP) {
        lir->defMask |= ENCODE_REG_SP;
    }

    if (flags & REG_DEF_LR) {
        lir->defMask |= ENCODE_REG_LR;
    }

    if (flags & REG_DEF_LIST0) {
        lir->defMask |= ENCODE_REG_LIST(lir->operands[0]);
    }

    if (flags & REG_DEF_LIST1) {
        lir->defMask |= ENCODE_REG_LIST(lir->operands[1]);
    }

    if (flags & SETS_CCODES) {
        lir->defMask |= ENCODE_CCODE;
    }

    /* Conservatively treat the IT block */
    if (flags & IS_IT) {
        lir->defMask = ENCODE_ALL;
    }

    /* Set up the mask for resources that are used */
    if (flags & IS_BRANCH) {
        lir->useMask |= ENCODE_REG_PC;
    }

    if (flags & (REG_USE0 | REG_USE1 | REG_USE2 | REG_USE3)) {
        int i;

        for (i = 0; i < 4; i++) {
            if (flags & (1 << (kRegUse0 + i))) {
                setupRegMask(&lir->useMask, lir->operands[i]);
            }
        }
    }

    if (flags & REG_USE_PC) {
        lir->useMask |= ENCODE_REG_PC;
    }

    if (flags & REG_USE_SP) {
        lir->useMask |= ENCODE_REG_SP;
    }

    if (flags & REG_USE_LIST0) {
        lir->useMask |= ENCODE_REG_LIST(lir->operands[0]);
    }

    if (flags & REG_USE_LIST1) {
        lir->useMask |= ENCODE_REG_LIST(lir->operands[1]);
    }

    if (flags & USES_CCODES) {
        lir->useMask |= ENCODE_CCODE;
    }
}


static UnicoreLIR *newLIR0(CompilationUnit *cUnit,BasicBlock *bb, UnicoreOpCode opCode)
{
    UnicoreLIR *insn = dvmCompilerNew(sizeof(UnicoreLIR), true);
//    assert(isPseudoOpCode(opCode) || (EncodingMap[opCode].flags & NO_OPERAND));
    insn->opCode = opCode;
    setupResourceMasks(insn);
    dvmCompilerAppendLIR(bb, (LIR *) insn);
    return insn;
}

static UnicoreLIR *newLIR1(CompilationUnit *cUnit,BasicBlock *bb , UnicoreOpCode opCode,
                           int dest)
{
    UnicoreLIR *insn = dvmCompilerNew(sizeof(UnicoreLIR), true);
//    assert(isPseudoOpCode(opCode) || (EncodingMap[opCode].flags & IS_UNARY_OP));
    insn->opCode = opCode;
    insn->operands[0] = dest;
    setupResourceMasks(insn);
    dvmCompilerAppendLIR(bb, (LIR *) insn);
    return insn;
}

static UnicoreLIR *newLIR2(CompilationUnit *cUnit,BasicBlock *bb, UnicoreOpCode opCode,
                           int dest, int src1)
{
    UnicoreLIR *insn = dvmCompilerNew(sizeof(UnicoreLIR), true);
//    assert(isPseudoOpCode(opCode) ||
//           (EncodingMap[opCode].flags & IS_BINARY_OP));
    insn->opCode = opCode;
    insn->operands[0] = dest;
    insn->operands[1] = src1;
    setupResourceMasks(insn);
    dvmCompilerAppendLIR(bb, (LIR *) insn);
    return insn;
}

static UnicoreLIR *newLIR3(CompilationUnit *cUnit,BasicBlock *bb, UnicoreOpCode opCode,
                           int dest, int src1, int src2)
{
    UnicoreLIR *insn = dvmCompilerNew(sizeof(UnicoreLIR), true);
    if (!(EncodingMap[opCode].flags & IS_TERTIARY_OP)) {
//        LOGE("Bad LIR3: %s[%d]",EncodingMap[opCode].name,opCode);
        printf("Bad LIR3: %s[%d]",EncodingMap[opCode].name,opCode);
    }
//    assert(isPseudoOpCode(opCode) ||
//           (EncodingMap[opCode].flags & IS_TERTIARY_OP));
    insn->opCode = opCode;
    insn->operands[0] = dest;
    insn->operands[1] = src1;
    insn->operands[2] = src2;
    setupResourceMasks(insn);
    dvmCompilerAppendLIR(bb, (LIR *) insn);
    return insn;
}

////#if defined(_ARMV7_A) || defined(_ARMV7_A_NEON)
static UnicoreLIR *newLIR4(CompilationUnit *cUnit,BasicBlock *bb, UnicoreOpCode opCode,
                           int dest, int src1, int src2, int info)
{
    UnicoreLIR *insn = dvmCompilerNew(sizeof(UnicoreLIR), true);
//    assert(isPseudoOpCode(opCode) ||
//           (EncodingMap[opCode].flags & IS_QUAD_OP));
    insn->opCode = opCode;
    insn->operands[0] = dest;
    insn->operands[1] = src1;
    insn->operands[2] = src2;
    insn->operands[3] = info;
    setupResourceMasks(insn);
    dvmCompilerAppendLIR(bb, (LIR *) insn);
    return insn;
}
