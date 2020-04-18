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
    glh::glfw::window window;
    window.set_input_mode ( GLFW_CURSOR, GLFW_CURSOR_DISABLED );

    glh::core::vshader basic_vshader { "shaders/vertex.basic.glsl" };
    glh::core::fshader basic_fshader { "shaders/fragment.basic.glsl" };
    glh::core::fshader model_fshader { "shaders/fragment.model.glsl" };
    glh::core::program basic_program { basic_vshader, basic_fshader };
    glh::core::program model_program { basic_vshader, model_fshader };

    auto basic_trans_uni = basic_program.get_struct_uniform ( "trans" );
    auto model_trans_uni = model_program.get_struct_uniform ( "trans" );
    auto model_transparent_mode_uni = model_program.get_uniform ( "transparent_mode" );
    
    glh::camera::camera camera { glh::math::rad ( 90 ), 16.0 / 9.0, 0.1, 500.0 };
    camera.cache_uniforms ( model_trans_uni.get_uniform ( "view" ), model_trans_uni.get_uniform ( "proj" ) );
    camera.enable_restrictive_mode ();

    glh::lighting::light_system light_system;
    light_system.dircoll.lights.emplace_back ( glh::math::vec3 { 0.0, -1.0, 0.0 }, glh::math::vec3 { 1.0 }, glh::math::vec3 { 1.0 }, glh::math::vec3 { 1.0 } );
    light_system.cache_uniforms ( model_program.get_struct_uniform ( "light_system" ) );

    window.get_mouseinfo ();
    auto dimensions = window.get_dimensions ();
    camera.set_aspect ( ( double ) dimensions.width / dimensions.height );
    glh::core::renderer::viewport ( 0, 0, dimensions.width, dimensions.height );

    glh::core::renderer::set_clear_color ( glh::math::vec4 { 0.5, 1.0, 1.0, 1.0 } );
    glh::core::renderer::enable_depth_test ();
    glh::core::renderer::blend_func ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );



    const GLfloat mirror_data [] =
    {
         1.0,  1.0, 0.0,    0.0, 0.0, 1.0,    1.0, 1.0, 0.0,
        -1.0,  1.0, 0.0,    0.0, 0.0, 1.0,    0.0, 1.0, 0.0,
        -1.0, -1.0, 0.0,    0.0, 0.0, 1.0,    0.0, 0.0, 0.0,
         1.0, -1.0, 0.0,    0.0, 0.0, 1.0,    1.0, 0.0, 0.0,
    };
    const GLuint mirror_elems [] =
    {
        0, 1, 2, 0, 2, 3
    };

    glh::core::vbo mirror_vbo { sizeof ( mirror_data ), mirror_data, GL_STATIC_DRAW };
    glh::core::ebo mirror_ebo { sizeof ( mirror_elems ), mirror_elems, GL_STATIC_DRAW };
    glh::core::vao mirror_vao;
    mirror_vao.set_vertex_attrib ( 0, mirror_vbo, 3, GL_FLOAT, GL_FALSE, 9 * sizeof ( GLfloat ), reinterpret_cast<GLvoid *> ( 0 * sizeof ( GLfloat ) ) );
    mirror_vao.set_vertex_attrib ( 1, mirror_vbo, 3, GL_FLOAT, GL_FALSE, 9 * sizeof ( GLfloat ), reinterpret_cast<GLvoid *> ( 3 * sizeof ( GLfloat ) ) );
    mirror_vao.set_vertex_attrib ( 2, mirror_vbo, 3, GL_FLOAT, GL_FALSE, 9 * sizeof ( GLfloat ), reinterpret_cast<GLvoid *> ( 6 * sizeof ( GLfloat ) ) );
    mirror_vao.bind_ebo ( mirror_ebo );

    glh::core::texture2d mirror_tex { 1000, 1000, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE };
    glh::core::rbo mirror_rbo { 1000, 1000, GL_DEPTH24_STENCIL8 };
    glh::core::fbo mirror_fbo;
    mirror_fbo.attach_texture2d ( mirror_tex, GL_COLOR_ATTACHMENT0 );
    mirror_fbo.attach_rbo ( mirror_rbo, GL_DEPTH_STENCIL_ATTACHMENT );

    glh::math::mat4 mirror_matrix = glh::math::translate3d ( glh::math::enlarge3d ( glh::math::identity<4> (), 5.0 ), glh::math::vec3 { 0.0, 30.0, -100.0 } );

    glh::camera::mirror_camera mirror_camera { camera, glh::math::vec3 { 0.0, 30.0, -100.0 }, glh::math::vec3 { 0.0, 0.0, 1.0 }, 10.0, 10.0 };
    


    glh::model::model island { "./assets/island", "scene.gltf" };
    island.cache_uniforms ( model_program.get_struct_uniform ( "material" ), model_trans_uni.get_uniform ( "model" ) );
    glh::math::mat4 island_matrix = glh::math::enlarge3d ( glh::math::identity<4> (), 0.2 );

    glh::model::model chappie ( "./assets/chappie", "scene.gltf" );
    chappie.cache_uniforms ( model_program.get_struct_uniform ( "material" ), model_trans_uni.get_uniform ( "model" ) );
    glh::math::mat4 chappie_matrix = glh::math::enlarge3d ( glh::math::identity<4> (), 0.2 );



    while ( !window.should_close () ) 
    {
        auto timeinfo = window.get_timeinfo ();
        auto dimensions = window.get_dimensions ();
        auto mouseinfo = window.get_mouseinfo ();

        if ( dimensions.deltawidth != 0.0 || dimensions.deltaheight != 0.0 ) 
        {
            camera.set_aspect ( ( double ) dimensions.width / dimensions.height );
        }

        if ( window.get_key ( GLFW_KEY_W ).action == GLFW_PRESS ) camera.move ( timeinfo.delta * glh::math::vec3 { 0.0, 0.0, -15.0 } );
        if ( window.get_key ( GLFW_KEY_A ).action == GLFW_PRESS ) camera.move ( timeinfo.delta * glh::math::vec3 { -15.0, 0.0, 0.0 } );
        if ( window.get_key ( GLFW_KEY_S ).action == GLFW_PRESS ) camera.move ( timeinfo.delta * glh::math::vec3 { 0.0, 0.0, 15.0 } );
        if ( window.get_key ( GLFW_KEY_D ).action == GLFW_PRESS ) camera.move ( timeinfo.delta * glh::math::vec3 { 15.0, 0.0, 0.0 } );

        if ( window.get_key ( GLFW_KEY_SPACE ).action == GLFW_PRESS ) camera.move ( timeinfo.delta * glh::math::vec3 { 0.0, 15.0, 0.0 } );
        if ( window.get_key ( GLFW_KEY_LEFT_SHIFT ).action == GLFW_PRESS ) camera.move ( timeinfo.delta * glh::math::vec3 { 0.0, -15.0, 0.0 } );

        if ( window.get_key ( GLFW_KEY_Z ).action == GLFW_PRESS ) camera.roll ( timeinfo.delta * glh::math::rad ( 80.0 ) );
        if ( window.get_key ( GLFW_KEY_X ).action == GLFW_PRESS ) camera.roll ( timeinfo.delta * glh::math::rad ( -80.0 ) ); 

        camera.pitch ( mouseinfo.deltayfrac * glh::math::rad ( -80 ) );
        camera.yaw ( mouseinfo.deltaxfrac * glh::math::rad ( -80 ) );



        model_program.use ();
        light_system.apply ();
        model_trans_uni.get_uniform ( "viewpos" ).set_vector ( camera.get_position () );
        model_transparent_mode_uni.set_int ( 0 );

        glh::core::renderer::enable_face_culling ();
        glh::core::renderer::disable_blend ();
        glh::core::renderer::set_depth_mask ( GL_TRUE );
                
        mirror_fbo.unbind ();
        glh::core::renderer::set_front_face ( GL_CCW );
        glh::core::renderer::clear ();
        glh::core::renderer::viewport ( 0, 0, dimensions.width, dimensions.height );
        camera.apply ( model_trans_uni.get_uniform ( "view" ), model_trans_uni.get_uniform ( "proj" ) );
        island.render ( island_matrix, false );
        chappie.render ( glh::math::inverse ( camera.get_view () ) * chappie_matrix, false );

        mirror_fbo.bind ();
        glh::core::renderer::set_front_face ( GL_CW );
        glh::core::renderer::clear ();
        glh::core::renderer::viewport ( 0, 0, 1000, 1000 );
        mirror_camera.apply ( model_trans_uni.get_uniform ( "view" ), model_trans_uni.get_uniform ( "proj" ) );
        island.render ( island_matrix, false );
        chappie.render ( glh::math::inverse ( camera.get_view () ) * chappie_matrix, false );



        glh::core::renderer::enable_blend ();
        glh::core::renderer::set_depth_mask ( GL_FALSE );
        model_transparent_mode_uni.set_int ( 1 );



        mirror_fbo.unbind ();
        glh::core::renderer::set_front_face ( GL_CCW );
        glh::core::renderer::viewport ( 0, 0, dimensions.width, dimensions.height );
        camera.apply ( model_trans_uni.get_uniform ( "view" ), model_trans_uni.get_uniform ( "proj" ) );
        island.render ( island_matrix, true );
        chappie.render ( glh::math::inverse ( camera.get_view () ) * chappie_matrix, true );

        mirror_fbo.bind ();
        glh::core::renderer::set_front_face ( GL_CW );
        glh::core::renderer::viewport ( 0, 0, 1000, 1000 );
        mirror_camera.apply ( model_trans_uni.get_uniform ( "view" ), model_trans_uni.get_uniform ( "proj" ) );
        island.render ( island_matrix, true );
        chappie.render ( glh::math::inverse ( camera.get_view () ) * chappie_matrix, true );



        basic_program.use ();
        basic_trans_uni.get_uniform ( "model" ).set_matrix ( mirror_matrix );
        camera.apply ( basic_trans_uni.get_uniform ( "view" ), basic_trans_uni.get_uniform ( "proj" ) );
        basic_trans_uni.get_uniform ( "viewpos" ).set_vector ( camera.get_position () );

        

        glh::core::renderer::disable_face_culling ();
        glh::core::renderer::set_depth_mask ( GL_TRUE );
        glh::core::renderer::viewport ( 0, 0, dimensions.width, dimensions.height );

        mirror_fbo.unbind ();
        mirror_vao.bind ();
        mirror_tex.bind ( 0 );
        glh::core::renderer::draw_elements ( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );



        window.swap_buffers ();
        window.poll_events ();
    }

    return 0;
}