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

    glh::vshader basic_vshader { "shaders/vertex.basic.glsl" };
    glh::fshader basic_fshader { "shaders/fragment.basic.glsl" };
    glh::fshader model_fshader { "shaders/fragment.model.glsl" };
    glh::program basic_program { basic_vshader, basic_fshader };
    glh::program model_program { basic_vshader, model_fshader };

    auto basic_trans_uni = basic_program.get_struct_uniform ( "trans" );
    auto model_trans_uni = model_program.get_struct_uniform ( "trans" );
    auto model_transparent_mode_uni = model_program.get_uniform ( "transparent_mode" );
    
    glh::camera camera { glh::math::rad ( 90 ), 16.0 / 9.0, 0.1, 500.0 };
    camera.cache_uniforms ( model_trans_uni.get_uniform ( "view" ), model_trans_uni.get_uniform ( "proj" ) );
    camera.enable_restrictive_mode ();

    glh::light_system light_system;
    light_system.dircoll.lights.emplace_back ( glh::math::vec3 { 0.0, -1.0, 0.0 }, glh::math::vec3 { 1.0 }, glh::math::vec3 { 1.0 }, glh::math::vec3 { 1.0 } );
    light_system.cache_uniforms ( model_program.get_struct_uniform ( "light_system" ) );

    window.get_mouseinfo ();
    auto dimensions = window.get_dimensions ();
    camera.set_aspect ( ( double ) dimensions.width / dimensions.height );
    glh::renderer::viewport ( 0, 0, dimensions.width, dimensions.height );

    glh::renderer::set_clear_color ( glh::math::vec4 { 0.5, 1.0, 1.0, 1.0 } );
    glh::renderer::enable_depth_test ();
    glh::renderer::blend_func ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );



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

    glh::vbo mirror_vbo { sizeof ( mirror_data ), mirror_data, GL_STATIC_DRAW };
    glh::ebo mirror_ebo { sizeof ( mirror_elems ), mirror_elems, GL_STATIC_DRAW };
    glh::vao mirror_vao;
    mirror_vao.set_vertex_attrib ( 0, mirror_vbo, 3, GL_FLOAT, GL_FALSE, 9 * sizeof ( GLfloat ), reinterpret_cast<GLvoid *> ( 0 * sizeof ( GLfloat ) ) );
    mirror_vao.set_vertex_attrib ( 1, mirror_vbo, 3, GL_FLOAT, GL_FALSE, 9 * sizeof ( GLfloat ), reinterpret_cast<GLvoid *> ( 3 * sizeof ( GLfloat ) ) );
    mirror_vao.set_vertex_attrib ( 2, mirror_vbo, 3, GL_FLOAT, GL_FALSE, 9 * sizeof ( GLfloat ), reinterpret_cast<GLvoid *> ( 6 * sizeof ( GLfloat ) ) );
    mirror_vao.bind_ebo ( mirror_ebo );

    glh::texture2d mirror_tex { 1000, 1000, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE };
    glh::rbo mirror_rbo { 1000, 1000, GL_DEPTH24_STENCIL8 };
    glh::fbo mirror_fbo;
    mirror_fbo.attach_texture2d ( mirror_tex, GL_COLOR_ATTACHMENT0 );
    mirror_fbo.attach_rbo ( mirror_rbo, GL_DEPTH_STENCIL_ATTACHMENT );



    //glh::model::model factory { "./assets/factory", "scene.gltf" };
    //factory.cache_uniforms ( model_program.get_struct_uniform ( "material" ), model_trans_uni.get_uniform ( "model" ) );
    glh::math::mat4 factory_matrix = glh::math::translate3d ( glh::math::rotate3d ( glh::math::enlarge3d ( glh::math::identity<4> (), 0.1 ), glh::math::rad ( 90 ), glh::math::vec3 { 1.0, 0.0, 0.0 } ), glh::math::vec3 { -20.0, 0.0, -20.0 } );

    //glh::model::model forest { "./assets/forest", "scene.gltf" };
    //forest.cache_uniforms ( model_program.get_struct_uniform ( "material" ), model_trans_uni.get_uniform ( "model" ) );

    glh::model::model island { "./assets/island", "scene.gltf" };
    island.cache_uniforms ( model_program.get_struct_uniform ( "material" ), model_trans_uni.get_uniform ( "model" ) );
    glh::math::mat4 island_matrix = glh::math::enlarge3d ( glh::math::identity<4> (), 0.2 );



    while ( !window.should_close () ) 
    {
        auto timeinfo = window.get_timeinfo ();
        auto dimensions = window.get_dimensions ();
        auto mouseinfo = window.get_mouseinfo ();

        if ( dimensions.deltawidth != 0.0 || dimensions.deltaheight != 0.0 ) 
        {
            camera.set_aspect ( ( double ) dimensions.width / dimensions.height );
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

        model_program.use ();
        camera.apply ( model_trans_uni.get_uniform ( "view" ), model_trans_uni.get_uniform ( "proj" ) );
        light_system.apply ();
        model_trans_uni.get_uniform ( "viewpos" ).set_vector ( camera.get_position () );
        model_transparent_mode_uni.set_int ( 0 );

        basic_program.use ();
        basic_trans_uni.get_uniform ( "model" ).set_matrix ( glh::math::enlarge3d ( glh::math::identity<4> (), 50 ) );
        camera.apply ( basic_trans_uni.get_uniform ( "view" ), basic_trans_uni.get_uniform ( "proj" ) );
        basic_trans_uni.get_uniform ( "viewpos" ).set_vector ( camera.get_position () );



        model_program.use ();
        //mirror_fbo.bind ();

        glh::renderer::enable_face_culling ();
        glh::renderer::disable_blend ();
        glh::renderer::set_depth_mask ( GL_TRUE );
        glh::renderer::clear ();
        
        //factory.render ( factory_matrix, false );
        //forest.render ( glh::math::identity<4> (), false );
        island.render ( island_matrix, false );

        glh::renderer::enable_blend ();
        glh::renderer::set_depth_mask ( GL_FALSE );
        model_transparent_mode_uni.set_int ( 1 );

        //factory.render ( factory_matrix, true );
        //forest.render ( glh::math::identity<4> (), true );
        island.render ( island_matrix, true );

/*

        basic_program.use ();
        mirror_fbo.unbind ();
        glh::object_manager::bind_default_fbo ();

        glh::renderer::disable_face_culling ();
        glh::renderer::set_depth_mask ( GL_TRUE );
        glh::renderer::clear ();

        mirror_vao.bind ();
        mirror_tex.bind ( 0 );
        glh::renderer::draw_elements ( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );

        */

        window.swap_buffers ();
        window.poll_events ();
    }

    return 0;
}