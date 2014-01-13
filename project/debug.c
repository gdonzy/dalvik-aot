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
	int size = (bb->sizeOfBuffer)/4;
	int i = 0;
	FILE *fp;
	char filename[30];
	u4 *buffer = bb->codeBuffer;
	for(i = 0; i<size ; i++){
		printf("[code buffer]:%x\n",buffer[i]);
	}
	
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
	BasicBlock *bb = pDebugCUnit->debugBB;
	//to do something (switch case .....)	
	debugNewLIR2(pDebugCUnit,bb,kUnicoreMovImm,r1,20);
	debugNewLIR2(pDebugCUnit,bb,kUnicoreMovImm,r0,0);
	debugNewLIR3(pDebugCUnit,bb,kUnicoreLdwRRR,r2,rFP,r1);
	debugNewLIR3(pDebugCUnit,bb,kUnicoreStwRRR,r2,rFP,r0);	

	debugDvmCompilerAssembleLIR(bb);		
	
	outputCodeBuffer(bb);
}


