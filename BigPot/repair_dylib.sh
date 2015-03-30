#!/bin/bash
# Fix library load path
# Author: @ohdarling88
# Thanks to http://stackoverflow.com/questions/4677044/how-to-use-dylib-in-mac-os-x-c#answer-11585225

BIN=$1
BINNAME=`basename $BIN`
DYLIBS=`otool -L $BIN | grep "/usr/local" | awk -F' ' '{ print $1 }'`

for dylib in $DYLIBS; do		
	echo found $dylib
	cp $dylib ../lib
done

for dylib in $DYLIBS; do
	#echo "s/$2/$3/g"
	#new=`echo $dylib | sed s/$2/$3/g`
	new=@loader_path/../lib/`basename $dylib`
	echo try change $dylib to $new
	if [ "$BINNAME" != "$name" ]; then
		install_name_tool -change $dylib $new $BIN
	fi
done
#otool -L $BIN
