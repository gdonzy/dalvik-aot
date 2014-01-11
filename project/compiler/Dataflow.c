#include "../DEX.h"
#include "Dataflow.h"
#include "CompilerUtility.h"
#include "CompilerIR.h"

int dvmConvertSSARegToDalvik(CompilationUnit *cUnit, int ssaReg)
{
      return GET_ELEM_N(cUnit->ssaToDalvikMap, int, ssaReg);
}
