/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * include/glhelper/glhelper_lighting.hpp
 * 
 * handling of setting up lighting in shaders
 * 
 * 
 * 
 * lighting classes expects uniform data to be formatted in a specific way to function
 * 
 * the formats and structures expected to be found in the program are explained below
 * 
 * although the names of the actual structure types are not of importance,
 * the variable names within the struct cannot be modified
 * 
 * 
 * 
 * LIGHT_STRUCT
 * 
 * struct light_struct
 * {
 *     vec3 position;
 *     vec3 direction;
 * 
 *     float inner_cone;
 *     float outer_cone;
 * 
 *     float att_const;
 *     float att_linear;
 *     float att_quad;
 * 
 *     vec3 ambient_color;
 *     vec3 diffuse_color;
 *     vec3 specular_color;
 * 
 *     bool enabled;
 * };
 * 
 * this structure contains a single light source of any type (directional/point/spotlight)
 * 
 * position: the position of the light (only for point or spot lights)
 * direction: the direction of the light (only for directional or spot lights)
 * inner_cone: angle in radians of the inner cone of a spotlight (spotlight only)
 * outer_cone: angle in radians of the outer cone of a spotlight (spotlight only)
 * att_const/linear/quad: attenuation parameters for the light (only for point and spot lights)
 * ambient/diffuse/specular_color: the colors of the different components of light produced (all types)
 * enabled: whether the light is 'turned on' (all types)
 * 
 * 
 * 
 * LIGHT_COLLECTION_STRUCT
 * 
 * struct light_collection_struct
 * {
 *     int size;
 *     light_struct lights [];
 * };
 * 
 * this structure holds an array of lights
 * the size of the array must be large enough to store as many lights as required
 * the idea is that the lights in the array are all of the same type, however they can be any type you wish
 * one could have more than one light collection for different types of light
 * 
 * size: the number of lights being held
 * lights: array of lights
 * 
 * 
 * 
 * LIGHT_SYSTEM_STRUCT
 * 
 * struct light_system_struct
 * {
 *     light_collection_struct dircoll;
 *     light_collection_struct pointcoll;
 *     light_collection_struct spotcoll;
 * };
 * 
 * this structure holds multiple types of collections of lights
 * 
 * dircoll: collection of dirrectional lights
 * pointcoll: collection of point lights
 * spotcoll: collection of spotlights
 * 
 */



/* HEADER GUARD */
#ifndef GLHELPER_LIGHTING_HPP_INCLUDED
#define GLHELPER_LIGHTING_HPP_INCLUDED



/* INCLUDES */

/* include core headers */
#include <iostream>
#include <memory>
#include <vector>

/* include glhelper_core.hpp */
#include <glhelper/glhelper_core.hpp>

/* include glhelper_exception.hpp */
#include <glhelper/glhelper_exception.hpp>

/* include glhelper_shader.hpp */
#include <glhelper/glhelper_shader.hpp>

/* include glhelper_math.hpp */
#include <glhelper/glhelper_math.hpp>



/* NAMESPACE FORWARD DECLARATIONS */

namespace glh
{
    /* class light
     *
     * abstract base class for any type of light
     */
    class light;

    /* is_light
     *
     * is_light::value is true, if the type is a light
     */
    template<class T> struct is_light;

    /* class dirlight : light
     *
     * directional light class
     */
    class dirlight;

    /* class pointlight : light
     *
     * point light class
     */
    class pointlight;

    /* class spotlight : light
     *
     * spotlight class
     */
    class spotlight;

    /* class light_collection
     *
     * class to store multiple lights of the same type
     */
    template<class T> class light_collection;

    /* using declarations for light_collection
     *
     * simplifies template
     */
    using dirlight_collection = light_collection<dirlight>;
    using pointlight_collection = light_collection<pointlight>;
    using spotlight_collection = light_collection<spotlight>;

    /* class light_system
     *
     * several collection of lights for each type
     */
    class light_system;
}



/* LIGHT DEFINITION */

/* class light
 *
 * abstract base class for any type of light
 */
class glh::light
{
public:

    /* full constructor
     *
     * set all of the values of the light
     */
    light ( const math::vec3& _position, const math::vec3& _direction
          , const double _inner_cone, const double _outer_cone, const double _att_const, const double _att_linear, const double _att_quad
          , const math::vec3& _ambient_color, const math::vec3& _diffuse_color, const math::vec3& _specular_color
          , const bool _enabled = true )
        : position { _position }, direction { _direction }
        , inner_cone { _inner_cone }, outer_cone { _outer_cone }, att_const { _att_const }, att_linear { _att_linear }, att_quad { _att_quad }
        , ambient_color { _ambient_color }, diffuse_color { _diffuse_color }, specular_color { _specular_color }
        , enabled { _enabled }
    {}

