#include <stdio.h>
#include <stdlib.h>
#include "../../../DEX.h"
#include "../../CompilerUtility.h"
#include "../../CompilerIR.h"
#include "../../Dataflow.h"
#include "UnicoreLIR.h"
#include "Codegen.h"
#include "Ralloc.h"

#define SREG(c, s) ((c)->regLocation[(s)].sRegLow) 

extern void dvmCompilerResetRegPool(CompilationUnit *cUnit)                              
{
    int i;
    for (i=0; i < cUnit->regPool->numCoreTemps; i++) {
        cUnit->regPool->coreTemps[i].inUse = false;
    }    
    for (i=0; i < cUnit->regPool->numFPTemps; i++) {
        cUnit->regPool->FPTemps[i].inUse = false;
    }    
}

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

/*eric:find RegisterInfo by the reg and cUnit->RegisterPool*/
static RegisterInfo *getRegInfo(CompilationUnit *cUnit, int reg)                          
{
    int numTemps = cUnit->regPool->numCoreTemps;
    RegisterInfo *p = cUnit->regPool->coreTemps;
    int i;
    for (i=0; i< numTemps; i++) {
        if (p[i].reg == reg) {
            return &p[i];
        }
    }
    p = cUnit->regPool->FPTemps;
    numTemps = cUnit->regPool->numFPTemps;
    for (i=0; i< numTemps; i++) {
        if (p[i].reg == reg) {
            return &p[i];
        }
    }
    printf("Tried to get info on a non-existant temp: r%d",reg);
    //dvmCompilerAbort(cUnit);
	exit(-1);
    return NULL;
}

static void flushRegWide(CompilationUnit *cUnit, int reg1, int reg2)
{
    RegisterInfo *info1 = getRegInfo(cUnit, reg1);
    RegisterInfo *info2 = getRegInfo(cUnit, reg2);
    assert(info1 && info2 && info1->pair && info2->pair &&
           (info1->partner == info2->reg) &&
           (info2->partner == info1->reg));
    if ((info1->live && info1->dirty) || (info2->live && info2->dirty)) {
        info1->dirty = false;
        info2->dirty = false;
        if (dvmCompilerS2VReg(cUnit, info2->sReg) <
            dvmCompilerS2VReg(cUnit, info1->sReg))
            info1 = info2;
        dvmCompilerFlushRegWideImpl(cUnit, rFP, 
                                    dvmCompilerS2VReg(cUnit, info1->sReg) << 2,
                                    info1->reg, info1->partner);
    }    
}

/*if reg is live and dirty, then flush reg*/
static void flushReg(CompilationUnit *cUnit, int reg)
{
    RegisterInfo *info = getRegInfo(cUnit, reg);
    if (info->live && info->dirty) {
        info->dirty = false;
        dvmCompilerFlushRegImpl(cUnit, rFP,
                                dvmCompilerS2VReg(cUnit, info->sReg) << 2,
                                reg, kWord);
    }
}

/*
 * 根据物理寄存器号reg和RegisterInfo查找ssa，
 * 如果查找到，并且ssa寄存器是live and dirty, 
 * 那么将虚拟寄存器的内容放入物理寄存器中
 * 否则直接返回
 */
static bool clobberRegBody(CompilationUnit *cUnit, RegisterInfo *p,
                           int numTemps, int reg)
{
    int i;
    for (i=0; i< numTemps; i++) {
        if (p[i].reg == reg) {
            if (p[i].live && p[i].dirty) {
                if (p[i].pair) {
                    flushRegWide(cUnit, p[i].reg, p[i].partner);
                } else {
                    flushReg(cUnit, p[i].reg);
					//这里应该是写回的作用
                }
            }
            p[i].live = false;
            p[i].sReg = INVALID_SREG;
            p[i].defStart = NULL;
            p[i].defEnd = NULL;
            if (p[i].pair) {
                p[i].pair = false;
                /* partners should be in same pool */
                clobberRegBody(cUnit, p, numTemps, p[i].partner);
            }
            return true;
        }
    }
    return false;
}

void dvmCompilerClobber(CompilationUnit *cUnit, int reg) 
{
    if (!clobberRegBody(cUnit, cUnit->regPool->coreTemps,
                        cUnit->regPool->numCoreTemps, reg)) {
        clobberRegBody(cUnit, cUnit->regPool->FPTemps,
                       cUnit->regPool->numFPTemps, reg);
    }    
}

