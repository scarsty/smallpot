CXX=g++ #-static 
CC=gcc
CPPFLAGS=-I. -I../include -I/usr/local/include -I/usr/local/include/ffmpeg -O2 -std=c++0x
CPPSOURCES=$(wildcard *.cpp) tinyxml2/tinyxml2.cpp hash/sha3.cpp
#CPPSOURCES=main.cpp mythRedis.cpp mythRedisKey.cpp mythBaseServer.cpp mythLua.cpp mythReader.cpp mythVirtualServer.cpp mythWriter.cpp PEOPLE.cpp
CPPOBJECTS=$(CPPSOURCES:.cpp=.o)
#CSOURCES=$(wildcard *.c)
#COBJECTS=$(CSOURCES:.c=.o)
TARGET=bigpot

.PHONY: all clean

all: .dcpp .dc $(CPPSOURCES) $(CSOURCES) $(TARGET) bigpot.res

.dcpp: $(CPPSOURCES)
	$(CXX) $(CPPFLAGS) -MM $(CPPSOURCES) >.dcpp
.dc: $(CSOURCES)
	$(CC) $(CFLAGS) -MM $(CSOURCES) >.dc
-include .dcpp
-include .dc
$(TARGET): $(CPPOBJECTS) $(COBJECTS) bigpot.res
	$(CXX) $(CPPOBJECTS) $(COBJECTS) -o $@ -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lavutil -lavformat -lavcodec -lswresample -lass -lpthread
	#$(CXX) $(CPPOBJECTS) $(COBJECTS) bigpot.res -o $@ -L/usr/local/lib -lmingw32 -lSDL2main -lSDL2_ttf -lSDL2_image -lSDL2 -liconv -lavformat -lavcodec -lswresample -lavutil -lass -lfribidi -lfreetype -lpng -lz -lwinmm -lole32 -lm -limm32 -loleaut32 -lshell32 -lversion -luuid -lws2_32 -mwindows
	strip bigpot.exe
    
clean:
	rm $(CPPOBJECTS) $(COBJECTS) .dcpp .dc
    
bigpot.res: BigPot1.rc
	windres -i app_res.rc -o app_res.o
