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

//donzy:BBfilter
//array[    OP_MOVE                         ]=markBB_filter;    
//array[    OP_MOVE_FROM16                  ]=markBB_filter; 
array[    OP_MOVE_16                      ]=markBB_filter; 
array[    OP_MOVE_WIDE                    ]=markBB_filter; 
array[    OP_MOVE_WIDE_FROM16             ]=markBB_filter; 
array[    OP_MOVE_WIDE_16                 ]=markBB_filter; 
array[    OP_MOVE_OBJECT                  ]=markBB_filter; 
array[    OP_MOVE_OBJECT_FROM16           ]=markBB_filter; 
array[    OP_MOVE_OBJECT_16               ]=markBB_filter; 

//array[    OP_MOVE_RESULT                  ]=markBB_filter; 
array[    OP_MOVE_RESULT_WIDE             ]=markBB_filter; 
array[    OP_MOVE_RESULT_OBJECT           ]=markBB_filter; 
array[    OP_MOVE_EXCEPTION               ]=markBB_filter; 

array[    OP_RETURN_VOID                  ]=markBB_filter; 
array[    OP_RETURN                       ]=markBB_filter; 
array[    OP_RETURN_WIDE                  ]=markBB_filter; 
array[    OP_RETURN_OBJECT                ]=markBB_filter; 

//array[    OP_CONST_4                      ]=markBB_filter; 
array[    OP_CONST_16                     ]=markBB_filter; 
array[    OP_CONST                        ]=markBB_filter; 
array[    OP_CONST_HIGH16                 ]=markBB_filter; 
//array[    OP_CONST_WIDE_16                ]=markBB_filter; 
array[    OP_CONST_WIDE_32                ]=markBB_filter; 
array[    OP_CONST_WIDE                   ]=markBB_filter; 
array[    OP_CONST_WIDE_HIGH16            ]=markBB_filter; 
array[    OP_CONST_STRING                 ]=markBB_filter; 
array[    OP_CONST_STRING_JUMBO           ]=markBB_filter; 
array[    OP_CONST_CLASS                  ]=markBB_filter; 

array[    OP_MONITOR_ENTER                ]=markBB_filter; 
array[    OP_MONITOR_EXIT                 ]=markBB_filter; 

array[    OP_CHECK_CAST                   ]=markBB_filter; 
array[    OP_INSTANCE_OF                  ]=markBB_filter; 

array[    OP_ARRAY_LENGTH                 ]=markBB_filter; 

array[    OP_NEW_INSTANCE                 ]=markBB_filter; 
array[    OP_NEW_ARRAY                    ]=markBB_filter; 

array[    OP_FILLED_NEW_ARRAY             ]=markBB_filter; 
array[    OP_FILLED_NEW_ARRAY_RANGE       ]=markBB_filter; 
array[    OP_FILL_ARRAY_DATA              ]=markBB_filter; 

array[    OP_THROW                        ]=markBB_filter;  
array[    OP_PACKED_SWITCH                ]=markBB_filter; 
array[    OP_SPARSE_SWITCH                ]=markBB_filter; 

array[    OP_CMPL_FLOAT                   ]=markBB_filter; 
array[    OP_CMPG_FLOAT                   ]=markBB_filter; 
array[    OP_CMPL_DOUBLE                  ]=markBB_filter; 
array[    OP_CMPG_DOUBLE                  ]=markBB_filter; 
array[    OP_CMP_LONG                     ]=markBB_filter; 



array[    OP_AGET                         ]=markBB_filter; 
array[    OP_AGET_WIDE                    ]=markBB_filter; 
array[    OP_AGET_OBJECT                  ]=markBB_filter; 
array[    OP_AGET_BOOLEAN                 ]=markBB_filter; 
array[    OP_AGET_BYTE                    ]=markBB_filter; 
array[    OP_AGET_CHAR                    ]=markBB_filter; 
array[    OP_AGET_SHORT                   ]=markBB_filter; 
array[    OP_APUT                         ]=markBB_filter; 
array[    OP_APUT_WIDE                    ]=markBB_filter; 
array[    OP_APUT_OBJECT                  ]=markBB_filter; 
array[    OP_APUT_BOOLEAN                 ]=markBB_filter; 
array[    OP_APUT_BYTE                    ]=markBB_filter; 
array[    OP_APUT_CHAR                    ]=markBB_filter; 
array[    OP_APUT_SHORT                   ]=markBB_filter; 

//array[    OP_IGET                         ]=markBB_filter; 
//array[    OP_IGET_WIDE                    ]=markBB_filter; 
//array[    OP_IGET_OBJECT                  ]=markBB_filter; 
//array[    OP_IGET_BOOLEAN                 ]=markBB_filter; 
//array[    OP_IGET_BYTE                    ]=markBB_filter; 
//array[    OP_IGET_CHAR                    ]=markBB_filter; 
//array[    OP_IGET_SHORT                   ]=markBB_filter; 
//array[    OP_IPUT                         ]=markBB_filter; 
//array[    OP_IPUT_WIDE                    ]=markBB_filter; 
array[    OP_IPUT_OBJECT                  ]=markBB_filter; 
//array[    OP_IPUT_BOOLEAN                 ]=markBB_filter; 
//array[    OP_IPUT_BYTE                    ]=markBB_filter; 
//array[    OP_IPUT_CHAR                    ]=markBB_filter; 
//array[    OP_IPUT_SHORT                   ]=markBB_filter; 

