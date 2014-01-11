#include "../../../DEX.h"
#include "../../CompilerUtility.h"
#include "../../CompilerIR.h"
#include "UnicoreLIR.h"
//#include "Codegen.h"
#include "Ralloc.h"


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
