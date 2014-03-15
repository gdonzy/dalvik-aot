//OpCode;
int getFirstIndexFromBBMask(u4* BBMask,int size){
	int i = 0;
	u4 bits = 0;
	u4 res;
	
	res = 0;
	for(i=0; i<size; i++){
		if(BBMask[i] != 0){
			bits = BBMask[i];
			bits ^= (bits-1);
			while(bits != 0){
				bits >>= 1;
				res++;
			}
			res = res + (i<<5) - 1;
			break;
		}
		
	}
	if( 0==res  ){
		return 0xff;
	}
	return res;
}


void clearBBMask(u4 targetPos, u4* BBMask){
	u4 bitMask;
	bitMask = targetPos & 0x1f;
	bitMask = 0x01 << bitMask;
	bitMask = ~bitMask;

	BBMask[targetPos >> 5] = BBMask[targetPos >> 5] & bitMask;	
}

void setBBMaskIfNecessary(u4 targetPos, u4* BBMask){
	printf("mark pos is %x : %x ==\n", targetPos ,targetPos/2);
	u4 bitMask;
	bitMask = targetPos & 0x1f;
	
	if((BBMask[targetPos >> 5] & (0x01 << bitMask)) ==0 ){
		BBMask[targetPos >> 5] = BBMask[targetPos >> 5] | (0x01 << bitMask);
	}
}

int markBB_packedSwitch(DexCode* pDexCode, int insnIdx, int insnWidth, DecodedInstruction* pDecInsn, u4* BBMask){
	int targetPos = 0;	
	int index = 0;
	u2* insns = NULL;
	u2 size = 0;
	int i = 0;
	int* entries = NULL;
	
	if(pDecInsn->opCode != OP_PACKED_SWITCH){
		printf("error: opcode can't match in markBB_packedSwitch\n");
		return -1;
	}
	
	index = insnIdx + pDecInsn->vB;
	insns = &(pDexCode->insns[index]);
	
	size = insns[1];
	entries = (int *)&insns[4];
	for(i=0; i<size; i++){
		printf("entries[%d] is %x",i,entries[i]);
		targetPos = (insnIdx + (int)entries[i])*2;
		setBBMaskIfNecessary(targetPos,BBMask);	
	}
	
	if(insnIdx+insnWidth >= pDexCode->insnsSize){
		return 0;
	}
	targetPos = (insnIdx + insnWidth)*2 ;
	setBBMaskIfNecessary(targetPos,BBMask);

}

int markBB_sparseSwitch(DexCode* pDexCode, int insnIdx, int insnWidth, DecodedInstruction* pDecInsn, u4* BBMask){
	//to do something
	if(insnIdx+insnWidth >= pDexCode->insnsSize){
		return 0;
	}
	printf("this function doesn't have code!\n");
	return -1;
}

//return 0x0e~11
int markBB_return(DexCode* pDexCode, int insnIdx, int insnWidth, DecodedInstruction* pDecInsn, u4* BBMask){
	int targetPos = 0;

	if(pDecInsn->opCode<0x0e || pDecInsn->opCode>0x11){
		printf("error from markBB_return!\n");
		return -1;
	}

	if(insnIdx+insnWidth >= pDexCode->insnsSize){
		return 0;
	}
	
	targetPos = (insnIdx + insnWidth)*2 ;
	setBBMaskIfNecessary(targetPos,BBMask);
}

int markBB_invoke(DexCode* pDexCode, int insnIdx, int insnWidth, DecodedInstruction* pDecInsn, u4* BBMask){
	int targetPos = 0;

	if(pDecInsn->opCode<0x6e || pDecInsn->opCode>0x72){
		printf("error from markBB_invoke!\n");
		return -1;
	}
	if(insnIdx+insnWidth >= pDexCode->insnsSize){
		return 0;
	}
	
	targetPos = (insnIdx + insnWidth)*2 ;
	setBBMaskIfNecessary(targetPos,BBMask);
}

int markBB_invokeRange(DexCode* pDexCode, int insnIdx, int insnWidth, DecodedInstruction* pDecInsn, u4* BBMask){
	int targetPos = 0 ;

	if(pDecInsn->opCode<0x74 || pDecInsn->opCode>0x78){
		printf("error from markBB_invokeRange!\n");
		return -1;
	}
	if(insnIdx+insnWidth >= pDexCode->insnsSize){
		return 0;
	}
	
	targetPos = (insnIdx + insnWidth)*2 ;
	setBBMaskIfNecessary(targetPos,BBMask);
}


