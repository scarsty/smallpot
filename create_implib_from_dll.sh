#if [ $# == 1 ]; do

#fi
echo LIBRARY $2.dll > $1.def
echo EXPORTS >> $1.def
FUNCS=`nm /mingw64/lib/lib$1.dll.a | grep "I __imp_" | sed "s/.* I __imp_//"`

COUNT=1
for func in $FUNCS; do
  echo $func @$COUNT >> $1.def
  #echo $func >> $1.def
  COUNT=`echo $COUNT+1|bc -l`
done

rm $1.lib
rm $1.exp
#lib /machine:X64 /def:$1.def
dlltool -d $1.def -l $1.lib -D $2.dll
