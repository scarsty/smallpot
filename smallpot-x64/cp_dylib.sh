#!/bin/bash

#DYLIBS=`ls -1`
DYLIBS=SmallPot.exe
for dylib in $DYLIBS; do		
	#echo found $dylib
	BIN=$dylib
	DYLIBS2=`ldd $BIN | grep "vcpkg" | grep $BIN -v | awk -F' ' '{ print $3 }'`
	for dylib2 in $DYLIBS2; do		
		echo found $dylib2
		cp -p $dylib2 .
	done
done




