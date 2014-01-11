

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
