/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * src/glhelper/glhelper_lighting.cpp
 * 
 * implementation of include/glhelper/glhelper_lighting.hpp
 * 
 */



/* INCLUDES */

/* include glhelper_lighting.hpp */
#include <glhelper/glhelper_lighting.hpp>



/* LIGHT IMPLEMENTATION */

/* apply
 *
 * apply the lighting to a uniform
 * the uniform should be of type light_struct (see top)
 * 
 * light_uni: the uniform to apply the light to (which will be cached)
 */
void glh::lighting::light::apply ( core::struct_uniform& light_uni )
{
    /* cache uniform */
    cache_uniforms ( light_uni );

    /* now apply the cached uniform */
    apply ();
}
void glh::lighting::light::apply () const
{
    /* throw if no uniform is cached */
    if ( !cached_uniforms ) throw exception::uniform_exception { "attempted to apply light to uniform with out a complete uniform cache" };

    /* now set all of the uniform values */
    cached_uniforms->position_uni.set_vector ( position );
    cached_uniforms->direction_uni.set_vector ( direction );
    cached_uniforms->inner_cone_uni.set_float ( inner_cone );
    cached_uniforms->outer_cone_uni.set_float ( outer_cone );
    cached_uniforms->att_const_uni.set_float ( att_const );
    cached_uniforms->att_linear_uni.set_float ( att_linear );
    cached_uniforms->att_quad_uni.set_float ( att_quad );
    cached_uniforms->ambient_color_uni.set_vector ( ambient_color );
    cached_uniforms->diffuse_color_uni.set_vector ( diffuse_color );
    cached_uniforms->specular_color_uni.set_vector ( specular_color );
    cached_uniforms->enabled_uni.set_int ( enabled );
    cached_uniforms->has_shadow_map_uni.set_int ( shadow_mapping_enabled );
}

/* cache_uniforms
 *
 * cache light uniforms for later use
 * 
 * light_uni: the uniform to cache
 */
void glh::lighting::light::cache_uniforms ( core::struct_uniform& light_uni )
{
    /* if uniforms are not already cached, cache the new ones */
    if ( !cached_uniforms || cached_uniforms->light_uni != light_uni )
    {
        cached_uniforms.reset ( new cached_uniforms_struct
        {
            light_uni,
            light_uni.get_uniform ( "position" ),
            light_uni.get_uniform ( "direction" ),
            light_uni.get_uniform ( "inner_cone" ),
            light_uni.get_uniform ( "outer_cone" ),
            light_uni.get_uniform ( "att_const" ),
            light_uni.get_uniform ( "att_linear" ),
            light_uni.get_uniform ( "att_quad" ),
            light_uni.get_uniform ( "ambient_color" ),
            light_uni.get_uniform ( "diffuse_color" ),
            light_uni.get_uniform ( "specular_color" ),
            light_uni.get_uniform ( "enabled" ),
            light_uni.get_uniform ( "has_shadow_map" ),
            light_uni.get_uniform ( "shadow_map_2d" ),
            light_uni.get_uniform ( "shadow_map_2d" )
        } );
    }
}



/* DIRLIGHT IMPLEMENTATION */

/* shadow_camera
 *
 * produce a camera for shadow mapping based on a region of space to capture
 * returns an orthographic movement camera capturing the whole scene
 * 
 * capture_region: a spherical region of what to capture in the shadow map
 */
glh::camera::camera_orthographic_movement glh::lighting::dirlight::shadow_camera ( const region::spherical_region<>& capture_region ) const
{
    /* produce a camera in the middle of the region, with an orthographic matrix spreading all of the region */
    return camera::camera_orthographic_movement
    {
        capture_region.centre, direction, math::any_perpandicular ( direction ),
        math::vec3 { -capture_region.radius }, math::vec3 { capture_region.radius }
    };
}

/* apply_shadow_map
 *
 * applies the shadow map
 */
void glh::lighting::dirlight::apply_shadow_map () const
{
    /* apply to correct uniform */
    cached_uniforms->shadow_map_2d_uni.set_int ( shadow_map.bind_map_loop () );
}



/* POINTLIGHT IMPLEMENTATION */

/* shadow_camera
 *
 * produce a camera for shadow mapping based on a region of space to capture
 * returns a perspective movement camera in the +ve-x direction
 * 90 degree rotation can be applied to the view matrix to aquire different directions
 * 
 * capture_region: a spherical region of what to capture in the shadow map
 */
glh::camera::camera_perspective_movement glh::lighting::pointlight::shadow_camera ( const region::spherical_region<>& capture_region ) const
{
    /* camera needs to point in the +ve-x direction and be able to see all of the sphere
     * the fov must be 90 degrees
     */
    return camera::camera_perspective_movement
    {
        position, math::vec3 { 1.0, 0.0, 0.0 }, math::vec3 { 0.0, 1.0, 0.0 },
        math::rad ( 90.0 ), 1.0, 0.1, math::modulus ( capture_region.centre - position ) + capture_region.radius
    };
}

/* apply_shadow_map
 *
 * applies the shadow map
 */