array[    OP_SGET                         ]=markBB_filter; 
array[    OP_SGET_WIDE                    ]=markBB_filter; 
array[    OP_SGET_OBJECT                  ]=markBB_filter; 
array[    OP_SGET_BOOLEAN                 ]=markBB_filter; 
array[    OP_SGET_BYTE                    ]=markBB_filter; 
array[    OP_SGET_CHAR                    ]=markBB_filter; 
array[    OP_SGET_SHORT                   ]=markBB_filter; 
array[    OP_SPUT                         ]=markBB_filter; 
array[    OP_SPUT_WIDE                    ]=markBB_filter; 
array[    OP_SPUT_OBJECT                  ]=markBB_filter; 
array[    OP_SPUT_BOOLEAN                 ]=markBB_filter; 
array[    OP_SPUT_BYTE                    ]=markBB_filter; 
array[    OP_SPUT_CHAR                    ]=markBB_filter; 
array[    OP_SPUT_SHORT                   ]=markBB_filter; 



array[    OP_NEG_INT                      ]=markBB_filter; 
array[    OP_NOT_INT                      ]=markBB_filter; 
array[    OP_NEG_LONG                     ]=markBB_filter; 
array[    OP_NOT_LONG                     ]=markBB_filter; 
array[    OP_NEG_FLOAT                    ]=markBB_filter; 
array[    OP_NEG_DOUBLE                   ]=markBB_filter; 
array[    OP_INT_TO_LONG                  ]=markBB_filter; 
array[    OP_INT_TO_FLOAT                 ]=markBB_filter; 
array[    OP_INT_TO_DOUBLE                ]=markBB_filter; 
array[    OP_LONG_TO_INT                  ]=markBB_filter; 
array[    OP_LONG_TO_FLOAT                ]=markBB_filter; 
array[    OP_LONG_TO_DOUBLE               ]=markBB_filter; 
array[    OP_FLOAT_TO_INT                 ]=markBB_filter; 
array[    OP_FLOAT_TO_LONG                ]=markBB_filter; 
array[    OP_FLOAT_TO_DOUBLE              ]=markBB_filter; 
array[    OP_DOUBLE_TO_INT                ]=markBB_filter; 
array[    OP_DOUBLE_TO_LONG               ]=markBB_filter; 
array[    OP_DOUBLE_TO_FLOAT              ]=markBB_filter; 
array[    OP_INT_TO_BYTE                  ]=markBB_filter; 
array[    OP_INT_TO_CHAR                  ]=markBB_filter; 
array[    OP_INT_TO_SHORT                 ]=markBB_filter; 

//array[    OP_ADD_INT                      ]=markBB_filter; 
//array[    OP_SUB_INT                      ]=markBB_filter; 
//array[    OP_MUL_INT                      ]=markBB_filter; 
array[    OP_DIV_INT                      ]=markBB_filter; 
array[    OP_REM_INT                      ]=markBB_filter; 
//array[    OP_AND_INT                      ]=markBB_filter; 
//array[    OP_OR_INT                       ]=markBB_filter; 
//array[    OP_XOR_INT                      ]=markBB_filter; 
//array[    OP_SHL_INT                      ]=markBB_filter; 
//array[    OP_SHR_INT                      ]=markBB_filter; 
//array[    OP_USHR_INT                     ]=markBB_filter; 

array[    OP_ADD_LONG                     ]=markBB_filter; 
array[    OP_SUB_LONG                     ]=markBB_filter; 
array[    OP_MUL_LONG                     ]=markBB_filter; 
array[    OP_DIV_LONG                     ]=markBB_filter; 
array[    OP_REM_LONG                     ]=markBB_filter; 
array[    OP_AND_LONG                     ]=markBB_filter; 
array[    OP_OR_LONG                      ]=markBB_filter; 
array[    OP_XOR_LONG                     ]=markBB_filter; 
array[    OP_SHL_LONG                     ]=markBB_filter; 
array[    OP_SHR_LONG                     ]=markBB_filter; 
array[    OP_USHR_LONG                    ]=markBB_filter; 

array[    OP_ADD_FLOAT                    ]=markBB_filter; 
array[    OP_SUB_FLOAT                    ]=markBB_filter; 
array[    OP_MUL_FLOAT                    ]=markBB_filter; 
array[    OP_DIV_FLOAT                    ]=markBB_filter; 
array[    OP_REM_FLOAT                    ]=markBB_filter; 
array[    OP_ADD_DOUBLE                   ]=markBB_filter; 
array[    OP_SUB_DOUBLE                   ]=markBB_filter; 
array[    OP_MUL_DOUBLE                   ]=markBB_filter; 
array[    OP_DIV_DOUBLE                   ]=markBB_filter; 
array[    OP_REM_DOUBLE                   ]=markBB_filter; 

