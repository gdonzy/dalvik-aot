#!/bin/bash
rm -f tmp.log BB-24fc.bin
./togcc.sh
./dexmain classes.dex > tmp.log
unicore32-linux-objdump -D -b binary -m unicore32 BB-24fe.bin

