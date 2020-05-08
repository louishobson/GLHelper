/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * include/glhelper/glhelper_lighting.hpp
 * 
 * handling of setting up lighting in shaders
 * lighting classes expects uniform data to be formatted in a specific way to function
 * the formats and structures expected to be found in the shader program are explained along with the C++ classes
 * although the names of the actual structure types are not of importance,
 * the variable names within the structs cannot be modified
 * notable constructs include:
 * 
 * 
 * 
 * CLASS GLH::LIGHTING::LIGHT
 * 
 * a class storing all of the possible attributes of any type of light
 * these attributes can be applied to a light_struct struct uniform:
 * 
 * 
 * 
 * GLSL STRUCT LIGHT_STRUCT
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
 * this GLSL structure contains a single light source of any type (directional/point/spotlight)
 * this is the structure the glh::lighting::light class expects to be supplied with to write to
 * 
 * position: the position of the light (only for point or spot lights)
 * direction: the direction of the light (only for directional or spot lights)
 * inner_cone: angle in radians of the inner cone of a spotlight (spotlight only)
 * outer_cone: angle in radians of the outer cone of a spotlight (spotlight only)
 * att_const/linear/quad: attenuation attributes for the light (only for point and spot lights)
 * ambient/diffuse/specular_color: the colors of the different components of light produced (all types)
 * enabled: whether the light is 'turned on' (all types)
 * 
 * 
 * 
 * CLASS GLH::LIGHTING::DIRLIGHT and ::POINTLIGHT and ::SPOTLIGHT
 * 
 * these classes are derivations of glh::lighting::light
 * for each, the irrelevant light attributes to that type are made private
 * this is purely for ease of use and to omit confusion about the type of light being modified
 * the attributes made private are defaulted, but still written to the struct uniform by the base class's apply method
 * 
 * 
 * 
 * CLASS GLH::LIGHTING::LIGHT_COLLECTION
 * 
 * template class to store a dynamically-allocated array of lights
 * the template parameter must be a type of light, but it defaults to glh::lighting::light
 * there are using declarations to abstract the template (e.g. dirlight_collection)
 * the only reason this class exists is so that applying all of the lights to uniforms is made easier
 * the collection can be applied to a light_collection_struct struct uniform:
 * 
 * 
 * 
 * GLSL STRUCT LIGHT_COLLECTION_STRUCT
 * 
 * struct light_collection_struct
 * {
 *     int size;
 *     light_struct lights [];
 * };
 * 
 * this structure holds an array of lights
 * this is the structure the glh::lighting::light_collection class expects to be supplied with to write to
 * the size of the array must be large enough to store as many lights as required
 * the idea is that the lights in the array are all of the same type, however they can be any type you wish
 * one could have more than one light collection for different types of light, for example
 * 
 * size: the number of lights being held
 * lights: array of lights
 * 
 * 
 * 
 * CLASS GLH::LIGHTING::LIGHT_SYSTEM
 * 
 * class to store three collections: one dirlight, one pointlight and one spotlight collection
 * similar to the theory behind the light_collection class, this simplifies applying the light collections to a uniform
 * the collection can be applied to a light_system_struct struct uniform:
 * 
 * 
 * 
 * GLSL STRUCT LIGHT_SYSTEM_STRUCT
 * 
 * struct light_system_struct
 * {
 *     light_collection_struct dircoll;
 *     light_collection_struct pointcoll;
 *     light_collection_struct spotcoll;
 * };
 * 
 * this structure holds multiple types of collections of lights
 * this is the structure the glh::lighting::light_system class expects to be supplied with to write to
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
#include <type_traits>
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
    namespace lighting
    {
        /* class light
         *
         * abstract base class for any type of light
         */
        class light;

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

    namespace meta
    {
        /* is_light
         *
         * is_light::value is true, if the type is a light
         */
        template<class T, class = void> struct is_light;
    }
}



/* IS_LIGHT DEFINITION */

/* struct is_light
 *
 * is_light::value is true if the type is a light
 */
template<class T, class> struct glh::meta::is_light : std::false_type {};
template<class T> struct glh::meta::is_light<T, typename std::enable_if<std::is_base_of<glh::lighting::light, T>::value>::type> : std::true_type {};



/* LIGHT DEFINITION */

/* class light
 *
 * abstract base class for any type of light
 */
class glh::lighting::light
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
    void apply ( const core::struct_uniform& light_uni );
    void apply () const;

    /* cache_uniforms
     *
     * cache light uniforms for later use
     * 
     * light_uni: the uniform to cache
     */
    void cache_uniforms ( const core::struct_uniform& light_uni );



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
    const void set_ambient_color ( const glh::math::vec3& _ambient_color ) { ambient_color = _ambient_color; }
    const math::vec3& get_diffuse_color () const { return diffuse_color; }
    const void set_diffuse_color ( const glh::math::vec3& _diffuse_color ) { diffuse_color = _diffuse_color; }
    const math::vec3& get_specular_color () const { return specular_color; }
    const void set_specular_color ( const glh::math::vec3& _specular_color ) { specular_color = _specular_color; }

    /* enable/disable
     *
     * enable/disable the light
     */
    void enable () { enabled = true; }
    void disable () { enabled = false; }



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
        const core::struct_uniform& light_uni;
        const core::uniform& position_uni;
        const core::uniform& direction_uni;
        const core::uniform& inner_cone_uni;
        const core::uniform& outer_cone_uni;
        const core::uniform& att_const_uni;
        const core::uniform& att_linear_uni;
        const core::uniform& att_quad_uni;
        const core::uniform& ambient_color_uni;
        const core::uniform& diffuse_color_uni;
        const core::uniform& specular_color_uni;
        const core::uniform& enabled_uni;
    };

    /* cached uniforms */
    std::unique_ptr<cached_uniforms_struct> cached_uniforms;

};



