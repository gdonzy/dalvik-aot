//donzy2fieldOffset
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
int findFieldOffset(MIR *mir){
	int res = 0;
	unsigned long offsetOfDex = mir->offsetOfDex;
	offsetOfDex += 2 ;
	int fd = open("./tmp.odex",O_RDONLY);
	printf("donzy2fieldOffset fd is %d\n",fd);
	lseek(fd,offsetOfDex,SEEK_SET);
	read(fd,&res,2);
	printf("donzy2fieldOffset is %lx : %x\n",offsetOfDex-2,res);
	close(fd);
	
	return res;		
}
//donzy2fieldOffset end

void dumpDecodedInstruction(const DecodedInstruction *pDecInsn, int insnIdx);

void outputMIRsOfBB(BasicBlock *bb){
	MIR * mir;

//	printf("[mirs of bb] =====startoffset is %lx  ======\n",bb->startOffset);
	for(mir = bb->firstMIRInsn ; mir != NULL ; mir = mir->next){
		printf("[mirs of bb]+++:%lx ",mir->offsetOfDex);
		dumpDecodedInstruction(&mir->dalvikInsn,mir->offset);
	}
}

void  debugInsertInsns2BB(BasicBlock *curBB , u2 * insns , int insnsCnt,DexFile *pDexFile);

void outputCodeBuffer(BasicBlock *bb){
	int codeEnd = (bb->used_codeBuffer)/4;
	// donzy:12 is for the code return mterp
	LOG("buffer size is %d\n", bb->sizeOfBuffer);
	int i = 0;
	u4 maskWidth;
	u4 maskTmp;
	FILE *fp;
	char filename[30];
	u4 *buffer = bb->codeBuffer;
	for(i = 0; i<codeEnd ; i++){
		printf("[code buffer]:%x\n",buffer[i]);
	}
	
	/****** insert return code ******/
	i = codeEnd;
	maskWidth = (bb->sizeOfWidthMIRs)*2;
	if(1023 < maskWidth){
		printf("error: num of mirs is too large!\n");
	}
	maskTmp = maskWidth;
	maskWidth &= 0x1f;
	maskWidth |= ((maskTmp&0x3e0)<<4);
	buffer[i++] = ((u4)0x5f8d0160|maskWidth);
	buffer[i++] = (u4)(0x20a700ff);
	buffer[i] = (u4)(0x08afcc1c);
	bb->used_codeBuffer += 12;
	
	int size = 0;	
	int *dataPtr = (int *) ((char *)bb->codeBuffer + bb->dataOffset); 
	UnicoreLIR *dataLIR = (UnicoreLIR *) bb->wordList; 
	while (dataLIR) {
        *dataPtr++ = dataLIR->operands[0];
        dataLIR = dataLIR->generic.next;
		size += 4;
    }
	bb->used_codeBuffer += size;
	
	snprintf(filename,30,"BB-%lx.bin",bb->startOffset);
	printf("[code buffer]:%s",filename);	
	if( NULL == (fp = fopen(filename,"w"))){
		printf("error:can't create file!\n");
	}
	
	fwrite(buffer,bb->used_codeBuffer,1,fp);
	fclose(fp);
	return ;
}

UnicoreLIR *  debugNewLIR0(CompilationUnit *pCUnit,BasicBlock *bb,UnicoreOpCode opCode);
UnicoreLIR *  debugNewLIR1(CompilationUnit *pCUnit,BasicBlock *bb,UnicoreOpCode opCode,int dest);
UnicoreLIR *  debugNewLIR2(CompilationUnit *pCUnit,BasicBlock *bb,UnicoreOpCode opCode,int dest,int src1);
UnicoreLIR *  debugNewLIR3(CompilationUnit *pCUnit,BasicBlock *bb,UnicoreOpCode opCode,int dest,int src1,int src2);
UnicoreLIR *  debugNewLIR4(CompilationUnit *pCUnit,BasicBlock *bb,UnicoreOpCode opCode,int dest,int src1,int src2,int info);

void debugNewLIR2Assemble(CompilationUnit *pDebugCUnit){
//	BasicBlock *bb = pDebugCUnit->debugBB;
	BasicBlock *bb = NULL;
//	for(bb = pDebugCUnit->firstBB; bb; bb = bb->next) {
//		if(bb->startOffset == 0x24fc)
//			break;
//	}
	//to do something (switch case .....)	
//	debugNewLIR2(pDebugCUnit,bb,kUnicoreMovImm,r1,20);
//	debugNewLIR2(pDebugCUnit,bb,kUnicoreMovImm,r0,0);
//	debugNewLIR3(pDebugCUnit,bb,kUnicoreLdwRRR,r2,rFP,r1);
//	debugNewLIR3(pDebugCUnit,bb,kUnicoreStwRRR,r2,rFP,r0);

	//return from nativecode to mterp	
	
	bb = pDebugCUnit->debugBB;	
	debugDvmCompilerAssembleLIR(bb);		
	
	outputCodeBuffer(bb);
}


