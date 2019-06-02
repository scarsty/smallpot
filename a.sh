NAME=smallpot
cd src
#cp CMakeLists-osx.txt CMakeLists.txt -f
rm $NAME
cmake .
make -j 
otool -L $NAME
cd ..
cp src/$NAME $NAME.app/Contents/MacOS/$NAME
cd $NAME.app/Contents/MacOS
../../../src/repair_dylib.sh
otool -L $NAME
