#!/bin/bash

#DYLIBS=`ls -1`
DYLIBS=TinyPot.exe
for dylib in $DYLIBS; do		
	#echo found $dylib
	BIN=$dylib
	DYLIBS2=`ldd $BIN | grep "mingw64" | grep $BIN -v | awk -F' ' '{ print $3 }'`
	for dylib2 in $DYLIBS2; do		
		echo found $dylib2
		cp -p $dylib2 .
	done
done




