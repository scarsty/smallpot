CXX=g++
CC=gcc
CPPFLAGS=-I. -I../include -I../include/ffmpeg -O2 -std=c++0x -D__NO_INLINE__
CPPSOURCES=$(wildcard *.cpp)
#CPPSOURCES=main.cpp mythRedis.cpp mythRedisKey.cpp mythBaseServer.cpp mythLua.cpp mythReader.cpp mythVirtualServer.cpp mythWriter.cpp PEOPLE.cpp
CPPOBJECTS=$(CPPSOURCES:.cpp=.o)
#CSOURCES=$(wildcard *.c)
#COBJECTS=$(CSOURCES:.c=.o)
TARGET=BigPot.exe

.PHONY: all clean

all: .dcpp .dc $(CPPSOURCES) $(CSOURCES) $(TARGET)

.dcpp: $(CPPSOURCES)
	$(CXX) $(CPPFLAGS) -MM $(CPPSOURCES) >.dcpp
.dc: $(CSOURCES)
	$(CC) $(CFLAGS) -MM $(CSOURCES) >.dc
-include .dcpp
-include .dc
$(TARGET): $(CPPOBJECTS) $(COBJECTS)
	$(CXX) $(CPPOBJECTS) $(COBJECTS) -o $@ -L../lib/x86 -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lavutil -lavformat -lavcodec -lswresample -ljsoncpp -lass -liconv

clean:
	rm $(CPPOBJECTS) $(COBJECTS) .dcpp .dc
