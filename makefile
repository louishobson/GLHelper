#!/bin/make

# COMMANDS AND FLAGS

# gcc setup
CC=g++
CFLAGS=-std=c++17 -Iinclude -fpic -g

# g++ setup
CPP=g++
CPPFLAGS=-std=c++17 -Iinclude -fpic -g

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
src/glhelper/libglhelper.a: src/glhelper/glhelper_glad.o src/glhelper/glhelper_glfw.o src/glhelper/glhelper_buffer.o src/glhelper/glhelper_shader.o src/glhelper/glhelper_texture.o src/glhelper/glhelper_camera.o src/glhelper/glhelper_model.o src/glhelper/glhelper_lighting.o src/glhelper/glhelper_render.o src/glhelper/glhelper_manager.o src/glhelper/glhelper_framebuffer.o src/glhelper/glhelper_vertices.o
	$(AR) $(ARFLAGS) $@ $^
src/glhelper/libglhelper.so: src/glhelper/glhelper_glad.o src/glhelper/glhelper_glfw.o src/glhelper/glhelper_buffer.o src/glhelper/glhelper_shader.o src/glhelper/glhelper_texture.o src/glhelper/glhelper_camera.o src/glhelper/glhelper_model.o src/glhelper/glhelper_lighting.o src/glhelper/glhelper_render.o src/glhelper/glhelper_manager.o src/glhelper/glhelper_framebuffer.o src/glhelper/glhelper_vertices.o
	$(CPP) -shared -o $@ $^


# test
#
# build test binary
.PHONY: test
test: test_shared
test_shared: test.o src/glad/glad.o src/glhelper/libglhelper.so
	$(CPP) $(CPPFLAGS) -Wl,-rpath=src/glhelper -Lsrc/glhelper test.o src/glad/glad.o -lglhelper -ldl -lGL -lglfw -lassimp -lm -o test
test_static: test.o src/glad/glad.o src/glhelper/libglhelper.a
	$(CPP) $(CPPFLAGS) test.o src/glad/glad.o src/glhelper/libglhelper.a -ldl -lGL -lglfw -lassimp -lm -o test

