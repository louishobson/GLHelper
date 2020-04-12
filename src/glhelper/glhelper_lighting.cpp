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

/* copy assignment operator
 *
 * non-default for the same reason as copy constructor
 */
glh::light& glh::light::operator= ( const light& other )
{
    /* set all values, ignoring cached uniform */
    position = other.position; direction = other.direction;
    inner_cone = other.inner_cone; outer_cone = other.outer_cone;
    att_const = other.att_const; att_linear = other.att_linear; att_quad = other.att_quad;
    ambient_color = other.ambient_color; diffuse_color = other.diffuse_color; specular_color = other.specular_color;
    enabled = other.enabled;

    /* return object */
    return * this;
}

/* apply
 *
 * apply the lighting to a uniform
 * the uniform should be of type light_struct (see top)
 * 
 * light_uni: the uniform to apply the light to (which will be cached)
 */
void glh::light::apply ( const struct_uniform& light_uni ) const
{
    /* cache uniform */
    cache_uniforms ( light_uni );

    /* now apply the cached uniform */
    apply ();
}
void glh::light::apply () const
{
    /* throw if no uniform is cached */
    if ( !cached_uniforms ) throw uniform_exception { "attempted to apply light to uniform with out a complete uniform cache" };

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
}

/* cache_uniforms
 *
 * cache light uniforms for later use
 * 
 * light_uni: the uniform to cache
 */
void glh::light::cache_uniforms ( const struct_uniform& light_uni ) const
{
    /* cache uniform, if not already cached */
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
            light_uni.get_uniform ( "enabled" )
        } );
    }
}



/* LIGHT_SYSTEM IMPLEMENTATION */

/* apply
 *
 * apply the lighting to uniforms
 * 
 * light_system_uni: the uniform to apply the lights to
 */
void glh::light_system::apply ( const struct_uniform& light_system_uni ) const
{
    /* cache uniform */
    cache_uniforms ( light_system_uni );

    /* apply */
    apply ();
}
void glh::light_system::apply () const
{
    /* throw if no uniform is cached */
    if ( !cached_uniforms ) throw uniform_exception { "attempted to apply light_system to uniform with out a complete uniform cache" };

    /* apply each light collection in turn */
    dircoll.apply ( cached_uniforms->dircoll_uni );
    pointcoll.apply ( cached_uniforms->pointcoll_uni );
    spotcoll.apply ( cached_uniforms->spotcoll_uni );
}

/* cache_uniforms
 *
 * cache uniforms for later use
 * 
 * light_system_uni: the uniform to cache
 */
void glh::light_system::cache_uniforms ( const struct_uniform& light_system_uni ) const
{
    /* if not already cached, cache uniforms */
    if ( !cached_uniforms || cached_uniforms->light_system_uni != light_system_uni )
    {
        cached_uniforms.reset ( new cached_uniforms_struct
        {
            light_system_uni,
            light_system_uni.get_struct_uniform ( "dircoll" ),
            light_system_uni.get_struct_uniform ( "pointcoll" ),
            light_system_uni.get_struct_uniform ( "spotcoll" )
        } );
    }
}