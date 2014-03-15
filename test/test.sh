#!/bin/sh
cat bb.list | while read line
do
	codeoffset=`echo $line | awk -F "+" '{print $2}'`  
	startoffset=`echo $line | awk -F "+" '{print $4}'` 
	echo "$codeoffset $startoffset"
done
