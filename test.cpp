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
    auto viewpos_uni = program.get_uniform ( "ViewPos" );
    auto material_uni = program.get_struct_uniform ( "Material" );
    auto pointlights_uni = program.get_array_uniform<glh::struct_uniform> ( "PointLights" );
    auto dirlight_uni = program.get_struct_uniform ( "DirLight" );
    auto trans_uni = program.get_struct_uniform ( "Trans" );

    glh::texture2d cratedifftex { "assets/container_diff.png", GL_TEXTURE0 };
    glh::texture2d cratespectex { "assets/container_spec.png", GL_TEXTURE1 };
    glh::texture2d floordifftex { "assets/grass.png", GL_TEXTURE0 };
    glh::texture2d floorspectex { "assets/black.png", GL_TEXTURE1 };
    floordifftex.set_wrap ( GL_REPEAT );
    floorspectex.set_wrap ( GL_REPEAT );

    glh::math::mat4 model;
    glh::camera_perspective camera { glh::math::rad ( 75 ), 16.0 / 9.0, 0.1, 1000 };
    camera.move ( glh::math::vec3 { 0.0, 10.0, 0.0 } );
    camera.enable_restrictive_mode ();
    
    glh::renderer::clear_colour ( 0.0, 0.0, 0.0, 1.0 );
    glh::renderer::enable_depth_test ();

    program.use ();
    trans_uni.get_uniform ( "Proj" ).set_matrix ( camera.get_proj () );
    material_uni.get_uniform ( "Diffuse" ).set_int ( 0 );
    material_uni.get_uniform ( "Specular" ).set_int ( 1 );
    for ( unsigned i = 0; i < 2; ++i )
    {
        pointlights_uni.at ( i ).get_struct_uniform ( "Attenuation" ).get_uniform ( "Constant" ).set_float ( 1.0 );
        pointlights_uni.at ( i ).get_struct_uniform ( "Attenuation" ).get_uniform ( "Linear" ).set_float ( 0.045 );
        pointlights_uni.at ( i ).get_struct_uniform ( "Attenuation" ).get_uniform ( "Quadratic" ).set_float ( 0.0075 );
        pointlights_uni.at ( i ).get_struct_uniform ( "Properties" ).get_uniform ( "Ambient" ).set_vector ( glh::math::vec3 { 0.0, 0.0, 0.0 } );
        pointlights_uni.at ( i ).get_struct_uniform ( "Properties" ).get_uniform ( "Diffuse" ).set_vector ( glh::math::vec3 { 0.7, 0.7, 0.7 } );
        pointlights_uni.at ( i ).get_struct_uniform ( "Properties" ).get_uniform ( "Specular" ).set_vector ( glh::math::vec3 { 1.0, 1.0, 1.0 } );
    }
    dirlight_uni.get_uniform ( "Direction" ).set_vector ( glh::math::norm ( glh::math::vec3 { 0.0, -1.0, 0.0 } ) );
    dirlight_uni.get_struct_uniform ( "Properties" ).get_uniform ( "Ambient" ).set_vector ( glh::math::vec3 { 0.05, 0.05, 0.05 } );
    dirlight_uni.get_struct_uniform ( "Properties" ).get_uniform ( "Diffuse" ).set_vector ( glh::math::vec3 { 0.1, 0.1, 0.1 } );
    dirlight_uni.get_struct_uniform ( "Properties" ).get_uniform ( "Specular" ).set_vector ( glh::math::vec3 { 0.3, 0.3, 0.3 } );

    while ( !window.should_close () ) 
    {
        auto timeinfo = window.get_timeinfo ();
        auto dimensions = window.get_dimensions ();
        auto mouseinfo = window.get_mouseinfo ();

        if ( dimensions.deltawidth != 0.0 || dimensions.deltaheight != 0.0 ) 
        {
            camera.set_aspect ( ( double ) dimensions.width / dimensions.height );
            trans_uni.get_uniform ( "Proj" ).set_matrix ( camera.get_proj () );
            glh::renderer::viewport ( 0, 0, dimensions.width, dimensions.height );
        }

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
        
        trans_uni.get_uniform ( "View" ).set_matrix ( camera.get_view () );
        viewpos_uni.set_vector ( camera.get_pos () );
        pointlights_uni.at ( 0 ).get_uniform ( "Position" ).set_vector ( glh::math::rotate ( glh::math::vec3 { 7.0, 2.0, -5.0 } * scale, glh::math::rad ( timeinfo.now * 30 ), glh::math::vec3 { 0.0, 1.0, 0.0 } ) );
        pointlights_uni.at ( 1 ).get_uniform ( "Position" ).set_vector ( glh::math::rotate ( glh::math::vec3 { 7.0, 2.0, -5.0 } * scale, - glh::math::rad ( timeinfo.now * 30 ), glh::math::vec3 { 0.0, 1.0, 0.0 } ) );
        
        glh::renderer::clear ();

        cratevao.bind ();
        cratedifftex.bind ();
        cratespectex.bind ();
        material_uni.get_uniform ( "Shininess" ).set_float ( 64 );
        for ( auto vec: cratepos )
        {
            glh::math::mat4 model = glh::math::translate ( glh::math::resize<4> ( glh::math::enlarge ( glh::math::identity<3> (), scale ) ), vec * scale );
            trans_uni.get_uniform ( "Model" ).set_matrix ( model );
            trans_uni.get_uniform ( "NormMat" ).set_matrix ( glh::math::transpose ( glh::math::inverse ( glh::math::resize<3> ( model ) ) ) );
            glh::renderer::draw_arrays ( GL_TRIANGLES, 0, 6 * 6 );
        }

        floorvao.bind ();
        floordifftex.bind ();
        floorspectex.bind ();
        material_uni.get_uniform ( "Shininess" ).set_float ( 2 );
        trans_uni.get_uniform ( "Model" ).set_matrix ( glh::math::identity<4> () );
        trans_uni.get_uniform ( "NormMat" ).set_matrix ( glh::math::identity<3> () );
        glh::renderer::draw_elements ( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );

        window.swap_buffers ();        
        window.poll_events ();
    }

    return 0;
}