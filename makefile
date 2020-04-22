#!/bin/make

# COMMANDS AND FLAGS

# gcc setup
CC=g++
CFLAGS=-std=c++14 -Iinclude -fpic -g

# g++ setup
CPP=g++
CPPFLAGS=-std=c++14 -Iinclude -fpic -g

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
# remove all object files and libraries
.PHONY: clean
clean:
	find . -type f -name "*\.o" -delete -print
	find . -type f -name "*\.a" -delete -print
	find . -type f -name "*\.so" -delete -print



# COMPILATION TARGETS

# glad
#
# purely compile glad source
glad: src/glad/glad.o

# glhelper
#
# compile glhelper to a library
glhelper: src/glhelper/libglhelper.a src/glhelper/libglhelper.so
src/glhelper/libglhelper.a: src/glhelper/glhelper_glad.o src/glhelper/glhelper_glfw.o src/glhelper/glhelper_buffer.o src/glhelper/glhelper_shader.o src/glhelper/glhelper_texture.o src/glhelper/glhelper_camera.o src/glhelper/glhelper_model.o src/glhelper/glhelper_lighting.o src/glhelper/glhelper_render.o src/glhelper/glhelper_manager.o src/glhelper/glhelper_framebuffer.o
	$(AR) $(ARFLAGS) $@ $^
src/glhelper/libglhelper.so: src/glhelper/glhelper_glad.o src/glhelper/glhelper_glfw.o src/glhelper/glhelper_buffer.o src/glhelper/glhelper_shader.o src/glhelper/glhelper_texture.o src/glhelper/glhelper_camera.o src/glhelper/glhelper_model.o src/glhelper/glhelper_lighting.o src/glhelper/glhelper_render.o src/glhelper/glhelper_manager.o src/glhelper/glhelper_framebuffer.o
	$(CPP) -shared -o $@ $^


# test
#
# test source
test: test.o glad glhelper
	$(CPP) -Wl,-rpath=src/glhelper -Lsrc/glhelper -lglhelper -ldl -lGL -lglfw -lassimp -lm -o $@ test.o src/glad/glad.o 