void glh::lighting::pointlight::apply_shadow_map () const
{
    /* apply to correct uniform */
    cached_uniforms->shadow_map_cube_uni.set_int ( shadow_map.bind_map_loop () );
}




/* SPOTLIGHT IMPLEMENTATION */

/* shadow_camera
 *
 * produce a camera for shadow mapping based on a region of space to capture
 * returns a perspective movement camera, aspect ratio 1:1, capturing all up to the outer cone
 *  
 * capture_region: a spherical region of what to capture in the shadow map
 */
glh::camera::camera_perspective_movement glh::lighting::spotlight::shadow_camera ( const region::spherical_region<>& capture_region ) const
{
    /* camera needs to point in the direction of the spotlight and see all of the sphere
     * the fov is the outer radius
     */
    return camera::camera_perspective_movement
    {
        position, direction, math::any_perpandicular ( direction ),
        outer_cone, 1.0, 0.1, math::modulus ( capture_region.centre - position ) + capture_region.radius
    };    
}

/* apply_shadow_map
 *
 * applies the shadow map
 */
void glh::lighting::spotlight::apply_shadow_map () const
{
    /* apply to correct uniform */
    cached_uniforms->shadow_map_2d_uni.set_int ( shadow_map.bind_map_loop () );
}



/* SHADOW_MAP_2D IMPLEMENTATION */

/* full constructor
*
 * initialise the texture to a given size and attach it to the fbo
 * 
 * width: the width and height to assign the texture to
 */
glh::lighting::shadow_map_2d::shadow_map_2d ( const unsigned width )
    : depth_texture { width, width, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT }
{
    /* attach the texture to the fbo and set the fbo to have no color component */
    shadow_fbo.attach_texture2d ( depth_texture, GL_DEPTH_ATTACHMENT );
    shadow_fbo.draw_buffer ( GL_NONE );
    shadow_fbo.read_buffer ( GL_NONE );
}

/* resize_map
 *
 * resize the texture used for the shadow map
 * 
 * width: the width/height to set the texture to
 */
void glh::lighting::shadow_map_2d::resize_map ( const unsigned width )
{
    /* resize the texture */
    depth_texture.set_texture ( width, width, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT );
}



/* SHADOW_MAP_CUBE IMPLEMENTATION */

/* full constructor
 *
 * initialise the cubemap to a given size and attach it to the fbo
 * 
 * width: the width and height to set the cubemap to
 */
glh::lighting::shadow_map_cube::shadow_map_cube ( const unsigned width )
    : depth_texture { width, width, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT }
{
    /* attach the cubemap to the fbo and set the fbo to have no color component */
    shadow_fbo.attach_cubemap ( depth_texture, GL_DEPTH_ATTACHMENT );
    shadow_fbo.draw_buffer ( GL_NONE );
    shadow_fbo.read_buffer ( GL_NONE );
}

/* resize_map
 *
 * resize the texture used for the shadow map
 * 
 * width: the width/height to set the texture to
 */
void glh::lighting::shadow_map_cube::resize_map ( const unsigned width )
{
    /* resize the texture */
    depth_texture.set_texture ( width, width, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT );
}



/* LIGHT_SYSTEM IMPLEMENTATION */

/* apply
 *
 * apply the lighting to uniforms
 * 
 * light_system_uni: the uniform to apply the lights to
 */
void glh::lighting::light_system::apply ( core::struct_uniform& light_system_uni )
{
    /* cache uniform */
    cache_uniforms ( light_system_uni );

    /* apply */
    apply ();
}
void glh::lighting::light_system::apply () const
{
    /* throw if no uniform is cached */
    if ( !cached_uniforms ) throw exception::uniform_exception { "attempted to apply light_system to uniform with out a complete uniform cache" };

    /* apply each light collection in turn */
    dirlights.apply ();
    pointlights.apply ();
    spotlights.apply ();
}

/* cache_uniforms
 *
 * cache uniforms for later use
 * 
 * light_system_uni: the uniform to cache
 */
void glh::lighting::light_system::cache_uniforms ( core::struct_uniform& light_system_uni )
{
    /* if uniforms are not already cached, cache the new ones */
    if ( !cached_uniforms || cached_uniforms->light_system_uni != light_system_uni )
    {
        cached_uniforms.reset ( new cached_uniforms_struct
        {
            light_system_uni
        } );
    }

    /* get light collections to cache their uniforms */
    dirlights.cache_uniforms ( cached_uniforms->light_system_uni.get_uniform ( "dirlights_size" ), cached_uniforms->light_system_uni.get_struct_array_uniform ( "dirlights" ) );
    pointlights.cache_uniforms ( cached_uniforms->light_system_uni.get_uniform ( "pointlights_size" ), cached_uniforms->light_system_uni.get_struct_array_uniform ( "pointlights" ) );
    spotlights.cache_uniforms ( cached_uniforms->light_system_uni.get_uniform ( "spotlights_size" ), cached_uniforms->light_system_uni.get_struct_array_uniform ( "spotlights" ) );
}

/* reload_uniforms
 *
 * reload the light collections based on the currently cached uniforms
 */
void glh::lighting::light_system::reload_uniforms ()
{
    /* recache uniforms */
    cache_uniforms ( cached_uniforms->light_system_uni );
}