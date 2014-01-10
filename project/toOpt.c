#include <stdio.h>
#include "DEX.h"
#include "toOpt.h"

/*
 * Copied from InterpCore.h.  Used for instruction decoding.
 */
#define FETCH(_offset)      (insns[(_offset)])
#define INST_INST(_inst)    ((_inst) & 0xff)
#define INST_A(_inst)       (((u2)(_inst) >> 8) & 0x0f)
#define INST_B(_inst)       ((u2)(_inst) >> 12)
#define INST_AA(_inst)      ((_inst) >> 8)

extern signed char* instrWidthTable;
extern unsigned char* instrFormatTable;

#include "toMarkBB.h"

inline unsigned char dexGetInstrFormat(const unsigned char* fmts, OpCode opCode){
	return fmts[opCode];
}

/*
 * Decode the instruction pointed to by "insns".
 *
 * Fills out the pieces of "pDec" that are affected by the current
 * instruction.  Does not touch anything else.
 */
void dexDecodeInstruction(const unsigned char* fmts, const u2* insns, DecodedInstruction* pDec){
    u2 inst = *insns;

    pDec->opCode = (OpCode) INST_INST(inst);  //取第八位，也就是取dex 操作码

    switch (dexGetInstrFormat(fmts, pDec->opCode)) {  //获得指令格式 如21t  32x
    case kFmt10x:       // op
        /* nothing to do; copy the AA bits out for the verifier */
        pDec->vA = INST_AA(inst);
        break;
    case kFmt12x:       // op vA, vB
        pDec->vA = INST_A(inst);  //B|A |op    操作码八位
        pDec->vB = INST_B(inst);
        break;
    case kFmt11n:       // op vA, #+B
        pDec->vA = INST_A(inst);
        pDec->vB = (s4) (INST_B(inst) << 28) >> 28; // sign extend 4-bit value
        break;
    case kFmt11x:       // op vAA
        pDec->vA = INST_AA(inst);
        break;
    case kFmt10t:       // op +AA
        pDec->vA = (s1) INST_AA(inst);              // sign-extend 8-bit value
        break;
    case kFmt20t:       // op +AAAA
        pDec->vA = (s2) FETCH(1);                   // sign-extend 16-bit value
        break;
    case kFmt20bc:      // op AA, thing@BBBB
    case kFmt21c:       // op vAA, thing@BBBB
    case kFmt22x:       // op vAA, vBBBB
        pDec->vA = INST_AA(inst);
        pDec->vB = FETCH(1);
        break;
    case kFmt21s:       // op vAA, #+BBBB
    case kFmt21t:       // op vAA, +BBBB
        pDec->vA = INST_AA(inst);
        pDec->vB = (s2) FETCH(1);                   // sign-extend 16-bit value
        break;
    case kFmt21h:       // op vAA, #+BBBB0000[00000000]
        pDec->vA = INST_AA(inst);
        /*
         * The value should be treated as right-zero-extended, but we don't
         * actually do that here. Among other things, we don't know if it's
         * the top bits of a 32- or 64-bit value.
         */
        pDec->vB = FETCH(1);
        break;
    case kFmt23x:       // op vAA, vBB, vCC
        pDec->vA = INST_AA(inst);
        pDec->vB = FETCH(1) & 0xff;
        pDec->vC = FETCH(1) >> 8;
        break;
    case kFmt22b:       // op vAA, vBB, #+CC
        pDec->vA = INST_AA(inst);
        pDec->vB = FETCH(1) & 0xff;
        pDec->vC = (s1) (FETCH(1) >> 8);            // sign-extend 8-bit value
        break;
    case kFmt22s:       // op vA, vB, #+CCCC
    case kFmt22t:       // op vA, vB, +CCCC
        pDec->vA = INST_A(inst);
        pDec->vB = INST_B(inst);
        pDec->vC = (s2) FETCH(1);                   // sign-extend 16-bit value
        break;
    case kFmt22c:       // op vA, vB, thing@CCCC
    case kFmt22cs:      // [opt] op vA, vB, field offset CCCC
        pDec->vA = INST_A(inst);
        pDec->vB = INST_B(inst);
        pDec->vC = FETCH(1);
        break;
    case kFmt30t:        // op +AAAAAAAA
        pDec->vA = FETCH(1) | ((u4) FETCH(2) << 16); // signed 32-bit value
        break;
    case kFmt31t:       // op vAA, +BBBBBBBB
    case kFmt31c:       // op vAA, thing@BBBBBBBB
        pDec->vA = INST_AA(inst);
        pDec->vB = FETCH(1) | ((u4) FETCH(2) << 16); // 32-bit value
        break;
    case kFmt32x:       // op vAAAA, vBBBB
        pDec->vA = FETCH(1);
        pDec->vB = FETCH(2);
        break;
    case kFmt31i:       // op vAA, #+BBBBBBBB
        pDec->vA = INST_AA(inst);
        pDec->vB = FETCH(1) | ((u4) FETCH(2) << 16);
        break;
    case kFmt35c:       // op vB, {vD..vG,vA}, thing@CCCC
    case kFmt35ms:      // [opt] invoke-virtual+super
        {
            /*
             * The lettering changes that came about when we went from 4 args
             * to 5 made the "range" versions of the calls different from
             * the non-range versions.  We have the choice between decoding
             * them the way the spec shows and having lots of conditionals
             * in the verifier, or mapping the values onto their original
             * registers and leaving the verifier intact.
             *
             * Current plan is to leave the verifier alone.  We can fix it
             * later if it's architecturally unbearable.
             *
             * Bottom line: method constant is always in vB.
             */
            u2 regList;
            int i, count;

            pDec->vA = INST_B(inst);
            pDec->vB = FETCH(1);
            regList = FETCH(2);

            if (pDec->vA > 5) {
                printf("Invalid arg count in 35c/35ms (%d)\n", pDec->vA);
                goto bail;
            }
            count = pDec->vA;
            if (count == 5) {
                /* 5th arg comes from A field in instruction */
                pDec->arg[4] = INST_A(inst);
                count--;
            }
            for (i = 0; i < count; i++) {
                pDec->arg[i] = regList & 0x0f;
                regList >>= 4;
            }
            /* copy arg[0] to vC; we don't have vD/vE/vF, so ignore those */
            if (pDec->vA > 0)
                pDec->vC = pDec->arg[0];
        }
        break;
    case kFmt3inline:   // [opt] inline invoke
        {
            u2 regList;
            int i;

            pDec->vA = INST_B(inst);
            pDec->vB = FETCH(1);
            regList = FETCH(2);

            if (pDec->vA > 4) {
                printf("Invalid arg count in 3inline (%d)\n", pDec->vA);
                goto bail;
            }
            for (i = 0; i < (int) pDec->vA; i++) {
                pDec->arg[i] = regList & 0x0f;
                regList >>= 4;
            }
            /* copy arg[0] to vC; we don't have vD/vE/vF, so ignore those */
            if (pDec->vA > 0)
                pDec->vC = pDec->arg[0];
        }
        break;
    case kFmt35fs:      // [opt] invoke-interface
//        assert(false);  // TODO
        break;
    case kFmt3rc:       // op {vCCCC .. v(CCCC+AA-1)}, meth@BBBB
    case kFmt3rms:      // [opt] invoke-virtual+super/range
    case kFmt3rinline:  // [opt] execute-inline/range
        pDec->vA = INST_AA(inst);
        pDec->vB = FETCH(1);
        pDec->vC = FETCH(2);
        break;
    case kFmt3rfs:      // [opt] invoke-interface/range
//        assert(false);  // TODO
        break;
    case kFmt51l:       // op vAA, #+BBBBBBBBBBBBBBBB
        pDec->vA = INST_AA(inst);
        pDec->vB_wide = FETCH(1);
        pDec->vB_wide |= (u8)FETCH(2) << 16;
        pDec->vB_wide |= (u8)FETCH(3) << 32;
        pDec->vB_wide |= (u8)FETCH(4) << 48;
        break;
    default:
        printf("Can't decode unexpected format %d (op=%d)\n",
            dexGetInstrFormat(fmts, pDec->opCode), pDec->opCode);
//        assert(false);
        break;
    }

bail:
    ;
}

