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























