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



/* MODEL_SWITCH
 *
 * defines which model is rendered
 */
#define MODEL_SWITCH island



int main ()
{
    /* CREATE WINDOW */

    /* create a window */
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

    /* create bloom and post-process shader programs */
    glh::core::vshader simple_vshader { "shaders/vertex.simple.glsl" };
    glh::core::fshader bloom_fshader { "shaders/function.glsl", "shaders/fragment.bloom.glsl" };
    glh::core::program bloom_program { simple_vshader, bloom_fshader };
    bloom_program.compile_and_link ();

    /* extract uniforms out of model program */
    auto& model_camera_uni = model_program.get_struct_uniform ( "camera" );
    auto& model_light_system_uni = model_program.get_struct_uniform ( "light_system" );
    auto& model_material_uni = model_program.get_struct_uniform ( "material" );
    auto& model_transparent_mode_uni = model_program.get_uniform ( "transparent_mode" );

    /* extract uniforms out of model program */
    auto& shadow_light_system_uni = shadow_program.get_struct_uniform ( "light_system" );
    auto& shadow_material_uni = shadow_program.get_struct_uniform ( "material" );

    /* extract uniforms out of bloom program */
    auto& bloom_texture_uni = bloom_program.get_uniform ( "bloom_texture" );
    auto& bloom_bloom_mode_uni = bloom_program.get_uniform ( "bloom_mode" );
    auto& bloom_function_uni = bloom_program.get_struct_uniform ( "bloom_function" );
    auto& bloom_radius_uni = bloom_program.get_uniform ( "bloom_radius" );



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
    const double mouse_sensitivity = glh::math::rad ( 0.1 );
    const double fov_sensitivity = glh::math::rad ( 15.0 );
    const double gamepad_cutoff_sensitivity = 0.2;
    const double gamepad_look_sensitivity = glh::math::rad ( 120.0 );
    const double light_rotation_sensitivity = glh::math::rad ( 10 );



    /* IMPORT MODELS */

    /* import island model */
    const glh::math::mat4 island_matrix =
    glh::math::enlarge3d
    (
        glh::math::identity<4, double> (),
        0.1
    );
    glh::model::model island { "assets/island", "scene.gltf", 
        glh::model::import_flags::GLH_CONFIGURE_REGIONS_ACCURATE |
        glh::model::import_flags::GLH_CONFIGURE_ONLY_ROOT_NODE_REGION |
        glh::model::import_flags::GLH_FLIP_V_TEXTURES |
        glh::model::import_flags::GLH_PRETRANSFORM_VERTICES,
        island_matrix
    };

    /* import box model */
    const glh::math::mat4 box_matrix =
    glh::math::enlarge3d
    (
        glh::math::identity<4, double> (),
        15.0
    );
    glh::model::model box { "assets/box", "scene.gltf", 
        glh::model::import_flags::GLH_CONFIGURE_REGIONS_ACCURATE |
        glh::model::import_flags::GLH_CONFIGURE_ONLY_ROOT_NODE_REGION |
        glh::model::import_flags::GLH_FLIP_V_TEXTURES |
        glh::model::import_flags::GLH_PRETRANSFORM_VERTICES,
        box_matrix
    };



    /* SET UP LIGHT SYSTEM */

    /* create light system */
    glh::lighting::light_system light_system { 4096 };

    /* add directional light */
    light_system.add_dirlight
    (
        glh::math::vec3 { 1.0, -0.5, 0.0 },
        glh::math::vec3 { 0.8 },
        glh::math::vec3 { 1.0 }, 
        glh::math::vec3 { 1.0 },
        island.model_region (),
        false, true, 0.007, //0.035
        16, 0.001
    );

    /* add point light */
    light_system.add_pointlight
    (
        glh::math::vec3 ( 30, 40, 20 ), 1.0, 0.0, 0.0,
        //glh::math::vec3 ( 20, 0, 20 ), 1.0, 0.0, 0.0,
        glh::math::vec3 { 0.1 },
        glh::math::vec3 { 1.0 }, 
        glh::math::vec3 { 1.0 },
        island.model_region (),
        true, true, 0.003,
        24, 2.5 / 4096.0
    );

    /* add spotlights */
    light_system.add_spotlight
    (
        glh::math::vec3 ( 30, 50, 20 ), -glh::math::vec3 ( 30, 50, 20 ),
        glh::math::rad ( 30 ), glh::math::rad ( 50 ), 1.0, 0.0, 0.0,
        glh::math::vec3 { 0.6 },
        glh::math::vec3 { 1.0 }, 
        glh::math::vec3 { 1.0 },
        island.model_region (),
        false, true, 0.007,
        16, 0.0005
    );



    /* SET UP FRAMEBUFFERS */

    /* define the number of samples */
    const unsigned num_samples = 4;

    /* create main and emission color textures */
    glh::core::texture2d_multisample main_color_texture;
    glh::core::texture2d_multisample emission_color_texture;

    main_color_texture.tex_storage ( 1920, 1080, num_samples, GL_RGBA8 ); 
    emission_color_texture.tex_storage ( 1920, 1080, num_samples, GL_RGBA8 );

    /* create the main depth attachment */
    glh::core::rbo main_depth_attachment { 1920, 1080, GL_DEPTH_COMPONENT, num_samples };

    /* create main framebuffer and attach buffers */
    glh::core::fbo main_fbo;
    main_fbo.attach_texture ( main_color_texture, GL_COLOR_ATTACHMENT0 );
    main_fbo.attach_texture ( emission_color_texture, GL_COLOR_ATTACHMENT1 );
    main_fbo.attach_rbo ( main_depth_attachment, GL_DEPTH_ATTACHMENT );
    main_fbo.draw_buffers ( GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 );

    /* create ping-pong textures */
    glh::core::texture2d ping_pong_texture_alpha;
    glh::core::texture2d ping_pong_texture_beta;
    ping_pong_texture_alpha.tex_storage ( 1920, 1080, GL_RGBA8 );
    ping_pong_texture_beta.tex_storage ( 1920, 1080, GL_RGBA8 );
    ping_pong_texture_alpha.set_min_filter ( GL_LINEAR ); ping_pong_texture_alpha.set_mag_filter ( GL_LINEAR );
    ping_pong_texture_beta.set_min_filter ( GL_LINEAR ); ping_pong_texture_beta.set_mag_filter ( GL_LINEAR );
    ping_pong_texture_alpha.set_wrap ( GL_CLAMP_TO_EDGE );
    ping_pong_texture_beta.set_wrap ( GL_CLAMP_TO_EDGE );

    /* create ping-pong framebuffers and attach buffers */
    glh::core::fbo ping_pong_fbo_alpha;
    glh::core::fbo ping_pong_fbo_beta;
    ping_pong_fbo_alpha.attach_texture ( ping_pong_texture_alpha, GL_COLOR_ATTACHMENT0 );
    ping_pong_fbo_beta.attach_texture ( ping_pong_texture_beta, GL_COLOR_ATTACHMENT0 );



    /* CREATE QUAD VAO */
    
    /* quad vbo */
    glh::core::vbo quad_vbo;
    quad_vbo.buffer_storage ( glh::vertices::square_vertex_data.begin (), glh::vertices::square_vertex_data.end () );

    /* quad vao */
    glh::core::vao quad_vao;
    quad_vao.set_vertex_attrib ( 0, quad_vbo, 3, GL_FLOAT, GL_FALSE, 3 * sizeof ( GLfloat ), 0 );



    /* SET UP BLOOM FUNCTION */

    /* create the bloom function
     * the coeficient is such that the area underneath the bell curve will always be one for the rms supplied
     * this will ensure that the emission map does not get brighter as it is blurred by the function
     */
    const double bloom_func_rms = 1.0;
    const double bloom_func_coef = 1.00 / ( std::sqrt ( glh::math::pi ( 2.0 ) ) * bloom_func_rms );
    glh::function::gaussian_function<double, double> bloom_func { bloom_func_coef, 0.0, bloom_func_rms };
    glh::function::glsl_function<1> glsl_bloom_func { bloom_func, 1024, -5.0, 5.0 };
    glsl_bloom_func.cache_uniforms ( bloom_function_uni );

    /* set parameters for bloom */
    const unsigned bloom_radius = 3;
    const unsigned bloom_iterations = 1;



    /* SET UP RENDERER */

    glh::core::renderer::set_clear_color ( glh::math::vec4 { 0.0, 0.0, 0.0, 1.0 } );
    glh::core::renderer::enable_face_culling ();
    glh::core::renderer::set_cull_face ( GL_BACK );
    glh::core::renderer::enable_multisample ();
    glh::core::renderer::enable_framebuffer_srgb ();



    /* RENDERING LOOP */

    for ( unsigned frame = 0; !window.should_close (); ++frame )
    {
        /* GET START TIME */
        const auto timestamp_start = std::chrono::system_clock::now ();

        /* GET PROPERTIES */

        /* get window properties */
        auto timeinfo = window.get_timeinfo ();
        auto mouseinfo = window.get_mouseinfo ();
        auto gamepadinfo = window.get_gamepadinfo ( GLFW_JOYSTICK_1 );

        /* set timestamp */
        const auto timestamp_window_properties = std::chrono::system_clock::now ();



        /* MOVE CAMERAS AND LIGHTS */

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
        camera.yaw ( -mouse_sensitivity * mouseinfo.deltaxpos );
        camera.pitch ( -mouse_sensitivity * mouseinfo.deltaypos );

        /* zoom keys */
        if ( window.get_key ( GLFW_KEY_Q ).action == GLFW_PRESS ) camera.set_fov ( camera.get_fov () + ( fov_sensitivity * timeinfo.delta ) );
        if ( window.get_key ( GLFW_KEY_E ).action == GLFW_PRESS ) camera.set_fov ( camera.get_fov () - ( fov_sensitivity * timeinfo.delta ) );
        
        /* rotate light */
        light_system.pointlight_at ( 0 ).set_position ( glh::math::rotate3d ( light_system.pointlight_at ( 0 ).get_position (), light_rotation_sensitivity * timeinfo.delta, glh::math::vec3 { 0.0, 1.0, 0.0 } ) );        

        /* set timestamp */
        const auto timestamp_movement = std::chrono::system_clock::now ();



        /* CREATE SHADOW MAPS */

        /* use the shadow program */
        shadow_program.use ();

        /* apply light system and cache model matrix uniform */
        light_system.apply ( shadow_light_system_uni );

        /* prepare for rendering  */
        glh::core::renderer::disable_blend ();
        glh::core::renderer::blend_func ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        glh::core::renderer::enable_depth_test ();
        glh::core::renderer::set_depth_mask ( GL_TRUE );
        glh::core::renderer::viewport ( 0, 0, light_system.get_shadow_map_width (), light_system.get_shadow_map_width () );

        /* create shadow maps */
        if ( light_system.requires_shadow_mapping () )
        {
            light_system.bind_shadow_maps_fbo ();
            glh::core::renderer::clear ( GL_DEPTH_BUFFER_BIT );
            MODEL_SWITCH.cache_material_uniforms ( shadow_material_uni );
            MODEL_SWITCH.render ( glh::model::render_flags::GLH_NO_MODEL_MATRIX );
        }

        /* set timestamp */
        const auto timestamp_shadow_maps = std::chrono::system_clock::now ();



        /* RENDER MODELS TO THE MAIN FRAMEBUFFER */

        /* bind the main framebuffer and resize the viewport */
        main_fbo.bind ();
        glh::core::renderer::viewport ( 0, 0, 1920, 1080 );        

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
        MODEL_SWITCH.cache_material_uniforms ( model_material_uni );
        MODEL_SWITCH.render ( glh::model::render_flags::GLH_NO_MODEL_MATRIX );
       
        /* render transparent */
        model_transparent_mode_uni.set_int ( 1 );
        glh::core::renderer::enable_blend ();
        glh::core::renderer::set_depth_mask ( GL_FALSE );
        MODEL_SWITCH.render ( glh::model::render_flags::GLH_TRANSPARENT_MODE | glh::model::render_flags::GLH_NO_MODEL_MATRIX );

        /* set timestamp */
        const auto timestamp_main_render = std::chrono::system_clock::now ();



        /* APPLY BLOOM */

        /* blit emission color texture into beta pingpong fbo */
        main_fbo.read_buffer ( GL_COLOR_ATTACHMENT1 );
        main_fbo.blit_copy ( ping_pong_fbo_beta, 0, 0, 1920, 1080, 0, 0, 1920, 1080, GL_COLOR_BUFFER_BIT, GL_LINEAR );

        /* use bloom program */
        bloom_program.use ();

        /* apply uniforms */
        glsl_bloom_func.apply ();
        bloom_radius_uni.set_int ( bloom_radius );        

        /* prepare for rendering */
        glh::core::renderer::disable_blend ();
        glh::core::renderer::disable_depth_test ();

        /* bloom loop */
        for ( unsigned i = 0; i < bloom_iterations; ++i )
        {
            /* bind alpha fbo, set remaining uniforms and render */
            ping_pong_fbo_alpha.bind ();
            bloom_texture_uni.set_int ( ping_pong_texture_beta.bind_loop () );
            bloom_bloom_mode_uni.set_int ( 0 );
            glh::core::renderer::draw_arrays ( quad_vao, GL_TRIANGLE_STRIP, 0, 4 );

            /* if not the final iteration, render to beta */
            if ( i != bloom_iterations - 1 )
            {
                /* bind alpha fbo, set remaining uniforms and render */
                ping_pong_fbo_beta.bind ();
                bloom_texture_uni.set_int ( ping_pong_texture_alpha.bind_loop () );
                bloom_bloom_mode_uni.set_int ( 1 );
                glh::core::renderer::draw_arrays ( quad_vao, GL_TRIANGLE_STRIP, 0, 4 );
            } else
            /* else blend to default framebuffer */
            {
                /* bind default framebuffer, enable additive blending and clear it */
                window.bind_framebuffer ();
                glh::core::renderer::enable_blend ();
                glh::core::renderer::blend_func ( GL_ONE, GL_ONE );
                glh::core::renderer::clear ( GL_COLOR_BUFFER_BIT );

                /* blit the main color fbo into the default fbo */
                main_fbo.read_buffer ( GL_COLOR_ATTACHMENT0 );
                main_fbo.blit_copy_to_default ( 0, 0, 1920, 1080, 0, 0, 1920, 1080, GL_COLOR_BUFFER_BIT, GL_LINEAR );

                /* render into the default framebuffer */
                bloom_texture_uni.set_int ( ping_pong_texture_alpha.bind_loop () );
                bloom_bloom_mode_uni.set_int ( 1 );
                glh::core::renderer::draw_arrays ( quad_vao, GL_TRIANGLE_STRIP, 0, 4 );
            }
        }

        /* set timestamp */
        const auto timestamp_bloom = std::chrono::system_clock::now ();

        

        /* OUTPUT TIMESTAMP INFO */

        /* get overall time */
        const std::chrono::duration<double> overall_time = timestamp_bloom - timestamp_start;
        
        /* get individual sections as fractions */
        const double fraction_window_properties = std::chrono::duration<double> { timestamp_window_properties - timestamp_start } / overall_time;
        const double fraction_movement = std::chrono::duration<double> { timestamp_movement - timestamp_window_properties } / overall_time;
        const double fraction_shadow_maps = std::chrono::duration<double> { timestamp_shadow_maps - timestamp_movement } / overall_time;
        const double fraction_main_render = std::chrono::duration<double> { timestamp_main_render - timestamp_shadow_maps } / overall_time;
        const double fraction_bloom = std::chrono::duration<double> { timestamp_bloom - timestamp_main_render } / overall_time;

        /* output the fractions as percentages every 10th frame */
        //if ( frame % 5 == 0 ) \
        std::cout << "% window properties : " << fraction_window_properties * 100.0 << std::endl \
                  << "% movement          : " << fraction_movement * 100.0 << std::endl \
                  << "% shadow maps       : " << fraction_shadow_maps * 100.0 << std::endl \
                  << "% main render       : " << fraction_main_render * 100.0 << std::endl \
                  << "% bloom             : " << fraction_bloom * 100.0 << std::endl \
                  << "\x1b[A\x1b[A\x1b[A\x1b[A\x1b[A";
        
        /* print framerate every 10th frame */
        if ( frame % 10 == 0 ) std::cout << "FPS: " << std::to_string ( 1.0 / timeinfo.delta ) << '\r' << std::flush;



        /* REFRESH THE SCREEN */

        /* swap buffers */
        window.swap_buffers ();

        /* poll events */
        window.poll_events ();

    }

    return 0;
}