size_t  dexGetInstrWidthAbs(const signed char* widths, OpCode opCode){
	int val = widths[opCode];
	if (val < 0)
		val = -val;
	return val;
}


char* dexGetOpcodeName(OpCode op){
	return (char *)gOpNames[op];
}


void dumpDecodedInstruction(const DecodedInstruction* pDecInsn,int insnIdx){
	int i = 0;	

    
    if (pDecInsn->opCode == OP_NOP) {
            printf("| nop // spacer");
    
    } else {
        printf("| %s",  dexGetOpcodeName(pDecInsn->opCode));
    }

	
    switch (dexGetInstrFormat(instrFormatTable, pDecInsn->opCode)) {
    case kFmt10x:        // op
        break;
    case kFmt12x:        // op vA, vB
        printf(" v%d, v%d", pDecInsn->vA, pDecInsn->vB);
        break;
    case kFmt11n:        // op vA, #+B
        printf(" v%d, #int %d // #%x",
            pDecInsn->vA, (s4)pDecInsn->vB, (u1)pDecInsn->vB);
        break;
    case kFmt11x:        // op vAA
        printf(" v%d", pDecInsn->vA);
        break;
    case kFmt10t:        // op +AA
    case kFmt20t:        // op +AAAA
        {
            s4 targ = (s4) pDecInsn->vA;
            printf(" %04x // %c%04x",
                insnIdx + targ,
                (targ < 0) ? '-' : '+',
                (targ < 0) ? -targ : targ);
        }
        break;
    case kFmt22x:        // op vAA, vBBBB
        printf(" v%d, v%d", pDecInsn->vA, pDecInsn->vB);
        break;
    case kFmt21t:        // op vAA, +BBBB
        {
            s4 targ = (s4) pDecInsn->vB;
            printf(" v%d, %04x // %c%04x", pDecInsn->vA,
                insnIdx + targ,
                (targ < 0) ? '-' : '+',
                (targ < 0) ? -targ : targ);
        }
        break;
    case kFmt21s:        // op vAA, #+BBBB
        printf(" v%d, #int %d // #%x",
            pDecInsn->vA, (s4)pDecInsn->vB, (u2)pDecInsn->vB);
        break;
    case kFmt21h:        // op vAA, #+BBBB0000[00000000]
        // The printed format varies a bit based on the actual opcode.
        if (pDecInsn->opCode == OP_CONST_HIGH16) {
            s4 value = pDecInsn->vB << 16;
            printf(" v%d, #int %d // #%x",
                pDecInsn->vA, value, (u2)pDecInsn->vB);
        } else {
            s8 value = ((s8) pDecInsn->vB) << 48;
            printf(" v%d, #long %lld // #%x",
                pDecInsn->vA,(long long int) value, (u2)pDecInsn->vB);
        }
        break;
    case kFmt21c:        // op vAA, thing@BBBB
        if (pDecInsn->opCode == OP_CONST_STRING) {
//donzy            printf(" v%d, \"%s\" // string@%04x", pDecInsn->vA,
//donzy                dexStringById(pDexFile, pDecInsn->vB), pDecInsn->vB);
            printf(" v%d, string@%04x", pDecInsn->vA,pDecInsn->vB);
        } else if (pDecInsn->opCode == OP_CHECK_CAST ||
                   pDecInsn->opCode == OP_NEW_INSTANCE ||
                   pDecInsn->opCode == OP_CONST_CLASS)
        {
//donzy            printf(" v%d, %s // class@%04x", pDecInsn->vA,
//donzy                getClassDescriptor(pDexFile, pDecInsn->vB), pDecInsn->vB);
            printf(" v%d,class@%04x", pDecInsn->vA, pDecInsn->vB);
        } else /* OP_SGET* */ {
//donzy            FieldMethodInfo fieldInfo;
//donzy            if (getFieldInfo(pDexFile, pDecInsn->vB, &fieldInfo)) {
//donzy                printf(" v%d, %s.%s:%s // field@%04x", pDecInsn->vA,
//donzy                    fieldInfo.classDescriptor, fieldInfo.name,
//donzy                    fieldInfo.signature, pDecInsn->vB);
//donzy            } else {
                printf(" v%d,field@%04x", pDecInsn->vA, pDecInsn->vB);
//donzy            }
        }
        break;
    case kFmt23x:        // op vAA, vBB, vCC
        printf(" v%d, v%d, v%d", pDecInsn->vA, pDecInsn->vB, pDecInsn->vC);
        break;
    case kFmt22b:        // op vAA, vBB, #+CC
        printf(" v%d, v%d, #int %d // #%02x",
            pDecInsn->vA, pDecInsn->vB, (s4)pDecInsn->vC, (u1)pDecInsn->vC);
        break;
    case kFmt22t:        // op vA, vB, +CCCC
        {
            s4 targ = (s4) pDecInsn->vC;
            printf(" v%d, v%d, %04x // %c%04x", pDecInsn->vA, pDecInsn->vB,
                insnIdx + targ,
                (targ < 0) ? '-' : '+',
                (targ < 0) ? -targ : targ);
        }
        break;
    case kFmt22s:        // op vA, vB, #+CCCC
        printf(" v%d, v%d, #int %d // #%04x",
            pDecInsn->vA, pDecInsn->vB, (s4)pDecInsn->vC, (u2)pDecInsn->vC);
        break;
    case kFmt22c:        // op vA, vB, thing@CCCC
        if (pDecInsn->opCode == OP_INSTANCE_OF ||
            pDecInsn->opCode == OP_NEW_ARRAY)
        {
            printf(" v%d, v%d, class@%04x",
                pDecInsn->vA, pDecInsn->vB, pDecInsn->vC);
        } else {
            /* iget* and iput*, including dexopt-generated -volatile */
//            FieldMethodInfo fieldInfo;
//            if (getFieldInfo(pDexFile, pDecInsn->vC, &fieldInfo)) {
//                printf(" v%d, v%d, %s.%s:%s // field@%04x", pDecInsn->vA,
//                    pDecInsn->vB, fieldInfo.classDescriptor, fieldInfo.name,
//                    fieldInfo.signature, pDecInsn->vC);
//            } else {
                printf(" v%d, v%d,field@%04x", pDecInsn->vA, pDecInsn->vB, pDecInsn->vC);
//            }
        }
        break;
    case kFmt22cs:       // [opt] op vA, vB, field offset CCCC
        printf(" v%d, v%d, [obj+%04x]",
            pDecInsn->vA, pDecInsn->vB, pDecInsn->vC);
        break;
    case kFmt30t:
        printf(" #%08x", pDecInsn->vA);
        break;
    case kFmt31i:        // op vAA, #+BBBBBBBB
        {
            /* this is often, but not always, a float */
            union {
                float f;
                u4 i;
            } conv;
            conv.i = pDecInsn->vB;
            printf(" v%d, #float %f // #%08x",
                pDecInsn->vA, conv.f, pDecInsn->vB);
        }
        break;
    case kFmt31c:        // op vAA, thing@BBBBBBBB
        printf(" v%d, string@%08x", pDecInsn->vA, pDecInsn->vB);
        break;
    case kFmt31t:       // op vAA, offset +BBBBBBBB
        printf(" v%d, %08x(insnIdx+vB) //vB= +%08x",
            pDecInsn->vA, insnIdx + pDecInsn->vB, pDecInsn->vB);
        break;
    case kFmt32x:        // op vAAAA, vBBBB
        printf(" v%d, v%d", pDecInsn->vA, pDecInsn->vB);
        break;
    case kFmt35c:        // op vB, {vD, vE, vF, vG, vA}, thing@CCCC
        {
            /* NOTE: decoding of 35c doesn't quite match spec */
            fputs(" {", stdout);
            for (i = 0; i < (int) pDecInsn->vA; i++) {
                if (i == 0)
                    printf("v%d", pDecInsn->arg[i]);
                else
                    printf(", v%d", pDecInsn->arg[i]);
            }
            if (pDecInsn->opCode == OP_FILLED_NEW_ARRAY) {
                printf("}, class@%04x", pDecInsn->vB);
            } else {
//                FieldMethodInfo methInfo;
//                if (getMethodInfo(pDexFile, pDecInsn->vB, &methInfo)) {
//                    printf("}, %s.%s:%s // method@%04x",
//                        methInfo.classDescriptor, methInfo.name,
//                        methInfo.signature, pDecInsn->vB);
//                } else {
                    printf("}, method@%04x", pDecInsn->vB);
//                }
            }
        }
        break;
    case kFmt35ms:       // [opt] invoke-virtual+super
    case kFmt35fs:       // [opt] invoke-interface
        {
            fputs(" {", stdout);
            for (i = 0; i < (int) pDecInsn->vA; i++) {
                if (i == 0)
                    printf("v%d", pDecInsn->arg[i]);
                else
                    printf(", v%d", pDecInsn->arg[i]);
            }
            printf("}, [%04x] // vtable #%04x", pDecInsn->vB, pDecInsn->vB);
        }
        break;
    case kFmt3rc:        // op {vCCCC .. v(CCCC+AA-1)}, meth@BBBB
        {
            /*
             * This doesn't match the "dx" output when some of the args are
             * 64-bit values -- dx only shows the first register.
             */
            fputs(" {", stdout);
            for (i = 0; i < (int) pDecInsn->vA; i++) {
                if (i == 0)
                    printf("v%d", pDecInsn->vC + i);
                else
                    printf(", v%d", pDecInsn->vC + i);
            }
            if (pDecInsn->opCode == OP_FILLED_NEW_ARRAY_RANGE) {
                printf("}, class@%04x", pDecInsn->vB);
            } else {
//                FieldMethodInfo methInfo;
//                if (getMethodInfo(pDexFile, pDecInsn->vB, &methInfo)) {
//                    printf("}, %s.%s:%s // method@%04x",
//                        methInfo.classDescriptor, methInfo.name,
//                        methInfo.signature, pDecInsn->vB);
//                } else {
                    printf("}, ??? // method@%04x", pDecInsn->vB);
//                }
            }
        }
        break;
    case kFmt3rms:       // [opt] invoke-virtual+super/range
    case kFmt3rfs:       // [opt] invoke-interface/range
        {
            /*
             * This doesn't match the "dx" output when some of the args are
             * 64-bit values -- dx only shows the first register.
             */
            fputs(" {", stdout);
            for (i = 0; i < (int) pDecInsn->vA; i++) {
                if (i == 0)
                    printf("v%d", pDecInsn->vC + i);
                else
                    printf(", v%d", pDecInsn->vC + i);
            }
            printf("}, [%04x] // vtable #%04x", pDecInsn->vB, pDecInsn->vB);
        }
        break;
    case kFmt3rinline:   // [opt] execute-inline/range
        {
            fputs(" {", stdout);
            for (i = 0; i < (int) pDecInsn->vA; i++) {
                if (i == 0)
                    printf("v%d", pDecInsn->vC + i);
                else
                    printf(", v%d", pDecInsn->vC + i);
            }
            printf("}, [%04x] // inline #%04x", pDecInsn->vB, pDecInsn->vB);
        }
        break;
    case kFmt3inline:    // [opt] inline invoke
        {
#if 0
            const InlineOperation* inlineOpsTable = dvmGetInlineOpsTable();
            u4 tableLen = dvmGetInlineOpsTableLength();
#endif

            fputs(" {", stdout);
            for (i = 0; i < (int) pDecInsn->vA; i++) {
                if (i == 0)
                    printf("v%d", pDecInsn->arg[i]);
                else
                    printf(", v%d", pDecInsn->arg[i]);
            }
#if 0
            if (pDecInsn->vB < tableLen) {
                printf("}, %s.%s:%s // inline #%04x",
                    inlineOpsTable[pDecInsn->vB].classDescriptor,
                    inlineOpsTable[pDecInsn->vB].methodName,
                    inlineOpsTable[pDecInsn->vB].methodSignature,
                    pDecInsn->vB);
            } else {
#endif
                printf("}, [%04x] // inline #%04x", pDecInsn->vB, pDecInsn->vB);
#if 0
            }
#endif
        }
        break;
    case kFmt51l:        // op vAA, #+BBBBBBBBBBBBBBBB
        {
            /* this is often, but not always, a double */
            union {
                double d;
                u8 j;
            } conv;
            conv.j = pDecInsn->vB_wide;
            printf(" v%d, #double %f // #%016llx",
                pDecInsn->vA, conv.d, (long long unsigned int)pDecInsn->vB_wide);
        }
        break;
    case kFmtUnknown:
        break;
    default:
        printf(" ???");
        break;
    }


    putchar('\n');

}


