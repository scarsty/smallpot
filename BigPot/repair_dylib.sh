#!/bin/bash
# Fix library load path
# Author: @ohdarling88
# Thanks to http://stackoverflow.com/questions/4677044/how-to-use-dylib-in-mac-os-x-c#answer-11585225

BIN=$1
BINNAME=`basename $BIN`
DYLIBS=`otool -L $BIN | grep "$2" | awk -F' ' '{ print $1 }'`

#for dylib in $DYLIBS; do		
#	echo found $dylib
#done

for dylib in $DYLIBS; do
	#echo "s/$2/$3/g"
	new=`echo $dylib | sed s/$2/$3/g`
	echo try change $dylib to $new
	if [ "$BINNAME" != "$name" ]; then
		install_name_tool -change $dylib $new $BIN
	fi
done
#otool -L $BIN
