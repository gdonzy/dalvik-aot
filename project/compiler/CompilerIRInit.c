#include <stdio.h>
#include <stdlib.h>
#include "../DEX.h"
#include "CompilerIR.h"

extern CodeList codeList;
extern CompilationUnitList cUnitList;

void cUnitInit(void){
	CodeItem *pCodeItem = NULL;
	CompilationUnit *cUnit = NULL;
	
	for(pCodeItem = codeList.header;pCodeItem != NULL;pCodeItem = pCodeItem->next){
		cUnit = malloc(sizeof(CompilationUnit));
		if(NULL == cUnit){
			printf("error:malloc cUnit!\n");
		}
		
		cUnit->numBlocks = 0;
		cUnit->firstBB = NULL;
		cUnit->lastBB = NULL;
		cUnit->pCodeItem = pCodeItem;
		cUnit->next = NULL;

		if(NULL != cUnitList.header){
			cUnitList.tail->next = cUnit;
			cUnitList.tail = cUnit;
		}
		else{
			cUnitList.header = cUnit;
			cUnitList.tail = cUnit;
		}
	}
	
	return ;
}