static void clobberSRegBody(RegisterInfo *p, int numTemps, int sReg)                      
{
    int i;
    for (i=0; i< numTemps; i++) {
        if (p[i].sReg == sReg) {
            p[i].live = false;
            p[i].defStart = NULL;
            p[i].defEnd = NULL;
        }    
    }    
}

/*eric: change live into false*/
/* Clobber any temp associated with an sReg.  Could be in either class */
extern void dvmCompilerClobberSReg(CompilationUnit *cUnit, int sReg)
{
    clobberSRegBody(cUnit->regPool->coreTemps, cUnit->regPool->numCoreTemps,
                    sReg);
    clobberSRegBody(cUnit->regPool->FPTemps, cUnit->regPool->numFPTemps,
                    sReg);                                                                
}

static int allocTempBody(CompilationUnit *cUnit, RegisterInfo *p, int numTemps,
                         int *nextTemp, bool required)
{
    int i;
    int next = *nextTemp;
    for (i=0; i< numTemps; i++) {
        if (next >= numTemps)
            next = 0;
        if (!p[next].inUse && !p[next].live) {
            dvmCompilerClobber(cUnit, p[next].reg);
            p[next].inUse = true;
            p[next].pair = false;
            *nextTemp = next + 1;
            return p[next].reg;
        }
        next++;
    }
    next = *nextTemp;
    for (i=0; i< numTemps; i++) {
        if (next >= numTemps)
            next = 0;
        if (!p[next].inUse) {
            dvmCompilerClobber(cUnit, p[next].reg);
            p[next].inUse = true;
            p[next].pair = false;
            *nextTemp = next + 1;
            return p[next].reg;
        }
        next++;
    }
    if (required) {
        printf("No free temp registers");
        //dvmCompilerAbort(cUnit);
    }
    return -1;  // No register available
}

/* Return a temp if one is available, -1 otherwise */
extern int dvmCompilerAllocFreeTemp(CompilationUnit *cUnit)
{
    return allocTempBody(cUnit, cUnit->regPool->coreTemps,
                         cUnit->regPool->numCoreTemps,
                         &cUnit->regPool->nextCoreTemp, true);
}                  

/*alloc a temp phy register*/
extern int dvmCompilerAllocTemp(CompilationUnit *cUnit)                                    
{
    return allocTempBody(cUnit, cUnit->regPool->coreTemps,
                         cUnit->regPool->numCoreTemps,
                         &cUnit->regPool->nextCoreTemp, true);
}

static RegisterInfo *allocLiveBody(RegisterInfo *p, int numTemps, int sReg)
{
    int i;                                                                                   
    if (sReg == -1)
        return NULL;
    for (i=0; i < numTemps; i++) {
        if (p[i].live && (p[i].sReg == sReg)) {
            p[i].inUse = true;
			LOG("The alloclive reg is %d\n", sReg);
            return &p[i];
        }    
    }    
    return NULL;
}

/*find sReg which live property is true*/
//the register in regPool is described by RegisterInfo
static RegisterInfo *allocLive(CompilationUnit *cUnit, int sReg,
                               int regClass)
{
    RegisterInfo *res = NULL;
    switch(regClass) {
        case kAnyReg:
/*live means the reg has a associated ssa name*/
            res = allocLiveBody(cUnit->regPool->FPTemps,
                                cUnit->regPool->numFPTemps, sReg);
            if (res)
                break;
            /* Intentional fallthrough */
        case kCoreReg:
            res = allocLiveBody(cUnit->regPool->coreTemps,
                                cUnit->regPool->numCoreTemps, sReg);
    		LOG(">>>>>>>>>>>>>>>The function is %s, core reg<<<<<<<<<<<<<<<<<\n", __func__);   
            break;
        case kFPReg:
            res = allocLiveBody(cUnit->regPool->FPTemps,
                                cUnit->regPool->numFPTemps, sReg);
            break;
        default:
            printf("Invalid register type");
            //dvmCompilerAbort(cUnit);
			exit(-1);
    }
    return res;
}

extern void dvmCompilerFreeTemp(CompilationUnit *cUnit, int reg)
{
    RegisterInfo *p = cUnit->regPool->coreTemps;
    int numTemps = cUnit->regPool->numCoreTemps;
    int i;
    for (i=0; i< numTemps; i++) {
        if (p[i].reg == reg) {
            p[i].inUse = false;
            p[i].pair = false;
            return;
        }
    }
    p = cUnit->regPool->FPTemps;
    numTemps = cUnit->regPool->numFPTemps;
    for (i=0; i< numTemps; i++) {
        if (p[i].reg == reg) {
            p[i].inUse = false;
            p[i].pair = false;
            return;
        }
    }
    printf("Tried to free a non-existant temp: r%d",reg);
    //dvmCompilerAbort(cUnit);
}

