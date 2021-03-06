//#include "../../CompilerUtility.h"
//#include "../../CompilerIR.h"
//#include ",,/../Dataflow.h"
//#include "UnicoreLIR.h"
static inline RegisterClass dvmCompilerRegClassBySize(OpSize size)
{
    return (size == kUnsignedHalf ||
            size == kSignedHalf ||
            size == kUnsignedByte ||
            size == kSignedByte ) ? kCoreReg : kAnyReg;
}

static inline int dvmCompilerS2VReg(CompilationUnit *cUnit, int sReg)
{                              
    assert(sReg != INVALID_SREG);
    return DECODE_REG(dvmConvertSSARegToDalvik(cUnit, sReg));
}

static inline int dvmCompilerSRegHi(int lowSreg) {
    return (lowSreg == INVALID_SREG) ? INVALID_SREG : lowSreg + 1;
}

static inline int dvmCompilerSSASrc(MIR *mir, int num)
{
	return mir->ssaRep->uses[num];
}


static inline bool dvmCompilerLiveOut(CompilationUnit *cUnit, int sReg)
{
    //TODO: fully implement
    return true;
}
/* Reset the tracker to unknown state */
/*
static inline void dvmCompilerResetNullCheck(CompilationUnit *cUnit)
{                                                                                            
    dvmClearAllBits(cUnit->regPool->nullCheckedRegs);
}
*/

extern void dvmCompilerMarkLive(CompilationUnit *cUnit, int reg, int sReg); 
extern void dvmCompilerMarkPair(CompilationUnit *cUnit, int lowReg, int highReg);
extern void dvmCompilerMarkClean(CompilationUnit *cUnit, int reg);
extern void dvmCompilerMarkDirty(CompilationUnit *cUnit, int reg);
extern RegLocation dvmCompilerEvalLoc(CompilationUnit *cUnit, RegLocation loc, int regClass, bool update);
extern void dvmCompilerClobber(CompilationUnit *cUnit, int reg); 
extern RegLocation dvmCompilerUpdateLoc(CompilationUnit *cUnit, RegLocation loc); 

extern void dvmCompilerInitPool(RegisterInfo *regs, int *regNums, int num);

extern void dvmCompilerMarkDef(CompilationUnit *cUnit, RegLocation rl, LIR *start, LIR *finish);
extern void dvmCompilerMarkDefWide(CompilationUnit *cUnit, RegLocation rl, LIR *start, LIR *finish);


extern RegLocation dvmCompilerGetSrcWide(CompilationUnit *cUnit, MIR *mir, int low, int high);
extern RegLocation dvmCompilerGetDestWide(CompilationUnit *cUnit, MIR *mir, int low, int high);
extern RegLocation dvmCompilerGetSrc(CompilationUnit *cUnit, MIR *mir, int num); 
extern RegLocation dvmCompilerGetDest(CompilationUnit *cUnit, MIR *mir, int num);

extern RegLocation dvmCompilerUpdateLocWide(CompilationUnit *cUnit,RegLocation loc);
extern RegLocation dvmCompilerGetReturnWide(CompilationUnit *cUnit); 

extern RegisterInfo *dvmCompilerIsLive(CompilationUnit *cUnit, int reg);
extern RegisterInfo *dvmCompilerIsTemp(CompilationUnit *cUnit, int reg);

/* Clobber all regs that might be used by an external C call */
extern void dvmCompilerClobberCallRegs(CompilationUnit *cUnit);  

extern void dvmCompilerFreeTemp(CompilationUnit *cUnit, int reg);
extern void dvmCompilerResetDefLocWide(CompilationUnit *cUnit, RegLocation rl); 
extern void dvmCompilerClobberAllRegs(CompilationUnit *cUnit);
extern void dvmCompilerMarkInUse(CompilationUnit *cUnit, int reg);

extern int dvmCompilerAllocTemp(CompilationUnit *cUnit); 

extern void dvmCompilerFlushAllRegs(CompilationUnit *cUnit);
extern RegLocation dvmCompilerGetReturnWideAlt(CompilationUnit *cUnit);

extern int dvmCompilerAllocFreeTemp(CompilationUnit *cUnit);  
extern void dvmCompilerResetRegPool(CompilationUnit *cUnit);
extern void dvmCompilerResetDef(CompilationUnit *cUnit, int reg);
extern void dvmCompilerResetDefTracking(CompilationUnit *cUnit);