    /* zero-parameter constructor
     *
     * a black light at the origin with no direction and no attenuation
     */
    light ()
        : light { math::vec3 { 0.0 }, math::vec3 { 0.0 }
                , 0.0, 0.0, 1.0, 0.0, 0.0
                , math::vec3 { 0.0 }, math::vec3 { 0.0 }, math::vec3 { 0.0 } }
    {}

    /* copy constructor
     *
     * not default as cannot copy cached uniforms
     */
    light ( const light& other )
        : position { other.position }, direction { other.direction }
        , inner_cone { other.inner_cone }, outer_cone { other.outer_cone }, att_const { other.att_const }, att_linear { other.att_linear }, att_quad { other.att_quad }
        , ambient_color { other.ambient_color }, diffuse_color { other.diffuse_color }, specular_color { other.specular_color }
        , enabled { other.enabled }
    {}

    /* default move constructor */
    light ( light&& other ) = default;

    /* copy assignment operator
     *
     * non-default for the same reason as copy constructor
     */
    light& operator= ( const light& other );

    /* default move assignment operator */
    light& operator= ( light&& other ) = default;

    /* default virtual destructor */
    virtual ~light () = default;



    /* apply
     *
     * apply the lighting to a uniform
     * the uniform should be of type light_struct (see top)
     * 
     * light_uni: the uniform to apply the light to (which will be cached)
     */
    void apply ( const struct_uniform& light_uni ) const;
    void apply () const;

    /* cache_uniforms
     *
     * cache light uniforms for later use
     * 
     * light_uni: the uniform to cache
     */
    void cache_uniforms ( const struct_uniform& light_uni ) const;



    /* get/set_position
     *
     * get/set the position of the light
     */
    const math::vec3 get_position () const { return position; }
    void set_position ( const math::vec3& _position ) { position = _position; }

    /* get/set_direction
     *
     * get/set the direction of the light
     */
    const math::vec3 get_direction () const { return direction; }
    void set_direction ( const math::vec3& _direction ) { direction = _direction; }

    /* get/set_inner/outer_cone
     *
     * get/set the different cone angles
     */
    const double& get_inner_cone () const { return inner_cone; }
    void set_inner_cone ( const double _inner_cone ) { inner_cone = _inner_cone; }
    const double& get_outer_cone () const { return outer_cone; }
    void set_outer_cone ( const double _outer_cone ) { inner_cone = _outer_cone; }

    /* get/set_att_...
     *
     * get/set attenuation constants
     */
    const double& get_att_const () const { return att_const; }
    void set_att_const ( const double _att_const ) { att_const = _att_const; }
    const double& get_att_linear () const { return att_linear; }
    void set_att_linear ( const double _att_linear ) { att_linear = _att_linear; }
    const double& get_att_quad () const { return att_quad; }
    void set_att_quad ( const double _att_quad ) { att_quad = _att_quad; }

    /* get/set_ambient/diffuse/specular_color
     *
     * get/set color components
     */
    const math::vec3& get_ambient_color () const { return ambient_color; }
    const math::vec3& get_diffuse_color () const { return diffuse_color; }
    const math::vec3& get_specular_color () const { return specular_color; }



private:

    /* position of the light */
    math::vec3 position;

    /* direction of the light */
    math::vec3 direction;

    /* inner cone and outer cone angles of spotlights */
    double inner_cone;
    double outer_cone;

    /* attenuation parameters */
    double att_const;
    double att_linear;
    double att_quad;

    /* colors of light */
    math::vec3 ambient_color;
    math::vec3 diffuse_color;
    math::vec3 specular_color;

    /* whether the light is enables */
    bool enabled;

    /* struct for cached uniforms */
    struct cached_uniforms_struct
    {
        struct_uniform light_uni;
        uniform position_uni;
        uniform direction_uni;
        uniform inner_cone_uni;
        uniform outer_cone_uni;
        uniform att_const_uni;
        uniform att_linear_uni;
        uniform att_quad_uni;
        uniform ambient_color_uni;
        uniform diffuse_color_uni;
        uniform specular_color_uni;
        uniform enabled_uni;
    };

    /* cached uniforms */
    mutable std::unique_ptr<cached_uniforms_struct> cached_uniforms;

};



/* IS_LIGHT DEFINITION */

/* struct is_light
 *
 * is_light::value is true if the type is a light
 */
template<class T> struct glh::is_light
{
    static const bool value = false;
    operator bool () { return value; } 
};
template<> struct glh::is_light<glh::dirlight>
{
    static const bool value = true;
    operator bool () { return value; } 
};
template<> struct glh::is_light<glh::pointlight>
{
    static const bool value = true;
    operator bool () { return value; } 
};
template<> struct glh::is_light<glh::spotlight>
{
    static const bool value = true;
    operator bool () { return value; } 
};



