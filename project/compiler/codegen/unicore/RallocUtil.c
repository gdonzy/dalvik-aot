#include "../../../DEX.h"
#include "../../CompilerUtility.h"
#include "../../CompilerIR.h"
#include "UnicoreLIR.h"
//#include "Codegen.h"
#include "Ralloc.h"

#define SREG(c, s) ((c)->regLocation[(s)].sRegLow) 

extern void dvmCompilerInitPool(RegisterInfo *regs, int *regNums, int num)
{
    int i;
    for (i=0; i < num; i++) {
        regs[i].reg = regNums[i];
        regs[i].inUse = false;
        regs[i].pair = false;
        regs[i].live = false;
        regs[i].dirty = false;
        regs[i].sReg = INVALID_SREG;
    }
}

static inline int getDestSSAName(MIR *mir, int num) 
{
	return mir->ssaRep->defs[num];   
}

extern RegLocation dvmCompilerGetSrc(CompilationUnit *cUnit, MIR *mir, int num)  
{ 
    RegLocation loc = cUnit->regLocation[ 
         SREG(cUnit, dvmCompilerSSASrc(mir, num))]; 
    loc.fp = cUnit->regLocation[dvmCompilerSSASrc(mir, num)].fp; 
    loc.wide = false;  
    return loc; 
}

extern RegLocation dvmCompilerGetDest(CompilationUnit *cUnit, MIR *mir, int num)
{
	RegLocation loc = cUnit->regLocation[SREG(cUnit, getDestSSAName(mir, num))]; 
	loc.fp = cUnit->regLocation[getDestSSAName(mir, num)].fp; 
	loc.wide = false;  
	return loc; 
}

static RegLocation getLocWide(CompilationUnit *cUnit, MIR *mir, int low, int high, bool isSrc)
{
	RegLocation lowLoc;
	RegLocation highLoc;
	
	if(isSrc) {
		lowLoc = dvmCompilerGetSrc(cUnit, mir, low);  
		highLoc = dvmCompilerGetSrc(cUnit, mir, high);    
	} else {
		lowLoc = dvmCompilerGetDest(cUnit, mir, low); 
		highLoc = dvmCompilerGetDest(cUnit, mir, high);  
	}

	lowLoc.wide = true;
	lowLoc.highReg = highLoc.lowReg;
	return lowLoc;
}

extern RegLocation dvmCompilerGetDestWide(CompilationUnit *cUnit, MIR *mir, int low, int high)
{
	return getLocWide(cUnit, mir, low, high, false);
}

extern RegLocation dvmCompilerGetSrcWide(CompilationUnit *cUnit, MIR *mir, int low, int high)
{
	return getLocWide(cUnit, mir, low, high, true);
}
