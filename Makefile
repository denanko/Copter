#all: 
#	g++ control.cpp dumpData.cpp itg3200.cpp PruProxy.cpp -o control

CC=g++
CFLAGS=-c -w -std=c++0x
#CFLAGS=
LDFLAGS= 
SOURCES=controlCopter.cpp PruProxy.cpp MPU6050.cpp I2Cdev.cpp pid.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=RunControlCopter

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS)  -lrt -lpthread -o $@ 

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@