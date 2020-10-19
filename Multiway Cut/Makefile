# Macros:
CPP= g++
CFLAGS= -Wall -g -o
CODE= main_aux.cpp SimpleMultiwayDecoder.cpp

# Targets:
all:
	$(CPP) $(CFLAGS) alg_teste $(CODE)

linkobjs: generateobjs
	$(CPP) main_aux.o SimpleMultiwayDecoder.o

generateobjs:
	$(CPP) $(CFLAGS) -c main_aux.cpp SimpleMultiwayDecoder.cpp

# Remove:
clean:
	rm -f $(OBJECTS)
