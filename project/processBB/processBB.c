#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "../DEX.h"
#include "../toOpt.h"
#include "htoMarkBB.h"
#include "../compiler/CompilerUtility.h"
#include "../compiler/CompilerIR.h"

extern CompilationUnitList cUnitList;

void outputCode(DexFile * pDexFile, DexCode * pDexCode, CodeItem * pCodeItem);
void outputBBInsns(DexFile * pDexFile, DexCode * pDexCode,int startIdx,int endIdx);
void parseInsn(const u2 *codePtr,DecodedInstruction *decInsn);

void insertBB2cUnit(DexFile *pDexFile,CodeItem *pCodeItem,int startIdx){
	CompilationUnit *cUnit = NULL;
	for(cUnit = cUnitList.header ; cUnit != NULL ; cUnit = cUnit->next){
		if(pCodeItem == cUnit->pCodeItem){
			break;
		}
	}
	if(NULL == cUnit){
		printf("error:can't find cUnit matching the codeItem,insertBB2cUnit!\n");
		return ;
	}
	
	BasicBlock *pBB = dvmCompilerNew(sizeof(BasicBlock),true);
	if(NULL == pBB){
		printf("error:malloc BB!\n");
		return ;
	}

	pBB->startOffset = (u4)( (u1*)(&pCodeItem->item->insns[startIdx]) - (u1*)pDexFile->baseAddr);
	pBB->firstMIRInsn = NULL;
	pBB->lastMIRInsn = NULL;
	pBB->firstLIRInsn = NULL;
	pBB->lastLIRInsn = NULL;
	pBB->next = NULL;
	printf("BB startOffset is : 0x%lx",pBB->startOffset);

	if(NULL == cUnit->firstBB){
		cUnit->firstBB = pBB;	
		cUnit->lastBB = pBB;
	}
	else
	{
		cUnit->lastBB->next = pBB;
		cUnit->lastBB = pBB;
	}
	
	return ;

}

void dvmCompilerAppendMIR(BasicBlock *bb,MIR *mir){
	if(NULL == bb->firstMIRInsn){
		bb->lastMIRInsn = bb->firstMIRInsn = mir;
		mir->prev = mir->next = NULL;
	}
	else
	{
		bb->lastMIRInsn->next = mir;
		mir->prev = bb->lastMIRInsn;
		mir->next = NULL;
		bb->lastMIRInsn = mir;
	}
	
	return ;
}

void insertInsns2BB(DexFile * pDexFile, CodeItem * pCodeItem ,int startIdx,int endIdx){
	int insnIdx = startIdx;
	DexCode * pDexCode = pCodeItem->item;
	u2* insns = pDexCode->insns;
	CompilationUnit *cUnit = NULL;
	BasicBlock *curBB = NULL;
		
        DecodedInstruction decInsn;
	
	//to find to BB
	for(cUnit = cUnitList.header ; cUnit != NULL ; cUnit = cUnit->next){
		if(pCodeItem == cUnit->pCodeItem){
			break;
		}
	}
	if(NULL == cUnit){
		printf("error:can't find cUnit matching the codeItem,insertInsns2BB!\n");
		return ;
	}
	curBB = cUnit->lastBB;
    while (insnIdx < endIdx) {
	MIR * mir;
	mir = dvmCompilerNew(sizeof(MIR),true);
	if( NULL==mir ){
		printf("error : alloc mem of mir!\n");
		return ;
	}
	mir->offset = insnIdx;
	mir->prev = NULL;
	mir->next = NULL;
	mir->ssaRep = NULL;

	parseInsn(&insns[insnIdx],&mir->dalvikInsn);
	dvmCompilerAppendMIR(curBB,mir);
	nextInsn(insns,&insnIdx);
    }
}
		

void debugInsertInsns2BB(BasicBlock *curBB,u2 * insns,int insnsCnt){
	int insnIdx = 0;
		
        DecodedInstruction decInsn;
	
    while (insnIdx < insnsCnt) {
	MIR * mir;
	mir = dvmCompilerNew(sizeof(MIR),true);
	if( NULL==mir ){
		printf("error : alloc mem of mir[debug]!\n");
		return ;
	}
	mir->offset = insnIdx;
	mir->prev = NULL;
	mir->next = NULL;
	mir->ssaRep = NULL;

	parseInsn(&insns[insnIdx],&mir->dalvikInsn);
	dvmCompilerAppendMIR(curBB,mir);
	nextInsn(insns,&insnIdx);
    }
    
    return ;
}

void outputBBMask(u4 *pBBMask,int count){
	u4 i=0;
	u4 *pTmp = malloc(sizeof(u4)*count);
	if(NULL == pTmp){
		printf("malloc error from outputBBMask!\n");
		return ;
	}
	memcpy(pTmp,pBBMask,sizeof(u4)*count);
	i=getFirstIndexFromBBMask(pTmp,count);
	printf("BB begin is : ");
	while(0xff != i){
		printf(" %x--%x  ",i,i/2);
		clearBBMask(i,pTmp);
		i=getFirstIndexFromBBMask(pTmp,count);
	}
	printf("\n");
	free(pTmp);
	return ;
}


