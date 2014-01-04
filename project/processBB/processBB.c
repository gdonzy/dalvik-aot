#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "../DEX.h"
#include "../toOpt.h"
#include "htoMarkBB.h"

void outputCode(DexFile * pDexFile, DexCode * pDexCode, CodeItem * pCodeItem);
void outputBBInsns(DexFile * pDexFile, DexCode * pDexCode,int startIdx,int endIdx);

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
					printf("=::= -----------------------\n");
					outputBBInsns(pDexFile,pDexCode,startFirstIdx,curIdx);
					printf("=::= -----------------------\n");
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
					printf("=::= -----------------------\n");
					outputBBInsns(pDexFile,pDexCode,startFirstIdx,curIdx);
					printf("=::= -----------------------\n");
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
					printf("=::= -----------------------\n");
					outputBBInsns(pDexFile,pDexCode,startFirstIdx,curIdx);
					printf("=::= -----------------------\n");
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
					printf("=::= -----------------------\n");
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

