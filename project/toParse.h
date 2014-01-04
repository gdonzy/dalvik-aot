#include "DEX.h"
#include <stdio.h>
signed char * dexCreateInstrWidthTable(void);
unsigned char * dexCreateInstrFormatTable(void);
DexFile* parseDexFile(const u1* addr,size_t length);
void* dumpClass(DexFile* pDexFile,int idx);
