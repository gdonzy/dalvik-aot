#include "DEX.h"
#include "compiler/CompilerUtility.h"
#include "compiler/CompilerIR.h"
#include "compiler/codegen/unicore/Ralloc.h"

/* Load a word at base + displacement.  Displacement must be word multiple */
static UnicoreLIR *loadWordDisp(CompilationUnit *cUnit, int rBase, int displacement,
                            int rDest)
{
    return loadBaseDisp(cUnit, NULL, rBase, displacement, rDest, kWord,                      
                        INVALID_SREG);
}

/*
 * Load a Dalvik register into a physical register.  Take care when
 * using this routine, as it doesn't perform any bookkeeping regarding
 * register liveness.  That is the responsibility of the caller.
 */                              
static void loadValueDirect(CompilationUnit *cUnit, RegLocation rlSrc,
                                int reg1)
{
    rlSrc = dvmCompilerUpdateLoc(cUnit, rlSrc);
    if (rlSrc.location == kLocPhysReg) {
        genRegCopy(cUnit, reg1, rlSrc.lowReg);
    } else  if (rlSrc.location == kLocRetval) {
        loadWordDisp(cUnit, rGLUE, offsetof(InterpState, retval), reg1);
    } else {
        assert(rlSrc.location == kLocDalvikFrame);
        loadWordDisp(cUnit, rFP, dvmCompilerS2VReg(cUnit, rlSrc.sRegLow) << 2,
                     reg1);
    }
}

static RegLocation loadValue(CompilationUnit *cUnit, RegLocation rlSrc,
                             RegisterClass opKind)
{
    rlSrc = dvmCompilerEvalLoc(cUnit, rlSrc, opKind, false);
    if (rlSrc.location == kLocDalvikFrame) {
        loadValueDirect(cUnit, rlSrc, rlSrc.lowReg);
        rlSrc.location = kLocPhysReg;
        dvmCompilerMarkLive(cUnit, rlSrc.lowReg, rlSrc.sRegLow);
    } else if (rlSrc.location == kLocRetval) {
        loadWordDisp(cUnit, rGLUE, offsetof(InterpState, retval), rlSrc.lowReg);
        rlSrc.location = kLocPhysReg;
        dvmCompilerClobber(cUnit, rlSrc.lowReg);
    }
    return rlSrc;
}

static void storeValue(CompilationUnit *cUnit, RegLocation rlDest, RegLocation rlSrc)
{
	LIR *defStart;
	LIR *defEnd;
	assert(!rlDest.wide); assert(!rlSrc.wide);  
}