int markBB_goto(DexCode* pDexCode, int insnIdx, int insnWidth, DecodedInstruction* pDecInsn, u4* BBMask){
	int targetPos = 0 ;

	if(pDecInsn->opCode<0x28 || pDecInsn->opCode>0x2a){
		printf("error from markBB_goto!\n");
		return -1;
	}
	
	targetPos = (insnIdx + (int)pDecInsn->vA)*2 ;
	setBBMaskIfNecessary(targetPos,BBMask);
	
	if(insnIdx+insnWidth >= pDexCode->insnsSize){
		return 0;
	}
	targetPos = (insnIdx + insnWidth)*2 ;
	setBBMaskIfNecessary(targetPos,BBMask);
}

int markBB_if(DexCode* pDexCode, int insnIdx, int insnWidth, DecodedInstruction* pDecInsn, u4* BBMask){
	int targetPos = 0 ;

	if(pDecInsn->opCode<0x32 || pDecInsn->opCode>0x37){
		printf("error from markBB_if!\n");
		return -1;
	}
	
	targetPos = (insnIdx + (int)pDecInsn->vC)*2 ;
	setBBMaskIfNecessary(targetPos,BBMask);
	
	if(insnIdx+insnWidth >= pDexCode->insnsSize){
		return 0;
	}
	targetPos = (insnIdx + insnWidth)*2 ;
	setBBMaskIfNecessary(targetPos,BBMask);
}

int markBB_ifZ(DexCode* pDexCode, int insnIdx, int insnWidth, DecodedInstruction* pDecInsn, u4* BBMask){
	int targetPos = 0 ;

	if(pDecInsn->opCode<0x38 || pDecInsn->opCode>0x3d){
		printf("error from markBB_ifZ!\n");
		return -1;
	}
	
	targetPos = (insnIdx + (int)pDecInsn->vB)*2 ;
	setBBMaskIfNecessary(targetPos,BBMask);
	
	if(insnIdx+insnWidth >= pDexCode->insnsSize){
		return 0;
	}
	targetPos = (insnIdx + insnWidth)*2 ;
	setBBMaskIfNecessary(targetPos,BBMask);
}


int markBB_throw(DexCode* pDexCode, int insnIdx, int insnWidth, DecodedInstruction* pDecInsn, u4* BBMask){
	return 0;
}

////donzy2delete iget
//int markBB_iget(DexCode* pDexCode, int insnIdx, int insnWidth, DecodedInstruction* pDecInsn, u4* BBMask){
//	int targetPos = 0;
//	
//	targetPos = (insnIdx + insnWidth)*2;
//	setBBMaskIfNecessary(targetPos,BBMask);
//}
//
////donzy2delete iput
//int markBB_iput(DexCode* pDexCode, int insnIdx, int insnWidth, DecodedInstruction* pDecInsn, u4* BBMask){
//	int targetPos = 0;
//	
//	targetPos = (insnIdx + insnWidth)*2;
//	setBBMaskIfNecessary(targetPos,BBMask);
//}
//
//
////donzy2delete sget
//int markBB_sget(DexCode* pDexCode, int insnIdx, int insnWidth, DecodedInstruction* pDecInsn, u4* BBMask){
//	int targetPos = 0;
//	
//	targetPos = (insnIdx + insnWidth)*2;
//	setBBMaskIfNecessary(targetPos,BBMask);
//}
//
//
////donzy2delete sput
//int markBB_sput(DexCode* pDexCode, int insnIdx, int insnWidth, DecodedInstruction* pDecInsn, u4* BBMask){
//	int targetPos = 0;
//	
//	targetPos = (insnIdx + insnWidth)*2;
//	setBBMaskIfNecessary(targetPos,BBMask);
//}
//
//
////donzy2delete aget
//int markBB_aget(DexCode* pDexCode, int insnIdx, int insnWidth, DecodedInstruction* pDecInsn, u4* BBMask){
//	int targetPos = 0;
//	
//	targetPos = (insnIdx + insnWidth)*2;
//	setBBMaskIfNecessary(targetPos,BBMask);
//}
//
//
////donzy2delete aput
//int markBB_aput(DexCode* pDexCode, int insnIdx, int insnWidth, DecodedInstruction* pDecInsn, u4* BBMask){
//	int targetPos = 0;
//	
//	targetPos = (insnIdx + insnWidth)*2;
//	setBBMaskIfNecessary(targetPos,BBMask);
//}
//


//donzy:BBfilter
int markBB_filter(DexCode* pDexCode, int insnIdx, int insnWidth, DecodedInstruction* pDecInsn, u4* BBMask){
	int targetPos = 0;
	
	targetPos = (insnIdx + insnWidth)*2;
	setBBMaskIfNecessary(targetPos,BBMask);
}




