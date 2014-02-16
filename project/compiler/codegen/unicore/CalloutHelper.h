#define LOAD_FUNC_ADDR(cUnit, reg, addr) loadConstant(cUnit, reg, addr)

/* Integer arithmetics */
//int __aeabi_idivmod(int op1, int op2);  // OP_REM_INT[_2ADDR|_LIT8|_LIT16]
//int __aeabi_idiv(int op1, int op2);     // OP_DIV_INT[_2ADDR|_LIT8|_LIT16]
//for unicore
int __modsi3(int op1, int op2);  // OP_REM_INT[_2ADDR|_LIT8|_LIT16]
int __divsi3(int op1, int op2);     // OP_DIV_INT[_2ADDR|_LIT8|_LIT16]          
