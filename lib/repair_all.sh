dylibs=`ls *.dylib`
for dylib in $dylibs; do
./repair_dylib2.sh $dylib loader_path loader_path\\\/\\\.\\\.\\\/lib
done
