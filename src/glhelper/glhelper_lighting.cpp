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



/* DIRLIGHT IMPLEMENTATION */

/* apply
 *
 * apply the lighting to a uniform
 * the uniform should be of type light_struct (see top)
 * 
 * light_uni: the uniform to apply the light to (which will be cached)
 */
void glh::lighting::dirlight::apply ( core::struct_uniform& light_uni )
{
    /* cache uniform */
    cache_uniforms ( light_uni );

    /* now apply the cached uniform */
    apply ();
}
void glh::lighting::dirlight::apply () const
{
    /* throw if no uniform is cached */
    if ( !cached_uniforms ) throw exception::uniform_exception { "attempted to apply dirlight to uniform with out a complete uniform cache" };

    /* update the shadow camera if necessary */
    if ( shadow_mapping_enabled && shadow_camera_change )
    {
        shadow_camera.set_position ( shadow_region.centre );
        shadow_camera.set_direction ( direction, math::any_perpandicular ( direction ) );
        shadow_camera.set_lbn ( math::vec3 { -shadow_region.radius } );
        shadow_camera.set_rtf ( math::vec3 { shadow_region.radius } );
        shadow_camera_change = false;
    }

    /* now set all of the uniform values */
    cached_uniforms->direction_uni.set_vector ( direction );
    cached_uniforms->ambient_color_uni.set_vector ( ambient_color );
    cached_uniforms->diffuse_color_uni.set_vector ( diffuse_color );
    cached_uniforms->specular_color_uni.set_vector ( specular_color );
    cached_uniforms->enabled_uni.set_int ( enabled );
    cached_uniforms->shadow_mapping_enabled_uni.set_int ( shadow_mapping_enabled );
    cached_uniforms->shadow_bias_uni.set_float ( shadow_bias );

    /* apply the shadow camera */
    shadow_camera.apply ();
}

/* cache_uniforms
 *
 * cache light uniforms for later use
 * 
 * light_uni: the uniform to cache
 */
void glh::lighting::dirlight::cache_uniforms ( core::struct_uniform& light_uni )
{
    /* if uniforms are not already cached, cache the new ones */
    if ( !cached_uniforms || cached_uniforms->light_uni != light_uni )
    {
        cached_uniforms.reset ( new cached_uniforms_struct
        {
            light_uni,
            light_uni.get_uniform ( "direction" ),
            light_uni.get_uniform ( "ambient_color" ),
            light_uni.get_uniform ( "diffuse_color" ),
            light_uni.get_uniform ( "specular_color" ),
            light_uni.get_uniform ( "enabled" ),
            light_uni.get_uniform ( "shadow_mapping_enabled" ),
            light_uni.get_uniform ( "shadow_bias" )
        } );
        shadow_camera.cache_uniforms ( light_uni.get_struct_uniform ( "shadow_camera" ) );
    }
}



/* POINTLIGHT IMPLEMENTATION */

/* apply
 *
 * apply the lighting to a uniform
 * the uniform should be of type light_struct (see top)
 * 
 * light_uni: the uniform to apply the light to (which will be cached)
 */
