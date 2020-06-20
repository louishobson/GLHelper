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
#define GLH_MODEL_MAX_UV_CHANNELS 2

/* include glhelper.hpp */
#include <glhelper/glhelper.hpp>



int main ()
{
    /* CREATE WINDOW */

    /* create a window with 4xMSAA */
    glh::glfw::window window { "Test Window", 600, 400, 4 };

    /* disable mouse */
    window.set_input_mode ( GLFW_CURSOR, GLFW_CURSOR_DISABLED );
    


    /* SET UP PROGRAMS */

    /* create model shader program */
    glh::core::vshader model_vshader { "shaders/vertex.model.glsl" };
    glh::core::fshader model_fshader { "shaders/fragment.model.glsl" };
    glh::core::program model_program { model_vshader, model_fshader };

    /* create shadow shadow program */
    glh::core::vshader shadow_vshader { "shaders/vertex.shadow.glsl" };
    glh::core::gshader shadow_gshader { "shaders/geometry.shadow.glsl" };
    glh::core::fshader shadow_fshader { "shaders/fragment.shadow.glsl" };
    glh::core::program shadow_program { shadow_vshader, shadow_gshader, shadow_fshader };

    /* extract uniforms out of model program */
    auto& model_trans_uni = model_program.get_struct_uniform ( "trans" );
    auto& model_light_system_uni = model_program.get_struct_uniform ( "light_system" );
    auto& model_material_uni = model_program.get_struct_uniform ( "material" );
    auto& model_transparent_mode_uni = model_program.get_uniform ( "transparent_mode" );

    /* extract uniforms out of model program */
    auto& shadow_model_matrix_uni = shadow_program.get_uniform ( "model_matrix" );
    auto& shadow_light_system_uni = shadow_program.get_struct_uniform ( "light_system" );



    /* SET UP CAMERA */

    /* create camera */
    glh::camera::camera_perspective_movement camera 
    { 
        glh::math::vec3 { 0.0, 25.0, 25.0 }, glh::math::vec3 { 0.0, 0.0, -1.0 }, glh::math::vec3 { 0.0, 1.0, 0.0 },
        glh::math::rad ( 90.0 ), 16.0 / 9.0, 0.5, 200.0 
    };

    /* restrict movement */
    camera.enable_restrictive_mode ();

    /* cache uniforms */
    camera.cache_uniforms ( model_trans_uni.get_uniform ( "view" ), model_trans_uni.get_uniform ( "proj" ) );

    /* movement per second of button hold
     * angle of rotation per side to side of window movement of mouse
     * the angle to change the fov per second of button hold
     * cutoff sensitivity for gamepads
     */
    const double movement_sensitivity = 10.0;
    const double mouse_sensitivity = glh::math::rad ( 120.0 );
    const double fov_sensitivity = glh::math::rad ( 15.0 );
    const double gamepad_cutoff_sensitivity = 0.2;
    const double gamepad_look_sensitivity = glh::math::rad ( 120.0 );
    


    /* IMPORT MODELS */

    /* import island model */
    glh::model::model island { "assets/island", "scene.gltf", 
        glh::model::import_flags::GLH_CONFIGURE_REGIONS_ACCURATE |
        glh::model::import_flags::GLH_CONFIGURE_ONLY_ROOT_NODE_REGION
    };
    const glh::math::mat4 island_matrix =
    glh::math::enlarge3d
    (
        glh::math::identity<4, GLdouble> (),
        0.1
    );
    
    /* cache material uniform */
    island.cache_material_uniforms ( model_material_uni );



    /* SET UP LIGHT SYSTEM */

    /* create light system */
    glh::lighting::light_system light_system;

    /* add directional light */
    light_system.add_dirlight
    (
        glh::math::vec3 { 1.0, -1.0, 0.0 },
        glh::math::vec3 { 1.0 },
        glh::math::vec3 { 1.0 }, 
        glh::math::vec3 { 1.0 },
        island.model_region ( island_matrix )
    );

    /* cache uniforms */
    //light_system.cache_uniforms ( model_light_system_uni );



    /* SET UP RENDERER */

    glh::core::renderer::set_clear_color ( glh::math::vec4 { 0.0, 0.5, 1.0, 1.0 } );
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
        auto gamepadinfo = window.get_gamepadinfo ( GLFW_JOYSTICK_1 );

        /* print framerate every 10th frame */
        //if ( frame % 10 == 0 ) std::cout << "FPS: " << std::to_string ( 1.0 / timeinfo.delta ) << '\r' << std::flush;
        
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
        if ( window.get_key ( GLFW_KEY_SPACE ).action == GLFW_PRESS || gamepadinfo.button_a == GLFW_PRESS ) camera.move ( glh::math::vec3 { 0.0, movement_sensitivity * timeinfo.delta, 0.0 } );
        if ( window.get_key ( GLFW_KEY_LEFT_SHIFT ).action == GLFW_PRESS ||gamepadinfo.button_b == GLFW_PRESS ) camera.move ( glh::math::vec3 { 0.0, -movement_sensitivity * timeinfo.delta, 0.0 } );

        /* apply joystick movement */
        if ( std::abs ( gamepadinfo.axis_lh_y ) > gamepad_cutoff_sensitivity ) 
            camera.move ( glh::math::vec3 { 0.0, 0.0, movement_sensitivity * gamepadinfo.axis_lh_y * timeinfo.delta } );
        if ( std::abs ( gamepadinfo.axis_lh_x ) > gamepad_cutoff_sensitivity ) 
            camera.move ( glh::math::vec3 { movement_sensitivity * gamepadinfo.axis_lh_x * timeinfo.delta, 0.0, 0.0 } );
        if ( std::abs ( gamepadinfo.axis_rh_x ) > gamepad_cutoff_sensitivity )
            camera.yaw ( -gamepad_look_sensitivity * gamepadinfo.axis_rh_x * timeinfo.delta );
        if ( std::abs ( gamepadinfo.axis_rh_y ) > gamepad_cutoff_sensitivity ) 
            camera.pitch ( -gamepad_look_sensitivity * gamepadinfo.axis_rh_y * timeinfo.delta );

        /* get mouse movement and apply changes to camera */
        camera.yaw ( -mouse_sensitivity * mouseinfo.deltaxfrac );
        camera.pitch ( -mouse_sensitivity * mouseinfo.deltayfrac );

        /* zoom keys */
        if ( window.get_key ( GLFW_KEY_Q ).action == GLFW_PRESS ) camera.set_fov ( camera.get_fov () + ( fov_sensitivity * timeinfo.delta ) );
        if ( window.get_key ( GLFW_KEY_E ).action == GLFW_PRESS ) camera.set_fov ( camera.get_fov () - ( fov_sensitivity * timeinfo.delta ) );
        


        /* first create shadow maps */

        /* bind the 2d shadow fbo */
        light_system.bind_shadow_maps_2d_fbo ();

        /* clear depth buffer */
        glh::core::renderer::clear ( GL_DEPTH_BUFFER_BIT );

        /* use the shadow program */
        shadow_program.use ();

        /* apply light system and cache model matrix uniform */
        light_system.apply ( shadow_light_system_uni );
        island.cache_model_uniform ( shadow_model_matrix_uni );

        /* render the model */
        island.render ( island_matrix, glh::model::render_flags::GLH_NO_MATERIAL );



        /* now render the island to the default framebuffer */

        /* bind the default framebuffer */
        window.bind_framebuffer ();

        /* clear screen */
        glh::core::renderer::clear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        /* use the model program */
        model_program.use ();

        /* apply camera and light system and cache model matrix uniform */
        camera.apply ();
        light_system.apply ( model_light_system_uni );
        island.cache_model_uniform ( model_trans_uni.get_uniform ( "model" ) );

        /* render opaque */
        model_transparent_mode_uni.set_int ( false );
        glh::core::renderer::disable_blend ();
        island.render ( island_matrix );
       
        /* render transparent */
        model_transparent_mode_uni.set_int ( true );
        glh::core::renderer::enable_blend ();
        island.render ( island_matrix, glh::model::render_flags::GLH_TRANSPARENT_MODE );
        


        /* swap buffers */
        window.swap_buffers ();

        /* poll events */
        window.poll_events ();

    }

    return 0;
}