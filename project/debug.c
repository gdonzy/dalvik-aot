void dumpDecodedInstruction(const DecodedInstruction *pDecInsn, int insnIdx);

void outputMIRsOfBB(BasicBlock *bb){
	MIR * mir;

	printf("[mirs of bb] =====startoffset is %x  ======\n",bb->startOffset);
	for(mir = bb->firstMIRInsn ; mir != NULL ; mir = mir->next){
		printf("[mirs of bb] ");
		dumpDecodedInstruction(&mir->dalvikInsn,mir->offset);
	}
}
