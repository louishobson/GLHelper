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

    GLfloat cratebuff [] = 
    {
        -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,   0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,   1.0f, 0.0f,   0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,   1.0f, 1.0f,   0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,   1.0f, 1.0f,   0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,   0.0f, 1.0f,   0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,   0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,   0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,   1.0f, 0.0f,   0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,   1.0f, 1.0f,   0.0f,  0.0f,  1.0f,   
         0.5f,  0.5f,  0.5f,   1.0f, 1.0f,   0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,   0.0f, 1.0f,   0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,   0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f,   1.0f, 0.0f,  -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,   1.0f, 1.0f,  -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,   0.0f, 1.0f,  -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,   0.0f, 1.0f,  -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,  -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,   1.0f, 0.0f,  -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,   1.0f, 0.0f,   1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,   1.0f, 1.0f,   1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,   0.0f, 1.0f,   1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,   0.0f, 1.0f,   1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,   0.0f, 0.0f,   1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,   1.0f, 0.0f,   1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,   0.0f, 1.0f,   0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,   1.0f, 1.0f,   0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,   1.0f, 0.0f,   0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,   1.0f, 0.0f,   0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,   0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,   0.0f, 1.0f,   0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,   0.0f, 1.0f,   0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,   1.0f, 1.0f,   0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,   1.0f, 0.0f,   0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,   1.0f, 0.0f,   0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,   0.0f, 0.0f,   0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,   0.0f, 1.0f,   0.0f,  1.0f,  0.0f,
    };

    glh::math::vec3 cratepos []
    {
        glh::math::vec3 { 0.0, 0.5, 0.0 },
        glh::math::vec3 { 0.0, 1.5, 0.0 },
        glh::math::vec3 { 0.0, 2.5, 0.0 },

        glh::math::vec3 { 5.0, 0.5, 3.0 },
        glh::math::vec3 { 5.0, 1.5, 3.0 },

        glh::math::vec3 { -3.0, 0.5, -6.0 },
    };

    GLfloat floorbuff []
    {
        -50.0f, 0.0f,  50.0f,     0.0f,   0.0f,   0.0f, 1.0f, 0.0f,
         50.0f, 0.0f,  50.0f,   100.0f,   0.0f,   0.0f, 1.0f, 0.0f,
        -50.0f, 0.0f, -50.0f,     0.0f, 100.0f,   0.0f, 1.0f, 0.0f,
         50.0f, 0.0f, -50.0f,   100.0f, 100.0f,   0.0f, 1.0f, 0.0f
    };

    GLuint floorelems []
    {
        0, 1, 2, 1, 2, 3
    };

    double scale = 1;

    glh::window window;
    window.set_input_mode ( GLFW_CURSOR, GLFW_CURSOR_DISABLED );

    glh::vbo cratevbo { sizeof ( cratebuff ), cratebuff, GL_STATIC_DRAW };
    glh::vbo floorvbo { sizeof ( floorbuff ), floorbuff, GL_STATIC_DRAW };
    glh::ebo floorebo { sizeof ( floorelems ), floorelems, GL_STATIC_DRAW };

    glh::vao cratevao;
    cratevao.set_vertex_attrib ( 0, cratevbo, 3, GL_FLOAT, GL_FALSE, 8 * sizeof ( GLfloat ), ( GLvoid * ) 0 );
    cratevao.set_vertex_attrib ( 1, cratevbo, 2, GL_FLOAT, GL_FALSE, 8 * sizeof ( GLfloat ), ( GLvoid * ) ( 3 * sizeof ( GLfloat ) ) );
    cratevao.set_vertex_attrib ( 2, cratevbo, 3, GL_FLOAT, GL_FALSE, 8 * sizeof ( GLfloat ), ( GLvoid * ) ( 5 * sizeof ( GLfloat ) ) );
    glh::vao floorvao;
    floorvao.set_vertex_attrib ( 0, floorvbo, 3, GL_FLOAT, GL_FALSE, 8 * sizeof ( GLfloat ), ( GLvoid * ) 0 );
    floorvao.set_vertex_attrib ( 1, floorvbo, 2, GL_FLOAT, GL_FALSE, 8 * sizeof ( GLfloat ), ( GLvoid * ) ( 3 * sizeof ( GLfloat ) ) );
    floorvao.set_vertex_attrib ( 2, floorvbo, 3, GL_FLOAT, GL_FALSE, 8 * sizeof ( GLfloat ), ( GLvoid * ) ( 5 * sizeof ( GLfloat ) ) );
    floorvao.bind_ebo ( floorebo );

    glh::vshader vshader { "shaders/vertex.glsl" };
    glh::fshader fshader { "shaders/fragment.glsl" };
    glh::program program { vshader, fshader };
    glh::struct_uniform material_uni = program.get_structure_uniform ( "Material" );
    glh::struct_uniform lighting_uni = program.get_structure_uniform ( "Lighting" );
    glh::struct_uniform trans_uni = program.get_structure_uniform ( "Trans" );

    glh::texture2d cratetex { "assets/crate.png", GL_RGBA, GL_TEXTURE0 };
    glh::texture2d floortex { "assets/grass.png", GL_RGB, GL_TEXTURE1 };
    floortex.set_wrap ( GL_REPEAT );

    glh::math::mat4 model;
    glh::camera_perspective camera { glh::math::rad ( 60 ), 16.0 / 9.0, 0.1, 1000 };
    camera.move ( glh::math::vec3 { 0.0, 10.0, 0.0 } );
    camera.enable_restrictive_mode ();

    glh::static_renderable craterend { window, cratevao, program, cratetex };
    glh::static_renderable floorrend { window, floorvao, program, floortex };
    
    glh::renderer::clear_colour ( 0.0, 0.0, 0.0, 1.0 );
    glh::renderer::enable_depth_test ();

    program.use ();
    trans_uni [ "Proj" ].set_matrix ( camera.get_proj () );
    lighting_uni [ "Ambient" ].set_vector ( glh::math::vec3 { 0.4, 0.4, 0.4 } );
    lighting_uni [ "Colour" ].set_vector ( glh::math::vec3 { 1.0, 1.0, 1.0 } );
    lighting_uni [ "SpecStrength" ].set_float ( 0.5 );

    while ( !window.should_close () ) 
    {
        auto timeinfo = window.get_timeinfo ();
        auto dimensions = window.get_dimensions ();
        auto mouseinfo = window.get_mouseinfo ();

        glh::renderer::viewport ( 0, 0, dimensions.width, dimensions.height );

        if ( window.get_key ( GLFW_KEY_W ).action == GLFW_PRESS ) camera.move ( timeinfo.delta * glh::math::vec3 { 0., 0., -10. } );
        if ( window.get_key ( GLFW_KEY_A ).action == GLFW_PRESS ) camera.move ( timeinfo.delta * glh::math::vec3 { -10, 0., 0. } );
        if ( window.get_key ( GLFW_KEY_S ).action == GLFW_PRESS ) camera.move ( timeinfo.delta * glh::math::vec3 { 0., 0., 10. } );
        if ( window.get_key ( GLFW_KEY_D ).action == GLFW_PRESS ) camera.move ( timeinfo.delta * glh::math::vec3 { 10, 0., 0. } );

        if ( window.get_key ( GLFW_KEY_SPACE ).action == GLFW_PRESS ) camera.move ( timeinfo.delta * glh::math::vec3 { 0., 10., 0. } );
        if ( window.get_key ( GLFW_KEY_LEFT_SHIFT ).action == GLFW_PRESS ) camera.move ( timeinfo.delta * glh::math::vec3 { 0., -10., 0. } );

        if ( window.get_key ( GLFW_KEY_Z ).action == GLFW_PRESS ) camera.roll ( timeinfo.delta * glh::math::rad ( 80 ) );
        if ( window.get_key ( GLFW_KEY_X ).action == GLFW_PRESS ) camera.roll ( timeinfo.delta * glh::math::rad ( -80 ) ); 

        camera.pitch ( mouseinfo.deltayfrac * glh::math::rad ( -80 ) );
        camera.yaw ( mouseinfo.deltaxfrac * glh::math::rad ( -80 ) );
        
        trans_uni [ "View" ].set_matrix ( camera.get_view () );
        lighting_uni [ "ViewPos" ].set_vector ( camera.get_pos () );
        lighting_uni [ "LightPos" ].set_vector ( glh::math::rotate ( glh::math::vec3 { 7.0, 5.0, -5.0 } * scale, glh::math::rad ( timeinfo.now * 30 ), glh::math::vec3 { 0.0, 1.0, 0.0 } ) );
        
        glh::renderer::clear ();

        craterend.prepare ();
        material_uni [ "TexUnit" ].set_int ( 0 );
        material_uni [ "Shininess" ].set_float ( 2 );
        for ( auto vec: cratepos )
        {
            glh::math::mat4 model = glh::math::translate ( glh::math::resize<4> ( glh::math::enlarge ( glh::math::identity<3> (), scale ) ), vec * scale );
            trans_uni [ "Model" ].set_matrix ( model );
            trans_uni [ "NormMat" ].set_matrix ( glh::math::transpose ( glh::math::inverse ( glh::math::resize<3> ( model ) ) ) );
            glh::renderer::draw_arrays ( GL_TRIANGLES, 0, 6 * 6 );
        }

        floorrend.prepare ();
        material_uni [ "TexUnit" ].set_int ( 1 );
        material_uni [ "Shininess" ].set_float ( 2 );
        trans_uni [ "Model" ].set_matrix ( glh::math::identity<4> () );
        trans_uni [ "NormMat" ].set_matrix ( glh::math::identity<3> () );
        glh::renderer::draw_elements ( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );

        window.swap_buffers ();

        std::this_thread::sleep_for ( std::chrono::milliseconds ( 1000 / 45 ) );

        window.poll_events ();
    }

    return 0;
}