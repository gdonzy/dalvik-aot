#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "DEX.h"

extern CodeList codeList;

signed char * dexCreateInstrWidthTable(void)
{
    signed char * instrWidth;
    int i;

    instrWidth = malloc(sizeof(char) * kNumDalvikInstructions);
    if (instrWidth == NULL)
        return NULL;

    for (i = 0; i < kNumDalvikInstructions; i++) {
        OpCode opc = (OpCode) i;
        int width = 0;

        switch (opc) {
        case OP_NOP:    /* note data for e.g. switch-* encoded "inside" a NOP */
        case OP_MOVE:
        case OP_MOVE_WIDE:
        case OP_MOVE_OBJECT:
        case OP_MOVE_RESULT:
        case OP_MOVE_RESULT_WIDE:
        case OP_MOVE_RESULT_OBJECT:
        case OP_MOVE_EXCEPTION:
        case OP_RETURN_VOID:
        case OP_RETURN:
        case OP_RETURN_WIDE:
        case OP_RETURN_OBJECT:
        case OP_CONST_4:
        case OP_MONITOR_ENTER:
        case OP_MONITOR_EXIT:
        case OP_ARRAY_LENGTH:
        case OP_THROW:
        case OP_GOTO:
        case OP_NEG_INT:
        case OP_NOT_INT:
        case OP_NEG_LONG:
        case OP_NOT_LONG:
        case OP_NEG_FLOAT:
        case OP_NEG_DOUBLE:
        case OP_INT_TO_LONG:
        case OP_INT_TO_FLOAT:
        case OP_INT_TO_DOUBLE:
        case OP_LONG_TO_INT:
        case OP_LONG_TO_FLOAT:
        case OP_LONG_TO_DOUBLE:
        case OP_FLOAT_TO_INT:
        case OP_FLOAT_TO_LONG:
        case OP_FLOAT_TO_DOUBLE:
        case OP_DOUBLE_TO_INT:
        case OP_DOUBLE_TO_LONG:
        case OP_DOUBLE_TO_FLOAT:
        case OP_INT_TO_BYTE:
        case OP_INT_TO_CHAR:
        case OP_INT_TO_SHORT:
        case OP_ADD_INT_2ADDR:
        case OP_SUB_INT_2ADDR:
        case OP_MUL_INT_2ADDR:
        case OP_DIV_INT_2ADDR:
        case OP_REM_INT_2ADDR:
        case OP_AND_INT_2ADDR:
        case OP_OR_INT_2ADDR:
        case OP_XOR_INT_2ADDR:
        case OP_SHL_INT_2ADDR:
        case OP_SHR_INT_2ADDR:
        case OP_USHR_INT_2ADDR:
        case OP_ADD_LONG_2ADDR:
        case OP_SUB_LONG_2ADDR:
        case OP_MUL_LONG_2ADDR:
        case OP_DIV_LONG_2ADDR:
        case OP_REM_LONG_2ADDR:
        case OP_AND_LONG_2ADDR:
        case OP_OR_LONG_2ADDR:
        case OP_XOR_LONG_2ADDR:
        case OP_SHL_LONG_2ADDR:
        case OP_SHR_LONG_2ADDR:
        case OP_USHR_LONG_2ADDR:
        case OP_ADD_FLOAT_2ADDR:
        case OP_SUB_FLOAT_2ADDR:
        case OP_MUL_FLOAT_2ADDR:
        case OP_DIV_FLOAT_2ADDR:
        case OP_REM_FLOAT_2ADDR:
        case OP_ADD_DOUBLE_2ADDR:
        case OP_SUB_DOUBLE_2ADDR:
        case OP_MUL_DOUBLE_2ADDR:
        case OP_DIV_DOUBLE_2ADDR:
        case OP_REM_DOUBLE_2ADDR:
            width = 1;
            break;

        case OP_MOVE_FROM16:
        case OP_MOVE_WIDE_FROM16:
        case OP_MOVE_OBJECT_FROM16:
        case OP_CONST_16:
        case OP_CONST_HIGH16:
        case OP_CONST_WIDE_16:
        case OP_CONST_WIDE_HIGH16:
        case OP_CONST_STRING:
        case OP_CONST_CLASS:
        case OP_CHECK_CAST:
        case OP_INSTANCE_OF:
        case OP_NEW_INSTANCE:
        case OP_NEW_ARRAY:
        case OP_CMPL_FLOAT:
        case OP_CMPG_FLOAT:
        case OP_CMPL_DOUBLE:
        case OP_CMPG_DOUBLE:
        case OP_CMP_LONG:
        case OP_GOTO_16:
        case OP_IF_EQ:
        case OP_IF_NE:
        case OP_IF_LT:
        case OP_IF_GE:
        case OP_IF_GT:
        case OP_IF_LE:
        case OP_IF_EQZ:
        case OP_IF_NEZ:
        case OP_IF_LTZ:
        case OP_IF_GEZ:
        case OP_IF_GTZ:
        case OP_IF_LEZ:
        case OP_AGET:
        case OP_AGET_WIDE:
        case OP_AGET_OBJECT:
        case OP_AGET_BOOLEAN:
        case OP_AGET_BYTE:
        case OP_AGET_CHAR:
        case OP_AGET_SHORT:
        case OP_APUT:
        case OP_APUT_WIDE:
        case OP_APUT_OBJECT:
        case OP_APUT_BOOLEAN:
        case OP_APUT_BYTE:
        case OP_APUT_CHAR:
        case OP_APUT_SHORT:
        case OP_IGET:
        case OP_IGET_WIDE:
        case OP_IGET_OBJECT:
        case OP_IGET_BOOLEAN:
        case OP_IGET_BYTE:
        case OP_IGET_CHAR:
        case OP_IGET_SHORT:
        case OP_IPUT:
        case OP_IPUT_WIDE:
        case OP_IPUT_OBJECT:
        case OP_IPUT_BOOLEAN:
        case OP_IPUT_BYTE:
        case OP_IPUT_CHAR:
        case OP_IPUT_SHORT:
        case OP_SGET:
        case OP_SGET_WIDE:
        case OP_SGET_OBJECT:
        case OP_SGET_BOOLEAN:
        case OP_SGET_BYTE:
        case OP_SGET_CHAR:
        case OP_SGET_SHORT:
        case OP_SPUT:
        case OP_SPUT_WIDE:
        case OP_SPUT_OBJECT:
        case OP_SPUT_BOOLEAN:
        case OP_SPUT_BYTE:
        case OP_SPUT_CHAR:
        case OP_SPUT_SHORT:
        case OP_ADD_INT:
        case OP_SUB_INT:
        case OP_MUL_INT:
        case OP_DIV_INT:
        case OP_REM_INT:
        case OP_AND_INT:
        case OP_OR_INT:
        case OP_XOR_INT:
        case OP_SHL_INT:
        case OP_SHR_INT:
        case OP_USHR_INT:
        case OP_ADD_LONG:
        case OP_SUB_LONG:
        case OP_MUL_LONG:
        case OP_DIV_LONG:
        case OP_REM_LONG:
        case OP_AND_LONG:
        case OP_OR_LONG:
        case OP_XOR_LONG:
        case OP_SHL_LONG:
        case OP_SHR_LONG:
        case OP_USHR_LONG:
        case OP_ADD_FLOAT:
        case OP_SUB_FLOAT:
        case OP_MUL_FLOAT:
        case OP_DIV_FLOAT:
        case OP_REM_FLOAT:
        case OP_ADD_DOUBLE:
        case OP_SUB_DOUBLE:
        case OP_MUL_DOUBLE:
        case OP_DIV_DOUBLE:
        case OP_REM_DOUBLE:
        case OP_ADD_INT_LIT16:
        case OP_RSUB_INT:
        case OP_MUL_INT_LIT16:
        case OP_DIV_INT_LIT16:
        case OP_REM_INT_LIT16:
        case OP_AND_INT_LIT16:
        case OP_OR_INT_LIT16:
        case OP_XOR_INT_LIT16:
        case OP_ADD_INT_LIT8:
        case OP_RSUB_INT_LIT8:
        case OP_MUL_INT_LIT8:
        case OP_DIV_INT_LIT8:
        case OP_REM_INT_LIT8:
        case OP_AND_INT_LIT8:
        case OP_OR_INT_LIT8:
        case OP_XOR_INT_LIT8:
        case OP_SHL_INT_LIT8:
        case OP_SHR_INT_LIT8:
        case OP_USHR_INT_LIT8:
            width = 2;
            break;

        case OP_MOVE_16:
        case OP_MOVE_WIDE_16:
        case OP_MOVE_OBJECT_16:
        case OP_CONST:
        case OP_CONST_WIDE_32:
        case OP_CONST_STRING_JUMBO:
        case OP_GOTO_32:
        case OP_FILLED_NEW_ARRAY:
        case OP_FILLED_NEW_ARRAY_RANGE:
        case OP_FILL_ARRAY_DATA:
        case OP_PACKED_SWITCH:
        case OP_SPARSE_SWITCH:
        case OP_INVOKE_VIRTUAL:
        case OP_INVOKE_SUPER:
        case OP_INVOKE_DIRECT:
        case OP_INVOKE_STATIC:
        case OP_INVOKE_INTERFACE:
        case OP_INVOKE_VIRTUAL_RANGE:
        case OP_INVOKE_SUPER_RANGE:
        case OP_INVOKE_DIRECT_RANGE:
        case OP_INVOKE_STATIC_RANGE:
        case OP_INVOKE_INTERFACE_RANGE:
            width = 3;
            break;

        case OP_CONST_WIDE:
            width = 5;
            break;

        /*
         * Optimized instructions.  We return negative size values for these
         * to distinguish them.
         */
        case OP_IGET_QUICK:
        case OP_IGET_WIDE_QUICK:
        case OP_IGET_OBJECT_QUICK:
        case OP_IPUT_QUICK:
        case OP_IPUT_WIDE_QUICK:
        case OP_IPUT_OBJECT_QUICK:
        case OP_IGET_VOLATILE:
        case OP_IPUT_VOLATILE:
        case OP_SGET_VOLATILE:
        case OP_SPUT_VOLATILE:
        case OP_IGET_OBJECT_VOLATILE:
        case OP_IPUT_OBJECT_VOLATILE:
        case OP_SGET_OBJECT_VOLATILE:
        case OP_SPUT_OBJECT_VOLATILE:
        case OP_IGET_WIDE_VOLATILE:
        case OP_IPUT_WIDE_VOLATILE:
        case OP_SGET_WIDE_VOLATILE:
        case OP_SPUT_WIDE_VOLATILE:
        case OP_THROW_VERIFICATION_ERROR:
            width = -2;
            break;
        case OP_INVOKE_VIRTUAL_QUICK:
        case OP_INVOKE_VIRTUAL_QUICK_RANGE:
        case OP_INVOKE_SUPER_QUICK:
        case OP_INVOKE_SUPER_QUICK_RANGE:
        case OP_EXECUTE_INLINE:
        case OP_EXECUTE_INLINE_RANGE:
        case OP_INVOKE_DIRECT_EMPTY:
            width = -3;
            break;

        /* these should never appear when scanning bytecode */
        case OP_UNUSED_3E:
        case OP_UNUSED_3F:
        case OP_UNUSED_40:
        case OP_UNUSED_41:
        case OP_UNUSED_42:
        case OP_UNUSED_43:
        case OP_UNUSED_73:
        case OP_UNUSED_79:
        case OP_UNUSED_7A:
        case OP_BREAKPOINT:
        case OP_UNUSED_F1:
        case OP_UNUSED_FF:
		width=0;
            break;

        /*
         * DO NOT add a "default" clause here.  Without it the compiler will
         * complain if an instruction is missing (which is desirable).
         */
        }

        instrWidth[opc] = width;
    }

    return instrWidth;
}