/* DIRLIGHT DEFINITION */

/* class dirlight : light
 *
 * directional light class
 */
class glh::lighting::dirlight : public light
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
class glh::lighting::pointlight : public light
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
class glh::lighting::spotlight : public light
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
template<class T> class glh::lighting::light_collection
{

    /* static assert that T is a light */
    static_assert ( meta::is_light<T>::value, "class light_collection must have a template paramater of a light type" );

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



    /* typedef of T */
    typedef T type;



    /* array of lights */
    std::vector<T> lights;



    /* apply
     *
     * apply the lighting to uniforms
     * 
     * light_collection_uni: the uniform to apply the lights to
     */
    void apply ( const core::struct_uniform& light_collection_uni );
    void apply () const;

    /* cache_uniforms
     *
     * cache uniforms for later use
     * 
     * light_collection_uni: the uniform to cache
     */
    void cache_uniforms ( const core::struct_uniform& light_collection_uni );

    /* reload_uniforms
     *
     * reload the lights uniform caches based on current uniform cache
     */
    void reload_uniforms ();



private:

    /* struct for cached uniforms */
    struct cached_uniforms_struct
    {
        const core::struct_uniform& light_collection_uni;
        const core::uniform& size_uni;
        const core::struct_array_uniform& lights_uni;
    };

    /* cached uniforms */
    std::unique_ptr<cached_uniforms_struct> cached_uniforms;
};



/* LIGHT_SYSTEM DEFINITION */

/* class light_system
 *
 * several collection of lights for each type
 */
class glh::lighting::light_system
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
    void apply ( const core::struct_uniform& light_system_uni );
    void apply () const;

    /* cache_uniforms
     *
     * cache uniforms for later use
     * 
     * light_system_uni: the uniform to cache
     */
    void cache_uniforms ( const core::struct_uniform& light_system_uni );

    /* reload_uniforms
     *
     * reload the light collections based on the currently cached uniforms
     */
    void reload_uniforms ();



private:

    /* struct for cached uniforms */
    struct cached_uniforms_struct
    {
        const core::struct_uniform& light_system_uni;
        const core::struct_uniform& dircoll_uni;
        const core::struct_uniform& pointcoll_uni;
        const core::struct_uniform& spotcoll_uni;
    };

    /* cached uniforms */
    std::unique_ptr<cached_uniforms_struct> cached_uniforms;

};



/* LIGHT_COLLECTION IMPLEMENTATION */

/* apply
 *
 * apply the lighting to uniforms
 * 
 * light_collection_uni: the uniform to apply the lights to
 */
template<class T>
inline void glh::lighting::light_collection<T>::apply ( const core::struct_uniform& light_collection_uni )
{
    /* cache uniform */
    cache_uniforms ( light_collection_uni );

    /* apply */
    apply ();
}
template<class T>
inline void glh::lighting::light_collection<T>::apply () const
{
    /* throw if no uniform is cached */
    if ( !cached_uniforms ) throw exception::uniform_exception { "attempted to apply light_collection to uniform with out a complete uniform cache" };

    /* set uniforms */
    cached_uniforms->size_uni.set_int ( lights.size () );
    for ( unsigned i = 0; i < lights.size (); ++i ) lights.at ( i ).apply ();
}

/* cache_uniforms
 *
 * cache uniforms for later use
 * 
 * light_collection_uni: the uniform to cache
 */
template<class T>
inline void glh::lighting::light_collection<T>::cache_uniforms ( const core::struct_uniform& light_collection_uni )
{
    /* cache uniforms */
    cached_uniforms.reset ( new cached_uniforms_struct
    {
        light_collection_uni,
        light_collection_uni.get_uniform ( "size" ),
        light_collection_uni.get_struct_array_uniform ( "lights" )
    } );
    /* cache each light's uniforms */
    for ( unsigned i = 0; i < lights.size (); ++i ) lights.at ( i ).cache_uniforms ( cached_uniforms->lights_uni.at ( i ) );

}

/* reload_uniforms
 *
 * reload the lights uniform caches based on current uniform cache
 */
template <class T>
inline void glh::lighting::light_collection<T>::reload_uniforms ()
{
    /* recache uniforms */
    cache_uniforms ( cached_uniforms->light_collection_uni );
}



/* #ifndef GLHELPER_LIGHTING_HPP_INCLUDED */
#endif