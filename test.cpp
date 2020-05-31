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

#define GLH_MODEL_MAX_COLOR_SETS 1
#define GLH_MODEL_MAX_UV_CHANNELS 4
#define GLH_MODEL_MAX_TEXTURE_STACK_SIZE 2

/* include glhelper.hpp */
#include <glhelper/glhelper.hpp>



int main ()
{

    /* CREATE WINDOW */

    /* create a window with 4xMSAA */
    glh::glfw::window window { "Test Window", 600, 400, 4 };

    /* disable mouse */
    window.set_input_mode ( GLFW_CURSOR, GLFW_CURSOR_DISABLED );



    /* SET UP PROGRAM */

    /* create shader program */
    glh::core::vshader vshader { "shaders/vertex.model.glsl" };
    glh::core::fshader fshader { "shaders/fragment.model.glsl" };
    glh::core::program program { vshader, fshader };

    /* extract uniforms out of program */
    auto& trans_uni = program.get_struct_uniform ( "trans" );
    auto& light_system_uni = program.get_struct_uniform ( "light_system" );
    auto& material_uni = program.get_struct_uniform ( "material" );
    auto& transparent_mode_uni = program.get_uniform ( "transparent_mode" );



    /* SET UP CAMERA */

    /* create camera */
    glh::camera::camera camera { glh::math::rad ( 90.0 ), 16.0 / 9.0, 0.5, 200.0 };
    camera.set_position ( glh::math::vec3 { 0.0, 25.0, 25.0 } );

    /* restrict movement */
    camera.enable_restrictive_mode ();

    /* cache uniforms */
    camera.cache_uniforms ( trans_uni.get_uniform ( "view" ), trans_uni.get_uniform ( "proj" ) );

    /* movement per second of button hold
     * angle of rotation per side to side of window movement of mouse
     * the angle to change the fov per second of button hold
     */
    const double movement_sensitivity = 10;
    const double mouse_sensitivity = glh::math::rad ( 120.0 );
    const double fov_sensitivity = glh::math::rad ( 15.0 );



    /* IMPORT MODEL */

    /* import the model */
    glh::model::model island { "assets/island", "scene.gltf" };
    const glh::math::mat4 island_matrix =
    glh::math::enlarge3d
    (
        glh::math::identity<4, GLdouble> (),
        0.1
    );

    /* cache uniforms */
    island.cache_uniforms ( material_uni, trans_uni.get_uniform ( "model" ) );



    /* SET UP LIGHT SYSTEM */

    /* create light system */
    glh::lighting::light_system light_system;

    /* add directional light */
    light_system.dircoll.lights.emplace_back ( glh::lighting::dirlight
    {
        glh::math::vec3 { 0.0, -1.0, 0.0 },
        glh::math::vec3 { 1.0 }, 
        glh::math::vec3 { 1.5 }, 
        glh::math::vec3 { 1.0 }
    } );

    /* cache uniforms */
    light_system.cache_uniforms ( light_system_uni );



    /* SET UP RENDERER */

    glh::core::renderer::set_clear_color ( glh::math::vec4 { 1.0, 0.0, 1.0, 1.0 } );
    glh::core::renderer::enable_depth_test ();
    glh::core::renderer::enable_face_culling ();
    glh::core::renderer::enable_multisample ();
    glh::core::renderer::enable_blend ();
    glh::core::renderer::blend_func ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );



    /* RENDERING LOOP */

    for ( unsigned frame = 0; !window.should_close (); ++frame )
    {
        /* get window properties */
        auto dimensions = window.get_dimensions ();
        auto timeinfo = window.get_timeinfo ();
        auto mouseinfo = window.get_mouseinfo ();

        /* print framerate every 10th frame */
        if ( frame % 10 == 0 ) std::cout << "FPS: " << std::to_string ( 1.0 / timeinfo.delta ) << '\r' << std::flush;

        /* apply any viewport size changes */
        if ( dimensions.deltaheight || dimensions.deltawidth || frame % 15 == 0 )
        {
            camera.set_aspect ( ( double ) dimensions.width / dimensions.height );
            glh::core::renderer::viewport ( 0, 0, dimensions.width, dimensions.height );
        }

        /* get movement keys and apply changes to camera */
        if ( window.get_key ( GLFW_KEY_W ).action == GLFW_PRESS ) camera.move ( glh::math::vec3 { 0.0, 0.0, -movement_sensitivity * timeinfo.delta } );
        if ( window.get_key ( GLFW_KEY_A ).action == GLFW_PRESS ) camera.move ( glh::math::vec3 { -movement_sensitivity * timeinfo.delta, 0.0, 0.0 } );
        if ( window.get_key ( GLFW_KEY_S ).action == GLFW_PRESS ) camera.move ( glh::math::vec3 { 0.0, 0.0, movement_sensitivity * timeinfo.delta } );
        if ( window.get_key ( GLFW_KEY_D ).action == GLFW_PRESS ) camera.move ( glh::math::vec3 { movement_sensitivity * timeinfo.delta, 0.0, 0.0 } );
        if ( window.get_key ( GLFW_KEY_SPACE ).action == GLFW_PRESS ) camera.move ( glh::math::vec3 { 0.0, movement_sensitivity * timeinfo.delta, 0.0 } );
        if ( window.get_key ( GLFW_KEY_LEFT_SHIFT ).action == GLFW_PRESS ) camera.move ( glh::math::vec3 { 0.0, -movement_sensitivity * timeinfo.delta, 0.0 } );

        /* get mouse movement and apply changes to camera */
        camera.yaw ( mouse_sensitivity * -mouseinfo.deltaxfrac );
        camera.pitch ( mouse_sensitivity * -mouseinfo.deltayfrac );

        /* zoom keys */
        if ( window.get_key ( GLFW_KEY_Q ).action == GLFW_PRESS ) camera.set_fov ( camera.get_fov () + ( fov_sensitivity * timeinfo.delta ) );
        if ( window.get_key ( GLFW_KEY_E ).action == GLFW_PRESS ) camera.set_fov ( camera.get_fov () - ( fov_sensitivity * timeinfo.delta ) );



        /* apply camera and light system */
        camera.apply ();
        light_system.apply ();

        /* clear screen */
        glh::core::renderer::clear ();

        /* render opaque */
        transparent_mode_uni.set_int ( false );
        glh::core::renderer::disable_blend ();
        island.render ( program, island_matrix );

        /* render transparent */
        transparent_mode_uni.set_int ( true );
        glh::core::renderer::enable_blend ();
        island.render ( program, island_matrix, glh::model::render_flags::GLH_TRANSPARENT_MODE );



        /* swap buffers */
        window.swap_buffers ();

        /* poll events */
        window.poll_events ();

    }

    return 0;
}