unsigned char * dexCreateInstrFormatTable(void)
{
    unsigned char * instFmt;
    int i;

    instFmt = malloc(sizeof(instFmt) * kNumDalvikInstructions);
    if (instFmt == NULL)
        return NULL;

    for (i = 0; i < kNumDalvikInstructions; i++) {
        OpCode opc = (OpCode) i;
        unsigned char fmt = kFmtUnknown;

        switch (opc) {
        case OP_GOTO:
            fmt = kFmt10t;
            break;
        case OP_NOP:
        case OP_RETURN_VOID:
            fmt = kFmt10x;
            break;
        case OP_CONST_4:
            fmt = kFmt11n;
            break;
        case OP_CONST_HIGH16:
        case OP_CONST_WIDE_HIGH16:
            fmt = kFmt21h;
            break;
        case OP_MOVE_RESULT:
        case OP_MOVE_RESULT_WIDE:
        case OP_MOVE_RESULT_OBJECT:
        case OP_MOVE_EXCEPTION:
        case OP_RETURN:
        case OP_RETURN_WIDE:
        case OP_RETURN_OBJECT:
        case OP_MONITOR_ENTER:
        case OP_MONITOR_EXIT:
        case OP_THROW:
            fmt = kFmt11x;
            break;
        case OP_MOVE:
        case OP_MOVE_WIDE:
        case OP_MOVE_OBJECT:
        case OP_ARRAY_LENGTH:
        case OP_NEG_INT:
        case OP_NOT_INT:
        case OP_NEG_LONG:
        case OP_NOT_LONG:
        case OP_NEG_FLOAT:
        case OP_NEG_DOUBLE:
        case OP_INT_TO_LONG:
        case OP_INT_TO_FLOAT:
        case OP_INT_TO_DOUBLE:
        case OP_LONG_TO_INT:
        case OP_LONG_TO_FLOAT:
        case OP_LONG_TO_DOUBLE:
        case OP_FLOAT_TO_INT:
        case OP_FLOAT_TO_LONG:
        case OP_FLOAT_TO_DOUBLE:
        case OP_DOUBLE_TO_INT:
        case OP_DOUBLE_TO_LONG:
        case OP_DOUBLE_TO_FLOAT:
        case OP_INT_TO_BYTE:
        case OP_INT_TO_CHAR:
        case OP_INT_TO_SHORT:
        case OP_ADD_INT_2ADDR:
        case OP_SUB_INT_2ADDR:
        case OP_MUL_INT_2ADDR:
        case OP_DIV_INT_2ADDR:
        case OP_REM_INT_2ADDR:
        case OP_AND_INT_2ADDR:
        case OP_OR_INT_2ADDR:
        case OP_XOR_INT_2ADDR:
        case OP_SHL_INT_2ADDR:
        case OP_SHR_INT_2ADDR:
        case OP_USHR_INT_2ADDR:
        case OP_ADD_LONG_2ADDR:
        case OP_SUB_LONG_2ADDR:
        case OP_MUL_LONG_2ADDR:
        case OP_DIV_LONG_2ADDR:
        case OP_REM_LONG_2ADDR:
        case OP_AND_LONG_2ADDR:
        case OP_OR_LONG_2ADDR:
        case OP_XOR_LONG_2ADDR:
        case OP_SHL_LONG_2ADDR:
        case OP_SHR_LONG_2ADDR:
        case OP_USHR_LONG_2ADDR:
        case OP_ADD_FLOAT_2ADDR:
        case OP_SUB_FLOAT_2ADDR:
        case OP_MUL_FLOAT_2ADDR:
        case OP_DIV_FLOAT_2ADDR:
        case OP_REM_FLOAT_2ADDR:
        case OP_ADD_DOUBLE_2ADDR:
        case OP_SUB_DOUBLE_2ADDR:
        case OP_MUL_DOUBLE_2ADDR:
        case OP_DIV_DOUBLE_2ADDR:
        case OP_REM_DOUBLE_2ADDR:
            fmt = kFmt12x;
            break;
        case OP_GOTO_16:
            fmt = kFmt20t;
            break;
        case OP_GOTO_32:
            fmt = kFmt30t;
            break;
        case OP_CONST_STRING:
        case OP_CONST_CLASS:
        case OP_CHECK_CAST:
        case OP_NEW_INSTANCE:
        case OP_SGET:
        case OP_SGET_WIDE:
        case OP_SGET_OBJECT:
        case OP_SGET_BOOLEAN:
        case OP_SGET_BYTE:
        case OP_SGET_CHAR:
        case OP_SGET_SHORT:
        case OP_SPUT:
        case OP_SPUT_WIDE:
        case OP_SPUT_OBJECT:
        case OP_SPUT_BOOLEAN:
        case OP_SPUT_BYTE:
        case OP_SPUT_CHAR:
        case OP_SPUT_SHORT:
            fmt = kFmt21c;
            break;
        case OP_CONST_16:
        case OP_CONST_WIDE_16:
            fmt = kFmt21s;
            break;
        case OP_IF_EQZ:
        case OP_IF_NEZ:
        case OP_IF_LTZ:
        case OP_IF_GEZ:
        case OP_IF_GTZ:
        case OP_IF_LEZ:
            fmt = kFmt21t;
            break;
        case OP_FILL_ARRAY_DATA:
        case OP_PACKED_SWITCH:
        case OP_SPARSE_SWITCH:
            fmt = kFmt31t;
            break;
        case OP_ADD_INT_LIT8:
        case OP_RSUB_INT_LIT8:
        case OP_MUL_INT_LIT8:
        case OP_DIV_INT_LIT8:
        case OP_REM_INT_LIT8:
        case OP_AND_INT_LIT8:
        case OP_OR_INT_LIT8:
        case OP_XOR_INT_LIT8:
        case OP_SHL_INT_LIT8:
        case OP_SHR_INT_LIT8:
        case OP_USHR_INT_LIT8:
            fmt = kFmt22b;
            break;
        case OP_INSTANCE_OF:
        case OP_NEW_ARRAY:
        case OP_IGET:
        case OP_IGET_WIDE:
        case OP_IGET_OBJECT:
        case OP_IGET_BOOLEAN:
        case OP_IGET_BYTE:
        case OP_IGET_CHAR:
        case OP_IGET_SHORT:
        case OP_IPUT:
        case OP_IPUT_WIDE:
        case OP_IPUT_OBJECT:
        case OP_IPUT_BOOLEAN:
        case OP_IPUT_BYTE:
        case OP_IPUT_CHAR:
        case OP_IPUT_SHORT:
            fmt = kFmt22c;
            break;
        case OP_ADD_INT_LIT16:
        case OP_RSUB_INT:
        case OP_MUL_INT_LIT16:
        case OP_DIV_INT_LIT16:
        case OP_REM_INT_LIT16:
        case OP_AND_INT_LIT16:
        case OP_OR_INT_LIT16:
        case OP_XOR_INT_LIT16:
            fmt = kFmt22s;
            break;
        case OP_IF_EQ:
        case OP_IF_NE:
        case OP_IF_LT:
        case OP_IF_GE:
        case OP_IF_GT:
        case OP_IF_LE:
            fmt = kFmt22t;
            break;
        case OP_MOVE_FROM16:
        case OP_MOVE_WIDE_FROM16:
        case OP_MOVE_OBJECT_FROM16:
            fmt = kFmt22x;
            break;
        case OP_CMPL_FLOAT:
        case OP_CMPG_FLOAT:
        case OP_CMPL_DOUBLE:
        case OP_CMPG_DOUBLE:
        case OP_CMP_LONG:
        case OP_AGET:
        case OP_AGET_WIDE:
        case OP_AGET_OBJECT:
        case OP_AGET_BOOLEAN:
        case OP_AGET_BYTE:
        case OP_AGET_CHAR:
        case OP_AGET_SHORT:
        case OP_APUT:
        case OP_APUT_WIDE:
        case OP_APUT_OBJECT:
        case OP_APUT_BOOLEAN:
        case OP_APUT_BYTE:
        case OP_APUT_CHAR:
        case OP_APUT_SHORT:
        case OP_ADD_INT:
        case OP_SUB_INT:
        case OP_MUL_INT:
        case OP_DIV_INT:
        case OP_REM_INT:
        case OP_AND_INT:
        case OP_OR_INT:
        case OP_XOR_INT:
        case OP_SHL_INT:
        case OP_SHR_INT:
        case OP_USHR_INT:
        case OP_ADD_LONG:
        case OP_SUB_LONG:
        case OP_MUL_LONG:
        case OP_DIV_LONG:
        case OP_REM_LONG:
        case OP_AND_LONG:
        case OP_OR_LONG:
        case OP_XOR_LONG:
        case OP_SHL_LONG:
        case OP_SHR_LONG:
        case OP_USHR_LONG:
        case OP_ADD_FLOAT:
        case OP_SUB_FLOAT:
        case OP_MUL_FLOAT:
        case OP_DIV_FLOAT:
        case OP_REM_FLOAT:
        case OP_ADD_DOUBLE:
        case OP_SUB_DOUBLE:
        case OP_MUL_DOUBLE:
        case OP_DIV_DOUBLE:
        case OP_REM_DOUBLE:
            fmt = kFmt23x;
            break;
        case OP_CONST:
        case OP_CONST_WIDE_32:
            fmt = kFmt31i;
            break;
        case OP_CONST_STRING_JUMBO:
            fmt = kFmt31c;
            break;
        case OP_MOVE_16:
        case OP_MOVE_WIDE_16:
        case OP_MOVE_OBJECT_16:
            fmt = kFmt32x;
            break;
        case OP_FILLED_NEW_ARRAY:
        case OP_INVOKE_VIRTUAL:
        case OP_INVOKE_SUPER:
        case OP_INVOKE_DIRECT:
        case OP_INVOKE_STATIC:
        case OP_INVOKE_INTERFACE:
            fmt = kFmt35c;
            break;
        case OP_FILLED_NEW_ARRAY_RANGE:
        case OP_INVOKE_VIRTUAL_RANGE:
        case OP_INVOKE_SUPER_RANGE:
        case OP_INVOKE_DIRECT_RANGE:
        case OP_INVOKE_STATIC_RANGE:
        case OP_INVOKE_INTERFACE_RANGE:
            fmt = kFmt3rc;
            break;
        case OP_CONST_WIDE:
            fmt = kFmt51l;
            break;

        /*
         * Optimized instructions.
         */
        case OP_THROW_VERIFICATION_ERROR:
            fmt = kFmt20bc;
            break;
        case OP_IGET_WIDE_VOLATILE:
        case OP_IPUT_WIDE_VOLATILE:
        case OP_IGET_VOLATILE:
        case OP_IPUT_VOLATILE:
        case OP_IGET_OBJECT_VOLATILE:
        case OP_IPUT_OBJECT_VOLATILE:
            fmt = kFmt22c;
            break;
        case OP_SGET_OBJECT_VOLATILE:
        case OP_SPUT_OBJECT_VOLATILE:
        case OP_SGET_VOLATILE:
        case OP_SPUT_VOLATILE:
        case OP_SGET_WIDE_VOLATILE:
        case OP_SPUT_WIDE_VOLATILE:
            fmt = kFmt21c;
            break;
        case OP_IGET_QUICK:
        case OP_IGET_WIDE_QUICK:
        case OP_IGET_OBJECT_QUICK:
        case OP_IPUT_QUICK:
        case OP_IPUT_WIDE_QUICK:
        case OP_IPUT_OBJECT_QUICK:
            fmt = kFmt22cs;
            break;
        case OP_INVOKE_VIRTUAL_QUICK:
        case OP_INVOKE_SUPER_QUICK:
            fmt = kFmt35ms;
            break;
        case OP_INVOKE_VIRTUAL_QUICK_RANGE:
        case OP_INVOKE_SUPER_QUICK_RANGE:
            fmt = kFmt3rms;
            break;
        case OP_EXECUTE_INLINE:
            fmt = kFmt3inline;
            break;
        case OP_EXECUTE_INLINE_RANGE:
            fmt = kFmt3rinline;
            break;
        case OP_INVOKE_DIRECT_EMPTY:
            fmt = kFmt35c;
            break;

        /* these should never appear when scanning code */
        case OP_UNUSED_3E:
        case OP_UNUSED_3F:
        case OP_UNUSED_40:
        case OP_UNUSED_41:
        case OP_UNUSED_42:
        case OP_UNUSED_43:
        case OP_UNUSED_73:
        case OP_UNUSED_79:
        case OP_UNUSED_7A:
        case OP_BREAKPOINT:
        case OP_UNUSED_F1:
        case OP_UNUSED_FF:
            fmt = kFmtUnknown;
            break;

        /*
         * DO NOT add a "default" clause here.  Without it the compiler will
         * complain if an instruction is missing (which is desirable).
         */
        }

        instFmt[opc] = fmt;
    }

    return instFmt;
}