void dumpInstruction(DexFile* pDexFile, const DexCode* pCode, int insnIdx,
    int insnWidth, const DecodedInstruction* pDecInsn){
	int i = 0;	
	u2* insns = (u2 *)pCode->insns;

    printf("%06x:", (unsigned int)((u1*)insns - pDexFile->baseAddr) + insnIdx*2);
    for (i = 0; i < 8; i++) {
        if (i < insnWidth) {
            if (i == 7) {
                printf(" ... ");
            } else {
                /* print 16-bit value in little-endian order */
                const u1* bytePtr = (const u1*) &insns[insnIdx+i];
                printf(" %02x%02x", bytePtr[0], bytePtr[1]);
            }
        } else {
            fputs("     ", stdout);
        }
    }
    
    if (pDecInsn->opCode == OP_NOP) {
        u2 instr = get2LE((const u1*) &insns[insnIdx]);
        if (instr == kPackedSwitchSignature) {
            printf("|%04x: packed-switch-data (%d units)",
                insnIdx, insnWidth);
        } else if (instr == kSparseSwitchSignature) {
            printf("|%04x: sparse-switch-data (%d units)",
                insnIdx, insnWidth);
        } else if (instr == kArrayDataSignature) {
            printf("|%04x: array-data (%d units)",
                insnIdx, insnWidth);
        } else {
            printf("|%04x: nop // spacer", insnIdx);
        }
    } else {
        printf("|%04x: %s", insnIdx, dexGetOpcodeName(pDecInsn->opCode));
    }

	
    switch (dexGetInstrFormat(instrFormatTable, pDecInsn->opCode)) {
    case kFmt10x:        // op
        break;
    case kFmt12x:        // op vA, vB
        printf(" v%d, v%d", pDecInsn->vA, pDecInsn->vB);
        break;
    case kFmt11n:        // op vA, #+B
        printf(" v%d, #int %d // #%x",
            pDecInsn->vA, (s4)pDecInsn->vB, (u1)pDecInsn->vB);
        break;
    case kFmt11x:        // op vAA
        printf(" v%d", pDecInsn->vA);
        break;
    case kFmt10t:        // op +AA
    case kFmt20t:        // op +AAAA
        {
            s4 targ = (s4) pDecInsn->vA;
            printf(" %04x // %c%04x",
                insnIdx + targ,
                (targ < 0) ? '-' : '+',
                (targ < 0) ? -targ : targ);
        }
        break;
    case kFmt22x:        // op vAA, vBBBB
        printf(" v%d, v%d", pDecInsn->vA, pDecInsn->vB);
        break;
    case kFmt21t:        // op vAA, +BBBB
        {
            s4 targ = (s4) pDecInsn->vB;
            printf(" v%d, %04x // %c%04x", pDecInsn->vA,
                insnIdx + targ,
                (targ < 0) ? '-' : '+',
                (targ < 0) ? -targ : targ);
        }
        break;
    case kFmt21s:        // op vAA, #+BBBB
        printf(" v%d, #int %d // #%x",
            pDecInsn->vA, (s4)pDecInsn->vB, (u2)pDecInsn->vB);
        break;
    case kFmt21h:        // op vAA, #+BBBB0000[00000000]
        // The printed format varies a bit based on the actual opcode.
        if (pDecInsn->opCode == OP_CONST_HIGH16) {
            s4 value = pDecInsn->vB << 16;
            printf(" v%d, #int %d // #%x",
                pDecInsn->vA, value, (u2)pDecInsn->vB);
        } else {
            s8 value = ((s8) pDecInsn->vB) << 48;
            printf(" v%d, #long %lld // #%x",
                pDecInsn->vA,(long long int) value, (u2)pDecInsn->vB);
        }
        break;
    case kFmt21c:        // op vAA, thing@BBBB
        if (pDecInsn->opCode == OP_CONST_STRING) {
//donzy            printf(" v%d, \"%s\" // string@%04x", pDecInsn->vA,
//donzy                dexStringById(pDexFile, pDecInsn->vB), pDecInsn->vB);
            printf(" v%d, string@%04x", pDecInsn->vA,pDecInsn->vB);
        } else if (pDecInsn->opCode == OP_CHECK_CAST ||
                   pDecInsn->opCode == OP_NEW_INSTANCE ||
                   pDecInsn->opCode == OP_CONST_CLASS)
        {
//donzy            printf(" v%d, %s // class@%04x", pDecInsn->vA,
//donzy                getClassDescriptor(pDexFile, pDecInsn->vB), pDecInsn->vB);
            printf(" v%d,class@%04x", pDecInsn->vA, pDecInsn->vB);
        } else /* OP_SGET* */ {
//donzy            FieldMethodInfo fieldInfo;
//donzy            if (getFieldInfo(pDexFile, pDecInsn->vB, &fieldInfo)) {
//donzy                printf(" v%d, %s.%s:%s // field@%04x", pDecInsn->vA,
//donzy                    fieldInfo.classDescriptor, fieldInfo.name,
//donzy                    fieldInfo.signature, pDecInsn->vB);
//donzy            } else {
                printf(" v%d,field@%04x", pDecInsn->vA, pDecInsn->vB);
//donzy            }
        }
        break;
    case kFmt23x:        // op vAA, vBB, vCC
        printf(" v%d, v%d, v%d", pDecInsn->vA, pDecInsn->vB, pDecInsn->vC);
        break;
    case kFmt22b:        // op vAA, vBB, #+CC
        printf(" v%d, v%d, #int %d // #%02x",
            pDecInsn->vA, pDecInsn->vB, (s4)pDecInsn->vC, (u1)pDecInsn->vC);
        break;
    case kFmt22t:        // op vA, vB, +CCCC
        {
            s4 targ = (s4) pDecInsn->vC;
            printf(" v%d, v%d, %04x // %c%04x", pDecInsn->vA, pDecInsn->vB,
                insnIdx + targ,
                (targ < 0) ? '-' : '+',
                (targ < 0) ? -targ : targ);
        }
        break;
    case kFmt22s:        // op vA, vB, #+CCCC
        printf(" v%d, v%d, #int %d // #%04x",
            pDecInsn->vA, pDecInsn->vB, (s4)pDecInsn->vC, (u2)pDecInsn->vC);
        break;
    case kFmt22c:        // op vA, vB, thing@CCCC
        if (pDecInsn->opCode == OP_INSTANCE_OF ||
            pDecInsn->opCode == OP_NEW_ARRAY)
        {
            printf(" v%d, v%d, class@%04x",
                pDecInsn->vA, pDecInsn->vB, pDecInsn->vC);
        } else {
            /* iget* and iput*, including dexopt-generated -volatile */
//            FieldMethodInfo fieldInfo;
//            if (getFieldInfo(pDexFile, pDecInsn->vC, &fieldInfo)) {
//                printf(" v%d, v%d, %s.%s:%s // field@%04x", pDecInsn->vA,
//                    pDecInsn->vB, fieldInfo.classDescriptor, fieldInfo.name,
//                    fieldInfo.signature, pDecInsn->vC);
//            } else {
                printf(" v%d, v%d,field@%04x", pDecInsn->vA, pDecInsn->vB, pDecInsn->vC);
//            }
        }
        break;
    case kFmt22cs:       // [opt] op vA, vB, field offset CCCC
        printf(" v%d, v%d, [obj+%04x]",
            pDecInsn->vA, pDecInsn->vB, pDecInsn->vC);
        break;
    case kFmt30t:
        printf(" #%08x", pDecInsn->vA);
        break;
    case kFmt31i:        // op vAA, #+BBBBBBBB
        {
            /* this is often, but not always, a float */
            union {
                float f;
                u4 i;
            } conv;
            conv.i = pDecInsn->vB;
            printf(" v%d, #float %f // #%08x",
                pDecInsn->vA, conv.f, pDecInsn->vB);
        }
        break;
    case kFmt31c:        // op vAA, thing@BBBBBBBB
        printf(" v%d, string@%08x", pDecInsn->vA, pDecInsn->vB);
        break;
    case kFmt31t:       // op vAA, offset +BBBBBBBB
        printf(" v%d, %08x(insnIdx+vB) //vB= +%08x",
            pDecInsn->vA, insnIdx + pDecInsn->vB, pDecInsn->vB);
        break;
    case kFmt32x:        // op vAAAA, vBBBB
        printf(" v%d, v%d", pDecInsn->vA, pDecInsn->vB);
        break;
    case kFmt35c:        // op vB, {vD, vE, vF, vG, vA}, thing@CCCC
        {
            /* NOTE: decoding of 35c doesn't quite match spec */
            fputs(" {", stdout);
            for (i = 0; i < (int) pDecInsn->vA; i++) {
                if (i == 0)
                    printf("v%d", pDecInsn->arg[i]);
                else
                    printf(", v%d", pDecInsn->arg[i]);
            }
            if (pDecInsn->opCode == OP_FILLED_NEW_ARRAY) {
                printf("}, class@%04x", pDecInsn->vB);
            } else {
//                FieldMethodInfo methInfo;
//                if (getMethodInfo(pDexFile, pDecInsn->vB, &methInfo)) {
//                    printf("}, %s.%s:%s // method@%04x",
//                        methInfo.classDescriptor, methInfo.name,
//                        methInfo.signature, pDecInsn->vB);
//                } else {
                    printf("}, method@%04x", pDecInsn->vB);
//                }
            }
        }
        break;
    case kFmt35ms:       // [opt] invoke-virtual+super
    case kFmt35fs:       // [opt] invoke-interface
        {
            fputs(" {", stdout);
            for (i = 0; i < (int) pDecInsn->vA; i++) {
                if (i == 0)
                    printf("v%d", pDecInsn->arg[i]);
                else
                    printf(", v%d", pDecInsn->arg[i]);
            }
            printf("}, [%04x] // vtable #%04x", pDecInsn->vB, pDecInsn->vB);
        }
        break;
    case kFmt3rc:        // op {vCCCC .. v(CCCC+AA-1)}, meth@BBBB
        {
            /*
             * This doesn't match the "dx" output when some of the args are
             * 64-bit values -- dx only shows the first register.
             */
            fputs(" {", stdout);
            for (i = 0; i < (int) pDecInsn->vA; i++) {
                if (i == 0)
                    printf("v%d", pDecInsn->vC + i);
                else
                    printf(", v%d", pDecInsn->vC + i);
            }
            if (pDecInsn->opCode == OP_FILLED_NEW_ARRAY_RANGE) {
                printf("}, class@%04x", pDecInsn->vB);
            } else {
//                FieldMethodInfo methInfo;
//                if (getMethodInfo(pDexFile, pDecInsn->vB, &methInfo)) {
//                    printf("}, %s.%s:%s // method@%04x",
//                        methInfo.classDescriptor, methInfo.name,
//                        methInfo.signature, pDecInsn->vB);
//                } else {
                    printf("}, ??? // method@%04x", pDecInsn->vB);
//                }
            }
        }
        break;
    case kFmt3rms:       // [opt] invoke-virtual+super/range
    case kFmt3rfs:       // [opt] invoke-interface/range
        {
            /*
             * This doesn't match the "dx" output when some of the args are
             * 64-bit values -- dx only shows the first register.
             */
            fputs(" {", stdout);
            for (i = 0; i < (int) pDecInsn->vA; i++) {
                if (i == 0)
                    printf("v%d", pDecInsn->vC + i);
                else
                    printf(", v%d", pDecInsn->vC + i);
            }
            printf("}, [%04x] // vtable #%04x", pDecInsn->vB, pDecInsn->vB);
        }
        break;
    case kFmt3rinline:   // [opt] execute-inline/range
        {
            fputs(" {", stdout);
            for (i = 0; i < (int) pDecInsn->vA; i++) {
                if (i == 0)
                    printf("v%d", pDecInsn->vC + i);
                else
                    printf(", v%d", pDecInsn->vC + i);
            }
            printf("}, [%04x] // inline #%04x", pDecInsn->vB, pDecInsn->vB);
        }
        break;
    case kFmt3inline:    // [opt] inline invoke
        {
#if 0
            const InlineOperation* inlineOpsTable = dvmGetInlineOpsTable();
            u4 tableLen = dvmGetInlineOpsTableLength();
#endif

            fputs(" {", stdout);
            for (i = 0; i < (int) pDecInsn->vA; i++) {
                if (i == 0)
                    printf("v%d", pDecInsn->arg[i]);
                else
                    printf(", v%d", pDecInsn->arg[i]);
            }
#if 0
            if (pDecInsn->vB < tableLen) {
                printf("}, %s.%s:%s // inline #%04x",
                    inlineOpsTable[pDecInsn->vB].classDescriptor,
                    inlineOpsTable[pDecInsn->vB].methodName,
                    inlineOpsTable[pDecInsn->vB].methodSignature,
                    pDecInsn->vB);
            } else {
#endif
                printf("}, [%04x] // inline #%04x", pDecInsn->vB, pDecInsn->vB);
#if 0
            }
#endif
        }
        break;
    case kFmt51l:        // op vAA, #+BBBBBBBBBBBBBBBB
        {
            /* this is often, but not always, a double */
            union {
                double d;
                u8 j;
            } conv;
            conv.j = pDecInsn->vB_wide;
            printf(" v%d, #double %f // #%016llx",
                pDecInsn->vA, conv.d, (long long unsigned int)pDecInsn->vB_wide);
        }
        break;
    case kFmtUnknown:
        break;
    default:
        printf(" ???");
        break;
    }


    putchar('\n');

}

