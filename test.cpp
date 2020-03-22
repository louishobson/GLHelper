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
    GLfloat vdata []
    {
        -1.f, 1.f, 0.0f,
        1.f, 1.f, 0.0f,
        -1.f, -1.f, 0.0f,
        1.f, -1.f, 0.0f
    };

    GLuint edata []
    {
        0, 1, 2,
        1, 2, 3
    };
    
    glh::window window;
    window.set_window_size_callback ( window_size_callback );

    glh::vbo vbo { sizeof ( vdata ), vdata, GL_STATIC_DRAW };
    glh::ebo ebo { sizeof ( edata ), edata, GL_STATIC_DRAW };
    
    glh::vao vao;
    vao.set_vertex_attrib ( 0, vbo, 3, GL_FLOAT, GL_FALSE, 3 * sizeof ( GLfloat ), ( GLvoid * ) 0 );
    vao.bind_ebo ( ebo );

    glh::vshader vshader { "/home/louis/OneDrive/Documents/Programming/Mandelbrot/src/shader/generic_vertex.glsl" };
    glh::fshader fshader { "/home/louis/OneDrive/Documents/Programming/Mandelbrot/src/shader/mandelbrot_fragment.glsl" };
    glh::program program { vshader, fshader };

    program.set_uniform_float ( "mandelbrot_stretch", 0.002, 0.002, 1, 1 );
    program.set_uniform_float ( "mandelbrot_translation", -2, -1, 0, 0 );
    program.set_uniform_matrix ( "mandelbrot_rotation", glh::math::rotate ( glh::math::identity<2> (), 0, 1, glh::math::pi ( 0.1 ) ) );
    program.set_uniform_float ( "mandelbrot_breakout", 2 );
    program.set_uniform_int ( "mandelbrot_max_it", 40 );
    program.set_uniform_int ( "mandelbrot_power", 2 );

    while ( !window.should_close () )
    {
        window.clear ( 1., 1., 1., 1. );
        window.draw_elements ( vao, program, GL_TRIANGLES, 6, GL_UNSIGNED_INT, ( GLvoid * ) 0 );
        window.swap_buffers ();
        window.wait_events ( 0.0 );
    }

    return 0;
}