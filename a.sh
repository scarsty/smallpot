cd src
rm tinypot
cmake .
make -j 
otool -L tinypot
cd ..
cp src/tinypot tinypot.app/Contents/MacOS/tinypot
cd tinypot.app/Contents/MacOS
../../../src/repair_dylib.sh
otool -L tinypot
