
/* Offset to distingish FP regs */
#define FP_REG_OFFSET 32
/* Offset to distinguish DP FP regs */
#define FP_DOUBLE 64
/* Reg types */
#define REGTYPE(x) (x & (FP_REG_OFFSET | FP_DOUBLE))
#define FPREG(x) ((x & FP_REG_OFFSET) == FP_REG_OFFSET)
#define LOWREG(x) ((x & 0x7) == x)
#define DOUBLEREG(x) ((x & FP_DOUBLE) == FP_DOUBLE)
#define SINGLEREG(x) (FPREG(x) && !DOUBLEREG(x))
/*
 * Note: the low register of a floating point pair is sufficient to
 * create the name of a double, but require both names to be passed to
 * allow for asserts to verify that the pair is consecutive if significant
 * rework is done in this area.  Also, it is a good reminder in the calling
 * code that reg locations always describe doubles as a pair of singles.
 */
#define S2D(x,y) ((x) | FP_DOUBLE)
/* Mask to strip off fp flags */
#define FP_REG_MASK (FP_REG_OFFSET-1)
/* non-existent Dalvik register */
#define vNone   (-1)
/* non-existant physical register */
#define rNone   (-1)

/* RegisterLocation templates return values (r0, or r0/r1) */
#define LOC_C_RETURN {kLocPhysReg, 0, 0, r0, 0, -1}
#define LOC_C_RETURN_WIDE {kLocPhysReg, 1, 0, r0, r1, -1}
/* RegisterLocation templates for interpState->retVal; */
#define LOC_DALVIK_RETURN_VAL {kLocRetval, 0, 0, 0, 0, -1}
#define LOC_DALVIK_RETURN_VAL_WIDE {kLocRetval, 1, 0, 0, 0, -1}


#define ENCODE_REG_LIST(N)      ((u8) N)
#define ENCODE_REG_SP           (1ULL << kRegSP)
#define ENCODE_REG_LR           (1ULL << kRegLR)
#define ENCODE_REG_PC           (1ULL << kRegPC)
#define ENCODE_CCODE            (1ULL << kCCode)
#define ENCODE_FP_STATUS        (1ULL << kFPStatus)

/*
Data structure tracking the mapping between a
Dalvik virtual register/pair and a native register/pair
*/
typedef struct RegisterInfo {
    int reg;                    // Reg number
    bool inUse;                 // Has it been allocated?
    bool pair;                  // Part of a register pair?
    int partner;                // If pair, other reg of pair

    bool live;                  // Is there an associated SSA name?
    bool dirty;                 // If live, is it dirty?
    int sReg;                   // Name of live value

    struct LIR *defStart;       // Starting inst in last def sequence
    struct LIR *defEnd;         // Ending inst in last def sequence
} RegisterInfo;

typedef struct RegisterPool {
//    BitVector *nullCheckedRegs; // Track which registers have been null-checked
    int numCoreTemps;
    RegisterInfo *coreTemps;
    int nextCoreTemp;
    int numFPTemps;
    RegisterInfo *FPTemps;
    int nextFPTemp;
    int numCoreRegs;
    RegisterInfo *coreRegs;
    int numFPRegs;
    RegisterInfo *FPRegs;
} RegisterPool;

typedef enum ResourceEncodingPos {
    kGPReg0     = 0,
    kRegSP      = 29,
    kRegLR      = 30,
    kRegPC      = 31,
    kFPReg0     = 32,
    kRegEnd     = 48,
    kCCode      = kRegEnd,
    kFPStatus,
    kDalvikReg,
    kLiteral,
    kFrameRef,
    kHeapRef,
    kLitPoolRef
} ResourceEncodingPos;

#define ENCODE_REG_LIST(N)      ((u8) N)
#define ENCODE_REG_SP           (1ULL << kRegSP)
#define ENCODE_REG_LR           (1ULL << kRegLR)
#define ENCODE_REG_PC           (1ULL << kRegPC)
#define ENCODE_CCODE            (1ULL << kCCode)
#define ENCODE_FP_STATUS        (1ULL << kFPStatus)

    /* Must alias */
#define ENCODE_DALVIK_REG       (1ULL << kDalvikReg)
#define ENCODE_LITERAL          (1ULL << kLiteral)

    /* May alias */
#define ENCODE_FRAME_REF        (1ULL << kFrameRef)
#define ENCODE_HEAP_REF         (1ULL << kHeapRef)
#define ENCODE_LITPOOL_REF      (1ULL << kLitPoolRef)

#define ENCODE_ALL              (~0ULL)
#define ENCODE_MEM_DEF          (ENCODE_FRAME_REF | ENCODE_HEAP_REF)
#define ENCODE_MEM_USE          (ENCODE_FRAME_REF | ENCODE_HEAP_REF \
                                 | ENCODE_LITPOOL_REF)