void dexFileSetupBasicPointers(DexFile* pDexFile, const u1* data) {
    DexHeader *pHeader = (DexHeader*) data;
	printf("sizeof(u2) is %d\n",(int)sizeof(u2));
	printf("sizeof(u4) is %d\n",(int)sizeof(u4));
	printf("sizeof(unsigned long int) is %d\n",(int)sizeof(unsigned long int));
	printf("stringIdsOff is %x\n",pHeader->stringIdsOff);
	printf("typeIdsOff is %x\n",pHeader->typeIdsOff);
	printf("fieldIdsOff is %x\n",pHeader->fieldIdsOff);
	printf("methodIdsOff is %x\n",pHeader->methodIdsOff);
	printf("protoIdsOff  is %x\n",pHeader->protoIdsOff);
	printf("classDefsOff is %x\n",pHeader->classDefsOff);

    pDexFile->baseAddr = data;
    pDexFile->pHeader = pHeader;
    pDexFile->pStringIds = (const DexStringId*) (data + pHeader->stringIdsOff);
    pDexFile->pTypeIds = (const DexTypeId*) (data + pHeader->typeIdsOff);
    pDexFile->pFieldIds = (const DexFieldId*) (data + pHeader->fieldIdsOff);
    pDexFile->pMethodIds = (const DexMethodId*) (data + pHeader->methodIdsOff);
    pDexFile->pProtoIds = (const DexProtoId*) (data + pHeader->protoIdsOff);
    pDexFile->pClassDefs = (const DexClassDef*) (data + pHeader->classDefsOff);
	printf("pHeader->classDefsOff is %d",pHeader->classDefsOff);
    pDexFile->pLinkData = (const DexLink*) (data + pHeader->linkOff);
}