void identifyBB(DexFile* pDexFile, DexCode* pDexCode, CodeItem* pCodeItem ){
//	printf("this is to identify BB!\n");
	//for help in dumpCode dumpBytecodes

	//printf("from identifyBB function: ##=##registersize= %d\n",pDexCode->registersSize);

	u4* BBMask = pCodeItem->BBMask;
	u2* insns;
	insns = pDexCode->insns;	

	int insnIdx = 0;
	int insnWidth; 
        OpCode opCode;
        DecodedInstruction decInsn;
        u2 instr;

	
//toDoIdentify 	
	int i = 0;
	int bool_bbmask = 0;
	MARKBB markBB[256];
	int maskBBPos = 0;
	
	for(i=0; i<256; i++){
		markBB[i] = NULL;
	}
#include "markBB.h"

	printf("~~~~~~codeoffset=%p; insns off=%p;registers is %d;insns size is %d~~~~~~~~\n",(void *)((u1 *)pDexCode - pDexFile->baseAddr),(void *)((u1*)(pDexCode->insns) - pDexFile->baseAddr),pDexCode->registersSize,pDexCode->insnsSize);

//	setBBMaskIfNecessary(0,BBMask);

	insnIdx = 0;
	insns = pDexCode->insns;	
    while (insnIdx < (int) pDexCode->insnsSize) {

        instr = get2LE((const u1*)insns);
        if (instr == kPackedSwitchSignature) {
            insnWidth = 4 + get2LE((const u1*)(insns+1)) * 2;
	    insns += insnWidth;
	    insnIdx += insnWidth;
	    continue;
        } else if (instr == kSparseSwitchSignature) {
            insnWidth = 2 + get2LE((const u1*)(insns+1)) * 4;
	    insns += insnWidth;
	    insnIdx += insnWidth;
	    continue;
        } else if (instr == kArrayDataSignature) {
            int width = get2LE((const u1*)(insns+1));
            int size = get2LE((const u1*)(insns+2)) |
                       (get2LE((const u1*)(insns+3))<<16);
            // The plus 1 is to round up for odd size and width
            insnWidth = 4 + ((size * width) + 1) / 2;
	    insns += insnWidth;
	    insnIdx += insnWidth;
	    continue;
        } else {
            opCode = instr & 0xff;
            insnWidth = dexGetInstrWidthAbs(instrWidthTable, opCode);
            if (insnWidth == 0) {
                printf("error: GLITCH: zero-width instruction at idx=0x%04x\n", insnIdx);
                break;
            }
        }

        dexDecodeInstruction(instrFormatTable, insns, &decInsn);
        dumpInstruction(pDexFile, pDexCode, insnIdx, insnWidth, &decInsn);

	if(markBB[opCode] != NULL){
		bool_bbmask = markBB[opCode](pDexCode, insnIdx, insnWidth, &decInsn, BBMask);
		if(bool_bbmask < 0){
			printf("error");
			return ;
		}
	}

        insns += insnWidth;
        insnIdx += insnWidth;
    }
	
}



