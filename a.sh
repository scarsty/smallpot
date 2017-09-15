cd src
rm tinypot
make -f Makefile.clang 
./repair_dylib.sh tinypot loader_path loader_path\\\/\\\.\\\.\\\/lib
./repair_dylib.sh tinypot rpath loader_path\\\/\\\.\\\.\\\/lib
otool -L tinypot
cd ..
cp src/tinypot tinypot.app/Contents/MacOS/tinypot
