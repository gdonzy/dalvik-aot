/*#include "DEX.h"
#include "compiler/CompilerUtility.h"
#include "compiler/CompilerIR.h"
#include "compiler/codegen/unicore/Ralloc.h"
*/

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
        //eric
		//loadWordDisp(cUnit, rGLUE, offsetof(InterpState, retval), reg1);
    } else {
        assert(rlSrc.location == kLocDalvikFrame);
        loadWordDisp(cUnit, rFP, dvmCompilerS2VReg(cUnit, rlSrc.sRegLow) << 2,
                     reg1);
    }
}


/*
 * Similar to loadValueDirect, but clobbers and allocates the target
 * register.  Should be used when loading to a fixed register (for example,
 * loading arguments to an out of line call.                                             
 */
static void loadValueDirectFixed(CompilationUnit *cUnit, RegLocation rlSrc,
                                 int reg1)
{
    dvmCompilerClobber(cUnit, reg1);
    dvmCompilerMarkInUse(cUnit, reg1);
    loadValueDirect(cUnit, rlSrc, reg1);
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
        //eric
		//loadWordDisp(cUnit, rGLUE, offsetof(InterpState, retval), rlSrc.lowReg);
        //rlSrc.location = kLocPhysReg;
        //dvmCompilerClobber(cUnit, rlSrc.lowReg);
    }
    return rlSrc;
}

static void storeValue(CompilationUnit *cUnit, RegLocation rlDest, RegLocation rlSrc)
{
	LOG("rlDest.location is %d\nrlSrc.location is %d\n", rlDest.location, rlSrc.location);
	LIR *defStart;
	LIR *defEnd;
	assert(!rlDest.wide); 
	assert(!rlSrc.wide);  
	//eric 
	//dvmCompilerKillNullCheckedLoc(cUnit, rlDest);   
	rlSrc = dvmCompilerUpdateLoc(cUnit, rlSrc);    
	//eric:if the register is DalvikFrame, so change it to physical
	rlDest = dvmCompilerUpdateLoc(cUnit, rlDest);  
	LOG(">>>>>>>>>>>>>sRegLow is %d<<<<<<<<<<<<\n", rlDest.sRegLow);
	if (rlSrc.location == kLocPhysReg) {  
    	LOG(">>>>>>>>>>>>>>>The function is %s<<<<<<<<<<<<<<<<<\n", __func__);   
		LOG(">>>>>>>>>>>>>>>the Src reg is phy<<<<<<<<<<<<<<<<<\n");

        if (dvmCompilerIsLive(cUnit, rlSrc.lowReg) || (rlDest.location == kLocPhysReg)) {
			// Src is live or Dest has assigned reg.
			rlDest = dvmCompilerEvalLoc(cUnit, rlDest, kAnyReg, false); 
			genRegCopy(cUnit, rlDest.lowReg, rlSrc.lowReg);
		} else { 
			// Just re-assign the registers.  Dest gets Src's regs 
			rlDest.lowReg = rlSrc.lowReg;
			dvmCompilerClobber(cUnit, rlSrc.lowReg);  
		}
	} else {
    	LOG(">>>>>>>>>>>>>>>The function is %s<<<<<<<<<<<<<<<<<\n", __func__);   
		LOG("the Src reg is not phy\n");

		// Load Src either into promoted Dest or temps allocated for Dest
		rlDest = dvmCompilerEvalLoc(cUnit, rlDest, kAnyReg, false);   
		loadValueDirect(cUnit, rlSrc, rlDest.lowReg);  
	}

	
	LOG(">>>>>>>>>>>>>sRegLow is %d<<<<<<<<<<<<\n", rlDest.sRegLow);
	// Dest is now live and dirty (until/if we flush it to home location)
	dvmCompilerMarkLive(cUnit, rlDest.lowReg, rlDest.sRegLow); 
	dvmCompilerMarkDirty(cUnit, rlDest.lowReg); 

	if (rlDest.location == kLocRetval) { 
		//eric
		//	storeBaseDisp(cUnit, rGLUE, offsetof(InterpState, retval), rlDest.lowReg, kWord);
		//dvmCompilerClobber(cUnit, rlDest.lowReg); 
	} else {
		dvmCompilerResetDefLoc(cUnit, rlDest); 
		//if (dvmCompilerLiveOut(cUnit, rlDest.sRegLow)) { 
		if (true) {
//eric
		//	defStart = (LIR *)cUnit->lastLIRInsn;
			LOG(">>>>>>>>>>>>>sRegLow is %d<<<<<<<<<<<<\n", rlDest.sRegLow);
			int vReg = dvmCompilerS2VReg(cUnit, rlDest.sRegLow);
			LOG(">>>>>>>>>>>>>vReg is v%d<<<<<<<<<<<<<\n", vReg);
			storeBaseDisp(cUnit, rFP, vReg << 2, rlDest.lowReg, kWord);  
//			storeBaseDisp(cUnit, rFP, 20, rlDest.lowReg, kWord);  
			dvmCompilerMarkClean(cUnit, rlDest.lowReg); 
		//	defEnd = (LIR *)cUnit->lastLIRInsn; 
	//		dvmCompilerMarkDef(cUnit, rlDest, defStart, defEnd);	

		}
	}
}

/*
 * Perform null-check on a register. sReg is the ssa register being checked,
 * and mReg is the machine register holding the actual value. If internal state
 * indicates that sReg has been checked before the check request is ignored.
 */
//
//static UnicoreLIR *genNullCheck(CompilationUnit *cUnit, int sReg, int mReg,
//                                int dOffset, UnicoreLIR *pcrLabel)
//{   
//    /* This particular Dalvik register has been null-checked */
//    if (dvmIsBitSet(cUnit->regPool->nullCheckedRegs, sReg)) {
//        return pcrLabel;
//    }                                                                                  
//    dvmSetBit(cUnit->regPool->nullCheckedRegs, sReg);
//    return genRegImmCheck(cUnit, kArmCondEq, mReg, 0, dOffset, pcrLabel);
//}