void outputCode(DexFile* pDexFile, DexCode* pDexCode, CodeItem* pCodeItem ){
	
	static int const_flag = 1; //to delete!
//	printf("this is to identify BB!\n");
	//for help in dumpCode dumpBytecodes
	u4* BBMask = pCodeItem->BBMask;
	u2* insns;
	insns = pDexCode->insns;	

	int insnIdx = 0;
	int insnWidth; 
        OpCode opCode;
        DecodedInstruction decInsn;
        u2 instr;
    while (insnIdx < (int) pDexCode->insnsSize) {

        instr = get2LE((const u1*)insns);
        if (instr == kPackedSwitchSignature) {
            insnWidth = 4 + get2LE((const u1*)(insns+1)) * 2;
        } else if (instr == kSparseSwitchSignature) {
            insnWidth = 2 + get2LE((const u1*)(insns+1)) * 4;
        } else if (instr == kArrayDataSignature) {
            int width = get2LE((const u1*)(insns+1));
            int size = get2LE((const u1*)(insns+2)) |
                       (get2LE((const u1*)(insns+3))<<16);
            // The plus 1 is to round up for odd size and width
            insnWidth = 4 + ((size * width) + 1) / 2;
        } else {
            opCode = instr & 0xff;
            insnWidth = dexGetInstrWidthAbs(instrWidthTable, opCode);
            if (insnWidth == 0) {
                printf("error: GLITCH: zero-width instruction at idx=0x%04x\n", insnIdx);
                break;
            }
        }

        dexDecodeInstruction(instrFormatTable, insns, &decInsn);
        dumpInstruction(pDexFile, pDexCode, insnIdx, insnWidth, &decInsn);

        insns += insnWidth;
        insnIdx += insnWidth;
    }
}

