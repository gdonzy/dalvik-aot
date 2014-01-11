#include "../DEX.h"
#include "CompilerUtility.h"
#include "CompilerIR.h"
#include "Dataflow.h"

void dvmInitializeSSAConversion(CompilationUnit *cUnit)
{
	int i;
	int numDalvikReg = cUnit->pCodeItem->item->registersSize;
	
	dvmInitGrowableList(cUnit->ssaToDalvikMap, numDalvikReg);
	
	cUnit->numSSARegs = numDalvikReg;

	for(i = 0; i < numDalvikReg; i++) {
		dvmInsertGrowableList(cUnit->ssaToDalvikMap, (void *)ENCODE_REG_SUB(i, 0));
	}
	
	cUnit->dalvikToSSAMap = dvmCompilerNew(sizeof(int) * numDalvikReg, false);
	for (i = 0; i < numDalvikReg; i++) {  
		cUnit->dalvikToSSAMap[i] = i; 
	}
/*  这里暂时作用不知道，所以删除
	for (i = 0; i < cUnit->numBlocks; i++) { 
		BasicBlock *bb = cUnit->blockList[i]; 
		if (bb->blockType == kDalvikByteCode || 
			bb->blockType == kTraceEntryBlock) { 
			bb->dataFlowInfo = dvmCompilerNew(sizeof(BasicBlockDataFlow), true); 
		}
	}	
*/
}
