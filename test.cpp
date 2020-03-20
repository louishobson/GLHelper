/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * test.cpp
 * 
 * test parts of the library
 * 
 */



/* INCLUDES */

/* include core headers */
#include <iostream>
#include <chrono>
#include <thread>

/* include glhelper.hpp */
#include <glhelper/glhelper.hpp>



void window_size_callback ( GLFWwindow * winptr, int width, int height )
{
    glh::window { winptr }.set_viewport_size ( width, height );
}

int main ()
{
    float vdata []
    {
        -0.5f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
    };

    unsigned edata []
    {
        0, 1, 2,
        1, 2, 3
    };
    
    glh::window window;
    window.set_window_size_callback ( window_size_callback );

    glh::vbo vbo { sizeof ( vdata ), vdata, GL_STATIC_DRAW };
    glh::ebo ebo { sizeof ( edata ), edata, GL_STATIC_DRAW };
    
    glh::vao vao;
    vao.set_vertex_attrib ( 0, vbo, 3, GL_FLOAT, GL_FALSE, 3 * sizeof ( float ), ( void * ) 0 );
    vao.bind_ebo ( ebo );

    glh::vshader vshader { "/home/louis/OneDrive/Documents/Programming/Mandelbrot/src/shader/generic_vertex.glsl" };
    glh::fshader fshader { "/home/louis/OneDrive/Documents/Programming/Mandelbrot/src/shader/generic_fragment.glsl" };
    glh::program program { vshader, fshader };

    window.clear ( 1., 0.3, .5, 1. );
    window.swap_buffers ();

    while ( !glfwWindowShouldClose ( window.internal_ptr () ) )
    {
        window.clear ( 1., 0.3, .5, 1. );
        window.draw_elements ( vao, program, GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );
        window.swap_buffers ();
        window.wait_events ( 0.0 );
    }

    return 0;
}