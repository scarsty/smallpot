#!/bin/bash

DYLIBS=`ls -1`

for dylib in $DYLIBS; do		
	#echo found $dylib
	BIN=$dylib
	DYLIBS2=`otool -L $BIN | grep "/usr/local" | grep $BIN -v | awk -F' ' '{ print $1 }'`
	for dylib2 in $DYLIBS2; do		
		#echo found $dylib
		cp $dylib2 ../lib
	done
done




