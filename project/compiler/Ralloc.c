#include <stdio.h>
#include "../DEX.h"

#include "CompilerUtility.h"
#include "Dataflow.h"
#include "CompilerIR.h"

int dvmConvertSSARegToDalvik(CompilationUnit *cUnit, int ssaReg);

typedef struct LiveRange {
    int ssaName;
    bool active;
    int first;
    int last;
} LiveRange;

static bool simpleTrace(CompilationUnit *cUnit)
{
    //TODO: flesh out
    return true;
}

static void inferTypes(CompilationUnit *cUnit, BasicBlock *bb)
{
    MIR *mir;
//    if (bb->blockType != kDalvikByteCode &&
//        bb->blockType != kTraceEntryBlock)
//        return;

    for (mir = bb->firstMIRInsn; mir; mir = mir->next) {
        SSARepresentation *ssaRep = mir->ssaRep;
        if (ssaRep) {
            int i;
            for (i=0; ssaRep->fpUse && i< ssaRep->numUses; i++) {
                if (ssaRep->fpUse[i])
                    cUnit->regLocation[ssaRep->uses[i]].fp = true;
            }
            for (i=0; ssaRep->fpDef && i< ssaRep->numDefs; i++) {
                if (ssaRep->fpDef[i])
                    cUnit->regLocation[ssaRep->defs[i]].fp = true;
            }
        }
    }
}

static const RegLocation freshLoc = {kLocDalvikFrame, 0, 0, INVALID_REG,
                                     INVALID_REG, INVALID_SREG};

/*alloc RegLocation and initialize sRegLow*/
void dvmCompilerRegAlloc(CompilationUnit *cUnit)
{
    int i;
    int seqNum = 0;
    LiveRange *ranges;
    RegLocation *loc;
    BasicBlock *curBB = NULL;

    /* Allocate the location map */
    loc = (RegLocation*)dvmCompilerNew(cUnit->numSSARegs * sizeof(*loc), true);
    for (i=0; i< cUnit->numSSARegs; i++) {
        loc[i] = freshLoc;
        loc[i].sRegLow = i;
	LOG(">>>>>>>>>>>>>loc[%d]sRegLow is %d<<<<<<<<<<<<\n", i, loc[i].sRegLow);
    }
    cUnit->regLocation = loc;

    /* Do type inference pass */
//    for (i=0; i < cUnit->numBlocks; i++) {
//        inferTypes(cUnit, cUnit->blockList[i]);
//    }
    for (curBB = cUnit->firstBB ; curBB != NULL ; curBB = curBB->next) {
		inferTypes(cUnit,curBB);
    }

    if (simpleTrace(cUnit)) {
        /*
         * Just rename everything back to subscript 0 names and don't do
         * any explicit promotion.  Local allocator will opportunistically
         * promote on the fly.
         */
        for (i=0; i < cUnit->numSSARegs; i++) {
            cUnit->regLocation[i].sRegLow =
                DECODE_REG(dvmConvertSSARegToDalvik(cUnit, loc[i].sRegLow));
	LOG(">>>>>>>>>>>>>cUnit->regLocation[%d].sRegLow is %d<<<<<<<<<<<<\n", i, cUnit->regLocation[i].sRegLow);
        }
    }// else {
//        // Compute live ranges
//        ranges = dvmCompilerNew(cUnit->numSSARegs * sizeof(*ranges), true);
//        for (i=0; i < cUnit->numSSARegs; i++)
//            ranges[i].active = false;
//        seqNum = computeLiveRange(ranges, cUnit->blockList[i], seqNum);
//        //TODO: phi squash & linear scan promotion
//    }
}
