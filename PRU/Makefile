#all: 
#	g++ control.cpp dumpData.cpp itg3200.cpp PruProxy.cpp -o control

CC=g++
CFLAGS=-c -w
#CFLAGS=
LDFLAGS= 
SOURCES=ReadPWM_3.cpp PruProxy.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=RunReadPWM

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS)  -lrt -lpthread -o $@ 

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@