# Macros:
CPP= g++
CFLAGS= -Wall -g
OBJECTS= main_aux.o SimpleMultiwayDecoder.o

# Targets:
all: generateobjs
	./a.out

linkobjs: generateobjs
	$(CPP) main_aux.o SimpleMultiwayDecoder.o

generateobjs:
	$(CPP) $(CFLAGS) -c main_aux.cpp SimpleMultiwayDecoder.cpp

# Remove:
clean:
	rm -f $(OBJECTS)
