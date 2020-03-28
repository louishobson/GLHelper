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
    GLfloat vdata[] = 
    {
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
    program.use ();

    glh::texture2d texture { "assets/crate.png", GL_RGBA };
    texture.bind ();

    glh::math::mat4 model;
    glh::camera_perspective camera;
    camera.move_relative ( glh::math::vec3 { 0., 0., 30. } );

    glh::uniform proj_uni = program.get_uniform ( "trans" );

    glEnable ( GL_DEPTH_TEST );

    double prev_time = glfwGetTime ();
    while ( !window.should_close () ) 
    {
        double delta_time = glfwGetTime () - prev_time;

        if ( glfwGetKey ( window.internal_ptr (), GLFW_KEY_W ) == GLFW_TRUE ) camera.move_relative ( delta_time * glh::math::vec3 { 0., 0., -20. } );
        if ( glfwGetKey ( window.internal_ptr (), GLFW_KEY_A ) == GLFW_TRUE ) camera.move_relative ( delta_time * glh::math::vec3 { -10, 0., 0. } );
        if ( glfwGetKey ( window.internal_ptr (), GLFW_KEY_S ) == GLFW_TRUE ) camera.move_relative ( delta_time * glh::math::vec3 { 0., 0., 20. } );
        if ( glfwGetKey ( window.internal_ptr (), GLFW_KEY_D ) == GLFW_TRUE ) camera.move_relative ( delta_time * glh::math::vec3 { 10, 0., 0. } );

        if ( glfwGetKey ( window.internal_ptr (), GLFW_KEY_UP ) == GLFW_TRUE ) camera.pitch ( delta_time * glh::math::rad ( 80 ) );
        if ( glfwGetKey ( window.internal_ptr (), GLFW_KEY_LEFT ) == GLFW_TRUE ) camera.yaw ( delta_time * glh::math::rad ( 120 ) );
        if ( glfwGetKey ( window.internal_ptr (), GLFW_KEY_DOWN ) == GLFW_TRUE ) camera.pitch ( delta_time * glh::math::rad ( -80 ) );
        if ( glfwGetKey ( window.internal_ptr (), GLFW_KEY_RIGHT ) == GLFW_TRUE ) camera.yaw ( delta_time * glh::math::rad ( -120 ) );

        prev_time = glfwGetTime ();
        window.clear ( 1., 1., 1., 1. );
        for ( unsigned i = 0; i < 10; ++i )
        {
            model = glh::math::translate ( glh::math::rotate ( glh::math::identity<4> (), glh::math::rad ( i * 16 ), glh::math::norm ( glh::math::vec3 { 123, 53, 1 } ) ), posdata [ i ] );
            proj_uni.set_matrix ( camera.get_trans () * model );
            window.draw_arrays ( vao, program, GL_TRIANGLES, 0, 6 * 6 );
        }
        window.swap_buffers ();
        window.poll_events ();
    }

    return 0;
}