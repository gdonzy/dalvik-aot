#include <stdio.h>
#include "../DEX.h"
//#include "compiler/codegen/unicore/UnicoreLIR.h"
#include "CompilerUtility.h"
#include "CompilerIR.h"

void dvmCompilerAppendLIR(BasicBlock *bb, LIR *lir)
{
    if (bb->firstLIRInsn == NULL) {
        //assert(cUnit->lastLIRInsn == NULL);
        bb->lastLIRInsn = bb->firstLIRInsn = lir;
        lir->prev = lir->next = NULL;
    } else {
        bb->lastLIRInsn->next = lir;
        lir->prev = bb->lastLIRInsn;
        lir->next = NULL;
        bb->lastLIRInsn = lir;
    }
}