//array[    OP_ADD_INT_2ADDR                ]=markBB_filter; 
//array[    OP_SUB_INT_2ADDR                ]=markBB_filter; 
//array[    OP_MUL_INT_2ADDR                ]=markBB_filter; 
array[    OP_DIV_INT_2ADDR                ]=markBB_filter; 
array[    OP_REM_INT_2ADDR                ]=markBB_filter; 
//array[    OP_AND_INT_2ADDR                ]=markBB_filter; 
//array[    OP_OR_INT_2ADDR                 ]=markBB_filter; 
//array[    OP_XOR_INT_2ADDR                ]=markBB_filter; 
//array[    OP_SHL_INT_2ADDR                ]=markBB_filter; 
//array[    OP_SHR_INT_2ADDR                ]=markBB_filter; 
//array[    OP_USHR_INT_2ADDR               ]=markBB_filter; 

array[    OP_ADD_LONG_2ADDR               ]=markBB_filter; 
array[    OP_SUB_LONG_2ADDR               ]=markBB_filter; 
array[    OP_MUL_LONG_2ADDR               ]=markBB_filter; 
array[    OP_DIV_LONG_2ADDR               ]=markBB_filter; 
array[    OP_REM_LONG_2ADDR               ]=markBB_filter; 
array[    OP_AND_LONG_2ADDR               ]=markBB_filter; 
array[    OP_OR_LONG_2ADDR                ]=markBB_filter; 
array[    OP_XOR_LONG_2ADDR               ]=markBB_filter; 
array[    OP_SHL_LONG_2ADDR               ]=markBB_filter; 
array[    OP_SHR_LONG_2ADDR               ]=markBB_filter; 
array[    OP_USHR_LONG_2ADDR              ]=markBB_filter; 

array[    OP_ADD_FLOAT_2ADDR              ]=markBB_filter; 
array[    OP_SUB_FLOAT_2ADDR              ]=markBB_filter; 
array[    OP_MUL_FLOAT_2ADDR              ]=markBB_filter; 
array[    OP_DIV_FLOAT_2ADDR              ]=markBB_filter; 
array[    OP_REM_FLOAT_2ADDR              ]=markBB_filter; 
array[    OP_ADD_DOUBLE_2ADDR             ]=markBB_filter; 
array[    OP_SUB_DOUBLE_2ADDR             ]=markBB_filter; 
array[    OP_MUL_DOUBLE_2ADDR             ]=markBB_filter; 
array[    OP_DIV_DOUBLE_2ADDR             ]=markBB_filter; 
array[    OP_REM_DOUBLE_2ADDR             ]=markBB_filter; 

array[    OP_ADD_INT_LIT16                ]=markBB_filter; 
array[    OP_RSUB_INT                     ]=markBB_filter;
array[    OP_MUL_INT_LIT16                ]=markBB_filter; 
array[    OP_DIV_INT_LIT16                ]=markBB_filter; 
array[    OP_REM_INT_LIT16                ]=markBB_filter; 
array[    OP_AND_INT_LIT16                ]=markBB_filter; 
array[    OP_OR_INT_LIT16                 ]=markBB_filter; 
array[    OP_XOR_INT_LIT16                ]=markBB_filter; 

//array[    OP_ADD_INT_LIT8                 ]=markBB_filter; 
array[    OP_RSUB_INT_LIT8                ]=markBB_filter; 
//array[    OP_MUL_INT_LIT8                 ]=markBB_filter; 
array[    OP_DIV_INT_LIT8                 ]=markBB_filter; 
array[    OP_REM_INT_LIT8                 ]=markBB_filter; 
array[    OP_AND_INT_LIT8                 ]=markBB_filter; 
array[    OP_OR_INT_LIT8                  ]=markBB_filter; 
array[    OP_XOR_INT_LIT8                 ]=markBB_filter; 
array[    OP_SHL_INT_LIT8                 ]=markBB_filter; 
array[    OP_SHR_INT_LIT8                 ]=markBB_filter; 
array[    OP_USHR_INT_LIT8                ]=markBB_filter; 

array[    OP_IGET_VOLATILE                ]=markBB_filter;
array[    OP_IPUT_VOLATILE                ]=markBB_filter; 
array[    OP_SGET_VOLATILE                ]=markBB_filter; 
array[    OP_SPUT_VOLATILE                ]=markBB_filter; 
array[    OP_IGET_OBJECT_VOLATILE         ]=markBB_filter; 

array[    OP_IGET_WIDE_VOLATILE           ]=markBB_filter; 
array[    OP_IPUT_WIDE_VOLATILE           ]=markBB_filter; 
array[    OP_SGET_WIDE_VOLATILE           ]=markBB_filter; 
array[    OP_SPUT_WIDE_VOLATILE           ]=markBB_filter; 








