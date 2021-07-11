#all: 
#	g++ control.cpp dumpData.cpp itg3200.cpp PruProxy.cpp -o control

CC=arm-linux-gnueabihf-g++
CFLAGS=-static -c -w -std=c++0x
#CFLAGS=
LDFLAGS=-static 
SOURCES=controlCopter.cpp PruProxy.cpp MPU6050.cpp I2Cdev.cpp pid.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=RunControlCopter
REMDIR ?= /srv/nfs/busybox/

export CROSS_COMPILE='arm-linux-gnueabihf-'
export PATH := /opt/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/bin:$(PATH)
export ARCH=arm

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ -xc++ -lstdc++ -shared-libgcc 

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

deploy:
	sudo cp $(EXECUTABLE) $(REMDIR)

clean:
	rm -f *.o $(EXECUTABLE)