/* DIRLIGHT DEFINITION */

/* class dirlight : light
 *
 * directional light class
 */
class glh::dirlight : public light
{
public:

    /* full constructor
     *
     * set all of the values associated with directional light
     */
    dirlight ( const math::vec3& _direction
             , const math::vec3& _ambient_color, const math::vec3& _diffuse_color, const math::vec3& _specular_color
             , const bool _enabled = true )
        : light { math::vec3 { 0.0 }, _direction, 0.0, 0.0, 1.0, 0.0, 0.0, _ambient_color, _diffuse_color, _specular_color, _enabled }
    {}

    /* default zero-parameter constructor */
    dirlight () = default;

    /* default copy constructor */
    dirlight ( const dirlight& other ) = default;

    /* default move constructor
     *
     * this will use the more efficient move constructor on base class
     */
    dirlight ( dirlight&& other ) = default;

    /* default copy assignment operator */
    dirlight& operator= ( const dirlight& other ) = default;

    /* default move assignment operator
     *
     * this will use the more efficient move assignment operator on base class
     */
    dirlight& operator= ( dirlight&& other ) = default;

    /* default destructor */
    ~dirlight () = default;



private:

    /* make get/set_position functions private */
    using light::get_position;
    using light::set_position;

    /* make get/set_att_... functions private */
    using light::get_att_const;
    using light::set_att_const;
    using light::get_att_linear;
    using light::set_att_linear;
    using light::get_att_quad;
    using light::set_att_quad;

    /* make get/set_inner/outer_cone functions private */
    using light::get_inner_cone;
    using light::set_inner_cone;
    using light::get_outer_cone;
    using light::set_outer_cone;

};



/* POINTLIGHT DEFINITION */

/* class pointlight : light
 *
 * point light class
 */
class glh::pointlight : public light
{
public:

    /* full constructor
     *
     * set all of the values associated with directional light
     */
    pointlight ( const math::vec3& _position
               , const double _att_const, const double _att_linear, const double _att_quad
               , const math::vec3& _ambient_color, const math::vec3& _diffuse_color, const math::vec3& _specular_color
               , const bool _enabled = true )
        : light { _position, math::vec3 { 0.0 }, 0.0, 0.0, _att_const, _att_linear, _att_quad, _ambient_color, _diffuse_color, _specular_color, _enabled }
    {}

    /* default zero-parameter constructor */
    pointlight () = default;

    /* default copy constructor */
    pointlight ( const pointlight& other ) = default;

    /* default move constructor
     *
     * this will use the more efficient move constructor on base class
     */
    pointlight ( pointlight&& other ) = default;

    /* default copy assignment operator */
    pointlight& operator= ( const pointlight& other ) = default;

    /* default move assignment operator
     *
     * this will use the more efficient move assignment operator on base class
     */
    pointlight& operator= ( pointlight&& other ) = default;

    /* default destructor */
    ~pointlight () = default;



private:

    /* make get/set_direction functions private */
    using light::get_direction;
    using light::set_direction;

    /* make get/set_inner/outer_cone functions private */
    using light::get_inner_cone;
    using light::set_inner_cone;
    using light::get_outer_cone;
    using light::set_outer_cone;

};



/* SPOTLIGHT DEFINITION */

/* class spotlight : light
 *
 * spotlight class
 */
class glh::spotlight : public light
{
public:

    /* full constructor
     *
     * set all of the values associated with directional light
     */
    spotlight ( const math::vec3& _position, const math::vec3& _direction
              , const double _inner_cone, const double _outer_cone,const double _att_const, const double _att_linear, const double _att_quad
              , const math::vec3& _ambient_color, const math::vec3& _diffuse_color, const math::vec3& _specular_color
              , const bool _enabled = true )
        : light { _position, _direction, _inner_cone, _outer_cone, _att_const, _att_linear, _att_quad, _ambient_color, _diffuse_color, _specular_color, _enabled }
    {}

    /* default zero-parameter constructor */
    spotlight () = default;

    /* default copy constructor */
    spotlight ( const spotlight& other ) = default;

    /* default move constructor
     *
     * this will use the more efficient move constructor on base class
     */
    spotlight ( spotlight&& other ) = default;

    /* default copy assignment operator */
    spotlight& operator= ( const spotlight& other ) = default;

    /* default move assignment operator
     *
     * this will use the more efficient move assignment operator on base class
     */
    spotlight& operator= ( spotlight&& other ) = default;

    /* default destructor */
    ~spotlight () = default;

};



/* LIGHT_COLLECTION DEFINITION */

/* class light_collection
 *
 * class to store multiple lights of the same type
 */
