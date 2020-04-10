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
    if ( !cached_uniforms ) throw lighting_exception { "attempted to apply light to uniform with out a complete uniform cache" };

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



/* LIGHTING IMPLEMENTATION */

/* apply
 *
 * apply the lighting to a uniform
 * the uniform should be of type lighting_struct (see top)
 * 
 * lightint_uni: the uniform to apply the lights to (which will be cached)
 */
void glh::lighting::apply ( const struct_uniform& lighting_uni ) const
{
    /* update the uniform cache */
    cache_uniforms ( lighting_uni );

    /* apply */
    apply ();
}
void glh::lighting::apply () const
{
    /* throw if no uniform is cached */
    if ( !cached_uniforms ) throw lighting_exception { "attempted to apply lighting to uniform with out a complete uniform cache" };

    /* set the size of dirlights */
    cached_uniforms->dirlights_size_uni.set_int ( dirlights.size () );
    /* apply the dirlight uniforms */
    for ( unsigned i = 0; i < dirlights.size (); ++i ) dirlights.at ( i ).apply ( cached_uniforms->dirlights_uni.at ( i ) );

    /* set the size of pointlights */
    cached_uniforms->pointlights_size_uni.set_int ( pointlights.size () );
    /* apply the pointlight uniforms */
    for ( unsigned i = 0; i < pointlights.size (); ++i ) pointlights.at ( i ).apply ( cached_uniforms->pointlights_uni.at ( i ) );

    /* set the size of spotlights */
    cached_uniforms->spotlights_size_uni.set_int ( spotlights.size () );
    /* apply the spotlight uniforms */
    for ( unsigned i = 0; i < spotlights.size (); ++i ) spotlights.at ( i ).apply ( cached_uniforms->spotlights_uni.at ( i ) );
}

/* cache_uniforms
 *
 * cache lighting uniforms for later use
 * 
 * lighting_uni: the uniform to cache
 */
void glh::lighting::cache_uniforms ( const struct_uniform& lighting_uni ) const
{
    /* cache uniform, if not already cached */
    if ( !cached_uniforms || cached_uniforms->lighting_uni != lighting_uni )
    {
        cached_uniforms.reset ( new cached_uniforms_struct
        {
            lighting_uni,
            lighting_uni.get_uniform ( "dirlights_size" ),
            lighting_uni.get_array_uniform<struct_uniform> ( "dirlights" ),
            lighting_uni.get_uniform ( "pointlights_size" ),
            lighting_uni.get_array_uniform<struct_uniform> ( "pointlights" ),
            lighting_uni.get_uniform ( "spotlights_size" ),
            lighting_uni.get_array_uniform<struct_uniform> ( "spotlights" )
        } );
    }
}