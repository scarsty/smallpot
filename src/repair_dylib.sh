#!/bin/bash

DYLIBS=`ls -1`

for dylib in $DYLIBS; do
	BASENAME=`basename $dylib`			
	DYLIBS2=`otool -L $dylib | grep -E "/usr/local|@loader_path" | grep $BASENAME -v | awk -F' ' '{ print $1 }'`
	echo $dylib
	install_name_tool -id @loader_path/`basename $dylib` $dylib
	for dylib2 in $DYLIBS2; do
		new=@loader_path/`basename $dylib2`
		if [ $dylib2 != $new ]; then
			echo ' ' change $dylib2 to $new at $BASENAME
			install_name_tool -change $dylib2 $new $dylib
		fi
	done
	
done