void glh::lighting::pointlight::apply ( core::struct_uniform& light_uni )
{
    /* cache uniform */
    cache_uniforms ( light_uni );

    /* now apply the cached uniform */
    apply ();
}
void glh::lighting::pointlight::apply () const
{
    /* throw if no uniform is cached */
    if ( !cached_uniforms ) throw exception::uniform_exception { "attempted to apply pointlight to uniform with out a complete uniform cache" };

    /* update the shadow camera if necessary */
    if ( shadow_camera_change )
    {
        shadow_camera.set_position ( position );
        shadow_camera.set_far ( math::modulus ( shadow_region.centre - position ) + shadow_region.radius );
        shadow_camera_change = false;
    }

    /* now set all of the uniform values */
    cached_uniforms->position_uni.set_vector ( position );
    cached_uniforms->att_const_uni.set_float ( att_const );
    cached_uniforms->att_linear_uni.set_float ( att_linear );
    cached_uniforms->att_quad_uni.set_float ( att_quad );
    cached_uniforms->ambient_color_uni.set_vector ( ambient_color );
    cached_uniforms->diffuse_color_uni.set_vector ( diffuse_color );
    cached_uniforms->specular_color_uni.set_vector ( specular_color );
    cached_uniforms->enabled_uni.set_int ( enabled );
    cached_uniforms->shadow_mapping_enabled_uni.set_int ( shadow_mapping_enabled );
    cached_uniforms->shadow_bias_uni.set_float ( shadow_bias );
    cached_uniforms->shadow_depth_range_mult_uni.set_float ( 1.0 / ( shadow_camera.get_far () * std::sqrt ( 2.0 ) ) );

    /* apply the shadow camera */
    shadow_camera.apply ();

    /* set the shadow cube matrices */
    cached_uniforms->shadow_cube_matrices_uni.at ( 0 ).set_matrix 
        ( shadow_camera.get_proj () * math::look_along ( math::vec3 { 0.0 }, math::vec3 { 1.0, 0.0, 0.0 }, math::vec3 { 0.0, -1.0, 0.0 } ) * shadow_camera.get_view () );
    cached_uniforms->shadow_cube_matrices_uni.at ( 1 ).set_matrix 
        ( shadow_camera.get_proj () * math::look_along ( math::vec3 { 0.0 }, math::vec3 { -1.0, 0.0, 0.0 }, math::vec3 { 0.0, -1.0, 0.0 } ) * shadow_camera.get_view () );
    cached_uniforms->shadow_cube_matrices_uni.at ( 2 ).set_matrix 
        ( shadow_camera.get_proj () * math::look_along ( math::vec3 { 0.0 }, math::vec3 { 0.0, 1.0, 0.0 }, math::vec3 { 0.0, 0.0, 1.0 } ) * shadow_camera.get_view () );
    cached_uniforms->shadow_cube_matrices_uni.at ( 3 ).set_matrix 
        ( shadow_camera.get_proj () * math::look_along ( math::vec3 { 0.0 }, math::vec3 { 0.0, -1.0, 0.0 }, math::vec3 { 0.0, 0.0, -1.0 } ) * shadow_camera.get_view () );
    cached_uniforms->shadow_cube_matrices_uni.at ( 4 ).set_matrix 
        ( shadow_camera.get_proj () * math::look_along ( math::vec3 { 0.0 }, math::vec3 { 0.0, 0.0, 1.0 }, math::vec3 { 0.0, -1.0, 0.0 } ) * shadow_camera.get_view () );
    cached_uniforms->shadow_cube_matrices_uni.at ( 5 ).set_matrix 
        ( shadow_camera.get_proj () * math::look_along ( math::vec3 { 0.0 }, math::vec3 { 0.0, 0.0, -1.0 }, math::vec3 { 0.0, -1.0, 0.0 } ) * shadow_camera.get_view () );
}

/* cache_uniforms
 *
 * cache light uniforms for later use
 * 
 * light_uni: the uniform to cache
 */
void glh::lighting::pointlight::cache_uniforms ( core::struct_uniform& light_uni )
{
    /* if uniforms are not already cached, cache the new ones */
    if ( !cached_uniforms || cached_uniforms->light_uni != light_uni )
    {
        cached_uniforms.reset ( new cached_uniforms_struct
        {
            light_uni,
            light_uni.get_uniform ( "position" ),
            light_uni.get_uniform ( "att_const" ),
            light_uni.get_uniform ( "att_linear" ),
            light_uni.get_uniform ( "att_quad" ),
            light_uni.get_uniform ( "ambient_color" ),
            light_uni.get_uniform ( "diffuse_color" ),
            light_uni.get_uniform ( "specular_color" ),
            light_uni.get_uniform ( "enabled" ),
            light_uni.get_uniform ( "shadow_mapping_enabled" ),
            light_uni.get_uniform_array_uniform ( "shadow_cube_matrices" ),
            light_uni.get_uniform ( "shadow_bias" ),
            light_uni.get_uniform ( "shadow_depth_range_mult" )
        } );
        shadow_camera.cache_uniforms ( light_uni.get_struct_uniform ( "shadow_camera" ) );
    }
}



/* SPOTLIGHT IMPLEMENTATION */

/* apply
 *
 * apply the lighting to a uniform
 * the uniform should be of type light_struct (see top)
 * 
 * light_uni: the uniform to apply the light to (which will be cached)
 */
