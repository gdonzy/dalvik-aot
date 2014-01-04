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

/////////global static var///////////////
signed char * instrWidthTable=NULL;	
unsigned char * instrFormatTable=NULL;
CodeList codeList;

////////////////////////////////////////

int main(int argc , char * argv[]){
	int fd=-1;
	off_t start , end;
	size_t length;
	void* memPtr=NULL;
	int i=0;
/////////////define fo var///////////
	DexFile* pDexFile = NULL;
	CodeItem* pCodeItem = NULL;
	DexCode* pDexCode = NULL;
////////////////////////////////////
	codeList.header = NULL;
	codeList.tail = NULL;

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
//////////////////////////call of function///////////////////////////////////		
	char * cp = NULL;

	instrWidthTable=dexCreateInstrWidthTable();
	instrFormatTable=dexCreateInstrFormatTable();
printf("memPtr address is %p", memPtr);

	pDexFile = parseDexFile(memPtr,length);
	
	for(i = 0; i < (int) pDexFile->pHeader->classDefsSize;i++){
		printf(">>>>>>class %d\n",i);
		dumpClass(pDexFile,i);
	}
	
	i = 0;
	for(pCodeItem = codeList.header; pCodeItem != NULL; pCodeItem = pCodeItem->next){
		pDexCode = pCodeItem->item;
//		printf("before bb , sizeof int should be 4,in fact %d",sizeof(int));
		pCodeItem->BBMask_count = (pDexCode->insnsSize*2+3)/4;
		pCodeItem->BBMask = malloc((pDexCode->insnsSize * 2+3)/4*sizeof(int));
		if(NULL == pCodeItem->BBMask){
			printf("error from BBMask!\n");
		}
		memset(pCodeItem->BBMask,0,(pDexCode->insnsSize*2+3)/4*sizeof(int));
		printf("[[[[[[[[[[ code %d ]]]]]]]]]]\n",i++);
		identifyBB(pDexFile, pDexCode, pCodeItem);
	//toidentify  in toOpt.c/h ; //to add BB into pDexCode;	
	}


	printf("begin to process BB!\n");
	i = 0;
	for(pCodeItem = codeList.header; pCodeItem != NULL; pCodeItem = pCodeItem->next){
		pDexCode = pCodeItem->item;
		printf("<<<<<<<<<<< code %d  processing BB >>>>>>>>>>>\n",i++);
		//to do something on processing BB.
		processBB(pDexFile,pDexCode,pCodeItem);		
	}
	
	for(pCodeItem = codeList.header; pCodeItem != NULL; pCodeItem = pCodeItem->next){
		//free more 子项 也要释放的
		free(pCodeItem);
	}
/////////////////////////////////////////////////////////////
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