#define DECODE_ALIAS_INFO_REG(X)        (X & 0xffff)
#define DECODE_ALIAS_INFO_WIDE(X)       ((X & 0x80000000) ? 1 : 0)

typedef enum OpSize {                                                                                                                                                                           
    kWord,
    kLong,
    kSingle,
    kDouble,
    kUnsignedHalf,
    kSignedHalf,
    kUnsignedByte,
    kSignedByte,
} OpSize;

typedef enum OpKind {
    kOpMov,
    kOpMvn,
    kOpCmp,
    kOpLsl,
    kOpLsr,
    kOpAsr,
    kOpRor,
    kOpNot,
    kOpAnd,
    kOpOr,
    kOpXor,
    kOpNeg,
    kOpAdd,
    kOpAdc,
    kOpSub,
    kOpSbc,
    kOpRsub,
    kOpMul,
    kOpDiv,
    kOpRem,
    kOpBic,
    kOpCmn,
    kOpTst,
    kOpBkpt,
    kOpBlx,
    kOpPush,
    kOpPop,
    kOp2Char,
    kOp2Short,
    kOp2Byte,
    kOpCondBr,
    kOpUncondBr,
} OpKind;

typedef enum NativeRegisterPool {
    r0 = 0,
    r1 = 1,
    r2 = 2,
    r3 = 3,
    r4 = 4,
    //r5_l = 5,
    //r6_l = 6,
    //r7_l = 7,
    //r8_l = 8,
    //r9_l = 9,
    //r10_l = 10,
    //r11_l = 11,
    //r12_l = 12,
    //r13_l = 13,
    //r14_l = 14,
    //r15_l = 15,
//    r16_l = 16,   //not use for conservative
    //chenglin change
    r4PC = 17,
    rFP = 18,
    rGLUE = 19,

    //r7 = 7,
    r7 = 20,
    //r8 = 8,
    r8 = 21,
    //r9 = 9,
    r9 = 22,
    r10 = 23,
    r11 = 27,
    r12 = 28,
    r13 = 29,
    r20 = 20,
    r21 = 21,
    r22 = 22,
    r23 = 23,
    r29 = 29,
    rlr = 30,
    rpc = 31,
//    fr0  =  0 + FP_REG_OFFSET,
//    fr1  =  1 + FP_REG_OFFSET,
//    fr2  =  2 + FP_REG_OFFSET,
//    fr3  =  3 + FP_REG_OFFSET,
//    fr4  =  4 + FP_REG_OFFSET,
//    fr5  =  5 + FP_REG_OFFSET,
//    fr6  =  6 + FP_REG_OFFSET,
//    fr7  =  7 + FP_REG_OFFSET,
//    fr8  =  8 + FP_REG_OFFSET,
//    fr9  =  9 + FP_REG_OFFSET,
//    fr10 = 10 + FP_REG_OFFSET,
//    fr11 = 11 + FP_REG_OFFSET,
//    fr12 = 12 + FP_REG_OFFSET,
//    fr13 = 13 + FP_REG_OFFSET,
//    fr14 = 14 + FP_REG_OFFSET,
//    fr15 = 15 + FP_REG_OFFSET,
//    fr16 = 16 + FP_REG_OFFSET,
//    fr17 = 17 + FP_REG_OFFSET,
//    fr18 = 18 + FP_REG_OFFSET,
//    fr19 = 19 + FP_REG_OFFSET,
//    fr20 = 20 + FP_REG_OFFSET,
//    fr21 = 21 + FP_REG_OFFSET,
//    fr22 = 22 + FP_REG_OFFSET,
//    fr23 = 23 + FP_REG_OFFSET,
//    fr24 = 24 + FP_REG_OFFSET,
//    fr25 = 25 + FP_REG_OFFSET,
//    fr26 = 26 + FP_REG_OFFSET,
//    fr27 = 27 + FP_REG_OFFSET,
//    fr28 = 28 + FP_REG_OFFSET,
//    fr29 = 29 + FP_REG_OFFSET,
//    fr30 = 30 + FP_REG_OFFSET,
//    fr31 = 31 + FP_REG_OFFSET,
//    dr0 = fr0 + FP_DOUBLE,
//    dr1 = fr2 + FP_DOUBLE,
//    dr2 = fr4 + FP_DOUBLE,
//    dr3 = fr6 + FP_DOUBLE,
//    dr4 = fr8 + FP_DOUBLE,
//    dr5 = fr10 + FP_DOUBLE,
//    dr6 = fr12 + FP_DOUBLE,
//    dr7 = fr14 + FP_DOUBLE,
//    dr8 = fr16 + FP_DOUBLE,
//    dr9 = fr18 + FP_DOUBLE,
//    dr10 = fr20 + FP_DOUBLE,
//    dr11 = fr22 + FP_DOUBLE,
//    dr12 = fr24 + FP_DOUBLE,
//    dr13 = fr26 + FP_DOUBLE,
//    dr14 = fr28 + FP_DOUBLE,
//    dr15 = fr30 + FP_DOUBLE,
} NativeRegisterPool;


