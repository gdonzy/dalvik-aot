int dvmCompilerAllocTypedTemp(CompilationUnit *cUnit, bool fpHint, int regClass)
{
    return dvmCompilerAllocTemp(cUnit);
}


int dvmCompilerAllocTypedTempPair(CompilationUnit *cUnit, bool fpHint,
                                  int regClass)
{
    int highReg;
    int lowReg;
    int res = 0;
    lowReg = dvmCompilerAllocTemp(cUnit);
    highReg = dvmCompilerAllocTemp(cUnit);
    res = (lowReg & 0xff) | ((highReg & 0xff) << 8);
    return res;
}
