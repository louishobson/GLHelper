#!/bin/make

#!/bin/make

# COMMANDS AND FLAGS

# gcc setup
CC=gcc
CFLAGS=-O3 -std=c99 -Iinclude -static

# ar setup
AR=ar
ARFLAGS=-rc



# USEFUL TARGETS

# all
#
# make glhelper
all: glhelper

# clean
#
# remove all object files and static libraries
.PHONY: clean
clean:
	find . -type f -name "*\.o" -delete -print
	find . -type f -name "*\.a" -delete -print



# COMPILATION TARGETS

# glad
#
# purely compile glad source
glad: src/glad/glad.o