DexFile* parseDexFile(const u1* addr,size_t length){
	printf("this is from parseDexFile\n");
	DexFile* pDexFile = NULL;
	const DexHeader* pHeader;
	const u1* magic;

	if(length < sizeof (DexHeader)){
		printf("the given dex file is invalid!\n");
		return NULL;
	}	
	
	pDexFile = (DexFile*) malloc(sizeof(DexFile));
	if(pDexFile == NULL)
		return NULL;
	memset(pDexFile,0,sizeof(DexFile));

	if(memcmp(addr,"dey\n",4) == 0){
		magic = addr;
		if(memcmp(magic+4,"036\0",4)){
			printf("bad opt version\n");
		}
		
		pDexFile->pOptHeader = (const DexOptHeader*) addr;
		//todo something for dexParseOptData();.....
		
		addr += pDexFile->pOptHeader->dexOffset;
		length -=pDexFile->pOptHeader->dexOffset;
		if(pDexFile->pOptHeader->dexLength > length){
			printf("dexlength wrong\n");
			return NULL;
		}
		length = pDexFile->pOptHeader->dexLength;//????
	}
printf("addr addr is %p",addr);
	dexFileSetupBasicPointers(pDexFile,addr);
	pHeader = pDexFile->pHeader;
	
	magic = pHeader->magic;
	if(memcmp(magic,"dex\n",4)!=0){
		printf("error: wrong dex magic number\n");
		return NULL;
	}
	if(memcmp(magic+4,"035\0",4)!=0){
		printf("error: wrong dex version\n");
		return NULL;
	}

	//todo something for verify checksum

	return pDexFile;	
}

