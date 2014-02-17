#define  array markBB
//switch
array[OP_PACKED_SWITCH] = markBB_packedSwitch ;
array[OP_SPARSE_SWITCH] = markBB_sparseSwitch ;

//return
array[OP_RETURN_VOID] = markBB_return ;
array[OP_RETURN] = markBB_return ;
array[OP_RETURN_WIDE] = markBB_return ;
array[OP_RETURN_OBJECT] = markBB_return ;

//invoke-kind
array[OP_INVOKE_VIRTUAL] = markBB_invoke ;
array[OP_INVOKE_SUPER] = markBB_invoke ;
array[OP_INVOKE_DIRECT] = markBB_invoke ;
array[OP_INVOKE_STATIC] = markBB_invoke ;
array[OP_INVOKE_INTERFACE] = markBB_invoke ;

//invoke-kind/range
array[OP_INVOKE_VIRTUAL_RANGE] = markBB_invokeRange ;
array[OP_INVOKE_SUPER_RANGE] = markBB_invokeRange ;
array[OP_INVOKE_DIRECT_RANGE] = markBB_invokeRange ;
array[OP_INVOKE_STATIC_RANGE] = markBB_invokeRange ;
array[OP_INVOKE_INTERFACE_RANGE] = markBB_invokeRange ;

//goto
array[OP_GOTO] = markBB_goto ;
array[OP_GOTO_16] = markBB_goto ;
array[OP_GOTO_32] = markBB_goto ;

//if-test
array[OP_IF_EQ] = markBB_if ;
array[OP_IF_NE] = markBB_if ;
array[OP_IF_LT] = markBB_if ;
array[OP_IF_GE] = markBB_if ;
array[OP_IF_GT] = markBB_if ;
array[OP_IF_LE] = markBB_if ;

//if_testz
array[OP_IF_EQZ] = markBB_ifZ;
array[OP_IF_NEZ] = markBB_ifZ;
array[OP_IF_LTZ] = markBB_ifZ;
array[OP_IF_GEZ] = markBB_ifZ;
array[OP_IF_GTZ] = markBB_ifZ;
array[OP_IF_LEZ] = markBB_ifZ;

//throw
array[OP_THROW] = markBB_throw;

////donzy2delete iget/iput...
//array[OP_IGET] = markBB_iget;
//array[OP_IGET_WIDE] = markBB_iget;
//array[OP_IGET_OBJECT] = markBB_iget; 
//array[OP_IGET_BOOLEAN] = markBB_iget;
//array[OP_IGET_BYTE]= markBB_iget;
//array[OP_IGET_CHAR]= markBB_iget;
//array[OP_IGET_SHORT]= markBB_iget;
//array[OP_IPUT]= markBB_iput;
//array[OP_IPUT_WIDE]= markBB_iput;
//array[OP_IPUT_OBJECT]= markBB_iput;
//array[OP_IPUT_BOOLEAN]= markBB_iput;
//array[OP_IPUT_BYTE]= markBB_iput;
//array[OP_IPUT_CHAR]= markBB_iput;
//array[OP_IPUT_SHORT]= markBB_iput;
//
////donzy2delete sget/sput...
//array[OP_SGET] = markBB_sget;
//array[OP_SGET_WIDE] = markBB_sget;
//array[OP_SGET_OBJECT] = markBB_sget;
//array[OP_SGET_BOOLEAN] = markBB_sget;
//array[OP_SGET_BYTE] = markBB_sget;
//array[OP_SGET_CHAR] = markBB_sget;
//array[OP_SGET_SHORT] = markBB_sget;
//array[OP_SPUT] = markBB_sput;
//array[OP_SPUT_WIDE] = markBB_sput;
//array[OP_SPUT_OBJECT] = markBB_sput;
//array[OP_SPUT_BOOLEAN] = markBB_sput;
//array[OP_SPUT_BYTE] = markBB_sput;
//array[OP_SPUT_CHAR] = markBB_sput;
//array[OP_SPUT_SHORT] = markBB_sput;
//
////donzy2delete aget/aput...
//array[OP_AGET] = markBB_aget;
//array[OP_AGET_WIDE] = markBB_aget;
//array[OP_AGET_OBJECT] = markBB_aget;
//array[OP_AGET_BOOLEAN] = markBB_aget;
//array[OP_AGET_BYTE] = markBB_aget;
//array[OP_AGET_CHAR] = markBB_aget;
//array[OP_AGET_SHORT] = markBB_aget;
//array[OP_APUT] = markBB_aput;
//array[OP_APUT_WIDE] = markBB_aput;
//array[OP_APUT_OBJECT] = markBB_aput;
//array[OP_APUT_BOOLEAN] = markBB_aput;
//array[OP_APUT_BYTE] = markBB_aput;
//array[OP_APUT_CHAR] = markBB_aput;
//array[OP_APUT_SHORT] = markBB_aput;
//










