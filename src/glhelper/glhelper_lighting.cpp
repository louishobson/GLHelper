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
    if ( shadow_camera_change )
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
            light_uni.get_uniform ( "shadow_mapping_enabled" )
        } );
        shadow_camera.cache_uniforms ( light_uni.get_uniform ( "shadow_view" ), light_uni.get_uniform ( "shadow_proj" ) );
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

    /* apply the shadow camera */
    shadow_camera.apply ();
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
            light_uni.get_uniform ( "shadow_mapping_enabled" )
        } );
        shadow_camera.cache_uniforms ( light_uni.get_uniform ( "shadow_view" ), light_uni.get_uniform ( "shadow_proj" ) );
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
        shadow_camera.set_fov ( outer_cone );
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
            light_uni.get_uniform ( "shadow_mapping_enabled" )
        } );
        shadow_camera.cache_uniforms ( light_uni.get_uniform ( "shadow_view" ), light_uni.get_uniform ( "shadow_proj" ) );
    }
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

    /* apply all lights */
    cached_uniforms->dirlights_size_uni.set_int ( dirlights.size () );
    for ( unsigned i = 0; i < dirlights.size (); ++i ) dirlights.at ( i ).apply ();
    cached_uniforms->pointlights_size_uni.set_int ( pointlights.size () );
    for ( unsigned i = 0; i < pointlights.size (); ++i ) pointlights.at ( i ).apply ();
    cached_uniforms->spotlights_size_uni.set_int ( spotlights.size () );
    for ( unsigned i = 0; i < spotlights.size (); ++i ) spotlights.at ( i ).apply ();
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
            light_system_uni.get_struct_array_uniform ( "pointlights" ),
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