typedef enum UnicoreOpCode {
//    kArmChainingCellBottom = -18,
//    kArmPseudoBarrier = -17,
//    kArmPseudoExtended = -16,
//    kArmPseudoSSARep = -15,
//    kArmPseudoEntryBlock = -14,
//    kArmPseudoExitBlock = -13,
//    kArmPseudoTargetLabel = -12,
//    kArmPseudoChainingCellBackwardBranch = -11,
//    kArmPseudoChainingCellHot = -10,
//    kArmPseudoChainingCellInvokePredicted = -9,
//    kArmPseudoChainingCellInvokeSingleton = -8,
//    kArmPseudoChainingCellNormal = -7,
//    kArmPseudoDalvikByteCodeBoundary = -6,
//    kArmPseudoPseudoAlign4 = -5,
//    kArmPseudoPCReconstructionCell = -4,
//    kArmPseudoPCReconstructionBlockLabel = -3,
//    kArmPseudoEHBlockLabel = -2,
//    kArmPseudoNormalBlockLabel = -1,
    /************************************************************************/
    kUnicore32BitData,
    kUnicoreAdcRRR,  //36    set_code
    kUnicoreAddPcRel,  //33
    kUnicoreAddRRI9,  //34   set_code
    kUnicoreAddRRR,   //18   set_code
    kUnicoreAddSpI9,  //33
    kUnicoreAddSpRel,   //33
    kUnicoreAndNRRR, //72    set_code
    kUnicoreAndRRR,  //54    set_code
    kUnicoreAsrRRI5,   //323  set_code
    kUnicoreAsrRRR,//331  set_code
    kUnicoreBCond,    //91
    kUnicoreBkpt, //special
    kUnicoreBl,   //90  special
    kUnicoreBUncond,  //89
    kUnicoreCmpAddRR, //111    set_code
    kUnicoreCmpAndRR, //  120   set_code
    kUnicoreCmpSubRI9,   //137 set_code
    kUnicoreCmpSubRR,  //129   set_code
    kUnicoreJumplR, //168
    kUnicoreLdbRRI14,  //203
    kUnicoreLdbRRR,//171
    kUnicoreLdhRRI5I5,//219 //special and error
    kUnicoreLdhRRR, //211  //error
    kUnicoreLdsbRRR,//245
    kUnicoreLdshRRR,  //261
    kUnicoreLdwPcRel,//311 pre_add
    kUnicoreLdwSpRel, //311 pre_add
    kUnicoreLdwRRI14, //311 pre_add
    kUnicoreLdwRRR,//279
    kUnicoreLslRRI5,  //320 set_code
    kUnicoreLslRRR, //328  set_code
    kUnicoreLsrRRI5, //321  set_code
    kUnicoreLsrRRR, //329  set_code
    kUnicoreMovImm, //335   set_code
    kUnicoreMovRR,   //320 set_code
    kUnicoreMulRRR,// 348 set_code
    kUnicoreNotRR, //360    set_code
    kUnicoreOrrRRR, //378   set_code
    kUnicorePop,//315
    kUnicorePush,//572
    kUnicoreRorRRR,//333  set_code
    kUnicoreRsubRR,   //412  set_code
    kUnicoreStbRRI14,//463
    kUnicoreStbRRR,//431
    kUnicoreSthRRI5I5, //479 special and error
    kUnicoreSthRRR,//471
    kUnicoreStwRRI14,//569 pre_add
    kUnicoreStwRRR,  //537 pre_add
    kUnicoreSubcRRR,  //596  set_code
    kUnicoreSubRRI9,  //594  set_code
    kUnicoreSubRRR,  //578   set_code
    kUnicoreSubSpI9,    //593
    kUnicoreXorRRR, //616  set_code
    kUnicoreLast,
} UnicoreOpCode;


typedef struct UnicoreLIR {
    LIR generic;
    UnicoreOpCode opCode;
    int operands[4];    // [0..3] = [dest, src1, src2, extra]
    bool isNop;         // LIR is optimized away
    bool branchInsertSV;// mark for insertion of branch before this instruction,
                        // used to identify mem ops for self verification mode
    int age;            // default is 0, set lazily by the optimizer
    int size;           // 16-bit unit size (1 for thumb, 1 or 2 for thumb2)
    int aliasInfo;      // For Dalvik register access & litpool disambiguation
    u8 useMask;         // Resource mask for use
    u8 defMask;         // Resource mask for def
} UnicoreLIR;


