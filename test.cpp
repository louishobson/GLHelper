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

    /* create forward model shader program */
    glh::core::vshader forward_model_vshader { "shaders/materials.glsl", "shaders/camera.glsl", "shaders/vertex.model.glsl" };
    glh::core::fshader forward_model_fshader { "shaders/materials.glsl", "shaders/camera.glsl", "shaders/lighting.glsl", "shaders/fragment.forward_model.glsl"  };
    glh::core::program forward_model_program { forward_model_vshader, forward_model_fshader };
    forward_model_program.compile_and_link ();

    /* create deferred model shader program */
    glh::core::vshader deferred_model_vshader { "shaders/materials.glsl", "shaders/camera.glsl", "shaders/vertex.model.glsl" };
    glh::core::fshader deferred_model_fshader { "shaders/materials.glsl", "shaders/fragment.deferred_model.glsl" };
    glh::core::program deferred_model_program { deferred_model_vshader, deferred_model_fshader };
    deferred_model_program.compile_and_link ();

    /* create shadow shader program */
    glh::core::vshader shadow_vshader { "shaders/materials.glsl", "shaders/vertex.shadow.glsl" };
    glh::core::gshader shadow_gshader { "shaders/materials.glsl", "shaders/camera.glsl", "shaders/lighting.glsl", "shaders/geometry.shadow.glsl" };
    glh::core::fshader shadow_fshader { "shaders/materials.glsl", "shaders/fragment.shadow.glsl" };
    glh::core::program shadow_program { shadow_vshader, shadow_gshader, shadow_fshader };
    shadow_program.compile_and_link ();

    /* create simple vshader */
    glh::core::vshader simple_vshader { "shaders/vertex.simple.glsl" };

    /* create lighting shader program */
    glh::core::fshader lighting_fshader { "shaders/materials.glsl", "shaders/camera.glsl", "shaders/lighting.glsl", "shaders/fragment.lighting.glsl"  };
    glh::core::program lighting_program { simple_vshader, lighting_fshader };
    lighting_program.compile_and_link ();

    /* create bloom shader program */
    glh::core::fshader bloom_fshader { "shaders/function.glsl", "shaders/fragment.bloom.glsl" };
    glh::core::program bloom_program { simple_vshader, bloom_fshader };
    bloom_program.compile_and_link ();

    /* create fxaa shader program */
    glh::core::fshader fxaa_fshader { "shaders/fragment.fxaa.glsl" };
    glh::core::program fxaa_program { simple_vshader, fxaa_fshader };
    fxaa_program.compile_and_link ();

    /* extract uniforms out of forward model program */
    auto& forward_model_camera_uni = forward_model_program.get_struct_uniform ( "camera" );
    auto& forward_model_light_system_uni = forward_model_program.get_struct_uniform ( "light_system" );
    auto& forward_model_material_uni = forward_model_program.get_struct_uniform ( "material" );
    auto& forward_model_transparent_mode_uni = forward_model_program.get_uniform ( "transparent_mode" );

    /* extract uniforms out of deferred model program */
    auto& deferred_model_camera_uni = deferred_model_program.get_struct_uniform ( "camera" );
    auto& deferred_model_material_uni = deferred_model_program.get_struct_uniform ( "material" );
    auto& deferred_model_transparent_mode_uni = deferred_model_program.get_uniform ( "transparent_mode" );

    /* extract uniforms out of shadow program */
    auto& shadow_light_system_uni = shadow_program.get_struct_uniform ( "light_system" );
    auto& shadow_material_uni = shadow_program.get_struct_uniform ( "material" );

    /* extract uniforms out of ligthing program */
    auto& lighting_camera_uni = lighting_program.get_struct_uniform ( "camera" );
    auto& lighting_light_system_uni = lighting_program.get_struct_uniform ( "light_system" );
    auto& lighting_gbuffer_positionshininess_uni = lighting_program.get_uniform ( "gbuffer_positionshininess" );
    auto& lighting_gbuffer_normalsstrength_uni = lighting_program.get_uniform ( "gbuffer_normalsstrength" );
    auto& lighting_gbuffer_albedospec_uni = lighting_program.get_uniform ( "gbuffer_albedospec" );

    /* extract uniforms out of bloom program */
    auto& bloom_texture_uni = bloom_program.get_uniform ( "bloom_texture" );
    auto& bloom_mode_uni = bloom_program.get_uniform ( "bloom_mode" );
    auto& bloom_function_uni = bloom_program.get_struct_uniform ( "bloom_function" );
    auto& bloom_radius_uni = bloom_program.get_uniform ( "bloom_radius" );

    /* extract uniforms out of fxaa program */
    auto& fxaa_texture_uni = fxaa_program.get_uniform ( "fxaa_texture" );
    auto& fxaa_contrast_constant_threshold_uni = fxaa_program.get_uniform ( "contrast_constant_threshold" );
    auto& fxaa_contrast_relative_threshold_uni = fxaa_program.get_uniform ( "contrast_relative_threshold" );



    /* SET UP CAMERA */

    /* create camera */
    glh::camera::camera_perspective_movement camera 
    { 
        glh::math::vec3 { 0.0, 25.0, 25.0 }, glh::math::vec3 { 0.0, 0.0, -1.0 }, glh::math::vec3 { 0.0, 1.0, 0.0 },
        glh::math::rad ( 90.0 ), 16.0 / 9.0, 0.5, 200.0 
    };

    /* restrict movement */
    camera.enable_restrictive_mode ();



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
        glh::model::import_flags::GLH_PRETRANSFORM_VERTICES |
        glh::model::import_flags::GLH_SPLIT_MESHES_BY_ALPHA_VALUES |
        glh::model::import_flags::GLH_IGNORE_VCOLOR_WHEN_ALPHA_TESTING |
        glh::model::import_flags::GLH_CONFIGURE_GLOBAL_VERTEX_ARRAYS |
        0,
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
        glh::model::import_flags::GLH_PRETRANSFORM_VERTICES |
        glh::model::import_flags::GLH_SPLIT_MESHES_BY_ALPHA_VALUES |
        glh::model::import_flags::GLH_IGNORE_VCOLOR_WHEN_ALPHA_TESTING |
        glh::model::import_flags::GLH_CONFIGURE_GLOBAL_VERTEX_ARRAYS |
        0,
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
        MODEL_SWITCH.model_region (),
        false, true, 0.007, //0.035
        16, 0.001
    );

    /* add point light */
    light_system.add_pointlight
    (
        glh::math::vec3 ( 30, 40, 20 ), 1.0, 0.007, 0.0002,
        //glh::math::vec3 ( 20, 0, 20 ), 1.0, 0.0, 0.0,
        glh::math::vec3 { 0.1 },
        glh::math::vec3 { 1.0 }, 
        glh::math::vec3 { 1.0 },
        MODEL_SWITCH.model_region (),
        true, true, 0.003,
        16, 2.0 / 4096.0
    );

    /* add spotlights */
    light_system.add_spotlight
    (
        glh::math::vec3 ( 30, 50, 20 ), -glh::math::vec3 ( 30, 50, 20 ),
        glh::math::rad ( 30 ), glh::math::rad ( 50 ), 1.0, 0.007, 0.0002,
        glh::math::vec3 { 0.05 },
        glh::math::vec3 { 1.0 }, 
        glh::math::vec3 { 1.0 },
        MODEL_SWITCH.model_region (),
        false, true, 0.003,
        16, 2.0 / 4096.0
    );



    /* SET UP FRAMEBUFFERS */

    /* create bloom textures (the other fbos depend on the bloom textures) */
    glh::core::texture2d bloom_texture_alpha;
    glh::core::texture2d bloom_texture_beta;
    bloom_texture_alpha.tex_storage ( 1920, 1080, GL_RGBA8, 1 );
    bloom_texture_beta.tex_storage ( 1920, 1080, GL_RGBA8, 1 );
    bloom_texture_alpha.set_min_filter ( GL_NEAREST ); bloom_texture_alpha.set_mag_filter ( GL_NEAREST );
    bloom_texture_beta.set_min_filter ( GL_NEAREST ); bloom_texture_beta.set_mag_filter ( GL_NEAREST );
    bloom_texture_alpha.set_wrap ( GL_CLAMP_TO_EDGE );
    bloom_texture_beta.set_wrap ( GL_CLAMP_TO_EDGE );

    /* create bloom framebuffers and attach buffers */
    glh::core::fbo bloom_fbo_alpha;
    glh::core::fbo bloom_fbo_beta;
    bloom_fbo_alpha.attach_texture ( bloom_texture_alpha, GL_COLOR_ATTACHMENT0 );
    bloom_fbo_beta.attach_texture ( bloom_texture_beta, GL_COLOR_ATTACHMENT0 );



    /* create the depth rbo (the fbos below depend on this) */
    glh::core::rbo depth_rbo { 1920, 1080, GL_DEPTH_COMPONENT };



    /* create gbuffer textures */
    glh::core::texture2d gbuffer_positionshininess;
    glh::core::texture2d gbuffer_normalsstrength;
    glh::core::texture2d gbuffer_albedospec;

    /* set up gbuffer texture storage */
    gbuffer_positionshininess.tex_storage ( 1920, 1080, GL_RGBA32F, 1 ); 
    gbuffer_normalsstrength.tex_storage ( 1920, 1080, GL_RGBA32F, 1 );
    gbuffer_albedospec.tex_storage ( 1920, 1080, GL_RGBA8, 1 );

    /* set up filter options */
    gbuffer_positionshininess.set_min_filter ( GL_NEAREST ); gbuffer_positionshininess.set_mag_filter ( GL_NEAREST );
    gbuffer_normalsstrength.set_min_filter ( GL_NEAREST ); gbuffer_normalsstrength.set_mag_filter ( GL_NEAREST );
    gbuffer_albedospec.set_min_filter ( GL_NEAREST ); gbuffer_albedospec.set_mag_filter ( GL_NEAREST );

    /* set up wrap options */
    gbuffer_positionshininess.set_wrap ( GL_CLAMP_TO_EDGE );
    gbuffer_normalsstrength.set_wrap ( GL_CLAMP_TO_EDGE );
    gbuffer_albedospec.set_wrap ( GL_CLAMP_TO_EDGE );

    /* create gbuffer framebuffer and attach buffers */
    glh::core::fbo gbuffer_fbo;
    gbuffer_fbo.attach_texture ( gbuffer_positionshininess, GL_COLOR_ATTACHMENT0 );
    gbuffer_fbo.attach_texture ( gbuffer_normalsstrength, GL_COLOR_ATTACHMENT1 );
    gbuffer_fbo.attach_texture ( gbuffer_albedospec, GL_COLOR_ATTACHMENT2 );
    gbuffer_fbo.attach_texture ( bloom_texture_beta, GL_COLOR_ATTACHMENT3 );
    gbuffer_fbo.attach_rbo ( depth_rbo, GL_DEPTH_ATTACHMENT );
    gbuffer_fbo.draw_buffers ( GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 );



    /* create final color texture */
    glh::core::texture2d final_color_texture { 1920, 1080, GL_RGBA8 };
    final_color_texture.set_min_filter ( GL_NEAREST ); final_color_texture.set_mag_filter ( GL_NEAREST );
    final_color_texture.set_wrap ( GL_CLAMP_TO_EDGE );

    /* create the final color framebuffer */
    glh::core::fbo final_color_fbo;
    final_color_fbo.attach_texture ( final_color_texture, GL_COLOR_ATTACHMENT0 );
    


    /* create transparent fbo */
    glh::core::fbo transparent_fbo;
    transparent_fbo.attach_texture ( final_color_texture, GL_COLOR_ATTACHMENT0 );
    transparent_fbo.attach_texture ( bloom_texture_beta, GL_COLOR_ATTACHMENT1 );
    transparent_fbo.attach_rbo ( depth_rbo, GL_DEPTH_ATTACHMENT );
    transparent_fbo.draw_buffers ( GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 );



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

        /* create shadow maps */
        if ( light_system.requires_shadow_mapping () )
        {
            /* bind the fbo */
            light_system.bind_shadow_maps_fbo ();

            /* use the shadow program */
            shadow_program.use ();

            /* apply light system and cache model matrix uniform */
            light_system.apply ( shadow_light_system_uni );

            /* prepare for rendering  */
            glh::core::renderer::disable_blend ();
            glh::core::renderer::enable_depth_test ();
            glh::core::renderer::set_depth_mask ( GL_TRUE );
            glh::core::renderer::enable_face_culling ();
            glh::core::renderer::set_cull_face ( GL_BACK );
            glh::core::renderer::viewport ( 0, 0, light_system.get_shadow_map_width (), light_system.get_shadow_map_width () );
            glh::core::renderer::clear ( GL_DEPTH_BUFFER_BIT );

            /* render */
            MODEL_SWITCH.cache_material_uniforms ( shadow_material_uni );
            MODEL_SWITCH.render ( glh::model::render_flags::GLH_LEAVE_GLOBAL_VERTEX_ARRAYS_BOUND | glh::model::render_flags::GLH_OPAQUE_MODE | glh::model::render_flags::GLH_NO_MODEL_MATRIX );
        }

        /* set timestamp */
        const auto timestamp_shadow_maps = std::chrono::system_clock::now ();



        /* RENDER MODELS TO GBUFFER */

        /* bind the gbuffer framebuffer */
        gbuffer_fbo.bind ();    

        /* use the deferred model program */
        deferred_model_program.use ();

        /* apply camera and set to opaque mode */
        camera.apply ( deferred_model_camera_uni );
        deferred_model_transparent_mode_uni.set_int ( 2 );

        /* set up renderer */
        glh::core::renderer::disable_blend ();
        glh::core::renderer::enable_depth_test ();
        glh::core::renderer::set_depth_mask ( GL_TRUE );
        glh::core::renderer::enable_face_culling ();
        glh::core::renderer::set_cull_face ( GL_BACK );
        glh::core::renderer::viewport ( 0, 0, 1920, 1080 );  
        glh::core::renderer::clear ( GL_DEPTH_BUFFER_BIT );
        
        /* render */
        MODEL_SWITCH.cache_material_uniforms ( deferred_model_material_uni );
        MODEL_SWITCH.render ( glh::model::render_flags::GLH_OPAQUE_MODE | glh::model::render_flags::GLH_NO_MODEL_MATRIX );

        /* set timestamp */
        const auto timestamp_gbuffer_render = std::chrono::system_clock::now ();



        /* LIGHTING PASS */

        /* bind the final color framebuffer */
        final_color_fbo.bind ();

        /* use the lighting program */
        lighting_program.use ();

        /* apply many uniforms */
        lighting_gbuffer_positionshininess_uni.set_int ( gbuffer_positionshininess.bind_loop () );
        lighting_gbuffer_normalsstrength_uni.set_int ( gbuffer_normalsstrength.bind_loop () );
        lighting_gbuffer_albedospec_uni.set_int ( gbuffer_albedospec.bind_loop () );
        camera.apply ( lighting_camera_uni );
        light_system.apply ( lighting_light_system_uni );

        /* set final color texture to use nearest filtering */
        final_color_texture.set_min_filter ( GL_NEAREST ); final_color_texture.set_mag_filter ( GL_NEAREST );

        /* set up renderer */
        glh::core::renderer::disable_blend ();
        glh::core::renderer::disable_depth_test ();
        glh::core::renderer::disable_face_culling ();
        glh::core::renderer::viewport ( 0, 0, 1920, 1080 );  

        /* render */
        quad_vao.bind ();
        glh::core::renderer::draw_arrays ( GL_TRIANGLE_STRIP, 0, 4 );
        quad_vao.unbind ();

        /* set timestamp */
        const auto timestamp_lighting_pass = std::chrono::system_clock::now ();



        /* FORWARD RENDER TRANSPARENT PARTS OF MODELS */

        /* bind the transparent framebuffer */
        transparent_fbo.bind ();

        /* use the forward model program */
        forward_model_program.bind ();

        /* apply many uniforms */
        camera.apply ( forward_model_camera_uni );
        light_system.apply ( forward_model_light_system_uni );
        forward_model_transparent_mode_uni.set_int ( 1 );

        /* set up renderer */
        glh::core::renderer::enable_blend ();
        glh::core::renderer::blend_equation ( GL_FUNC_ADD );
        glh::core::renderer::blend_func ( 0, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        glh::core::renderer::blend_func ( 1, GL_ONE, GL_ONE );
        glh::core::renderer::enable_depth_test ();
        glh::core::renderer::set_depth_mask ( GL_FALSE );
        glh::core::renderer::enable_face_culling ();
        glh::core::renderer::set_cull_face ( GL_BACK );
        glh::core::renderer::viewport ( 0, 0, 1920, 1080 );

        /* render */
        MODEL_SWITCH.cache_material_uniforms ( forward_model_material_uni );
        MODEL_SWITCH.render ( glh::model::render_flags::GLH_TRANSPARENT_MODE | glh::model::render_flags::GLH_NO_MODEL_MATRIX );

        /* set timestamp */
        const auto timestamp_transparent_render = std::chrono::system_clock::now ();



        /* APPLY BLOOM */

        /* use bloom program */
        bloom_program.use ();

        /* apply uniforms */
        glsl_bloom_func.apply ();
        bloom_radius_uni.set_int ( bloom_radius );        

        /* prepare for rendering */
        glh::core::renderer::disable_blend ();
        glh::core::renderer::disable_depth_test ();
        glh::core::renderer::disable_face_culling ();
        glh::core::renderer::viewport ( 0, 0, 1920, 1080 );

        /* bloom loop */
        quad_vao.bind ();
        for ( unsigned i = 0; i < bloom_iterations; ++i )
        {
            /* bind alpha fbo, set remaining uniforms and render */
            bloom_fbo_alpha.bind ();
            bloom_texture_uni.set_int ( bloom_texture_beta.bind_loop () );
            bloom_mode_uni.set_int ( 0 );
            glh::core::renderer::draw_arrays ( GL_TRIANGLE_STRIP, 0, 4 );

            /* if not the final iteration, render to beta */
            if ( i != bloom_iterations - 1 )
            {
                /* bind alpha fbo, set remaining uniforms and render */
                bloom_fbo_beta.bind ();
                bloom_texture_uni.set_int ( bloom_texture_alpha.bind_loop () );
                bloom_mode_uni.set_int ( 1 );
                glh::core::renderer::draw_arrays ( GL_TRIANGLE_STRIP, 0, 4 );
            } else
            /* else blend to final color framebuffer */
            {
                /* bind final color framebuffer */
                final_color_fbo.bind ();

                /* set up renderer */
                glh::core::renderer::enable_blend ();
                glh::core::renderer::blend_func ( GL_ONE, GL_ONE );
                glh::core::renderer::blend_equation ( GL_FUNC_ADD );

                /* render into the default framebuffer */
                bloom_texture_uni.set_int ( bloom_texture_alpha.bind_loop () );
                bloom_mode_uni.set_int ( 1 );
                glh::core::renderer::draw_arrays ( GL_TRIANGLE_STRIP, 0, 4 );
            }
        }

        /* set timestamp */
        const auto timestamp_bloom = std::chrono::system_clock::now ();



        /* APPLY FXAA */

        /* bind the default framebuffer */
        window.bind_framebuffer ();

        /* use the fxaa program */
        fxaa_program.use ();

        /* set uniforms */
        fxaa_texture_uni.set_int ( final_color_texture.bind_loop () );
        //fxaa_contrast_constant_threshold_uni.set_float ( 0.0312 );
        fxaa_contrast_constant_threshold_uni.set_float ( 0.01 );
        //fxaa_contrast_relative_threshold_uni.set_float ( 0.063 );
        fxaa_contrast_relative_threshold_uni.set_float ( 0.02 );

        /* set final color texture to use linear interpolation */
        final_color_texture.set_min_filter ( GL_LINEAR ); final_color_texture.set_mag_filter ( GL_LINEAR );

        /* set up renderer */
        glh::core::renderer::disable_blend ();

        /* render */
        glh::core::renderer::draw_arrays ( GL_TRIANGLE_STRIP, 0, 4 );
        quad_vao.unbind ();

        /* set timestamp */
        const auto timestamp_fxaa = std::chrono::system_clock::now ();



        /* BLIT FINAL COLOR INTO DEFAULT FRAMEBUFFER */
        //window.bind_framebuffer ();
        //final_color_fbo.blit_copy_to_default ( 0, 0, 1920, 1080, 0, 0, 1920, 1080, GL_COLOR_BUFFER_BIT, GL_NEAREST );

        

        /* OUTPUT TIMESTAMP INFO */

        /* get overall time */
        const std::chrono::duration<double> overall_time = timestamp_fxaa - timestamp_start;
        
        /* get individual sections as fractions */
        const double fraction_window_properties = std::chrono::duration<double> { timestamp_window_properties - timestamp_start } / overall_time;
        const double fraction_movement = std::chrono::duration<double> { timestamp_movement - timestamp_window_properties } / overall_time;
        const double fraction_shadow_maps = std::chrono::duration<double> { timestamp_shadow_maps - timestamp_movement } / overall_time;
        const double fraction_gbuffer_render = std::chrono::duration<double> { timestamp_gbuffer_render - timestamp_shadow_maps } / overall_time;
        const double fraction_lighting_pass = std::chrono::duration<double> { timestamp_lighting_pass - timestamp_gbuffer_render } / overall_time;
        const double fraction_transparent_render = std::chrono::duration<double> { timestamp_transparent_render - timestamp_lighting_pass } / overall_time;
        const double fraction_bloom = std::chrono::duration<double> { timestamp_bloom - timestamp_transparent_render } / overall_time;
        const double fraction_fxaa = std::chrono::duration<double> { timestamp_fxaa - timestamp_bloom } / overall_time;

        /* output the fractions as percentages every 10th frame */
        //if ( frame % 5 == 0 ) \
        std::cout << "% window properties  : " << fraction_window_properties * 100.0 << std::endl \
                  << "% movement           : " << fraction_movement * 100.0 << std::endl \
                  << "% shadow maps        : " << fraction_shadow_maps * 100.0 << std::endl \
                  << "% gbuffer render     : " << fraction_gbuffer_render * 100.0 << std::endl \
                  << "% lighting pass      : " << fraction_lighting_pass * 100.0 << std::endl \
                  << "% transparent render : " << fraction_transparent_render * 100.0 << std::endl \
                  << "% bloom              : " << fraction_bloom * 100.0 << std::endl \
                  << "% fxaa               : " << fraction_fxaa * 100.0 << std::endl \
                  << "\x1b[A\x1b[A\x1b[A\x1b[A\x1b[A\x1b[A\x1b[A\x1b[A";
        
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