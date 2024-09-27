# Makefile for cache simulator

CC = g++
OPT = -O3
# OPT = -g
WARN = -Wall
CFLAGS = $(OPT) $(WARN) -lm

# List all your .cpp files here (source files, excluding header files)
SIM_SRC = cache_sim.cpp

# List all header files here
HEADERS = parse.h cacheStructure.hpp

# List corresponding compiled object files here (.o files)
SIM_OBJ = $(SIM_SRC:.cpp=.o)

#################################

# Default rule
all: cache_sim
	@echo "My work is done here..."

# Rule for making cache_sim
cache_sim: $(SIM_OBJ)
	$(CC) -o $@ $(SIM_OBJ) $(CFLAGS)
	@echo "-----------DONE WITH CACHE_SIM-----------"

# Generic rule for converting any .cpp file to any .o file
%.o: %.cpp
	$(CC) $(CFLAGS) -c $<

# Type "make clean" to remove all .o files plus the cache_sim binary
clean:
	rm -f $(SIM_OBJ) cache_sim

# Type "make clobber" to remove all .o files (leaves cache_sim binary)
clobber:
	rm -f $(SIM_OBJ)

.PHONY: all clean clobber