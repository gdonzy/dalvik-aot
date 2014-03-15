#!/bin/bash
rm -f tmp.log *.bin
./togcc.sh
./dexmain classes.dex 0x1008 0x1078 > tmp.log
#./dexmain classes.dex 0x1008 0x106a > tmp.log
unicore32-linux-objdump -D -b binary -m unicore32 BB-1078.bin
