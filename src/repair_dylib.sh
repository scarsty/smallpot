#!/bin/bash

DYLIBS=`ls -1`

for dylib in $DYLIBS; do
	BASENAME=`basename $dylib`			
	DYLIBS2=`otool -L $dylib | grep "/usr/local" | grep $BASENAME -v | awk -F' ' '{ print $1 }'`
	for dylib2 in $DYLIBS2; do
		new=@loader_path/../lib/`basename $dylib2`
		if [ $BASENAME != `basename $dylib2` ]; then
			echo change $dylib2 to $new at $BASENAME
			install_name_tool -change $dylib2 $new $dylib
		fi
	done
	
done