DexClassDef* dexGetClassDef(DexFile* pDexFile, u4 idx){
	return (DexClassDef *)&pDexFile->pClassDefs[idx];
}

 u1* dexGetClassData(DexFile* pDexFile, DexClassDef* pClassDef){
	return (u1*) (pDexFile->baseAddr + pClassDef->classDataOff);
}

int readUnsignedLeb128(u1**  pStream){
	const u1* ptr = *pStream;
	int result = *(ptr++);

	if(result > 0x7f){
		int cur = *(ptr++);
		result = (result & 0x7f) | ((cur & 0x7f) << 7);
		if(cur > 0x7f){
			cur = *(ptr++);
			result |= (cur & 0x7f) << 14;
			if(cur > 0x7f){
				cur = *(ptr++);
				result |= cur << 28;
			}
		}
	}

	*pStream = (u1 *)ptr;
	return result;
}

void dexReadClassDataHeader(u1** pData, DexClassDataHeader *pHeader){
	pHeader->staticFieldsSize = readUnsignedLeb128(pData);
	pHeader->instanceFieldsSize = readUnsignedLeb128(pData);
	pHeader->directMethodsSize = readUnsignedLeb128(pData);
	pHeader->virtualMethodsSize = readUnsignedLeb128(pData);
}

bool dexReadAndFormatClassDataField(u1** pData,  DexField* pField, u4* lastIndex){

    u4 index = *lastIndex + readUnsignedLeb128(pData);

    pField->accessFlags = readUnsignedLeb128(pData);
    pField->fieldIdx = index;
    *lastIndex = index;

    return true;
}