void glh::lighting::spotlight::apply ( core::struct_uniform& light_uni )
{
    /* cache uniform */
    cache_uniforms ( light_uni );

    /* now apply the cached uniform */
    apply ();
}
void glh::lighting::spotlight::apply () const
{
    /* throw if no uniform is cached */
    if ( !cached_uniforms ) throw exception::uniform_exception { "attempted to apply spotlightlight to uniform with out a complete uniform cache" };

    /* update the shadow camera if necessary */
    if ( shadow_camera_change )
    {
        shadow_camera.set_position ( position );
        shadow_camera.set_direction ( direction, math::any_perpandicular ( direction ) );
        shadow_camera.set_far ( math::modulus ( shadow_region.centre - position ) + shadow_region.radius );
        shadow_camera.set_fov ( outer_cone * 2.0 );
        shadow_camera_change = false;
    }

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
    cached_uniforms->shadow_mapping_enabled_uni.set_int ( shadow_mapping_enabled );
    cached_uniforms->shadow_bias_uni.set_float ( shadow_bias );
    cached_uniforms->shadow_depth_range_mult_uni.set_float ( 1.0 / ( shadow_camera.get_far () * std::sqrt ( 2.0 ) ) );

    /* apply the shadow camera */
    shadow_camera.apply ();
}

/* cache_uniforms
 *
 * cache light uniforms for later use
 * 
 * light_uni: the uniform to cache
 */
void glh::lighting::spotlight::cache_uniforms ( core::struct_uniform& light_uni )
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
            light_uni.get_uniform ( "shadow_mapping_enabled" ),
            light_uni.get_uniform ( "shadow_bias" ),
            light_uni.get_uniform ( "shadow_depth_range_mult" )
        } );
        shadow_camera.cache_uniforms ( light_uni.get_struct_uniform ( "shadow_camera" ) );
    }
}



/* LIGHT_SYSTEM IMPLEMENTATION */

/* zero-parameter constructor */
glh::lighting::light_system::light_system ( const unsigned _shadow_map_width )
    : shadow_map_width { _shadow_map_width }
{
    /* assert that neither shadow map width is 0 */
    if ( shadow_map_width == 0 )
        throw exception::texture_exception { "shadow map width in light_system cannot be zero" };

    /* resize both shadow maps */
    shadow_maps_2d.tex_image ( shadow_map_width, shadow_map_width, 1, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT );
    shadow_maps_cube.tex_image ( shadow_map_width, shadow_map_width, 6, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT );

    /* set the 2d texture border color to be 0.0 and to clamp to border */
    shadow_maps_2d.set_border_color ( math::vec4 { 1.0 } ); shadow_maps_2d.set_s_wrap ( GL_CLAMP_TO_BORDER ); shadow_maps_2d.set_t_wrap ( GL_CLAMP_TO_BORDER );
   
    /* set cubemap to clamp to edge */
    shadow_maps_cube.set_s_wrap ( GL_CLAMP_TO_EDGE ); shadow_maps_cube.set_t_wrap ( GL_CLAMP_TO_EDGE ); shadow_maps_cube.set_r_wrap ( GL_CLAMP_TO_EDGE );

    /* set both shadow maps to use linear interpolation */
    shadow_maps_2d.set_mag_filter ( GL_LINEAR ); shadow_maps_2d.set_min_filter ( GL_LINEAR );
    shadow_maps_cube.set_mag_filter ( GL_LINEAR ); shadow_maps_cube.set_min_filter ( GL_LINEAR );

    /* set both shadow maps to use comparison */
    shadow_maps_2d.set_compare_mode ( GL_COMPARE_REF_TO_TEXTURE ); shadow_maps_2d.set_compare_func ( GL_LEQUAL );
    shadow_maps_cube.set_compare_mode ( GL_COMPARE_REF_TO_TEXTURE ); shadow_maps_cube.set_compare_func ( GL_LEQUAL );

    /* add the shadow map textures as attachments to their fbos */
    shadow_maps_2d_fbo.attach_texture ( shadow_maps_2d, GL_DEPTH_ATTACHMENT );
    shadow_maps_cube_fbo.attach_texture ( shadow_maps_cube, GL_DEPTH_ATTACHMENT );

    /* set the fbos to have no color attachments */
    shadow_maps_2d_fbo.read_buffer ( GL_NONE ); shadow_maps_2d_fbo.draw_buffer ( GL_NONE );
    shadow_maps_cube_fbo.read_buffer ( GL_NONE ); shadow_maps_cube_fbo.draw_buffer ( GL_NONE );
}

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

    /* apply all lights */
    cached_uniforms->dirlights_size_uni.set_int ( dirlights.size () );
    for ( unsigned i = 0; i < dirlights.size (); ++i ) dirlights.at ( i ).apply ();
    cached_uniforms->pointlights_size_uni.set_int ( pointlights.size () );
    for ( unsigned i = 0; i < pointlights.size (); ++i ) pointlights.at ( i ).apply ();
    cached_uniforms->spotlights_size_uni.set_int ( spotlights.size () );
    for ( unsigned i = 0; i < spotlights.size (); ++i ) spotlights.at ( i ).apply ();

    /* bind shadow maps */
    cached_uniforms->shadow_maps_2d_uni.set_int ( shadow_maps_2d.bind_loop () );
    cached_uniforms->shadow_maps_cube_uni.set_int ( shadow_maps_cube.bind_loop () );
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
            light_system_uni,
            light_system_uni.get_uniform ( "dirlights_size" ),
            light_system_uni.get_struct_array_uniform ( "dirlights" ),
            light_system_uni.get_uniform ( "pointlights_size" ),
            light_system_uni.get_struct_array_uniform ( "pointlights" ),
            light_system_uni.get_uniform ( "spotlights_size" ),
            light_system_uni.get_struct_array_uniform ( "spotlights" ),
            light_system_uni.get_uniform ( "shadow_maps_2d" ),
            light_system_uni.get_uniform ( "shadow_maps_cube" )
        } );
    }

    /* loop through all lights and cache uniforms */
    for ( unsigned i = 0; i < dirlights.size (); ++i ) dirlights.at ( i ).cache_uniforms ( cached_uniforms->dirlights_uni.at ( i ) );
    for ( unsigned i = 0; i < pointlights.size (); ++i ) pointlights.at ( i ).cache_uniforms ( cached_uniforms->pointlights_uni.at ( i ) );
    for ( unsigned i = 0; i < spotlights.size (); ++i ) spotlights.at ( i ).cache_uniforms ( cached_uniforms->spotlights_uni.at ( i ) );
}

