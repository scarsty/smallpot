CXX=g++ #-static 
CC=gcc
CPPFLAGS=-I. -I../include -I/usr/local/include -I/usr/local/include/ffmpeg -O2 -std=c++0x
CPPSOURCES=$(wildcard *.cpp) tinyxml2/tinyxml2.cpp hash/sha3.cpp
CPPOBJECTS=$(CPPSOURCES:.cpp=.o)
#CSOURCES=$(wildcard *.c)
#COBJECTS=$(CSOURCES:.c=.o)
TARGET=tinypot

.PHONY: all clean

all: .dcpp .dc $(CPPSOURCES) $(CSOURCES) $(TARGET) tinypot.res

.dcpp: $(CPPSOURCES)
	$(CXX) $(CPPFLAGS) -MM $(CPPSOURCES) >.dcpp
.dc: $(CSOURCES)
	$(CC) $(CFLAGS) -MM $(CSOURCES) >.dc
-include .dcpp
-include .dc
$(TARGET): $(CPPOBJECTS) $(COBJECTS) tinypot.res
	$(CXX) $(CPPOBJECTS) $(COBJECTS) -o $@ -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lavutil -lavformat -lavcodec -lswresample -lass -lpthread
    
clean:
	rm $(CPPOBJECTS) $(COBJECTS) .dcpp .dc
    
bigpot.res: TinyPot1.rc
	windres -i app_res.rc -o app_res.o
