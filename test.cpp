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
    glh::core::vshader model_vshader { "shaders/vertex.model.glsl" };
    glh::core::fshader model_fshader { "shaders/fragment.model.glsl" };
    glh::core::fshader mirror_fshader { "shaders/fragment.mirror.glsl" };
    glh::core::vshader cubemap_vshader { "shaders/vertex.cubemap.glsl" };
    glh::core::fshader cubemap_fshader { "shaders/fragment.cubemap.glsl" };
    glh::core::program basic_program { basic_vshader, basic_fshader };
    glh::core::program model_program { model_vshader, model_fshader };
    glh::core::program mirror_program { basic_vshader, mirror_fshader };
    glh::core::program cubemap_program { cubemap_vshader, cubemap_fshader };

    auto basic_trans_uni = basic_program.get_struct_uniform ( "trans" );
    auto model_trans_uni = model_program.get_struct_uniform ( "trans" );
    auto mirror_trans_uni = mirror_program.get_struct_uniform ( "trans" );
    auto cubemap_trans_uni = cubemap_program.get_struct_uniform ( "trans" );
    auto model_transparent_mode_uni = model_program.get_uniform ( "transparent_mode" );
    
    glh::camera::camera camera { glh::math::rad ( 90 ), 16.0 / 9.0, 0.1, 1000.0 };
    camera.cache_uniforms ( model_trans_uni.get_uniform ( "view" ), model_trans_uni.get_uniform ( "proj" ) );
    camera.enable_restrictive_mode ();
    camera.set_position ( glh::math::vec3 { -6.80822, 26.2452, -3.12343 } );

    glh::lighting::light_system light_system;
    light_system.dircoll.lights.emplace_back ( glh::math::vec3 { 0.0, -1.0, 0.0 }, glh::math::vec3 { 1.0 }, glh::math::vec3 { 1.0 }, glh::math::vec3 { 1.0 } );
    light_system.cache_uniforms ( model_program.get_struct_uniform ( "light_system" ) );

    window.get_mouseinfo ();
    auto dimensions = window.get_dimensions ();
    camera.set_aspect ( ( double ) dimensions.width / dimensions.height );
    glh::core::renderer::viewport ( 0, 0, dimensions.width, dimensions.height );



    glh::core::vbo mirror_vbo { glh::vertices::square_vertex_normal_texcoord_data.size () * sizeof ( GLfloat ), glh::vertices::square_vertex_normal_texcoord_data.data () };
    glh::core::ebo mirror_ebo { glh::vertices::square_element_data.size () * sizeof ( GLuint ), glh::vertices::square_element_data.data () };
    glh::core::vao mirror_vao;
    mirror_vao.set_vertex_attrib ( 0, mirror_vbo, 3, GL_FLOAT, GL_FALSE, 9 * sizeof ( GLfloat ), 0 * sizeof ( GLfloat ) );
    mirror_vao.set_vertex_attrib ( 1, mirror_vbo, 3, GL_FLOAT, GL_FALSE, 9 * sizeof ( GLfloat ), 3 * sizeof ( GLfloat ) );
    mirror_vao.set_vertex_attrib ( 2, mirror_vbo, 3, GL_FLOAT, GL_FALSE, 9 * sizeof ( GLfloat ), 6 * sizeof ( GLfloat ) );
    mirror_vao.bind_ebo ( mirror_ebo );

    glh::core::texture2d mirror_tex { 1000, 1000, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE };
    glh::core::rbo mirror_rbo { 1000, 1000, GL_DEPTH24_STENCIL8 };
    glh::core::fbo mirror_fbo;
    mirror_fbo.attach_texture2d ( mirror_tex, GL_COLOR_ATTACHMENT0 );
    mirror_fbo.attach_rbo ( mirror_rbo, GL_DEPTH_STENCIL_ATTACHMENT );

    glh::math::mat4 mirror_matrix = 
    glh::math::translate3d 
    ( 
        glh::math::rotate3d
        (
            glh::math::enlarge3d 
            ( 
                glh::math::identity<4> (),
                10.0
            ), 
            glh::math::rad ( 45 ), 
            glh::math::vec3 { 1.0, 0.0, 0.0 }
        ), 
        glh::math::vec3 { 0.0, 50.0, -50.0 }
    );

    glh::camera::mirror_camera mirror_camera 
    { 
        camera, 
        glh::math::vec3 { 0.0, 50.0, -50.0 }, 
        glh::math::normalise ( glh::math::vec3 { 0.0, -1.0, 1.0 } ), 
        glh::math::normalise ( glh::math::vec3 { 0.0, 1.0, 1.0 } ),
        10.0, 10.0 
    };



    glh::core::vbo skybox_vbo { glh::vertices::cube_reverse_vertex_normal_data.size () * sizeof ( GLfloat ), glh::vertices::cube_reverse_vertex_normal_data.data () };
    glh::core::ebo skybox_ebo { glh::vertices::cube_element_data.size () * sizeof ( GLuint ), glh::vertices::cube_element_data.data () };
    glh::core::vao skybox_vao;
    skybox_vao.set_vertex_attrib ( 0, skybox_vbo, 3, GL_FLOAT, GL_FALSE, 6 * sizeof ( GLfloat ), 0 * sizeof ( GLfloat ) );
    skybox_vao.set_vertex_attrib ( 1, skybox_vbo, 3, GL_FLOAT, GL_FALSE, 6 * sizeof ( GLfloat ), 3 * sizeof ( GLfloat ) );
    skybox_vao.bind_ebo ( skybox_ebo );

    glh::core::cubemap skybox_cubemap
    { {
        "assets/skybox/bluecloud_rt.jpg",
        "assets/skybox/bluecloud_lf.jpg",
        "assets/skybox/bluecloud_up.jpg",
        "assets/skybox/bluecloud_dn.jpg",
        "assets/skybox/bluecloud_bk.jpg",
        "assets/skybox/bluecloud_ft.jpg"
    } };
    skybox_cubemap.set_wrap ( GL_CLAMP_TO_EDGE );

    glh::math::mat4 skybox_matrix =
    glh::math::enlarge3d
    (
        glh::math::identity<4> (),
        900.0
    );
    


    glh::model::model island { "./assets/island", "scene.gltf" };
    island.cache_uniforms ( model_program.get_struct_uniform ( "material" ), model_trans_uni.get_uniform ( "model" ) );
    glh::math::mat4 island_matrix = 
    glh::math::enlarge3d 
    ( 
        glh::math::identity<4> (),
        0.2
    );



    glh::model::model chappie ( "./assets/chappie", "scene.gltf" );
    chappie.cache_uniforms ( model_program.get_struct_uniform ( "material" ), model_trans_uni.get_uniform ( "model" ) );
    glh::math::mat4 chappie_matrix = 
    glh::math::translate3d 
    ( 
        glh::math::rotate3d 
        ( 
            glh::math::enlarge3d 
            ( 
                glh::math::identity<4> (),
                0.01
            ), 
            glh::math::rad ( 180.0 ), 
            glh::math::vec3 { 0.0, 1.0, 0.0 } 
        ), 
        glh::math::vec3 { 0.0, 0.0, 2.0 } 
    );



    glh::core::renderer::set_clear_color ( glh::math::vec4 { 0.5, 1.0, 1.0, 1.0 } );
    glh::core::renderer::enable_depth_test ();
    glh::core::renderer::blend_func ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );



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



        /*

        cubemap_program.use ();
        cubemap_trans_uni.get_uniform ( "viewpos" ).set_vector ( camera.get_position () );
        camera.apply ( cubemap_trans_uni.get_uniform ( "view" ), cubemap_trans_uni.get_uniform ( "proj" ) );
        cubemap_trans_uni.get_uniform ( "model" ).set_matrix ( skybox_matrix );

        glh::core::renderer::disable_face_culling ();
        glh::core::renderer::disable_blend ();
        glh::core::renderer::set_depth_mask ( GL_TRUE );
        glh::core::renderer::set_front_face ( GL_CCW );
        glh::core::renderer::clear ();
        glh::core::renderer::viewport ( 0, 0, dimensions.width, dimensions.height );
        skybox_cubemap.bind ();
        skybox_vao.bind ();

        glh::core::renderer::draw_elements ( GL_TRIANGLES, glh::vertices::cube_reverse_vertex_normal_data.size (), GL_UNSIGNED_INT, 0 );



        model_program.use ();
        light_system.apply ();
        model_trans_uni.get_uniform ( "viewpos" ).set_vector ( camera.get_position () );
        camera.apply ( model_trans_uni.get_uniform ( "view" ), model_trans_uni.get_uniform ( "proj" ) );
        model_transparent_mode_uni.set_int ( 0 );
        model_program.get_uniform ( "skybox" ).set_int ( 1 );

        glh::core::renderer::enable_face_culling ();
        skybox_cubemap.bind ( 1 );

        chappie.render ( chappie_matrix, false );



        model_transparent_mode_uni.set_int ( 1 );

        glh::core::renderer::enable_blend ();
        glh::core::renderer::set_depth_mask ( GL_FALSE );
        skybox_cubemap.bind ( 1 );

        chappie.render ( chappie_matrix, true );

        */

        

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



        mirror_program.use ();
        mirror_trans_uni.get_uniform ( "model" ).set_matrix ( mirror_matrix );
        camera.apply ( mirror_trans_uni.get_uniform ( "view" ), mirror_trans_uni.get_uniform ( "proj" ) );
        mirror_trans_uni.get_uniform ( "viewpos" ).set_vector ( camera.get_position () );

        

        glh::core::renderer::disable_face_culling ();
        glh::core::renderer::set_depth_mask ( GL_TRUE );
        glh::core::renderer::viewport ( 0, 0, dimensions.width, dimensions.height );

        mirror_fbo.unbind ();
        mirror_tex.bind ( 0 );
        glh::core::renderer::draw_elements ( mirror_vao, GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );



        window.swap_buffers ();
        window.poll_events ();

    }

    return 0;
}