template<class T = glh::light> class glh::light_collection
{
    /* static assert that T is a light */
    static_assert ( is_light<T>::value, "cannot create light_collection object containing non-light type" );

public:

    /* default constructor */
    light_collection () = default;

    /* copy constructor
     *
     * not default as cannot copy uniform cache
     */
    light_collection ( const light_collection& other )
        : lights { other.lights }
    {}

    /* default move constructor */
    light_collection ( light_collection&& other ) = default;

    /* copy assignment operator
     *
     * not default for same reason as copy constructor
     */
    light_collection& operator= ( const light_collection& other )
    { lights = other.lights; return * this; }

    /* default move assignment operator */
    light_collection& operator= ( light_collection&& other ) = default;



    /* array of lights */
    std::vector<T> lights;



    /* apply
     *
     * apply the lighting to uniforms
     * 
     * light_collection_uni: the uniform to apply the lights to
     */
    void apply ( const struct_uniform& light_collection_uni ) const;
    void apply () const;

    /* cache_uniforms
     *
     * cache uniforms for later use
     * 
     * light_collection_uni: the uniform to cache
     */
    void cache_uniforms ( const struct_uniform& light_collection_uni ) const;



private:

    /* struct for cached uniforms */
    struct cached_uniforms_struct
    {
        struct_uniform light_collection_uni;
        uniform size_uni;
        struct_array_uniform lights_uni;
    };

    /* cached uniforms */
    mutable std::unique_ptr<cached_uniforms_struct> cached_uniforms;
};



/* LIGHT_SYSTEM DEFINITION */

/* class light_system
 *
 * several collection of lights for each type
 */
class glh::light_system
{
public:

    /* default constructor */
    light_system () = default;

    /* copy constructor
     *
     * not default as cannot copy uniform cache
     */
    light_system ( const light_system& other )
        : dircoll { other.dircoll }
        , pointcoll { other.pointcoll }
        , spotcoll { other.spotcoll }
    {}

    /* default move constructor */
    light_system ( light_system&& other ) = default;

    /* copy assignment operator 
     *
     * not default for same reason as copy constructor
     */
    light_system& operator= ( const light_system& other )
    { dircoll = other.dircoll; pointcoll = other.pointcoll; spotcoll = other.spotcoll; return * this; }

    /* default move assignment operator */
    light_system& operator= ( light_system&& other ) = default;

    /* default destructor */
    ~light_system () = default;



    /* light collections */
    dirlight_collection dircoll;
    pointlight_collection pointcoll;
    spotlight_collection spotcoll;



    /* apply
     *
     * apply the lighting to uniforms
     * 
     * light_system_uni: the uniform to apply the lights to
     */
    void apply ( const struct_uniform& light_system_uni ) const;
    void apply () const;

    /* cache_uniforms
     *
     * cache uniforms for later use
     * 
     * light_system_uni: the uniform to cache
     */
    void cache_uniforms ( const struct_uniform& light_system_uni ) const;



private:

    /* struct for cached uniforms */
    struct cached_uniforms_struct
    {
        struct_uniform light_system_uni;
        struct_uniform dircoll_uni;
        struct_uniform pointcoll_uni;
        struct_uniform spotcoll_uni;
    };

    /* cached uniforms */
    mutable std::unique_ptr<cached_uniforms_struct> cached_uniforms;

};



/* LIGHT_COLLECTION IMPLEMENTATION */

/* apply
 *
 * apply the lighting to uniforms
 * 
 * light_collection_uni: the uniform to apply the lights to
 */
template<class T>
inline void glh::light_collection<T>::apply ( const struct_uniform& light_collection_uni ) const
{
    /* cache uniform */
    cache_uniforms ( light_collection_uni );

    /* apply */
    apply ();
}
template<class T>
inline void glh::light_collection<T>::apply () const
{
    /* throw if no uniform is cached */
    if ( !cached_uniforms ) throw uniform_exception { "attempted to apply light_collection to uniform with out a complete uniform cache" };

    /* set uniforms */
    cached_uniforms->size_uni.set_int ( lights.size () );
    for ( unsigned i = 0; i < lights.size (); ++i ) lights.at ( i ).apply ( cached_uniforms->lights_uni.at ( i ) );
}

/* cache_uniforms
 *
 * cache uniforms for later use
 * 
 * light_collection_uni: the uniform to cache
 */
template<class T>
inline void glh::light_collection<T>::cache_uniforms ( const struct_uniform& light_collection_uni ) const
{
    /* if not already cached, cache uniforms */
    if ( !cached_uniforms || cached_uniforms->light_collection_uni != light_collection_uni )
    {
        cached_uniforms.reset ( new cached_uniforms_struct
        {
            light_collection_uni,
            light_collection_uni.get_uniform ( "size" ),
            light_collection_uni.get_struct_array_uniform ( "lights" )
        } );
    }
}



/* #ifndef GLHELPER_LIGHTING_HPP_INCLUDED */
#endif