/* recache_uniforms
 *
 * recache the light collections based on the currently cached uniforms
 */
void glh::lighting::light_system::recache_uniforms ()
{
    /* recache uniforms */
    cache_uniforms ( cached_uniforms->light_system_uni );
}

/* bind_shadow_maps_2d/cube_fbo
 *
 * 2d: reallocates the 2d texture array to size max ( dirlights + spotlights, 1 ), then binds the 2d shadow map fbo
 * cube: reallocates the cubemap array to size max ( pointlights, 1 ) * 6, then binds the cube shadow map fbo
 */
void glh::lighting::light_system::bind_shadow_maps_2d_fbo () const
{
    /* resize the texture array if necessary
     * ensure that at least one light is allocated, so that fbo is complete
     */
    const unsigned tex_depth = std::max<unsigned> ( dirlights.size () + spotlights.size (), 1 );
    if ( tex_depth != shadow_maps_2d.get_depth () )
        shadow_maps_2d.tex_image ( shadow_maps_2d.get_width (), shadow_maps_2d.get_height (), tex_depth, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT ); 

    /* check the fbo is complete */
    if ( !shadow_maps_2d_fbo.is_complete () ) throw exception::texture_exception { "2d shadow map fbo is not complete" };

    /* bind the fbo */
    shadow_maps_2d_fbo.bind ();
}
void glh::lighting::light_system::bind_shadow_maps_cube_fbo () const
{
    /* resize the texture arrays if necessary
     * ensure that at least one light is allocated, so that fbo is complete
     */
    const unsigned tex_depth = std::max<unsigned> ( dirlights.size (), 1 ) * 6;
    if ( tex_depth != shadow_maps_cube.get_depth () )
        shadow_maps_cube.tex_image ( shadow_maps_cube.get_width (), shadow_maps_cube.get_height (), tex_depth, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT ); 
    
    /* check the fbo is complete */
    if ( !shadow_maps_cube_fbo.is_complete () ) throw exception::texture_exception { "cube shadow map fbo is not complete" };

    /* bind the fbo */
    shadow_maps_cube_fbo.bind ();
}

/* requires_2d/cube_shadow_mapping
 *
 * true if there are lights which require 2d or cube shadow mapping respectively
 */
bool glh::lighting::light_system::requires_2d_shadow_mapping () const
{
    /* loop through dirlights and determine if any require shadow sampling */
    for ( const dirlight& light: dirlights ) if ( light.is_enabled () && light.is_shadow_mapping_enabled () ) return true;
    for ( const spotlight& light: spotlights ) if ( light.is_enabled () && light.is_shadow_mapping_enabled () ) return true;

    /* else return false */
    return false;
}
bool glh::lighting::light_system::requires_cube_shadow_mapping () const
{
    /* loop through spotlights and determine if any require shadow sampling */
    for ( const pointlight& light: pointlights ) if ( light.is_enabled () && light.is_shadow_mapping_enabled () ) return true;

    /* else return false */
    return false; 
}