bool dexReadAndFormatClassDataMethod(u1** pData, DexMethod* pMethod, u4* lastIndex){
	
    u4 index = *lastIndex + readUnsignedLeb128(pData);

    pMethod->accessFlags = readUnsignedLeb128(pData);
    pMethod->codeOff = readUnsignedLeb128(pData);
    pMethod->methodIdx = index;
    *lastIndex = index;
	return true;
}


//DexClassData* dexReadAndVerifyClassData(const u1** pData, const u1*)
DexClassData* dexReadAndFormatClassData(u1** pData){
	DexClassDataHeader header;
	bool okay = true;
	
	dexReadClassDataHeader(pData,&header);

	printf("header.staticFieldsSize is %d,header.instanceFieldsSize is %d,header.directMethodsSize is %d,header.virtualMethodsSize is %d\n",header.staticFieldsSize,header.instanceFieldsSize,header.directMethodsSize,header.virtualMethodsSize);

	size_t resultSize = sizeof(DexClassData) + 
		(header.staticFieldsSize * sizeof(DexFile)) +
		(header.instanceFieldsSize * sizeof(DexField)) +
		(header.directMethodsSize * sizeof(DexMethod)) +
		(header.virtualMethodsSize * sizeof(DexMethod));
	
	DexClassData* result = malloc(resultSize);
	u1* ptr = ((u1*) result) + sizeof(DexClassData);
	u4 i=0;
	u4 lastIndex = 0;

	if(result == NULL){
		printf("malloc failure!\n");
		return NULL;
	}
	
	result->header = header;
	
	
    if (header.staticFieldsSize != 0) {
        result->staticFields = (DexField*) ptr;
        ptr += header.staticFieldsSize * sizeof(DexField);
    } else {
        result->staticFields = NULL;
    }

    if (header.instanceFieldsSize != 0) {
        result->instanceFields = (DexField*) ptr;
        ptr += header.instanceFieldsSize * sizeof(DexField);
    } else {
        result->instanceFields = NULL;
    }

    if (header.directMethodsSize != 0) {
        result->directMethods = (DexMethod*) ptr;
        ptr += header.directMethodsSize * sizeof(DexMethod);
    } else {
        result->directMethods = NULL;
    }

    if (header.virtualMethodsSize != 0) {
        result->virtualMethods = (DexMethod*) ptr;
    } else {
        result->virtualMethods = NULL;
    }

    lastIndex = 0;
    for (i = 0; okay && (i < header.staticFieldsSize); i++) {
        okay = dexReadAndFormatClassDataField(pData,
                &result->staticFields[i], &lastIndex);
    }

    lastIndex = 0;
    for (i = 0; okay && (i < header.instanceFieldsSize); i++) {
        okay = dexReadAndFormatClassDataField(pData,
                &result->instanceFields[i], &lastIndex);
    }

    lastIndex = 0;
    for (i = 0; okay && (i < header.directMethodsSize); i++) {
        okay = dexReadAndFormatClassDataMethod(pData,
                &result->directMethods[i], &lastIndex);
	printf("directMethods[%d] codeoff is %x\n",i,result->directMethods[i].codeOff);
    }

    lastIndex = 0;
    for (i = 0; okay && (i < header.virtualMethodsSize); i++) {
        okay = dexReadAndFormatClassDataMethod(pData,
                &result->virtualMethods[i], &lastIndex);
	printf("virtualMethods[%d] codeoff is %x\n",i,result->virtualMethods[i].codeOff);
    }

	if(! okay){
		free(result);
		return NULL;
	}
	
	return result;
}

