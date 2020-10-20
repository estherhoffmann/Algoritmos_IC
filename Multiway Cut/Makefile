# Macros:
CPP= g++
CFLAGS= -Wall -g -o
CODE= main_aux.cpp SimpleMultiwayDecoder.cpp

# Targets:
all: alg_multiwaybrkga alg_simple_multiwaybrkga

alg_multiwaybrkga:
	$(CPP) $(CFLAGS) alg_multiwaybrkga main_multiwaybrkga.cpp Classes/MultiwayDecoder.cpp Classes/MultiwaySolver.cpp

alg_simple_multiwaybrkga:
	$(CPP) $(CFLAGS) alg_simple_multiwaybrkga main_simple_multiwaybrkga.cpp Classes/SimpleMultiwayDecoder.cpp

# Remove:
#clean:
#	rm -f $(OBJECTS)