/*          
 * FIXME - this needs to also check the preserved pool once we start
 * start using preserved registers.
 */ 
extern RegisterInfo *dvmCompilerIsLive(CompilationUnit *cUnit, int reg)
{
    RegisterInfo *p = cUnit->regPool->coreTemps;
    int numTemps = cUnit->regPool->numCoreTemps;
    int i;
    for (i=0; i< numTemps; i++) {
        if (p[i].reg == reg) {
            return p[i].live ? &p[i] : NULL;
        }
    }
    p = cUnit->regPool->FPTemps;
    numTemps = cUnit->regPool->numFPTemps;
    for (i=0; i< numTemps; i++) {
        if (p[i].reg == reg) {
            return p[i].live ? &p[i] : NULL;
        }
    }
    return NULL;
}

extern RegisterInfo *dvmCompilerIsTemp(CompilationUnit *cUnit, int reg)                  
{       
    RegisterInfo *p = cUnit->regPool->coreTemps;
    int numTemps = cUnit->regPool->numCoreTemps;
    int i; 
    for (i=0; i< numTemps; i++) {
        if (p[i].reg == reg) {
            return &p[i];
        }
    }
    p = cUnit->regPool->FPTemps;
    numTemps = cUnit->regPool->numFPTemps;
    for (i=0; i< numTemps; i++) {
        if (p[i].reg == reg) {
            return &p[i];
        }   
    }   
    return NULL;
}           

extern void dvmCompilerResetDef(CompilationUnit *cUnit, int reg)
{                                                                                          
    RegisterInfo *p = getRegInfo(cUnit, reg);
    p->defStart = NULL;
    p->defEnd = NULL;
}

static void nullifyRange(CompilationUnit *cUnit, LIR *start, LIR *finish,
                         int sReg1, int sReg2)
{
    if (start && finish) {
        LIR *p;
        assert(sReg1 == sReg2);
        for (p = start; ;p = p->next) {
            ((UnicoreLIR *)p)->isNop = true;
            if (p == finish)
                break;
        }
    }
}                    

/*
 * Mark the beginning and end LIR of a def sequence.  Note that
 * on entry start points to the LIR prior to the beginning of the
 * sequence.
 */
extern void dvmCompilerMarkDef(CompilationUnit *cUnit, RegLocation rl,                
                               LIR *start, LIR *finish)
{
    assert(!rl.wide);
    assert(start && start->next);
    assert(finish);
    RegisterInfo *p = getRegInfo(cUnit, rl.lowReg);
    p->defStart = start->next;
    p->defEnd = finish;
}

extern void dvmCompilerResetDefLoc(CompilationUnit *cUnit, RegLocation rl)
{                                                                                           
    assert(!rl.wide);
//eric
//    if (!(gDvmJit.disableOpt & (1 << kSuppressLoads))) {
        RegisterInfo *p = getRegInfo(cUnit, rl.lowReg);
        assert(!p->pair);
        nullifyRange(cUnit, p->defStart, p->defEnd,
                     p->sReg, rl.sRegLow);
//    }    
    dvmCompilerResetDef(cUnit, rl.lowReg);
}

extern void dvmCompilerResetDefTracking(CompilationUnit *cUnit)                          
{
    int i;
    for (i=0; i< cUnit->regPool->numCoreTemps; i++) {
        dvmCompilerResetDef(cUnit, cUnit->regPool->coreTemps[i].reg);
    }    
    for (i=0; i< cUnit->regPool->numFPTemps; i++) {
        dvmCompilerResetDef(cUnit, cUnit->regPool->FPTemps[i].reg);
    }    
}

extern void dvmCompilerClobberAllRegs(CompilationUnit *cUnit)
{
    int i;
    for (i=0; i< cUnit->regPool->numCoreTemps; i++) {
        dvmCompilerClobber(cUnit, cUnit->regPool->coreTemps[i].reg);
    }    
    for (i=0; i< cUnit->regPool->numFPTemps; i++) {
        dvmCompilerClobber(cUnit, cUnit->regPool->FPTemps[i].reg);            
	}    
}

static bool regClassMatches(int regClass, int reg)
{
	if (regClass == kAnyReg) {
		return true;
	} else if (regClass == kCoreReg) {
		return !FPREG(reg);
	} else {
		return FPREG(reg);
	}
}

