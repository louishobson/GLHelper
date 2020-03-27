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
    GLfloat vdata[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    glh::math::vec3 posdata[] = 
    {
        glh::math::vec3 {  0.0f,  0.0f,  0.0f  },
        glh::math::vec3 {  2.0f,  5.0f, -15.0f }, 
        glh::math::vec3 { -1.5f, -2.2f, -2.5f  },  
        glh::math::vec3 { -3.8f, -2.0f, -12.3f },  
        glh::math::vec3 {  2.4f, -0.4f, -3.5f  },  
        glh::math::vec3 { -1.7f,  3.0f, -7.5f  },  
        glh::math::vec3 {  1.3f, -2.0f, -2.5f  },  
        glh::math::vec3 {  1.5f,  2.0f, -2.5f  }, 
        glh::math::vec3 {  1.5f,  0.2f, -1.5f  }, 
        glh::math::vec3 { -1.3f,  1.0f, -1.5f  } 
    };

    glh::window window;
    window.set_window_size_callback ( window_size_callback );

    glh::vbo vbo { sizeof ( vdata ), vdata, GL_STATIC_DRAW };

    glh::vao vao;
    vao.set_vertex_attrib ( 0, vbo, 3, GL_FLOAT, GL_FALSE, 5 * sizeof ( GLfloat ), ( GLvoid * ) 0 );
    vao.set_vertex_attrib ( 1, vbo, 2, GL_FLOAT, GL_FALSE, 5 * sizeof ( GLfloat ), ( GLvoid * ) ( 3 * sizeof ( GLfloat ) ) );

    glh::vshader vshader { "shaders/vertex.glsl" };
    glh::fshader fshader { "shaders/fragment.glsl" };
    glh::program program { vshader, fshader };

    glh::texture2d texture { "assets/crate.png", GL_RGBA };
    texture.bind ();

    glh::math::mat4 model = glh::math::identity<4> ();
    glh::math::mat4 rot = glh::math::translate ( glh::math::identity<4> (), glh::math::vec3 { 0.0f, 0.0f, 5.0f } );
    glh::math::mat4 view = glh::math::translate ( glh::math::identity<4> (), glh::math::vec3 { 0., 0., -15 } );
    glh::math::mat4 proj = glh::math::perspective_fov ( glh::math::rad ( 45 ), 1., 0.1, 100. );
    
    program.use ();
    glh::uniform proj_uni = program.get_uniform ( "trans" );

    glEnable ( GL_DEPTH_TEST );

    while ( !window.should_close () ) 
    {
        window.clear ( 1., 1., 1., 1. );
        rot = glh::math::rotate ( rot, glh::math::vec3 { glh::math::rad ( 1 ), 0, glh::math::rad ( 2 ) } );
        for ( unsigned i = 0; i < 10; ++i )
        {
            model = glh::math::rotate ( glh::math::identity<4> (), glh::math::vec3 ( glh::math::rad ( ( i + 1 ) * 36 ), glh::math::rad ( ( i + 1 ) * 18 ), 0 ) );
            proj_uni.set_matrix ( proj * view * rot * glh::math::translate ( model, posdata [ i ] ) );
            window.draw_arrays ( vao, program, GL_TRIANGLES, 0, 6 * 6 );
        }
        window.swap_buffers ();
        window.poll_events ();
    }

    return 0;
}