void dumpDecodedInstruction(const DecodedInstruction *pDecInsn, int insnIdx);

void outputMIRsOfBB(BasicBlock *bb){
	MIR * mir;

	printf("[mirs of bb] =====startoffset is %lx  ======\n",bb->startOffset);
	for(mir = bb->firstMIRInsn ; mir != NULL ; mir = mir->next){
		printf("[mirs of bb] ");
		dumpDecodedInstruction(&mir->dalvikInsn,mir->offset);
	}
}

void  debugInsertInsns2BB(BasicBlock *curBB , u2 * insns , int insnsCnt);

void outputCodeBuffer(BasicBlock *bb){
	int codeEnd = (bb->sizeOfBuffer - 12)/4;
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
	
		
	snprintf(filename,30,"BB-%lx.bin",bb->startOffset);
	printf("[code buffer]:%s",filename);	
	if( NULL == (fp = fopen(filename,"w"))){
		printf("error:can't create file!\n");
	}
	
	fwrite(buffer,bb->sizeOfBuffer,1,fp);
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