DexCode* dexGetCode(DexFile* pDexFile, DexMethod* pDexMethod){
	if(pDexMethod->codeOff == 0){
		return NULL;
	}

	printf("baseAddr is %p,codeoff is %d\n",pDexFile->baseAddr,(int)pDexMethod->codeOff);
	return (DexCode *)(signed long)(pDexFile->baseAddr + pDexMethod->codeOff);
}

void dumpCode(DexFile* pDexFile, DexMethod* pDexMethod){

	
    DexCode* pCode = (void*)dexGetCode(pDexFile, pDexMethod);
    printf("pCode is %p\n",pCode);

    printf("      registers     : %d\n", pCode->registersSize);
    printf("      ins           : %d\n", pCode->insSize);
    printf("      outs          : %d\n", pCode->outsSize);
    printf("      insns size    : %d 16-bit code units\n", pCode->insnsSize);

//    if (gOptions.disassemble)
//        dumpBytecodes(pDexFile, pDexMethod);

//    dumpCatches(pDexFile, pCode);
    /* both of these are encoded in debug info */
//    dumpPositions(pDexFile, pCode, pDexMethod);
//    dumpLocals(pDexFile, pCode, pDexMethod);
		
 	CodeItem* codeItem = malloc(sizeof(CodeItem));
	codeItem->codeBaseAddr = pDexMethod->codeOff;
	codeItem->item = pCode;
	codeItem->next = NULL;
	codeItem->BBMask = NULL;
	codeItem->lastInstrPosHead.list_size = 0;
	codeItem->lastInstrPosHead.next = NULL;
	
	if(codeList.header!=NULL){
		codeList.tail->next = codeItem;
		codeList.tail = codeItem;
	}else{
		codeList.header = codeItem;
		codeList.tail = codeItem;
	}


}

