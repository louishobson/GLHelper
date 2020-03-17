#!/bin/make

#!/bin/make

# COMMANDS AND FLAGS

# gcc setup
CC=g++
CFLAGS=-O3 -std=c++14 -Iinclude -static

# g++ setup
CPP=g++
CPPFLAGS=-O3 -std=c++14 -Iinclude -static

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

# glhelper
#
# compile glhelper to a library
glhelper: src/glhelper/glhelper.a
src/glhelper/glhelper.a: src/glhelper/glhelper_glad.o src/glhelper/glhelper_glfw.o
	$(AR) $(ARFLAGS) $@ $^

# test
#
# test source
test: test.o src/glhelper/glhelper.a src/glad/glad.o
	$(CPP) -ldl -lGL -lglfw -lm -o $@ $^