extern void dvmCompilerMarkLive(CompilationUnit *cUnit, int reg, int sReg)
{
    RegisterInfo *info = getRegInfo(cUnit, reg);
    if ((info->reg == reg) && (info->sReg == sReg) && info->live) {
        return;  /* already live */
    } else if (sReg != INVALID_SREG) {
        dvmCompilerClobberSReg(cUnit, sReg);
        info->live = true;
    } else {                                       
        /* Can't be live if no associated sReg */
        info->live = false;
    }    
    info->sReg = sReg;
}

extern void dvmCompilerMarkClean(CompilationUnit *cUnit, int reg) 
{
    RegisterInfo *info = getRegInfo(cUnit, reg);                                            
    info->dirty = false;
}

extern void dvmCompilerMarkDirty(CompilationUnit *cUnit, int reg)     
{
    RegisterInfo *info = getRegInfo(cUnit, reg); 
	info->dirty = true; 
}

extern void dvmCompilerMarkInUse(CompilationUnit *cUnit, int reg) 
{
      RegisterInfo *info = getRegInfo(cUnit, reg);
      info->inUse = true;                                                                                                                                                                  
}

void copyRegInfo(CompilationUnit *cUnit, int newReg, int oldReg)                            
{
    RegisterInfo *newInfo = getRegInfo(cUnit, newReg);
    RegisterInfo *oldInfo = getRegInfo(cUnit, oldReg);
    *newInfo = *oldInfo;
    newInfo->reg = newReg;
}

extern RegLocation dvmCompilerUpdateLoc(CompilationUnit *cUnit, RegLocation loc)
{
    assert(!loc.wide);
    if (loc.location == kLocDalvikFrame) {
		//the mediator is ssa num
        RegisterInfo *infoLo = allocLive(cUnit, loc.sRegLow, kAnyReg);
        if (infoLo) {
            if (infoLo->pair) {
                dvmCompilerClobber(cUnit, infoLo->reg);
                dvmCompilerClobber(cUnit, infoLo->partner);
            } else {
                loc.lowReg = infoLo->reg;
                loc.location = kLocPhysReg;
            }
        }
    }

    return loc;
}

extern RegLocation dvmCompilerEvalLoc(CompilationUnit *cUnit, RegLocation loc, 
                                      int regClass, bool update)
{
    int newReg;
	/*
    if (loc.wide)
        return evalLocWide(cUnit, loc, regClass, update);
    */
	loc = dvmCompilerUpdateLoc(cUnit, loc);

//if a register is a phy register
    if (loc.location == kLocPhysReg) {
		//if the parameter regClass is kAnyReg, the code will not execute
        if (!regClassMatches(regClass, loc.lowReg)) {
            /* Wrong register class.  Realloc, copy and transfer ownership */
            newReg = dvmCompilerAllocTypedTemp(cUnit, loc.fp, regClass);
            dvmCompilerRegCopy(cUnit, newReg, loc.lowReg);
            copyRegInfo(cUnit, newReg, loc.lowReg);
            dvmCompilerClobber(cUnit, loc.lowReg);
            loc.lowReg = newReg;
        }    
        return loc; 
    }    

//if a register is Dalvik virtual register
    assert((loc.location != kLocRetval) || (loc.sRegLow == INVALID_SREG));

//alloc a new phy register
    newReg = dvmCompilerAllocTypedTemp(cUnit, loc.fp, regClass);
    loc.lowReg = newReg;

    if (update) {
        loc.location = kLocPhysReg;
        dvmCompilerMarkLive(cUnit, loc.lowReg, loc.sRegLow);
    }    
    return loc; 
}

static inline int getDestSSAName(MIR *mir, int num) 
{
	return mir->ssaRep->defs[num];   
}

extern RegLocation dvmCompilerGetSrc(CompilationUnit *cUnit, MIR *mir, int num)  
{ 
#ifdef DEBUG 
    printf(">>>>>>>>>>>>>>>The function is %s<<<<<<<<<<<<<<<<<\n", __func__);   
#endif
    RegLocation loc = cUnit->regLocation[ 
         SREG(cUnit, dvmCompilerSSASrc(mir, num))]; 
    loc.fp = cUnit->regLocation[dvmCompilerSSASrc(mir, num)].fp; 
    loc.wide = false;  
    return loc; 
}

extern RegLocation dvmCompilerGetDest(CompilationUnit *cUnit, MIR *mir, int num)
{
    LOG(">>>>>>>>>>>>>>>The function is %s<<<<<<<<<<<<<<<<<\n", __func__);   
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