void outputBBInsns(DexFile* pDexFile, DexCode* pDexCode, int startIdx ,int endIdx){
	
	u2* insns;
	insns = pDexCode->insns;
		

	int insnIdx = startIdx;
	int insnWidth; 
        OpCode opCode;
        DecodedInstruction decInsn;
        u2 instr;
	insns += insnIdx;
    while (insnIdx < endIdx) {

        instr = get2LE((const u1*)insns);
        if (instr == kPackedSwitchSignature) {
            insnWidth = 4 + get2LE((const u1*)(insns+1)) * 2;
        } else if (instr == kSparseSwitchSignature) {
            insnWidth = 2 + get2LE((const u1*)(insns+1)) * 4;
        } else if (instr == kArrayDataSignature) {
            int width = get2LE((const u1*)(insns+1));
            int size = get2LE((const u1*)(insns+2)) |
                       (get2LE((const u1*)(insns+3))<<16);
            // The plus 1 is to round up for odd size and width
            insnWidth = 4 + ((size * width) + 1) / 2;
        } else {
            opCode = instr & 0xff;
            insnWidth = dexGetInstrWidthAbs(instrWidthTable, opCode);
            if (insnWidth == 0) {
                printf("error: GLITCH: zero-width instruction at idx=0x%04x\n", insnIdx);
                break;
            }
        }

        dexDecodeInstruction(instrFormatTable, insns, &decInsn);
	printf("=::==   ");
        dumpInstruction(pDexFile, pDexCode, insnIdx, insnWidth, &decInsn);

        insns += insnWidth;
        insnIdx += insnWidth;
    }
}

