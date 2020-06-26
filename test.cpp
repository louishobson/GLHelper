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

void GLAPIENTRY
MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam )
{
  fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );
}

int main ()
{
    /* CREATE WINDOW */

    /* create a window with 4xMSAA */
    glh::glfw::window window { "Test Window", 600, 400 };

    /* disable mouse */
    window.set_input_mode ( GLFW_CURSOR, GLFW_CURSOR_DISABLED );
    
    
    
    // During init, enable debug output
    glEnable              ( GL_DEBUG_OUTPUT );
    glDebugMessageCallback( MessageCallback, 0 );



    /* SET UP PROGRAMS */

    /* create model shader program */
    glh::core::vshader model_vshader { "shaders/materials.glsl", "shaders/camera.glsl", "shaders/vertex.model.glsl" };
    glh::core::fshader model_fshader { "shaders/materials.glsl", "shaders/camera.glsl", "shaders/lighting.glsl", "shaders/fragment.model.glsl"  };
    glh::core::program model_program { model_vshader, model_fshader };
    model_program.compile_and_link ();

    /* create shadow shadow program */
    glh::core::vshader shadow_vshader { "shaders/materials.glsl", "shaders/vertex.shadow.glsl" };
    glh::core::gshader shadow_gshader { "shaders/materials.glsl", "shaders/camera.glsl", "shaders/lighting.glsl", "shaders/geometry.shadow.glsl" };
    glh::core::fshader shadow_fshader { "shaders/materials.glsl", "shaders/fragment.shadow.glsl" };
    glh::core::program shadow_program { shadow_vshader, shadow_gshader, shadow_fshader };
    shadow_program.compile_and_link ();

    /* extract uniforms out of model program */
    auto& model_camera_uni = model_program.get_struct_uniform ( "camera" );
    auto& model_model_matrix_uni = model_program.get_uniform ( "model_matrix" );
    auto& model_light_system_uni = model_program.get_struct_uniform ( "light_system" );
    auto& model_material_uni = model_program.get_struct_uniform ( "material" );
    auto& model_transparent_mode_uni = model_program.get_uniform ( "transparent_mode" );

    /* extract uniforms out of model program */
    auto& shadow_model_matrix_uni = shadow_program.get_uniform ( "model_matrix" );
    auto& shadow_light_system_uni = shadow_program.get_struct_uniform ( "light_system" );
    auto& shadow_material_uni = shadow_program.get_struct_uniform ( "material" );



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
    camera.cache_uniforms ( model_camera_uni );



    /* SET SENSITIVITIES */

    /* movement per second of button hold
     * angle of rotation per side to side of window movement of mouse
     * the angle to change the fov per second of button hold
     * cutoff sensitivity for gamepads
     * light rotation sensitivity
     */
    const double movement_sensitivity = 10.0;
    const double mouse_sensitivity = glh::math::rad ( 120.0 );
    const double fov_sensitivity = glh::math::rad ( 15.0 );
    const double gamepad_cutoff_sensitivity = 0.2;
    const double gamepad_look_sensitivity = glh::math::rad ( 120.0 );
    const double light_rotation_sensitivity = glh::math::rad ( 10 );



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



    /* SET UP LIGHT SYSTEM */

    /* create light system */
    glh::lighting::light_system light_system { 1750 };

    /* add directional light */
    light_system.add_dirlight
    (
        glh::math::vec3 { 1.0, -1.0, 0.0 },
        glh::math::vec3 { 0.0 },
        glh::math::vec3 { 0.5 }, 
        glh::math::vec3 { 0.5 },
        island.model_region ( island_matrix ),
        true, false, 0.035
    );

    /* add point light */
    light_system.add_pointlight
    (
        glh::math::vec3 ( 30, 40, 20 ), 1.0, 0.0, 0.0,//0.007, 0.0002,
        glh::math::vec3 { 0.2 },
        glh::math::vec3 { 1.5 }, 
        glh::math::vec3 { 1.0 },
        island.model_region ( island_matrix ),
        true, true, 0.005
    );

    /* cache uniforms */
    //light_system.cache_uniforms ( model_light_system_uni );



    /* SET UP RENDERER */

    glh::core::renderer::set_clear_color ( glh::math::vec4 { 0.0, 0.5, 1.0, 1.0 } );
    glh::core::renderer::enable_depth_test ();
    glh::core::renderer::enable_face_culling ();
    glh::core::renderer::set_cull_face ( GL_BACK );
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
        if ( frame % 10 == 0 ) std::cout << "FPS: " << std::to_string ( 1.0 / timeinfo.delta ) << '\r' << std::flush;



        /* apply aspect change to camera */
        if ( dimensions.deltaheight || dimensions.deltawidth || frame % 15 == 0 )
            camera.set_aspect ( ( double ) dimensions.width / dimensions.height );

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
        


        /* rotate light */
        light_system.pointlight_at ( 0 ).set_position ( glh::math::rotate3d ( light_system.pointlight_at ( 0 ).get_position (), light_rotation_sensitivity * timeinfo.delta, glh::math::vec3 { 0.0, 1.0, 0.0 } ) );



        /* first create shadow maps */

        /* bind the shadow fbo */
        light_system.bind_shadow_maps_fbo ();

        /* use the shadow program */
        shadow_program.use ();

        /* apply light system and cache model matrix uniform */
        light_system.apply ( shadow_light_system_uni );

        /* render the model */
        glh::core::renderer::disable_blend ();
        glh::core::renderer::set_depth_mask ( GL_TRUE );
        glh::core::renderer::clear ( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );
        island.render ( shadow_material_uni, shadow_model_matrix_uni, island_matrix );



        /* now render the island to the default framebuffer */

        /* bind the default framebuffer and resize the viewport */
        window.bind_framebuffer ();
        glh::core::renderer::viewport ( 0, 0, dimensions.width, dimensions.height );        

        /* use the model program */
        model_program.use ();

        /* apply camera and light system and cache model matrix uniform */
        camera.apply ();
        light_system.apply ( model_light_system_uni );

        /* render opaque */
        model_transparent_mode_uni.set_int ( 2 );
        glh::core::renderer::disable_blend ();
        glh::core::renderer::set_depth_mask ( GL_TRUE );
        glh::core::renderer::clear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        island.render ( model_material_uni, model_model_matrix_uni, island_matrix );
       
        /* render transparent */
        model_transparent_mode_uni.set_int ( 1 );
        glh::core::renderer::enable_blend ();
        glh::core::renderer::set_depth_mask ( GL_FALSE );
        island.render ( island_matrix, glh::model::render_flags::GLH_TRANSPARENT_MODE );
        


        /* swap buffers */
        window.swap_buffers ();

        /* poll events */
        window.poll_events ();

    }

    return 0;
}