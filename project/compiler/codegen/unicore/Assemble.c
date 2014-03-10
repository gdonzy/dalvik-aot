#include <stdio.h>
#include "../../../DEX.h"
//#include "Codegen.h"
#include "../../CompilerUtility.h"
#include "../../CompilerIR.h"
#include "UnicoreLIR.h"
#include "Codegen.h"

extern signed char * instrWidthTable;

#define ENCODING_MAP(opcode, skeleton, k0, ds, de, k1, s1s, s1e, k2, s2s, s2e, \
                     k3, k3s, k3e, flags, name, fmt, size) \
        {skeleton, {{k0, ds, de}, {k1, s1s, s1e}, {k2, s2s, s2e}, \
                    {k3, k3s, k3e}}, opcode, flags, name, fmt, size}



UnicoreEncodingMap EncodingMap[kUnicoreLast] = {
/*0*/    ENCODING_MAP(kUnicore32BitData,    0x00000000,
                 kFmtBitBlt, 31, 0, kFmtUnused, -1, -1, kFmtUnused, -1, -1,
                 kFmtUnused, -1, -1, IS_UNARY_OP, "data", "0x!0h(!0d)", 2),


/*1*/    ENCODING_MAP(kUnicoreAdcRRR,        0x0b000000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 4, 0,
                 kFmtUnused, -1, -1,
                 IS_TERTIARY_OP| REG_DEF0_USE12 | SETS_CCODES | USES_CCODES,
                 "adcs", "r!0d, r!1d", 2),
 

/*2*/    ENCODING_MAP(kUnicoreAddPcRel,    0x28f80000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 8, 0, kFmtUnused, -1, -1,
                 kFmtUnused, -1, -1, IS_TERTIARY_OP | IS_BRANCH,
                 "add", "r!0d, pc, #!1E", 2),
 

/*3*/    ENCODING_MAP(kUnicoreAddRRI9,      0x29000000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 8, 0,
                 kFmtUnused, -1, -1,
                 IS_TERTIARY_OP | REG_DEF0_USE1 | SETS_CCODES,
                 "adds", "r!0d, r!1d, #!2d", 2),


/*4*/    ENCODING_MAP(kUnicoreAddRRR,       0x09000000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 4, 0,
                 kFmtUnused, -1, -1,
                 IS_TERTIARY_OP | REG_DEF0_USE12 | SETS_CCODES,
                 "adds", "r!0d, r!1d, r!2d", 2),

/*5*/    ENCODING_MAP(kUnicoreAddSpI9,      0x28ef4000,
                 kFmtBitBlt, 8, 0, kFmtUnused, -1, -1, kFmtUnused, -1, -1,
                 kFmtUnused, -1, -1, IS_UNARY_OP | REG_DEF_SP | REG_USE_SP,
                 "add", "sp, #!0d*4", 2),
 
/*6*/    ENCODING_MAP(kUnicoreAddSpRel,    0x28e80000,
                 kFmtBitBlt, 18, 14, kFmtUnused, -1, -1, kFmtBitBlt, 8, 0,
                 kFmtUnused, -1, -1, IS_TERTIARY_OP | REG_DEF_SP | REG_USE_SP,
                 "add", "r!0d, sp, #!2E", 2),

/*7*/    ENCODING_MAP(kUnicoreAndNRRR,     0x1d000000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 4, 0,
                 kFmtUnused, -1, -1,
                 IS_TERTIARY_OP| REG_DEF0_USE12 | SETS_CCODES,
                 "bics", "r!0d, r!1d", 2),
 


/*8*/    ENCODING_MAP(kUnicoreAndRRR,      0x01000000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 4, 0,
                 kFmtUnused, -1, -1,
                 IS_TERTIARY_OP | REG_DEF0_USE12 | SETS_CCODES,
                 "ands", "r!0d, r!1d", 2),
 

/*9*/    ENCODING_MAP(kUnicoreAsrRRI5,      0x1b000080,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 4, 0, kFmtBitBlt, 13, 9,
                 kFmtUnused, -1, -1,
                 IS_TERTIARY_OP | REG_DEF0_USE1 | SETS_CCODES,
                 "asrs", "r!0d, r!1d, #!2d", 2),
 
/*10*/     ENCODING_MAP(kUnicoreAsrRRR,      0x1b0000a0,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 4, 0, kFmtBitBlt, 13, 9,
                 kFmtUnused, -1, -1,
                 IS_TERTIARY_OP | REG_DEF0_USE12 | SETS_CCODES,
                 "asrs", "r!0d, r!1d", 2),
 

/*11*/    ENCODING_MAP(kUnicoreBCond,        0xa0000000,
                 kFmtBitBlt, 23, 0, kFmtBitBlt, 28, 25, kFmtUnused, -1, -1,
                 kFmtUnused, -1, -1, IS_BINARY_OP | IS_BRANCH | USES_CCODES,
                 "b!1c", "!0t", 2),

/*12*/    ENCODING_MAP(kUnicoreBkpt,         0x00000000,
                 kFmtBitBlt, 31, 0, kFmtUnused, -1, -1, kFmtUnused, -1, -1,
                 kFmtUnused, -1, -1, IS_UNARY_OP | IS_BRANCH,
                 "bkpt", "!0d", 2),


/*13*/    ENCODING_MAP(kUnicoreBl,          0xbd000000,
                 kFmtBitBlt, 23, 0, kFmtUnused, -1, -1, kFmtUnused, -1, -1,
                 kFmtUnused, -1, -1, IS_UNARY_OP | IS_BRANCH | REG_DEF_LR,
                 "bl_1", "!0u", 2),
 


/*14*/    ENCODING_MAP(kUnicoreBUncond,      0xbc000000,
                 kFmtBitBlt, 23, 0, kFmtUnused, -1, -1, kFmtUnused, -1, -1,
                 kFmtUnused, -1, -1, NO_OPERAND | IS_BRANCH,
                 "b", "!0t", 2),//modify by zx 

/*15*/    ENCODING_MAP(kUnicoreCmpAddRR,        0x17000000,
                 kFmtBitBlt, 23, 19, kFmtBitBlt, 4, 0, kFmtUnused, -1, -1,
                 kFmtUnused, -1, -1, IS_BINARY_OP | REG_USE01 | SETS_CCODES,
                 "cmn", "r!0d, r!1d", 2),


/*16*/    ENCODING_MAP(kUnicoreCmpAndRR,        0x11000000,
                 kFmtBitBlt, 23, 19, kFmtBitBlt, 4, 0, kFmtUnused, -1, -1,
                 kFmtUnused, -1, -1, IS_BINARY_OP | REG_USE01 | SETS_CCODES,
                 "tst", "r!0d, r!1d", 2), //need check bug for IS_UNARY_OP 
 


/*17*/    ENCODING_MAP(kUnicoreCmpSubRI9,    0x35000000,
                 kFmtBitBlt, 23, 19, kFmtBitBlt, 8, 0, kFmtUnused, -1, -1,
                 kFmtUnused, -1, -1, IS_BINARY_OP | REG_USE0 | SETS_CCODES,
                 "cmp", "r!0d, #!1d", 2),
/*18*/    ENCODING_MAP(kUnicoreCmpSubRR,     0x15000000,
                 kFmtBitBlt, 23, 19, kFmtBitBlt, 4, 0, kFmtUnused, -1, -1,
                 kFmtUnused, -1, -1, IS_BINARY_OP | REG_USE01 | SETS_CCODES,
                 "cmp", "r!0d, r!1d", 2),
 


/*19*/    ENCODING_MAP(kUnicoreJumplR,       0x11ffc120,
                 kFmtBitBlt, 4, 0, kFmtUnused, -1, -1, kFmtUnused, -1, -1,
                 kFmtUnused, -1, -1,
                 IS_UNARY_OP | REG_USE0 | IS_BRANCH | REG_DEF_LR,
                 "blx", "r!0d", 2),

/*20*/    ENCODING_MAP(kUnicoreLdbRRI14,     0x7d000000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 13, 0,
                 kFmtUnused, -1, -1, IS_TERTIARY_OP | REG_DEF0_USE1 | IS_LOAD,
                 "ldrb", "r!0d, [r!1d, #2d]", 2),
 

/*21*/    ENCODING_MAP(kUnicoreLdbRRR,      0x5d000000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 4, 0,
                 kFmtUnused, -1, -1, IS_TERTIARY_OP | REG_DEF0_USE12 | IS_LOAD,
                 "ldrb", "r!0d, [r!1d, r!2d]", 2),
 
/*22*/    ENCODING_MAP(kUnicoreLdhRRI5I5,   0x5d000160 ,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 13, 9,
                 kFmtBitBlt, 4, 0, IS_QUAD_OP| REG_DEF0_USE1 | IS_LOAD,
                 "ldrh", "r!0d, [r!1d, #!2F]", 2),
 

/*23*/    ENCODING_MAP(kUnicoreLdhRRR,      0x59000160,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 4, 0,
                 kFmtUnused, -1, -1, IS_TERTIARY_OP | REG_DEF0_USE12 | IS_LOAD,
                 "ldrh", "r!0d, [r!1d, r!2d]", 2),

/*24*/    ENCODING_MAP(kUnicoreLdsbRRR,     0x590001a0,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 4, 0,
                 kFmtUnused, -1, -1, IS_TERTIARY_OP | REG_DEF0_USE12 | IS_LOAD,
                 "ldrsb", "r!0d, [r!1d, r!2d]", 2),
 


/*25*/    ENCODING_MAP(kUnicoreLdshRRR,     0x590001e0,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 4, 0,
                 kFmtUnused, -1, -1, IS_TERTIARY_OP | REG_DEF0_USE12 | IS_LOAD,
                 "ldrsh", "r!0d, [r!1d, r!2d]", 2),
 



/*26*/    ENCODING_MAP(kUnicoreLdwPcRel,    0x79f80000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 13, 0, kFmtUnused, -1, -1,
                 kFmtUnused, -1, -1, IS_TERTIARY_OP | REG_DEF0 | REG_USE_PC
                 | IS_LOAD, "ldr", "r!0d, [pc, #!1E]", 2),
/*27*/    ENCODING_MAP(kUnicoreLdwSpRel,    0x79e80000,
                 kFmtBitBlt, 18, 14, kFmtUnused, -1, -1, kFmtBitBlt, 13, 0,
                 kFmtUnused, -1, -1, IS_TERTIARY_OP | REG_DEF0 | REG_USE_SP
                 | IS_LOAD, "ldr", "r!0d, [sp, #!1E]", 2),
/*28*/    ENCODING_MAP(kUnicoreLdwRRI14,      0x79000000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 13, 0,
                 kFmtUnused, -1, -1, IS_TERTIARY_OP | REG_DEF0_USE1 | IS_LOAD,
                 "ldr", "r!0d, [r!1d, #!2E]", 2),
/*29*/    ENCODING_MAP(kUnicoreLdwRRR,       0x59000000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 4, 0,
                 kFmtUnused, -1, -1, IS_TERTIARY_OP | REG_DEF0_USE12 | IS_LOAD,
                 "ldr", "r!0d, [r!1d, r!2d]", 2),

/*30*/    ENCODING_MAP(kUnicoreLslRRI5,      0x1b000000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 4, 0, kFmtBitBlt, 13, 9,
                 kFmtUnused, -1, -1,
                 IS_TERTIARY_OP | REG_DEF0_USE1 | SETS_CCODES,
                 "lsls", "r!0d, r!1d, #!2d", 2),

/*31*/    ENCODING_MAP(kUnicoreLslRRR,        0x1b000020,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 4, 0, kFmtBitBlt, 13, 9,
                 kFmtUnused, -1, -1,
                 IS_TERTIARY_OP | REG_DEF0_USE12 | SETS_CCODES,
                 "lsls", "r!0d, r!1d", 2),
 
/*32*/    ENCODING_MAP(kUnicoreLsrRRI5,      0x1b000040,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 4, 0, kFmtBitBlt, 13, 9,
                 kFmtUnused, -1, -1,
                 IS_TERTIARY_OP | REG_DEF0_USE1 | SETS_CCODES,
                 "lsrs", "r!0d, r!1d, #!2d", 2),
 
/*33*/     ENCODING_MAP(kUnicoreLsrRRR,      0x1b000060,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 4, 0, kFmtBitBlt, 13, 9,
                 kFmtUnused, -1, -1,
                 IS_TERTIARY_OP | REG_DEF0_USE12 | SETS_CCODES,
                 "lsrs", "r!0d, r!1d", 2),
 

/*34*/    ENCODING_MAP(kUnicoreMovImm,       0x3b000000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 8, 0, kFmtUnused, -1, -1,
                 kFmtUnused, -1, -1,
                 IS_BINARY_OP | REG_DEF0 | SETS_CCODES,
                 "movs", "r!0d, #!1d", 2),
 
/*35*/    ENCODING_MAP(kUnicoreMovRR,        0x1b000000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 4, 0, kFmtUnused, -1, -1,
                 kFmtUnused, -1, -1,
                 IS_BINARY_OP | REG_DEF0_USE1 | SETS_CCODES,
                 "movs", "r!0d, r!1d", 2),

/*36*/    ENCODING_MAP(kUnicoreMulRRR,       0x01000120,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 4, 0,
                 kFmtUnused, -1, -1,
                 IS_TERTIARY_OP | REG_DEF0_USE12 | SETS_CCODES,
                 "muls", "r!0d, r!1d", 2),
 


/*37*/    ENCODING_MAP(kUnicoreNotRR,          0x1f000000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 4, 0, kFmtUnused, -1, -1,
                 kFmtUnused, -1, -1,
                 IS_BINARY_OP | REG_DEF0_USE1 | SETS_CCODES,
                 "mvns", "r!0d, r!1d", 2),

/*38*/    ENCODING_MAP(kUnicoreOrrRRR,         0x19000000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 4, 0,
                 kFmtUnused, -1, -1,
                 IS_TERTIARY_OP | REG_DEF0_USE12 | SETS_CCODES,
                 "orrs", "r!0d, r!1d", 2),


/*39*/    ENCODING_MAP(kUnicorePop,           0x69e80004,
                 kFmtBitBlt, 18, 14, kFmtUnused, -1, -1, kFmtUnused, -1, -1,
                 kFmtUnused, -1, -1,
                 IS_UNARY_OP | REG_DEF_SP | REG_USE_SP  
                 | IS_LOAD, "pop", "<!0R>", 2),

/*40*/    ENCODING_MAP(kUnicorePush,        0x72e80004,
                 kFmtBitBlt, 18, 14, kFmtUnused, -1, -1, kFmtUnused, -1, -1,
                 kFmtUnused, -1, -1,
                 IS_UNARY_OP | REG_DEF_SP | REG_USE_SP  
                 | IS_STORE, "push", "<!0R>", 2),
 


/*41*/    ENCODING_MAP(kUnicoreRorRRR,        0x1b0000e0,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 4, 0, kFmtBitBlt, 13, 9,
                 kFmtUnused, -1, -1,
                 IS_TERTIARY_OP | REG_DEF0_USE12 | SETS_CCODES,
                 "rors", "r!0d, r!1d", 2),
 
/*42*/    ENCODING_MAP(kUnicoreRsubRR,         0x27000000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtUnused, -1, -1,
                 kFmtUnused, -1, -1,
                 IS_BINARY_OP | REG_DEF0_USE1 | SETS_CCODES,
                 "negs", "r!0d, r!1d", 2),

/*43*/    ENCODING_MAP(kUnicoreStbRRI14,     0x7c000000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 13, 0,
                 kFmtUnused, -1, -1, IS_TERTIARY_OP | REG_USE01 | IS_STORE,
                 "strb", "r!0d, [r!1d, #!2d]", 2),
 


/*44*/    ENCODING_MAP(kUnicoreStbRRR,      0x5c000000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 4, 0,
                 kFmtUnused, -1, -1, IS_TERTIARY_OP | REG_USE012 | IS_STORE,
                 "strb", "r!0d, [r!1d, r!2d]", 2),
 
/*45*/    ENCODING_MAP(kUnicoreSthRRI5I5,   0x5c000160,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 13, 9,
                 kFmtBitBlt, 4, 0, IS_QUAD_OP| REG_USE01 | IS_STORE,
                 "strh", "r!0d, [r!1d, #!2F]", 2),
 

/*46*/    ENCODING_MAP(kUnicoreSthRRR,      0x58000160,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 4, 0,
                 kFmtUnused, -1, -1, IS_TERTIARY_OP | REG_USE012 | IS_STORE,
                 "strh", "r!0d, [r!1d, r!2d]", 2),
 



/*47*/    ENCODING_MAP(kUnicoreStwRRI14,      0x78000000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 13, 0,
                 kFmtUnused, -1, -1, IS_TERTIARY_OP | REG_USE01 | IS_STORE,
                 "str", "r!0d, [r!1d, #!2E]", 2),
/*48*/    ENCODING_MAP(kUnicoreStwRRR,       0x58000000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 4, 0,
                 kFmtUnused, -1, -1, IS_TERTIARY_OP | REG_USE012 | IS_STORE,
                 "str", "r!0d, [r!1d, r!2d]", 2),

/*49*/    ENCODING_MAP(kUnicoreSubcRRR,      0x0d000000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 4, 0,
                 kFmtUnused, -1, -1,
                 IS_TERTIARY_OP | REG_DEF0_USE12 | USES_CCODES | SETS_CCODES,
                 "sbcs", "r!0d, r!1d", 2),
 


/*50*/    ENCODING_MAP(kUnicoreSubRRI9,      0x25000000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 8, 0,
                 kFmtUnused, -1, -1,
                 IS_TERTIARY_OP | REG_DEF0_USE1 | SETS_CCODES,
                 "subs", "r!0d, r!1d, #!2d]", 2),
 
/*51*/    ENCODING_MAP(kUnicoreSubRRR,       0x05000000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 4, 0,
                 kFmtUnused, -1, -1,
                 IS_TERTIARY_OP | REG_DEF0_USE12 | SETS_CCODES,
                 "subs", "r!0d, r!1d, r!2d", 2),

/*52*/    ENCODING_MAP(kUnicoreSubSpI9,      0x24ef4000,
                 kFmtBitBlt, 8, 0, kFmtUnused, -1, -1, kFmtUnused, -1, -1,
                 kFmtUnused, -1, -1,
                 IS_UNARY_OP | REG_DEF_SP | REG_USE_SP,
                 "sub", "sp, #!0d", 2),
 
/*53*/     ENCODING_MAP(kUnicoreXorRRR,      0x03000000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 4, 0,
                 kFmtUnused, -1, -1,
                 IS_TERTIARY_OP | REG_DEF0_USE12 | SETS_CCODES,
                 "eors", "r!0d, r!1d", 2),
//	ENCODING_MAP(kUnicoreLdhW , 0x5f000140,
//		kFmtBitBlt,18,14, kFmtBitBlt, 23, 19, kFmtBitBlt, 4, 0,
//		kFmtUnused,-1,-1,
//		) 

};