void processBB(DexFile * pDexFile , DexCode * pDexCode , CodeItem * pCodeItem)
{
	int startFirst = 0xff;
	int startSec = 0xff;
	int startFirstIdx = 0;
	int startSecIdx = 0;
	int lengthmore = 0;
	int curIdx = 0 ;
	int insnsCountInBB = 0;	
	u2 * insns = pDexCode->insns;
	u2 instr;
	u4 * pBBMask = pCodeItem->BBMask;
	int BBMask_count = pCodeItem->BBMask_count;
	MARKBB markBB[256];
	int i=0;

	for(i=0;i<256;i++){
		markBB[i] = NULL;
	}	
#include "../markBB.h"

	//information of DexCode
	printf("=::= \n=::= ~~~~~~codeoffset=%p; insns off=%p;registers is %d;insns size is %d~~~~~~~~\n\n",(void *)((u1 *)pDexCode - pDexFile->baseAddr),(void *)((u1*)(pDexCode->insns) - pDexFile->baseAddr),pDexCode->registersSize,pDexCode->insnsSize);
	printf("==!\n");
	outputCode(pDexFile , pDexCode , pCodeItem);	
	outputBBMask(pBBMask,BBMask_count);

	startFirst = 0;
	startSec = getFirstIndexFromBBMask(pBBMask ,BBMask_count);
	startFirstIdx = startFirst/2;
	startSecIdx = startSec/2;
printf("start add mir ,first is %x ; second is %x!\n",startFirstIdx,startSecIdx);		
	while(0xff != startFirst){
//		curLen = nextInsn(insns,&curIdx);
			
		curIdx = startFirstIdx;
		insnsCountInBB = 0;
		while(1){
			instr = getInsn(insns,curIdx);
			if(markBB[instr&0xff] != NULL){
				//to add mir (from startFirstIdx to curIdx(not))
				printf("end with jump\n");
				printf("start of BB is %x, \n",startFirstIdx);
				printf("before end of BB is %x \n",curIdx);
				printf("count insns of BB is %d \n",insnsCountInBB);
				printf("========================\n");
				if(3 <= (curIdx-startFirstIdx)){
					printf("=::== -----------------------\n");
					outputBBInsns(pDexFile,pDexCode,startFirstIdx,curIdx);
					printf("=::= -----------------------\n");
					insertBB2cUnit(pDexFile,pCodeItem,startFirstIdx);
					insertInsns2BB(pDexFile,pCodeItem,startFirstIdx,curIdx);
				}
				break;
			}	
			else if(curIdx == startSecIdx){
				//to add mir (from startFirstIdx to curIdx(not))
				printf("end with second!\n");
				printf("start of BB is %x, \n",startFirstIdx);
				printf("before end of BB is %x \n",curIdx);
				printf("count insns of BB is %d \n",insnsCountInBB);
				printf("========================\n");
				if(3 <= (curIdx-startFirstIdx)){
					printf("=::== -----------------------\n");
					outputBBInsns(pDexFile,pDexCode,startFirstIdx,curIdx);
					printf("=::= -----------------------\n");
					insertBB2cUnit(pDexFile,pCodeItem,startFirstIdx);
					insertInsns2BB(pDexFile,pCodeItem,startFirstIdx,curIdx);
				}
				break;
			}
			else if(0xff == insnIsData(insns,curIdx)){
				//to add mir (from startFirstIdx to curIdx(not))
				//to set startFirst = 0xff;
				printf("end with data!\n");
				printf("start of BB is %x, \n",startFirstIdx);
				printf("before end of BB is %x \n",curIdx);
				printf("count insns of BB is %d \n",insnsCountInBB);
				printf("========================\n");
				if(3 <= (curIdx-startFirstIdx)){
					printf("=::== -----------------------\n");
					outputBBInsns(pDexFile,pDexCode,startFirstIdx,curIdx);
					printf("=::= -----------------------\n");
					insertBB2cUnit(pDexFile,pCodeItem,startFirstIdx);
					insertInsns2BB(pDexFile,pCodeItem,startFirstIdx,curIdx);
				}
				startFirst = 0xff;
				break;
			}
			else if(0xff == startSec && curIdx >= pDexCode->insnsSize){
				
				printf("end with end!\n");
				printf("start of BB is %x, \n",startFirstIdx);
				printf("before end of BB is %x \n",curIdx);
				printf("count insns of BB is %d \n",insnsCountInBB);
				printf("========================\n");
				if(3 <= (curIdx-startFirstIdx)){
					printf("=::== -----------------------\n");
					outputBBInsns(pDexFile,pDexCode,startFirstIdx,curIdx);
					printf("=::= -----------------------\n");
				}
				startFirst = 0xff;
				break;
			}
			else{
				nextInsn(insns,&curIdx);
				insnsCountInBB++;
			}
		}
//		printf("-----first is %x , second is %x , firstIdx is %x , secondIdx is %x\n",startFirst,startSec,startFirstIdx,startSecIdx);
		clearBBMask(startFirst,pBBMask);
		startFirst = startSec;
		clearBBMask(startSec,pBBMask);
		startSec = getFirstIndexFromBBMask(pBBMask ,BBMask_count);
		startFirstIdx = startFirst/2;
		startSecIdx = startSec/2;
//		printf("+++++ first is %x , second is %x , firstIdx is %x , secondIdx is %x\n",startFirst,startSec,startFirstIdx,startSecIdx);
	}	
			
}

