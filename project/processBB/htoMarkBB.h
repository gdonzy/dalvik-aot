
int markBB_packedSwitch(DexCode* pDexCode, int insnIdx, int insnWidth, DecodedInstruction* pDecInsn, u4* BBMask);


int markBB_sparseSwitch(DexCode* pDexCode, int insnIdx, int insnWidth, DecodedInstruction* pDecInsn, u4* BBMask);
//return 0x0e~11
int markBB_return(DexCode* pDexCode, int insnIdx, int insnWidth, DecodedInstruction* pDecInsn, u4* BBMask);

int markBB_invoke(DexCode* pDexCode, int insnIdx, int insnWidth, DecodedInstruction* pDecInsn, u4* BBMask);

int markBB_invokeRange(DexCode* pDexCode, int insnIdx, int insnWidth, DecodedInstruction* pDecInsn, u4* BBMask);


int markBB_goto(DexCode* pDexCode, int insnIdx, int insnWidth, DecodedInstruction* pDecInsn, u4* BBMask);

int markBB_if(DexCode* pDexCode, int insnIdx, int insnWidth, DecodedInstruction* pDecInsn, u4* BBMask);

int markBB_ifZ(DexCode* pDexCode, int insnIdx, int insnWidth, DecodedInstruction* pDecInsn, u4* BBMask);


int markBB_throw(DexCode* pDexCode, int insnIdx, int insnWidth, DecodedInstruction* pDecInsn, u4* BBMask);


//donzy2delete iget/iput;sget/sput;aget/aput
//int markBB_iget(DexCode* pDexCode, int insnIdx, int insnWidth, DecodedInstruction* pDecInsn, u4* BBMask);
//int markBB_iput(DexCode* pDexCode, int insnIdx, int insnWidth, DecodedInstruction* pDecInsn, u4* BBMask);
//int markBB_sget(DexCode* pDexCode, int insnIdx, int insnWidth, DecodedInstruction* pDecInsn, u4* BBMask);
//int markBB_sput(DexCode* pDexCode, int insnIdx, int insnWidth, DecodedInstruction* pDecInsn, u4* BBMask);
//int markBB_aget(DexCode* pDexCode, int insnIdx, int insnWidth, DecodedInstruction* pDecInsn, u4* BBMask);
//int markBB_aput(DexCode* pDexCode, int insnIdx, int insnWidth, DecodedInstruction* pDecInsn, u4* BBMask);
//





