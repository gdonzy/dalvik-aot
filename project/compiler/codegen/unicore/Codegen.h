
//#include "compiler/CompilerIR.h"
//#include "CalloutHelper.h"

/*
 * loadConstant() sometimes needs to add a small imm to a pre-existing constant
 */
static UnicoreLIR *opRegImm(CompilationUnit *cUnit, OpKind op, int rDestSrc1,
                        int value);
static UnicoreLIR *opRegReg(CompilationUnit *cUnit, OpKind op, int rDestSrc1,
                        int rSrc2);

/* Forward decalraton the portable versions due to circular dependency */
/*static bool genArithOpFloatPortable(CompilationUnit *cUnit, MIR *mir,
                                    RegLocation rlDest, RegLocation rlSrc1,
                                    RegLocation rlSrc2);

static bool genArithOpDoublePortable(CompilationUnit *cUnit, MIR *mir,
                                     RegLocation rlDest, RegLocation rlSrc1,
                                     RegLocation rlSrc2);

static bool genConversionPortable(CompilationUnit *cUnit, MIR *mir);
*/
//#if defined(WITH_DEADLOCK_PREDICTION) || defined(WITH_MONITOR_TRACKING) || \
    defined(__ARM_ARCH_5__)
//static void genMonitorPortable(CompilationUnit *cUnit, MIR *mir);
//#endif

//static void genInterpSingleStep(CompilationUnit *cUnit, MIR *mir);

//#endif


//#if defined(WITH_SELF_VERIFICATION)
/* Self Verification memory instruction decoder */
//void dvmSelfVerificationMemOpDecode(int lr, int* sp);
//#endif

/*
 * Architecture-dependent register allocation routines implemented in
 * Thumb[2]/Ralloc.c
 */
//extern int dvmCompilerAllocTypedTempPair(CompilationUnit *cUnit,
//                                         bool fpHint, int regClass);

extern int dvmCompilerAllocTypedTemp(CompilationUnit *cUnit, bool fpHint, int regClass);

//extern UnicoreLIR* dvmCompilerRegCopyNoInsert(CompilationUnit *cUnit, int rDest,
//                                          int rSrc);

extern UnicoreLIR* dvmCompilerRegCopy(CompilationUnit *cUnit, int rDest, int rSrc);

//extern void dvmCompilerRegCopyWide(CompilationUnit *cUnit, int destLo,
//                                   int destHi, int srcLo, int srcHi);

//extern void dvmCompilerSetupResourceMasks(ArmLIR *lir);

extern void dvmCompilerFlushRegImpl(CompilationUnit *cUnit, int rBase,
                                    int displacement, int rSrc, OpSize size);

extern void dvmCompilerFlushRegWideImpl(CompilationUnit *cUnit, int rBase,
                                        int displacement, int rSrcLo,
                                        int rSrcHi);
