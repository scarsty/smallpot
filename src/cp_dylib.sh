#!/bin/bash

DYLIBS=`ls -1`

for dylib in $DYLIBS; do		
	echo found existing file $dylib
	BIN=$dylib
	DYLIBS2=`otool -L $BIN | grep "/usr/local" | grep $BIN -v | awk -F' ' '{ print $1 }'`
	for dylib2 in $DYLIBS2; do
		echo ' ' found relied file $dylib2
		basename2=`basename $dylib2`
		if [ ! -f "$basename2" ]; then 
		    echo "  cp dylib"
			cp $dylib2 ../lib 
	    fi
	done
done




