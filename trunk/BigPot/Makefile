CXX=g++
CC=gcc
CPPFLAGS=-I. -I../include -I../include/ffmpeg -O2 -std=c++0x 
CPPSOURCES=$(wildcard *.cpp) tinyxml2/tinyxml2.cpp hash/sha3.cpp
#CPPSOURCES=main.cpp mythRedis.cpp mythRedisKey.cpp mythBaseServer.cpp mythLua.cpp mythReader.cpp mythVirtualServer.cpp mythWriter.cpp PEOPLE.cpp
CPPOBJECTS=$(CPPSOURCES:.cpp=.o)
#CSOURCES=$(wildcard *.c)
#COBJECTS=$(CSOURCES:.c=.o)
TARGET=bigpot

.PHONY: all clean

all: .dcpp .dc $(CPPSOURCES) $(CSOURCES) $(TARGET)

.dcpp: $(CPPSOURCES)
	$(CXX) $(CPPFLAGS) -MM $(CPPSOURCES) >.dcpp
.dc: $(CSOURCES)
	$(CC) $(CFLAGS) -MM $(CSOURCES) >.dc
-include .dcpp
-include .dc
$(TARGET): $(CPPOBJECTS) $(COBJECTS)
	#$(CXX) $(CPPOBJECTS) $(COBJECTS) -o $@ -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lavutil -lavformat -lavcodec -lswresample -ljsoncpp -lass -liconv
	$(CXX) $(CPPOBJECTS) $(COBJECTS) -o $@ -framework SDL2 -framework SDL2_ttf -framework SDL2_image -lavutil -lavformat -lavcodec -lswresample -ljsoncpp -lass -L/usr/local/lib -liconv

clean:
	rm $(CPPOBJECTS) $(COBJECTS) .dcpp .dc
