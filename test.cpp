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
    window.set_input_mode ( GLFW_CURSOR, GLFW_CURSOR_DISABLED );

    glh::vbo vbo { sizeof ( vdata ), vdata, GL_STATIC_DRAW };

    glh::vao vao;
    vao.set_vertex_attrib ( 0, vbo, 3, GL_FLOAT, GL_FALSE, 5 * sizeof ( GLfloat ), ( GLvoid * ) 0 );
    vao.set_vertex_attrib ( 1, vbo, 2, GL_FLOAT, GL_FALSE, 5 * sizeof ( GLfloat ), ( GLvoid * ) ( 3 * sizeof ( GLfloat ) ) );

    glh::vshader vshader { "shaders/vertex.glsl" };
    glh::fshader fshader { "shaders/fragment.glsl" };
    glh::program program { vshader, fshader };
    glh::uniform proj_uni = program.get_uniform ( "trans" );

    glh::texture2d texture { "assets/crate.png", GL_RGBA };
    texture.bind ();

    glh::math::mat4 model;
    glh::camera_perspective camera { glh::math::rad ( 75 ), 16. / 9., 0.1, 200 };
    camera.move_global ( glh::math::vec3 { 0., 0., 30. } );

    glh::static_renderable renderable { window, vao, program, texture };
    renderable.clear_colour ( 1., 1., 1., 1. );
    renderable.enable_depth_test ();

    renderable.prepare ();
    while ( !window.should_close () ) 
    {
        auto timeinfo = window.get_timeinfo ();
        auto dimensions = window.get_dimensions ();
        auto mouseinfo = window.get_mouseinfo ();
        renderable.viewport ( 0, 0, dimensions.width, dimensions.height );

        if ( window.get_key ( GLFW_KEY_W ).action == GLFW_PRESS ) camera.move_relative ( timeinfo.deltapoll * glh::math::vec3 { 0., 0., -20. } );
        if ( window.get_key ( GLFW_KEY_A ).action == GLFW_PRESS ) camera.move_relative ( timeinfo.deltapoll * glh::math::vec3 { -10, 0., 0. } );
        if ( window.get_key ( GLFW_KEY_S ).action == GLFW_PRESS ) camera.move_relative ( timeinfo.deltapoll * glh::math::vec3 { 0., 0., 20. } );
        if ( window.get_key ( GLFW_KEY_D ).action == GLFW_PRESS ) camera.move_relative ( timeinfo.deltapoll * glh::math::vec3 { 10, 0., 0. } );

        if ( window.get_key ( GLFW_KEY_SPACE ).action == GLFW_PRESS ) camera.move_relative ( timeinfo.deltapoll * glh::math::vec3 { 0., 10., 0. } );
        if ( window.get_key ( GLFW_KEY_LEFT_SHIFT ).action == GLFW_PRESS ) camera.move_relative ( timeinfo.deltapoll * glh::math::vec3 { 0., -10., 0. } );

        if ( window.get_key ( GLFW_KEY_Z ).action == GLFW_PRESS ) camera.roll ( timeinfo.deltapoll * glh::math::rad ( 80 ) );
        if ( window.get_key ( GLFW_KEY_X ).action == GLFW_PRESS ) camera.roll ( timeinfo.deltapoll * glh::math::rad ( -80 ) ); 

        camera.pitch ( mouseinfo.deltayfrac * glh::math::rad ( -80 ) );
        camera.yaw ( mouseinfo.deltaxfrac * glh::math::rad ( -80 ) );  

        renderable.clear ();
        for ( unsigned i = 0; i < 10; ++i )
        {
            model = glh::math::translate ( glh::math::rotate ( glh::math::identity<4> (), glh::math::rad ( i * 16 ), glh::math::norm ( glh::math::vec3 { 123, 53, 1 } ) ), posdata [ i ] );
            proj_uni.set_matrix ( camera.get_trans () * model );
            renderable.draw_arrays ( GL_TRIANGLES, 0, 6 * 6 );
        }
        window.swap_buffers ();
        window.poll_events ();
        
    }

    return 0;
}