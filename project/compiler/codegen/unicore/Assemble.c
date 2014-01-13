#include "../../../DEX.h"
#include "UnicoreLIR.h"
#include "../../CompilerUtility.h"
#include "../../CompilerIR.h"



#define ENCODING_MAP(opcode, skeleton, k0, ds, de, k1, s1s, s1e, k2, s2s, s2e, \
                     k3, k3s, k3e, flags, name, fmt, size) \
        {skeleton, {{k0, ds, de}, {k1, s1s, s1e}, {k2, s2s, s2e}, \
                    {k3, k3s, k3e}}, opcode, flags, name, fmt, size}



UnicoreEncodingMap EncodingMap[kUnicoreLast] = {
    ENCODING_MAP(kUnicore32BitData,    0x00000000,
                 kFmtBitBlt, 31, 0, kFmtUnused, -1, -1, kFmtUnused, -1, -1,
                 kFmtUnused, -1, -1, IS_UNARY_OP, "data", "0x!0h(!0d)", 2),


    ENCODING_MAP(kUnicoreAdcRRR,        0x0b000000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 4, 0,
                 kFmtUnused, -1, -1,
                 IS_TERTIARY_OP| REG_DEF0_USE12 | SETS_CCODES | USES_CCODES,
                 "adcs", "r!0d, r!1d", 2),
 

    ENCODING_MAP(kUnicoreAddPcRel,    0x28f80000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 8, 0, kFmtUnused, -1, -1,
                 kFmtUnused, -1, -1, IS_TERTIARY_OP | IS_BRANCH,
                 "add", "r!0d, pc, #!1E", 2),
 

    ENCODING_MAP(kUnicoreAddRRI9,      0x29000000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 8, 0,
                 kFmtUnused, -1, -1,
                 IS_TERTIARY_OP | REG_DEF0_USE1 | SETS_CCODES,
                 "adds", "r!0d, r!1d, #!2d", 2),


    ENCODING_MAP(kUnicoreAddRRR,       0x09000000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 4, 0,
                 kFmtUnused, -1, -1,
                 IS_TERTIARY_OP | REG_DEF0_USE12 | SETS_CCODES,
                 "adds", "r!0d, r!1d, r!2d", 2),

    ENCODING_MAP(kUnicoreAddSpI9,      0x28ef4000,
                 kFmtBitBlt, 8, 0, kFmtUnused, -1, -1, kFmtUnused, -1, -1,
                 kFmtUnused, -1, -1, IS_UNARY_OP | REG_DEF_SP | REG_USE_SP,
                 "add", "sp, #!0d*4", 2),
 
    ENCODING_MAP(kUnicoreAddSpRel,    0x28e80000,
                 kFmtBitBlt, 18, 14, kFmtUnused, -1, -1, kFmtBitBlt, 8, 0,
                 kFmtUnused, -1, -1, IS_TERTIARY_OP | REG_DEF_SP | REG_USE_SP,
                 "add", "r!0d, sp, #!2E", 2),

    ENCODING_MAP(kUnicoreAndNRRR,     0x1d000000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 4, 0,
                 kFmtUnused, -1, -1,
                 IS_TERTIARY_OP| REG_DEF0_USE12 | SETS_CCODES,
                 "bics", "r!0d, r!1d", 2),
 


    ENCODING_MAP(kUnicoreAndRRR,      0x01000000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 4, 0,
                 kFmtUnused, -1, -1,
                 IS_TERTIARY_OP | REG_DEF0_USE12 | SETS_CCODES,
                 "ands", "r!0d, r!1d", 2),
 

    ENCODING_MAP(kUnicoreAsrRRI5,      0x1b000080,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 4, 0, kFmtBitBlt, 13, 9,
                 kFmtUnused, -1, -1,
                 IS_TERTIARY_OP | REG_DEF0_USE1 | SETS_CCODES,
                 "asrs", "r!0d, r!1d, #!2d", 2),
 
     ENCODING_MAP(kUnicoreAsrRRR,      0x1b0000a0,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 4, 0, kFmtBitBlt, 13, 9,
                 kFmtUnused, -1, -1,
                 IS_TERTIARY_OP | REG_DEF0_USE12 | SETS_CCODES,
                 "asrs", "r!0d, r!1d", 2),
 

    ENCODING_MAP(kUnicoreBCond,        0xa0000000,
                 kFmtBitBlt, 23, 0, kFmtBitBlt, 28, 25, kFmtUnused, -1, -1,
                 kFmtUnused, -1, -1, IS_BINARY_OP | IS_BRANCH | USES_CCODES,
                 "b!1c", "!0t", 2),

    ENCODING_MAP(kUnicoreBkpt,         0x00000000,
                 kFmtBitBlt, 31, 0, kFmtUnused, -1, -1, kFmtUnused, -1, -1,
                 kFmtUnused, -1, -1, IS_UNARY_OP | IS_BRANCH,
                 "bkpt", "!0d", 2),



    ENCODING_MAP(kUnicoreBl,          0xbd000000,
                 kFmtBitBlt, 23, 0, kFmtUnused, -1, -1, kFmtUnused, -1, -1,
                 kFmtUnused, -1, -1, IS_UNARY_OP | IS_BRANCH | REG_DEF_LR,
                 "bl_1", "!0u", 2),
 


    ENCODING_MAP(kUnicoreBUncond,      0xbc000000,
                 kFmtBitBlt, 23, 0, kFmtUnused, -1, -1, kFmtUnused, -1, -1,
                 kFmtUnused, -1, -1, NO_OPERAND | IS_BRANCH,
                 "b", "!0t", 2),//modify by zx 

    ENCODING_MAP(kUnicoreCmpAddRR,        0x17000000,
                 kFmtBitBlt, 23, 19, kFmtBitBlt, 4, 0, kFmtUnused, -1, -1,
                 kFmtUnused, -1, -1, IS_BINARY_OP | REG_USE01 | SETS_CCODES,
                 "cmn", "r!0d, r!1d", 2),


    ENCODING_MAP(kUnicoreCmpAndRR,        0x11000000,
                 kFmtBitBlt, 23, 19, kFmtBitBlt, 4, 0, kFmtUnused, -1, -1,
                 kFmtUnused, -1, -1, IS_BINARY_OP | REG_USE01 | SETS_CCODES,
                 "tst", "r!0d, r!1d", 2), //need check bug for IS_UNARY_OP 
 


    ENCODING_MAP(kUnicoreCmpSubRI9,    0x35000000,
                 kFmtBitBlt, 23, 19, kFmtBitBlt, 8, 0, kFmtUnused, -1, -1,
                 kFmtUnused, -1, -1, IS_BINARY_OP | REG_USE0 | SETS_CCODES,
                 "cmp", "r!0d, #!1d", 2),
    ENCODING_MAP(kUnicoreCmpSubRR,     0x15000000,
                 kFmtBitBlt, 23, 19, kFmtBitBlt, 4, 0, kFmtUnused, -1, -1,
                 kFmtUnused, -1, -1, IS_BINARY_OP | REG_USE01 | SETS_CCODES,
                 "cmp", "r!0d, r!1d", 2),
 


    ENCODING_MAP(kUnicoreJumplR,       0x11ffc120,
                 kFmtBitBlt, 4, 0, kFmtUnused, -1, -1, kFmtUnused, -1, -1,
                 kFmtUnused, -1, -1,
                 IS_UNARY_OP | REG_USE0 | IS_BRANCH | REG_DEF_LR,
                 "blx", "r!0d", 2),

    ENCODING_MAP(kUnicoreLdbRRI14,     0x7d000000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 13, 0,
                 kFmtUnused, -1, -1, IS_TERTIARY_OP | REG_DEF0_USE1 | IS_LOAD,
                 "ldrb", "r!0d, [r!1d, #2d]", 2),
 

    ENCODING_MAP(kUnicoreLdbRRR,      0x5d000000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 4, 0,
                 kFmtUnused, -1, -1, IS_TERTIARY_OP | REG_DEF0_USE12 | IS_LOAD,
                 "ldrb", "r!0d, [r!1d, r!2d]", 2),
 
    ENCODING_MAP(kUnicoreLdhRRI5I5,   0x5d000160 ,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 13, 9,
                 kFmtBitBlt, 4, 0, IS_QUAD_OP| REG_DEF0_USE1 | IS_LOAD,
                 "ldrh", "r!0d, [r!1d, #!2F]", 2),
 

    ENCODING_MAP(kUnicoreLdhRRR,      0x59000160,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 4, 0,
                 kFmtUnused, -1, -1, IS_TERTIARY_OP | REG_DEF0_USE12 | IS_LOAD,
                 "ldrh", "r!0d, [r!1d, r!2d]", 2),

    ENCODING_MAP(kUnicoreLdsbRRR,     0x590001a0,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 4, 0,
                 kFmtUnused, -1, -1, IS_TERTIARY_OP | REG_DEF0_USE12 | IS_LOAD,
                 "ldrsb", "r!0d, [r!1d, r!2d]", 2),
 


    ENCODING_MAP(kUnicoreLdshRRR,     0x590001e0,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 4, 0,
                 kFmtUnused, -1, -1, IS_TERTIARY_OP | REG_DEF0_USE12 | IS_LOAD,
                 "ldrsh", "r!0d, [r!1d, r!2d]", 2),
 



    ENCODING_MAP(kUnicoreLdwPcRel,    0x79f80000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 13, 0, kFmtUnused, -1, -1,
                 kFmtUnused, -1, -1, IS_TERTIARY_OP | REG_DEF0 | REG_USE_PC
                 | IS_LOAD, "ldr", "r!0d, [pc, #!1E]", 2),
    ENCODING_MAP(kUnicoreLdwSpRel,    0x79e80000,
                 kFmtBitBlt, 18, 14, kFmtUnused, -1, -1, kFmtBitBlt, 13, 0,
                 kFmtUnused, -1, -1, IS_TERTIARY_OP | REG_DEF0 | REG_USE_SP
                 | IS_LOAD, "ldr", "r!0d, [sp, #!1E]", 2),
    ENCODING_MAP(kUnicoreLdwRRI14,      0x79000000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 13, 0,
                 kFmtUnused, -1, -1, IS_TERTIARY_OP | REG_DEF0_USE1 | IS_LOAD,
                 "ldr", "r!0d, [r!1d, #!2E]", 2),
    ENCODING_MAP(kUnicoreLdwRRR,       0x59000000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 4, 0,
                 kFmtUnused, -1, -1, IS_TERTIARY_OP | REG_DEF0_USE12 | IS_LOAD,
                 "ldr", "r!0d, [r!1d, r!2d]", 2),

    ENCODING_MAP(kUnicoreLslRRI5,      0x1b000000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 4, 0, kFmtBitBlt, 13, 9,
                 kFmtUnused, -1, -1,
                 IS_TERTIARY_OP | REG_DEF0_USE1 | SETS_CCODES,
                 "lsls", "r!0d, r!1d, #!2d", 2),

    ENCODING_MAP(kUnicoreLslRRR,        0x1b000020,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 4, 0, kFmtBitBlt, 13, 9,
                 kFmtUnused, -1, -1,
                 IS_TERTIARY_OP | REG_DEF0_USE12 | SETS_CCODES,
                 "lsls", "r!0d, r!1d", 2),
 
    ENCODING_MAP(kUnicoreLsrRRI5,      0x1b000040,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 4, 0, kFmtBitBlt, 13, 9,
                 kFmtUnused, -1, -1,
                 IS_TERTIARY_OP | REG_DEF0_USE1 | SETS_CCODES,
                 "lsrs", "r!0d, r!1d, #!2d", 2),
 
     ENCODING_MAP(kUnicoreLsrRRR,      0x1b000060,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 4, 0, kFmtBitBlt, 13, 9,
                 kFmtUnused, -1, -1,
                 IS_TERTIARY_OP | REG_DEF0_USE12 | SETS_CCODES,
                 "lsrs", "r!0d, r!1d", 2),
 

    ENCODING_MAP(kUnicoreMovImm,       0x3b000000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 8, 0, kFmtUnused, -1, -1,
                 kFmtUnused, -1, -1,
                 IS_BINARY_OP | REG_DEF0 | SETS_CCODES,
                 "movs", "r!0d, #!1d", 2),
 
    ENCODING_MAP(kUnicoreMovRR,        0x1b000000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 4, 0, kFmtUnused, -1, -1,
                 kFmtUnused, -1, -1,
                 IS_BINARY_OP | REG_DEF0_USE1 | SETS_CCODES,
                 "movs", "r!0d, r!1d", 2),

    ENCODING_MAP(kUnicoreMulRRR,       0x01000120,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 4, 0,
                 kFmtUnused, -1, -1,
                 IS_TERTIARY_OP | REG_DEF0_USE12 | SETS_CCODES,
                 "muls", "r!0d, r!1d", 2),
 


    ENCODING_MAP(kUnicoreNotRR,          0x1f000000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 4, 0, kFmtUnused, -1, -1,
                 kFmtUnused, -1, -1,
                 IS_BINARY_OP | REG_DEF0_USE1 | SETS_CCODES,
                 "mvns", "r!0d, r!1d", 2),

    ENCODING_MAP(kUnicoreOrrRRR,         0x19000000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 4, 0,
                 kFmtUnused, -1, -1,
                 IS_TERTIARY_OP | REG_DEF0_USE12 | SETS_CCODES,
                 "orrs", "r!0d, r!1d", 2),


    ENCODING_MAP(kUnicorePop,           0x69e80004,
                 kFmtBitBlt, 18, 14, kFmtUnused, -1, -1, kFmtUnused, -1, -1,
                 kFmtUnused, -1, -1,
                 IS_UNARY_OP | REG_DEF_SP | REG_USE_SP  
                 | IS_LOAD, "pop", "<!0R>", 2),

    ENCODING_MAP(kUnicorePush,        0x72e80004,
                 kFmtBitBlt, 18, 14, kFmtUnused, -1, -1, kFmtUnused, -1, -1,
                 kFmtUnused, -1, -1,
                 IS_UNARY_OP | REG_DEF_SP | REG_USE_SP  
                 | IS_STORE, "push", "<!0R>", 2),
 


    ENCODING_MAP(kUnicoreRorRRR,        0x1b0000e0,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 4, 0, kFmtBitBlt, 13, 9,
                 kFmtUnused, -1, -1,
                 IS_TERTIARY_OP | REG_DEF0_USE12 | SETS_CCODES,
                 "rors", "r!0d, r!1d", 2),
 
    ENCODING_MAP(kUnicoreRsubRR,         0x27000000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtUnused, -1, -1,
                 kFmtUnused, -1, -1,
                 IS_BINARY_OP | REG_DEF0_USE1 | SETS_CCODES,
                 "negs", "r!0d, r!1d", 2),

    ENCODING_MAP(kUnicoreStbRRI14,     0x7c000000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 13, 0,
                 kFmtUnused, -1, -1, IS_TERTIARY_OP | REG_USE01 | IS_STORE,
                 "strb", "r!0d, [r!1d, #!2d]", 2),
 


    ENCODING_MAP(kUnicoreStbRRR,      0x5c000000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 4, 0,
                 kFmtUnused, -1, -1, IS_TERTIARY_OP | REG_USE012 | IS_STORE,
                 "strb", "r!0d, [r!1d, r!2d]", 2),
 
    ENCODING_MAP(kUnicoreSthRRI5I5,   0x5c000160,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 13, 9,
                 kFmtBitBlt, 4, 0, IS_QUAD_OP| REG_USE01 | IS_STORE,
                 "strh", "r!0d, [r!1d, #!2F]", 2),
 

    ENCODING_MAP(kUnicoreSthRRR,      0x58000160,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 4, 0,
                 kFmtUnused, -1, -1, IS_TERTIARY_OP | REG_USE012 | IS_STORE,
                 "strh", "r!0d, [r!1d, r!2d]", 2),
 



    ENCODING_MAP(kUnicoreStwRRI14,      0x78000000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 13, 0,
                 kFmtUnused, -1, -1, IS_TERTIARY_OP | REG_USE01 | IS_STORE,
                 "str", "r!0d, [r!1d, #!2E]", 2),
    ENCODING_MAP(kUnicoreStwRRR,       0x58000000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 4, 0,
                 kFmtUnused, -1, -1, IS_TERTIARY_OP | REG_USE012 | IS_STORE,
                 "str", "r!0d, [r!1d, r!2d]", 2),

    ENCODING_MAP(kUnicoreSubcRRR,      0x0d000000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 4, 0,
                 kFmtUnused, -1, -1,
                 IS_TERTIARY_OP | REG_DEF0_USE12 | USES_CCODES | SETS_CCODES,
                 "sbcs", "r!0d, r!1d", 2),
 


    ENCODING_MAP(kUnicoreSubRRI9,      0x25000000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 8, 0,
                 kFmtUnused, -1, -1,
                 IS_TERTIARY_OP | REG_DEF0_USE1 | SETS_CCODES,
                 "subs", "r!0d, r!1d, #!2d]", 2),
 
    ENCODING_MAP(kUnicoreSubRRR,       0x05000000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 4, 0,
                 kFmtUnused, -1, -1,
                 IS_TERTIARY_OP | REG_DEF0_USE12 | SETS_CCODES,
                 "subs", "r!0d, r!1d, r!2d", 2),

    ENCODING_MAP(kUnicoreSubSpI9,      0x24ef4000,
                 kFmtBitBlt, 8, 0, kFmtUnused, -1, -1, kFmtUnused, -1, -1,
                 kFmtUnused, -1, -1,
                 IS_UNARY_OP | REG_DEF_SP | REG_USE_SP,
                 "sub", "sp, #!0d", 2),
 
     ENCODING_MAP(kUnicoreXorRRR,      0x03000000,
                 kFmtBitBlt, 18, 14, kFmtBitBlt, 23, 19, kFmtBitBlt, 4, 0,
                 kFmtUnused, -1, -1,
                 IS_TERTIARY_OP | REG_DEF0_USE12 | SETS_CCODES,
                 "eors", "r!0d, r!1d", 2),
 

};