#define IS_LOAD         (1 << kMemLoad)
#define IS_STORE        (1 << kMemStore)
#define IS_BRANCH       (1 << kIsBranch)
#define REG_DEF0        (1 << kRegDef0)
#define REG_DEF1        (1 << kRegDef1)
#define REG_DEF_SP      (1 << kRegDefSP)
#define REG_DEF_LR      (1 << kRegDefLR)
#define REG_DEF_LIST0   (1 << kRegDefList0)
#define REG_DEF_LIST1   (1 << kRegDefList1)
#define REG_USE0        (1 << kRegUse0)
#define REG_USE1        (1 << kRegUse1)
#define REG_USE2        (1 << kRegUse2)
#define REG_USE3        (1 << kRegUse3)
#define REG_USE_SP      (1 << kRegUseSP)
#define REG_USE_PC      (1 << kRegUsePC)
#define REG_USE_LIST0   (1 << kRegUseList0)
#define REG_USE_LIST1   (1 << kRegUseList1)
#define NO_OPERAND      (1 << kNoOperand)
#define IS_UNARY_OP     (1 << kIsUnaryOp)
#define IS_BINARY_OP    (1 << kIsBinaryOp)
#define IS_TERTIARY_OP  (1 << kIsTertiaryOp)
#define IS_QUAD_OP      (1 << kIsQuadOp)
#define IS_IT           (1 << kIsIT)
#define SETS_CCODES     (1 << kSetsCCodes)
#define USES_CCODES     (1 << kUsesCCodes)

/* Common combo register usage patterns */
#define REG_USE01       (REG_USE0 | REG_USE1)
#define REG_USE012      (REG_USE01 | REG_USE2)
#define REG_USE12       (REG_USE1 | REG_USE2)
#define REG_DEF0_USE0   (REG_DEF0 | REG_USE0)
#define REG_DEF0_USE1   (REG_DEF0 | REG_USE1)
#define REG_DEF0_USE01  (REG_DEF0 | REG_USE01)
#define REG_DEF0_USE12  (REG_DEF0 | REG_USE12)
#define REG_DEF01_USE2  (REG_DEF0 | REG_DEF1 | REG_USE2)

/* Instruction assembly fieldLoc kind */
typedef enum UnicoreEncodingKind {
    kFmtUnused,
    kFmtBitBlt,        /* Bit string using end/start */
    kFmtDfp,           /* Double FP reg */
    kFmtSfp,           /* Single FP reg */
    kFmtModImm,        /* Shifted 8-bit immed using [26,14..12,7..0] */
    kFmtImm16,         /* Zero-extended immed using [26,19..16,14..12,7..0] */
    kFmtImm6,          /* Encoded branch target using [9,7..3]0 */
    kFmtImm12,         /* Zero-extended immediate using [26,14..12,7..0] */
    kFmtShift,         /* Shift descriptor, [14..12,7..4] */
    kFmtLsb,           /* least significant bit using [14..12][7..6] */
    kFmtBWidth,        /* bit-field width, encoded as width-1 */
    kFmtShift5,        /* Shift count, [14..12,7..6] */
    kFmtBrOffset,      /* Signed extended [26,11,13,21-16,10-0]:0 */
    kFmtFPImm,         /* Encoded floating point immediate */
} UnicoreEncodingKind;


typedef enum UnicoreOpFeatureFlags {
    kIsBranch = 0,
    kRegDef0,
    kRegDef1,
    kRegDefSP,
    kRegDefLR,
    kRegDefList0,
    kRegDefList1,
    kRegUse0,
    kRegUse1,
    kRegUse2,
    kRegUse3,
    kRegUseSP,
    kRegUsePC,
    kRegUseList0,
    kRegUseList1,
    kNoOperand,
    kIsUnaryOp,
    kIsBinaryOp,
    kIsTertiaryOp,
    kIsQuadOp,
    kIsIT,
    kSetsCCodes,
    kUsesCCodes,
    kMemLoad,
    kMemStore,
} UnicoreOpFeatureFlags;

/* Struct used to define the snippet positions for each Thumb opcode */
typedef struct UnicoreEncodingMap {
    u4 skeleton;
    struct {
        UnicoreEncodingKind kind;
        int end;   /* end for kFmtBitBlt, 1-bit slice end for FP regs */
        int start; /* start for kFmtBitBlt, 4-bit slice end for FP regs */
    } fieldLoc[4];
    UnicoreOpCode opCode;
    int flags;
    char *name;
    char* fmt;
    int size;
} UnicoreEncodingMap;

extern UnicoreEncodingMap EncodingMap[kUnicoreLast];