void* dumpClass(DexFile* pDexFile,int idx){
	DexClassDef* pClassDef = NULL;
	u1* pEncodedData = NULL;
	DexClassData* pClassData = NULL;
	int i = 0;

	pClassDef = dexGetClassDef(pDexFile,idx);
	
	pEncodedData = dexGetClassData(pDexFile,pClassDef);

//	pClassData = dexReadAndVerifyClassData(&pEncodedData,NULL);
	pClassData = dexReadAndFormatClassData(&pEncodedData);

		
        printf("  Direct methods    -\n");
    for (i = 0; i < (int) pClassData->header.directMethodsSize; i++) {
	printf("##======##%d",i);
	printf("codeoff is %x\n",pClassData->directMethods[i].codeOff);
	if(pClassData->directMethods[i].codeOff!=0)
        dumpCode(pDexFile, &pClassData->directMethods[i]);
    }

        printf("  Virtual methods   -\n");
    for (i = 0; i < (int) pClassData->header.virtualMethodsSize; i++) {
	printf("##======##%d",i);
	printf("codeoff is %x\n",pClassData->virtualMethods[i].codeOff);
	if(pClassData->virtualMethods[i].codeOff!=0)
        dumpCode(pDexFile, &pClassData->virtualMethods[i]);
    }

}