void memAlloc4Assemble(BasicBlock *bb) {
	int size = 0;
	UnicoreLIR * curLIR = NULL;
	for(curLIR = (UnicoreLIR*)(bb->firstLIRInsn) ; curLIR != NULL ; curLIR =(UnicoreLIR *)(curLIR->generic.next)){
		//size += EncodingMap[curLIR->opCode].size*2;		
		size += 4;		
		LOG(" The unicore opcode is %x\n", curLIR->opCode);
	}

	printf("!!!!!The current function is %s: malloc buffer size = %d\n", __func__, size);
	
	if( 0 != size ){
		bb->codeBuffer = dvmCompilerNew(size+12,true);
		bb->sizeOfBuffer = size+12;
		//donzy: 12 is for the code return mterp
	}
	
	if(NULL == bb->codeBuffer){
		printf("error:malloc codebuffer!\n");
	}
	
	return ;
}

//static void assembleInstructions(CompilationUnit *cUnit,
//                                            intptr_t startAddr)
static void assembleInstructions(BasicBlock * bb)
{
//    short *bufferAddr = (short *) cUnit->codeBuffer;
    u4 *bufferAddr = (u4 *) bb->codeBuffer;
    bb->used_codeBuffer = 0;
    UnicoreLIR *lir;

    for (lir = (UnicoreLIR *) bb->firstLIRInsn; lir; lir =(UnicoreLIR *) (lir->generic.next)) {

	if(lir->isNop == true){
		continue;
	}
        
	UnicoreEncodingMap *encoder = &EncodingMap[lir->opCode];
        u4 bits = encoder->skeleton;
		LOG("lir opcode is %x\n", bits);
        int i;
        for (i = 0; i < 4; i++) {
            u4 operand;
            u4 value;
            operand = lir->operands[i];
            switch(encoder->fieldLoc[i].kind) {
                case kFmtUnused:
                    break;
                case kFmtFPImm:
                    value = ((operand & 0xF0) >> 4) << encoder->fieldLoc[i].end;
                    value |= (operand & 0x0F) << encoder->fieldLoc[i].start;
                    bits |= value;
                    break;
                case kFmtBrOffset:
                    value = ((operand  & 0x80000) >> 19) << 26;
                    value |= ((operand & 0x40000) >> 18) << 11;
                    value |= ((operand & 0x20000) >> 17) << 13;
                    value |= ((operand & 0x1f800) >> 11) << 16;
                    value |= (operand  & 0x007ff);
                    bits |= value;
                    break;
                case kFmtShift5:
                    value = ((operand & 0x1c) >> 2) << 12;
                    value |= (operand & 0x03) << 6;
                    bits |= value;
                    break;
                case kFmtShift:
                    value = ((operand & 0x70) >> 4) << 12;
                    value |= (operand & 0x0f) << 4;
                    bits |= value;
                    break;
                case kFmtBWidth:
                    value = operand - 1;
                    bits |= value;
                   break;
               case kFmtLsb:
                    value = ((operand & 0x1c) >> 2) << 12;
                    value |= (operand & 0x03) << 6;
                    bits |= value;
                    break;
                case kFmtImm6:
                    value = ((operand & 0x20) >> 5) << 9;
                    value |= (operand & 0x1f) << 3;
                    bits |= value;
                    break;
                case kFmtBitBlt:
                    value = (operand << encoder->fieldLoc[i].start) &
                            ((1 << (encoder->fieldLoc[i].end + 1)) - 1);
                    bits |= value;
                    break;
                case kFmtDfp: {
//                    assert(DOUBLEREG(operand));
//                    assert((operand & 0x1) == 0);
                    int regName = (operand & FP_REG_MASK) >> 1;
                    /* Snag the 1-bit slice and position it */
                    value = ((regName & 0x10) >> 4) <<
                            encoder->fieldLoc[i].end;
                    /* Extract and position the 4-bit slice */
                    value |= (regName & 0x0f) <<
                            encoder->fieldLoc[i].start;
                    bits |= value;
                    break;
                }
                case kFmtSfp:
//                   assert(SINGLEREG(operand));
                    /* Snag the 1-bit slice and position it */
                    value = (operand & 0x1) <<
                            encoder->fieldLoc[i].end;
                    /* Extract and position the 4-bit slice */
                    value |= ((operand & 0x1e) >> 1) <<
                            encoder->fieldLoc[i].start;
                    bits |= value;
                    break;
                case kFmtImm12:
                case kFmtModImm:
                    value = ((operand & 0x800) >> 11) << 26;
                    value |= ((operand & 0x700) >> 8) << 12;
                    value |= operand & 0x0ff;
                    bits |= value;
                    break;
                case kFmtImm16:
                    value = ((operand & 0x0800) >> 11) << 26;
                    value |= ((operand & 0xf000) >> 12) << 16;
                    value |= ((operand & 0x0700) >> 8) << 12;
                    value |= operand & 0x0ff;
                    bits |= value;
                    break;
                default:
			;
//                    assert(0);
            }
        }
       // if (encoder->size == 2) {
       //     *bufferAddr++ = (bits >> 16) & 0xffff;
       // }
        *bufferAddr++ = bits;
	bb->used_codeBuffer += 4;
    }
//    return kSuccess;
}

//void dvmCompilerAssembleLIR(CompilationUnit *cUnit){
//}

void calWidthMIRs4BB(BasicBlock *bb){
	int size = 0;
	MIR * mir = NULL;
	for(mir = bb->firstMIRInsn ; mir !=NULL ; mir = mir->next){
		size += dexGetInstrWidthAbs(instrWidthTable,mir->dalvikInsn.opCode);
		LOG("The opcode is %x, vA is %d, vB is %d\n", mir->dalvikInsn.opCode, mir->dalvikInsn.vA, mir->dalvikInsn.vB);
	}
	
	bb->sizeOfWidthMIRs = size;
	
	return ;
}

void debugDvmCompilerAssembleLIR(BasicBlock *bb){
	calWidthMIRs4BB(bb);

	memAlloc4Assemble(bb);
	
	assembleInstructions(bb);
}