void parseInsn(const u2 *codePtr,DecodedInstruction *decInsn){
	dexDecodeInstruction(instrFormatTable, codePtr,decInsn);
	return ;
}

int nextInsn(u2 * insns , int * idx){
        int insnWidth = 0;
	OpCode opCode;
	u2 instr = get2LE((const u1*)(&insns[*idx]));
        if (instr == kPackedSwitchSignature) {
            insnWidth = 4 + get2LE((const u1*)(insns+1)) * 2;
        } else if (instr == kSparseSwitchSignature) {
            insnWidth = 2 + get2LE((const u1*)(insns+1)) * 4;
        } else if (instr == kArrayDataSignature) {
            int width = get2LE((const u1*)(insns+1));
            int size = get2LE((const u1*)(insns+2)) |
                       (get2LE((const u1*)(insns+3))<<16);
            // The plus 1 is to round up for odd size and width
            insnWidth = 4 + ((size * width) + 1) / 2;
        } else {
            opCode = instr & 0xff;
            insnWidth = dexGetInstrWidthAbs(instrWidthTable, opCode);
            if (insnWidth == 0) {
                printf("error: GLITCH: zero-width instruction at idx=0x%04x\n", *idx);
                return -1;
            }
        }

	*idx += insnWidth;
	return insnWidth;
}

int insnIsData(u2 * insns, int idx){
	u2 instr = get2LE((const u1*)(&insns[idx]));
        if (instr == kPackedSwitchSignature) {
            return 0xff;
        } else if (instr == kSparseSwitchSignature) {
            return 0xff;
        } else if (instr == kArrayDataSignature) {
            return 0xff;
        } else {
		return 0;	
	}
}

u2 getInsn(u2 * insns, int idx){
	u2 instr = get2LE((const u1*)(&insns[idx]));
	return instr;
}




