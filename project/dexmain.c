#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdlib.h>
#include "toParse.h"
#include "toOpt.h"
#include <string.h>
#include "processBB/hprocessBB.h"
#include "compiler/CompilerUtility.h"
#include "compiler/CompilerIR.h"
#include "compiler/codegen/unicore/UnicoreLIR.h"
#include "compiler/Compiler.h"

#ifdef DEBUG
	#include "compiler/Dataflow.h"
#endif

#include "compiler/codegen/CompilerCodegen.h"

#include "debug.c"

/*************global static var***********/
signed char * instrWidthTable = NULL;	
unsigned char * instrFormatTable = NULL;
CodeList codeList;
CompilationUnitList cUnitList;

/********global debug var************/
u8 cUnit_DexCode=0;
u8 bb_start=0;
int cnt=0;
int flag4debug=0;

int main(int argc , char * argv[]){
	int fd=-1;
	off_t start , end;
	size_t length;
	void* memPtr=NULL;
	int i=0;
/*************define fo var**************/
	DexFile* pDexFile = NULL;
	CodeItem* pCodeItem = NULL;
	DexCode* pDexCode = NULL;

	CompilationUnit *cUnit = NULL;
	BasicBlock *curBB = NULL;
	
/********codeList&cUnitList init*******/
	codeList.header = NULL;
	codeList.tail = NULL;
	cUnitList.header = NULL;
	cUnitList.tail = NULL;

	fd=open(argv[1],O_RDWR);
	start=lseek(fd,0L,SEEK_CUR);
	end=lseek(fd,0L,SEEK_END);
	(void) lseek(fd,start,SEEK_SET);
	
	if(start==(off_t)-1 || end==(off_t)-1){
		printf("could not determine length of file\n");
		return -1;
	}

	length = end - start;
	
	memPtr = mmap(NULL, length , PROT_READ | PROT_WRITE , MAP_FILE | MAP_PRIVATE, fd , start);
	if(memPtr==MAP_FAILED){
		printf("can't mmap dex file\n");
		return -1;
	}

	printf("prepare to parse dex file!\n");
	/*************calls of function*******************/		
	char * cp = NULL;

	instrWidthTable=dexCreateInstrWidthTable();
	instrFormatTable=dexCreateInstrFormatTable();
	printf("memPtr address is %p", memPtr);

	pDexFile = parseDexFile(memPtr,length);
	
	for(i = 0; i < (int) pDexFile->pHeader->classDefsSize;i++){
		printf(">>>>>>class %d\n",i);
		dumpClass(pDexFile,i);
	}
	/*****************identify BB*********************/	
	i = 0;
	for(pCodeItem = codeList.header; pCodeItem != NULL; pCodeItem = pCodeItem->next){
		pDexCode = pCodeItem->item;
		pCodeItem->BBMask_count = (pDexCode->insnsSize*2+3)/4;
		pCodeItem->BBMask = malloc((pDexCode->insnsSize * 2+3)/4*sizeof(int));
		if(NULL == pCodeItem->BBMask){
			printf("error from BBMask!\n");
		}
		memset(pCodeItem->BBMask,0,(pDexCode->insnsSize*2+3)/4*sizeof(int));
		
		printf("[[[[[[[[[[ code %d ]]]]]]]]]]\n",i++);
		identifyBB(pDexFile, pDexCode, pCodeItem);
	}
	/***************init some struct*********************/
	dvmCompilerHeapInit();
	cUnitInit();

	/*******************process BB *********************/
	i = 0;
	printf("begin to process BB!\n");
	for(pCodeItem = codeList.header; pCodeItem != NULL; pCodeItem = pCodeItem->next){
		pDexCode = pCodeItem->item;
		printf("<<<<<<<<<<< code %d  processing BB >>>>>>>>>>>\n",i++);
		//to do something on processing BB.
		processBB(pDexFile,pDexCode,pCodeItem);		
	}

	/******debug : print some info about BBs'mirs******/
	//outputMIRsOfBB
	for(cUnit = cUnitList.header ; cUnit != NULL ; cUnit = cUnit->next){
		for(curBB = cUnit->firstBB ; curBB != NULL ; curBB = curBB->next){
			LOG("[mirs of bb] =====codeoffset is +0x%x+  ======startoffset is +0x%lx+  ======\n", cUnit->pCodeItem->codeBaseAddr , curBB->startOffset);
			outputMIRsOfBB(curBB);
		}	
	}

	/************global debug var initialize**************/
	cUnit_DexCode = strtod(argv[2],NULL);
	bb_start = strtod(argv[3],NULL);
	printf("?????args: %x,%x\n",cUnit_DexCode,bb_start);
	//cUnit_DexCode=0x1e3c;
	//bb_start=0x1e4c;
	//cUnit_DexCode=0x24ec;
	//bb_start=0x24fc;
	flag4debug=0;
	cnt=100;
	

	/*********prepare SSAConversion***********/
	for(cUnit = cUnitList.header ; cUnit != NULL ; cUnit = cUnit->next){
		dvmInitializeSSAConversion(cUnit);
		dvmCompilerNonLoopAnalysis(cUnit);
		dvmCompilerInitializeRegAlloc(cUnit);
		dvmCompilerRegAlloc(cUnit);

	}


	for(cUnit = cUnitList.header ; cUnit != NULL ; cUnit = cUnit->next){
		if( flag4debug || (cUnit_DexCode == (u8)(cUnit->pCodeItem->item) - (u8)(pDexFile->baseAddr) )){
			dvmCompilerMIR2LIR(cUnit);
			for(curBB = cUnit->firstBB ; curBB != NULL ; curBB = curBB->next){
			   if( flag4debug || bb_start == (u8)(curBB->startOffset) ){	
				cUnit->debugBB = curBB;
				debugNewLIR2Assemble(cUnit);
			   }
			}
		}
	}
	
	for(pCodeItem = codeList.header; pCodeItem != NULL; pCodeItem = pCodeItem->next){
		//free more 子项 也要释放的
		free(pCodeItem);
	}
	if(instrWidthTable!=NULL){
		free(instrWidthTable);
	}
	if(instrFormatTable!=NULL){
		free(instrFormatTable);
	}
	if(munmap(memPtr,length)<0){
		printf("munmap failed!");
		return -1;
	}

}
