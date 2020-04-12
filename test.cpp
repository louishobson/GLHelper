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
    glh::window window;
    window.set_input_mode ( GLFW_CURSOR, GLFW_CURSOR_DISABLED );

    glh::vshader vshader { "shaders/vertex.glsl" };
    glh::fshader fshader { "shaders/fragment.glsl" };
    glh::program program { vshader, fshader };
    program.use ();
    auto trans_uni = program.get_struct_uniform ( "trans" );
    auto transparent_mode_uni = program.get_uniform ( "transparent_mode" );
    
    glh::camera_perspective camera { glh::math::rad ( 90 ), 16.0 / 9.0, 0.1, 500.0 };
    camera.enable_restrictive_mode ();

    glh::light_system light_system;
    light_system.dircoll.lights.emplace_back ( glh::math::vec3 { 0.0, -1.0, 0.0 }, glh::math::vec3 { 0.2 }, glh::math::vec3 { 0.8 }, glh::math::vec3 { 1.0 } );
    light_system.cache_uniforms ( program.get_struct_uniform ( "light_system" ) );

    window.get_mouseinfo ();
    auto dimensions = window.get_dimensions ();
    camera.set_aspect ( ( double ) dimensions.width / dimensions.height );
    trans_uni.get_uniform ( "proj" ).set_matrix ( camera.get_proj () );
    glh::renderer::viewport ( 0, 0, dimensions.width, dimensions.height );

    glh::model::model factory { "./assets/factory", "scene.gltf" };
    factory.cache_uniforms ( program.get_struct_uniform ( "material" ), trans_uni.get_uniform ( "model" ) );

    glh::math::mat4 model_matrix = glh::math::translate ( glh::math::resize<4> ( glh::math::rotate ( glh::math::enlarge ( glh::math::identity<3> (), 0.1 ), glh::math::rad ( 90 ), 1, 2 ) ), glh::math::vec3 { -20.0, 0.0, -20.0 } );

    glh::renderer::clear_color ( 1.0, 1.0, 1.0, 1.0 );
    glh::renderer::enable_depth_test ();
    glh::renderer::blend_func ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    while ( !window.should_close () ) 
    {
        auto timeinfo = window.get_timeinfo ();
        auto dimensions = window.get_dimensions ();
        auto mouseinfo = window.get_mouseinfo ();

        if ( dimensions.deltawidth != 0.0 || dimensions.deltaheight != 0.0 ) 
        {
            camera.set_aspect ( ( double ) dimensions.width / dimensions.height );
            trans_uni.get_uniform ( "proj" ).set_matrix ( camera.get_proj () );
            glh::renderer::viewport ( 0, 0, dimensions.width, dimensions.height );
        }


        if ( window.get_key ( GLFW_KEY_W ).action == GLFW_PRESS ) camera.move ( timeinfo.delta * glh::math::vec3 { 0., 0., -15. } );
        if ( window.get_key ( GLFW_KEY_A ).action == GLFW_PRESS ) camera.move ( timeinfo.delta * glh::math::vec3 { -15, 0., 0. } );
        if ( window.get_key ( GLFW_KEY_S ).action == GLFW_PRESS ) camera.move ( timeinfo.delta * glh::math::vec3 { 0., 0., 15. } );
        if ( window.get_key ( GLFW_KEY_D ).action == GLFW_PRESS ) camera.move ( timeinfo.delta * glh::math::vec3 { 15, 0., 0. } );

        if ( window.get_key ( GLFW_KEY_SPACE ).action == GLFW_PRESS ) camera.move ( timeinfo.delta * glh::math::vec3 { 0., 15., 0. } );
        if ( window.get_key ( GLFW_KEY_LEFT_SHIFT ).action == GLFW_PRESS ) camera.move ( timeinfo.delta * glh::math::vec3 { 0., -15., 0. } );

        if ( window.get_key ( GLFW_KEY_Z ).action == GLFW_PRESS ) camera.roll ( timeinfo.delta * glh::math::rad ( 80 ) );
        if ( window.get_key ( GLFW_KEY_X ).action == GLFW_PRESS ) camera.roll ( timeinfo.delta * glh::math::rad ( -80 ) ); 

        camera.pitch ( mouseinfo.deltayfrac * glh::math::rad ( -80 ) );
        camera.yaw ( mouseinfo.deltaxfrac * glh::math::rad ( -80 ) );

        trans_uni.get_uniform ( "view" ).set_matrix ( camera.get_view () );
        trans_uni.get_uniform ( "viewpos" ).set_vector ( camera.get_position () );
       
        light_system.apply ();

        transparent_mode_uni.set_int ( 0 );
        glh::renderer::disable_blend ();
        glh::renderer::depth_mask ( GL_TRUE );
        glh::renderer::clear ();
        
        factory.render ( model_matrix, false );

        glh::renderer::enable_blend ();
        glh::renderer::depth_mask ( GL_FALSE );
        transparent_mode_uni.set_int ( 1 );

        factory.render ( model_matrix, true );

        window.swap_buffers ();
        window.poll_events ();
    }

    return 0;
}