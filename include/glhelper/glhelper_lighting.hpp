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
 * CLASS GLH::LIGHTING::DIRLIGHT
 * CLASS GLH::LIGHTING::POINTLIGHT
 * CLASS GLH::LIGHTING::SPOTLIGHT
 * 
 * classes storing the attributes of a different types of light
 * these attributes can be applied to a the respective uniforms struct uniform:
 * 
 * 
 * 
 * GLSL STRUCT DIRLIGHT_STRUCT, POINTLIGHT_STRUCT AND SPOTLIGHT_STRUCT
 * 
 * struct dirlight_struct
 * {
 *     vec3 direction;
 * 
 *     vec3 ambient_color;
 *     vec3 diffuse_color;
 *     vec3 specular_color;
 * 
 *     bool enabled;
 * 
 *     mat4 shadow_view;
 *     mat4 shadow_proj;
 * };
 * 
 * struct pointlight_struct
 * {
 *     vec3 position;
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
 * 
 *     mat4 shadow_view;
 *     mat4 shadow_proj;
 * };
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
 * 
 *     mat4 shadow_view;
 *     mat4 shadow_proj;
 * };
 * 
 * these GLSL structures contain attributes for different light types
 * the attributes have the following meanings
 * 
 * position: the position of the light (only for point or spot lights)
 * direction: the direction of the light (only for directional or spot lights)
 * inner_cone: angle in radians of the inner cone of a spotlight (spotlight only)
 * outer_cone: angle in radians of the outer cone of a spotlight (spotlight only)
 * att_const/linear/quad: attenuation attributes for the light (only for point and spot lights)
 * ambient/diffuse/specular_color: the colors of the different components of light produced (all types)
 * enabled: whether the light is 'turned on' (all types)
 * shadow_view/proj: view and projection matrices for shadow mapping (all lights)
 *
 * 
 * 
 * CLASS GLH::LIGHTING::LIGHT_COLLECTION
 * 
 * template class to store a dynamically-allocated array of lights
 * the template parameter must be a type of light
 * there are using declarations to abstract the template (e.g. dirlight_collection)
 * this class exists so that applying all of the lights to uniforms insize the glsl LIGHT_SYSTEM_STRUCT is made easier
 * 
 * 
 * 
 * CLASS GLH::LIGHTING::LIGHT_SYSTEM
 * 
 * class to store arrays of directional lights, point lights and spotlights
 * the structure can be applied to a light_system_struct struct uniform:
 * 
 * 
 * 
 * GLSL STRUCT LIGHT_SYSTEM_STRUCT
 * 
 * struct light_system_struct
 * {
 *     int dirlights_size;
 *     light_struct dirlights [ MAX_NUM_LIGHTS ];
 *
 *     int pointlights_size;
 *     light_struct pointlights [ MAX_NUM_LIGHTS ];
 *
 *     int spotlights_size;
 *     light_struct spotlights_size [ MAX_NUM_LIGHTS ];
 * };
 * 
 * this structure holds multiple arrays of lights
 * this is the structure the glh::lighting::light_system class expects to be supplied with to write to
 * 
 * dirlights(_size): array of directional lights and its size
 * pointlights(_size): array of collection of point lights and its size
 * spotlights(_size): collection of spotlights and its size
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

/* include glhelper_shader.hpp */
#include <glhelper/glhelper_shader.hpp>

/* include glhelper_math.hpp */
#include <glhelper/glhelper_math.hpp>

/* include glhelper_region.hpp */
#include <glhelper/glhelper_region.hpp>

/* include glhelper_camera.hpp */
#include <glhelper/glhelper_camera.hpp>

/* include glhelper_texture.hpp */
#include <glhelper/glhelper_texture.hpp>

/* include glhelper_framebuffer.hpp */
#include <glhelper/glhelper_framebuffer.hpp>



/* NAMESPACE DECLARATIONS */

namespace glh
{
    namespace lighting
    {
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
        template<class T> struct is_light;
    }
}



/* IS_LIGHT DEFINITION */

/* struct is_light
 *
 * is_light::value is true if the type is a light
 */
template<class T> struct glh::meta::is_light : std::false_type {};
template<> struct glh::meta::is_light<glh::lighting::dirlight> : std::true_type {};
template<> struct glh::meta::is_light<glh::lighting::pointlight> : std::true_type {};
template<> struct glh::meta::is_light<glh::lighting::spotlight> : std::true_type {};



/* DIRLIGHT DEFINITION */

/* class dirlight : light
 *
 * directional light class
 */
class glh::lighting::dirlight
{
public:

    /* full constructor
     *
     * set all of the values associated with directional light
     */
    dirlight ( const math::vec3& _direction
             , const math::vec3& _ambient_color, const math::vec3& _diffuse_color, const math::vec3& _specular_color
             , const bool _enabled = true )
        : direction { _direction }
        , ambient_color { _ambient_color }, diffuse_color { _diffuse_color }, specular_color { _specular_color }
        , enabled { _enabled }
        , shadow_camera { math::vec3 { 0.0 }, _direction, math::any_perpandicular ( _direction ), math::vec3 { 0.0 }, math::vec3 { 0.0 } }
        , shadow_region { math::vec3 { 0.0 }, 0.0 }
        , shadow_camera_change { true }
    {}

    /* default zero-parameter constructor */
    dirlight () = default;

    /* default copy constructor */
    dirlight ( const dirlight& other ) = delete;

    /* deleted move constructor */
    dirlight ( dirlight&& other ) = default;

    /* deleted copy assignment operator */
    dirlight& operator= ( const dirlight& other ) = delete;

    /* default destructor */
    ~dirlight () = default;



    /* apply
     *
     * apply the lighting to a uniform
     * the uniform should be of type dirlight_struct (see top)
     * 
     * light_uni: the uniform to apply the light to (which will be cached)
     */
    void apply ( core::struct_uniform& light_uni );
    void apply () const;

    /* cache_uniforms
     *
     * cache light uniforms for later use
     * 
     * light_uni: the uniform to cache
     */
    void cache_uniforms ( core::struct_uniform& light_uni );
    


    /* get/set_direction
     * 
     * get/set the direction of the light
     */
    const math::vec3& get_direction () const { return direction; }
    void set_direction ( const math::vec3& _direction ) { direction = _direction; shadow_camera_change = true; }

    /* set/get_ambient/diffuse/specular_color
     *
     * get/set the colors of the light
     */
    const math::vec3& get_ambient_color () const { return ambient_color; }
    void set_ambient_color ( const math::vec3& _ambient_color ) { ambient_color = _ambient_color; }
    const math::vec3& get_diffuse_color () const { return diffuse_color; }
    void set_diffuse_color ( const math::vec3& _diffuse_color ) { diffuse_color = _diffuse_color; }
    const math::vec3& get_specular_color () const { return ambient_color; }
    void set_specular_color ( const math::vec3& _specular_color ) { specular_color = _specular_color; }
    void set_color ( const math::vec3& _ambient_color, const math::vec3& _diffuse_color, const math::vec3& _specular_color )
    { ambient_color = _ambient_color; diffuse_color = _diffuse_color; specular_color = _specular_color; }

    /* enable/disable/is_enabled
     *
     * enable/disable the light or get whether is enabled
     */
    void enable () { enabled = true; }
    void disable () { enabled = false; }
    bool is_enabled () const { return enabled; }



private:

    /* direction of the light */
    math::vec3 direction;

    /* colors of light */
    math::vec3 ambient_color;
    math::vec3 diffuse_color;
    math::vec3 specular_color;

    /* whether the light is enabled */
    bool enabled;

    /* struct for cached uniforms */
    struct cached_uniforms_struct
    {
        core::struct_uniform& light_uni;
        core::uniform& direction_uni;
        core::uniform& ambient_color_uni;
        core::uniform& diffuse_color_uni;
        core::uniform& specular_color_uni;
        core::uniform& enabled_uni;
    };

    /* cached uniforms */
    std::unique_ptr<cached_uniforms_struct> cached_uniforms;

    /* the camera for the shadow map */
    mutable camera::camera_orthographic_movement shadow_camera;

    /* the last shadow region used */
    region::spherical_region<> shadow_region;

    /* true if the shadow camera must be updated */
    mutable bool shadow_camera_change;

};



/* POINTLIGHT DEFINITION */

/* class pointlight : light
 *
 * point light class
 */
class glh::lighting::pointlight
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
        : position { _position }, att_const { _att_const }, att_linear { _att_linear }, att_quad { _att_quad }
        , ambient_color { _ambient_color }, diffuse_color { _diffuse_color }, specular_color { _specular_color }
        , enabled { _enabled }
        , shadow_camera { _position, math::vec3 { 1.0, 0.0, 0.0 }, math::vec3 { 0.0, 1.0, 0.0 }, math::rad ( 90.0 ), 1.0, 0.1, 0.1 }
        , shadow_region { math::vec3 { 0.0 }, 0.0 }
    , shadow_camera_change { true }
    {}

    /* default zero-parameter constructor */
    pointlight () = default;

    /* deleted copy constructor */
    pointlight ( const pointlight& other ) = delete;

    /* default move constructor */
    pointlight ( pointlight&& other ) = default;

    /* deleted copy assignment operator */
    pointlight& operator= ( const pointlight& other ) = delete;

    /* default destructor */
    ~pointlight () = default;



    /* apply
     *
     * apply the lighting to a uniform
     * the uniform should be of type pointlight_struct (see top)
     * 
     * light_uni: the uniform to apply the light to (which will be cached)
     */
    void apply ( core::struct_uniform& light_uni );
    void apply () const;

    /* cache_uniforms
     *
     * cache light uniforms for later use
     * 
     * light_uni: the uniform to cache
     */
    void cache_uniforms ( core::struct_uniform& light_uni );
    


    /* get/set_position
     * 
     * get/set the position of the light
     */
    const math::vec3& get_position () const { return position; }
    void set_position ( const math::vec3& _position ) { position = _position; shadow_camera_change = true; }

    /* get/set_att_const/linear/quad
     *
     * get/set the attenuation coefficients
     */
    const double& get_att_const () const { return att_const; }
    void set_att_const ( const double _att_const ) { att_const = _att_const; }
    const double& get_att_linear () const { return att_linear; }
    void set_att_linear ( const double _att_linear ) { att_linear = _att_linear; } 
    const double& get_att_quad () const { return att_quad; }
    void set_att_quad ( const double _att_quad ) { att_quad = _att_quad; }
    void set_att ( const double _att_const, const double _att_linear, const double _att_quad ) { att_const = _att_const; att_linear = _att_linear; att_quad = _att_quad; }

    /* set/get_ambient/diffuse/specular_color
     *
     * get/set the colors of the light
     */
    const math::vec3& get_ambient_color () const { return ambient_color; }
    void set_ambient_color ( const math::vec3& _ambient_color ) { ambient_color = _ambient_color; }
    const math::vec3& get_diffuse_color () const { return diffuse_color; }
    void set_diffuse_color ( const math::vec3& _diffuse_color ) { diffuse_color = _diffuse_color; }
    const math::vec3& get_specular_color () const { return ambient_color; }
    void set_specular_color ( const math::vec3& _specular_color ) { specular_color = _specular_color; }
    void set_color ( const math::vec3& _ambient_color, const math::vec3& _diffuse_color, const math::vec3& _specular_color )
    { ambient_color = _ambient_color; diffuse_color = _diffuse_color; specular_color = _specular_color; }

    /* enable/disable/is_enabled
     *
     * enable/disable the light or get whether is enabled
     */
    void enable () { enabled = true; }
    void disable () { enabled = false; }
    bool is_enabled () const { return enabled; }



private:

    /* position of the light */
    math::vec3 position;

    /* attenuation parameters */
    double att_const;
    double att_linear;
    double att_quad;

    /* colors of light */
    math::vec3 ambient_color;
    math::vec3 diffuse_color;
    math::vec3 specular_color;

    /* whether the light is enabled */
    bool enabled;

    /* struct for cached uniforms */
    struct cached_uniforms_struct
    {
        core::struct_uniform& light_uni;
        core::uniform& position_uni;
        core::uniform& att_const_uni;
        core::uniform& att_linear_uni;
        core::uniform& att_quad_uni;
        core::uniform& ambient_color_uni;
        core::uniform& diffuse_color_uni;
        core::uniform& specular_color_uni;
        core::uniform& enabled_uni;
    };

    /* cached uniforms */
    std::unique_ptr<cached_uniforms_struct> cached_uniforms;

    /* the camera for the shadow map */
    mutable camera::camera_perspective_movement shadow_camera;

    /* the last shadow region used */
    region::spherical_region<> shadow_region;

    /* true if the shadow camera must be updated */
    mutable bool shadow_camera_change; 

};



/* SPOTLIGHT DEFINITION */

/* class spotlight : light
 *
 * spotlight class
 */
class glh::lighting::spotlight
{
public:

    /* full constructor
     *
     * set all of the values associated with directional light
     */
    spotlight ( const math::vec3& _position, const math::vec3& _direction
              , const double _inner_cone, const double _outer_cone, const double _att_const, const double _att_linear, const double _att_quad
              , const math::vec3& _ambient_color, const math::vec3& _diffuse_color, const math::vec3& _specular_color
              , const bool _enabled = true )
        : position { _position }, direction { _direction }
        , inner_cone { _inner_cone }, outer_cone { _outer_cone }, att_const { _att_const }, att_linear { _att_linear }, att_quad { _att_quad }
        , ambient_color { _ambient_color }, diffuse_color { _diffuse_color }, specular_color { _specular_color }
        , enabled { _enabled }
        , shadow_camera { _position, _direction, math::any_perpandicular ( _direction ), _outer_cone, 1.0, 0.1, 0.1 }
        , shadow_region { math::vec3 { 0.0 }, 0.0 }
        , shadow_camera_change { true }
    {}

    /* default zero-parameter constructor */
    spotlight () = default;

    /* deleted copy constructor */
    spotlight ( const spotlight& other ) = delete;

    /* default move constructor */
    spotlight ( spotlight&& other ) = default;

    /* deleted copy assignment operator */
    spotlight& operator= ( const spotlight& other ) = delete;

    /* default destructor */
    ~spotlight () = default;



    /* apply
     *
     * apply the lighting to a uniform
     * the uniform should be of type spotlight_struct (see top)
     * 
     * light_uni: the uniform to apply the light to (which will be cached)
     */
    void apply ( core::struct_uniform& light_uni );
    void apply () const;

    /* cache_uniforms
     *
     * cache light uniforms for later use
     * 
     * light_uni: the uniform to cache
     */
    void cache_uniforms ( core::struct_uniform& light_uni );
    


    /* get/set_position
     * get/set_direction
     * 
     * get/set the position and direction of the light
     */
    const math::vec3& get_position () const { return position; }
    void set_position ( const math::vec3& _position ) { position = _position; shadow_camera_change = true; }
    const math::vec3& get_direction () const { return direction; }
    void set_direction ( const math::vec3& _direction ) { direction = _direction; shadow_camera_change = true; }

    /* get/set_inner/outer_cone
     *
     * get/set the inner and outer cone of the spotlight
     */
    const double& get_inner_cone () const { return inner_cone; }
    void set_inner_cone ( const double _inner_cone ) { inner_cone = _inner_cone; shadow_camera_change = true; }
    const double& get_outer_cone () const { return outer_cone; }
    void set_outer_cone ( const double _outer_cone ) { outer_cone = _outer_cone; shadow_camera_change = true; }
    void set_cone ( const double _inner_cone, const double _outer_cone ) { inner_cone = _inner_cone; outer_cone = _outer_cone; shadow_camera_change = true; }

    /* get/set_att_const/linear/quad
     *
     * get/set the attenuation coefficients
     */
    const double& get_att_const () const { return att_const; }
    void set_att_const ( const double _att_const ) { att_const = _att_const; }
    const double& get_att_linear () const { return att_linear; }
    void set_att_linear ( const double _att_linear ) { att_linear = _att_linear; } 
    const double& get_att_quad () const { return att_quad; }
    void set_att_quad ( const double _att_quad ) { att_quad = _att_quad; }
    void set_att ( const double _att_const, const double _att_linear, const double _att_quad ) { att_const = _att_const; att_linear = _att_linear; att_quad = _att_quad; }

    /* set/get_ambient/diffuse/specular_color
     *
     * get/set the colors of the light
     */
    const math::vec3& get_ambient_color () const { return ambient_color; }
    void set_ambient_color ( const math::vec3& _ambient_color ) { ambient_color = _ambient_color; }
    const math::vec3& get_diffuse_color () const { return diffuse_color; }
    void set_diffuse_color ( const math::vec3& _diffuse_color ) { diffuse_color = _diffuse_color; }
    const math::vec3& get_specular_color () const { return ambient_color; }
    void set_specular_color ( const math::vec3& _specular_color ) { specular_color = _specular_color; }
    void set_color ( const math::vec3& _ambient_color, const math::vec3& _diffuse_color, const math::vec3& _specular_color )
    { ambient_color = _ambient_color; diffuse_color = _diffuse_color; specular_color = _specular_color; }

    /* enable/disable/is_enabled
     *
     * enable/disable the light or get whether is enabled
     */
    void enable () { enabled = true; }
    void disable () { enabled = false; }
    bool is_enabled () const { return enabled; }



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

    /* whether the light is enabled */
    bool enabled;

    /* struct for cached uniforms */
    struct cached_uniforms_struct
    {
        core::struct_uniform& light_uni;
        core::uniform& position_uni;
        core::uniform& direction_uni;
        core::uniform& inner_cone_uni;
        core::uniform& outer_cone_uni;
        core::uniform& att_const_uni;
        core::uniform& att_linear_uni;
        core::uniform& att_quad_uni;
        core::uniform& ambient_color_uni;
        core::uniform& diffuse_color_uni;
        core::uniform& specular_color_uni;
        core::uniform& enabled_uni;
    };

    /* cached uniforms */
    std::unique_ptr<cached_uniforms_struct> cached_uniforms;

    /* the camera for the shadow map */
    mutable camera::camera_perspective_movement shadow_camera;

    /* the last shadow region used */
    region::spherical_region<> shadow_region;

    /* true if the shadow camera must be updated */
    mutable bool shadow_camera_change; 

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

    /* deleted copy constructor */
    light_collection ( const light_collection& other ) = delete;

    /* default move constructor */
    light_collection ( light_collection&& other ) = default;

    /* deleted copy assignment operator */
    light_collection& operator= ( const light_collection& other ) = delete;

    /* default destructor */
    ~light_collection () = default;



    /* typedef of T */
    typedef T type;



    /* at
     *
     * get the light at an index
     */
    T& at ( const unsigned index ) { return lights.at ( index ); }
    const T& at ( const unsigned index ) const { return lights.at ( index ); }

    /* size
     *
     * get the number of lights in the light collection
     */
    unsigned size () const { return lights.size (); }

    /* add_light
     *
     * add a light to the collection
     */
    void add_light ( T&& _light ) { lights.push_back ( std::forward<T> ( _light ) ); }

    /* remove_light
     *
     * remove a light at an index
     */
    void remove_light ( const unsigned index ) { lights.erase ( lights.begin () + index ); }



    /* apply
     *
     * apply the lighting to uniforms
     * 
     * size_uni/lights_uni: the uniform to apply the lights to
     */
    void apply ( core::uniform& size_uni, core::struct_array_uniform& lights_uni );
    void apply () const;

    /* cache_uniforms
     *
     * cache uniforms for later use
     * 
     * size_uni/lights_uni: the uniforms to cache
     */
    void cache_uniforms ( core::uniform& size_uni, core::struct_array_uniform& lights_uni );

    /* reload_uniforms
     *
     * reload the lights uniform caches based on current uniform cache
     */
    void reload_uniforms ();



private:

    /* array of lights */
    std::vector<T> lights;

    /* struct for cached uniforms */
    struct cached_uniforms_struct
    {
        core::uniform& size_uni;
        core::struct_array_uniform& lights_uni;
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

    /* deleted copy constructor */
    light_system ( const light_system& other ) = delete;

    /* default move constructor */
    light_system ( light_system&& other ) = default;

    /* deleted copy assignment operator */
    light_system& operator= ( const light_system& other ) = delete;

    /* default destructor */
    ~light_system () = default;



    /* light collections */
    dirlight_collection dirlights;
    pointlight_collection pointlights;
    spotlight_collection spotlights;



    /* add_light
     *
     * adds a light to a collection based on its type
     */
    void add_light ( dirlight&& _light ) { dirlights.add_light ( std::forward<dirlight> ( _light ) ); }
    void add_light ( pointlight&& _light ) { pointlights.add_light ( std::forward<pointlight> ( _light ) ); }  
    void add_light ( spotlight&& _light ) { spotlights.add_light ( std::forward<spotlight> ( _light ) ); }      



    /* apply
     *
     * apply the lighting to uniforms
     * 
     * light_system_uni: the uniform to apply the lights to
     */
    void apply ( core::struct_uniform& light_system_uni );
    void apply () const;

    /* cache_uniforms
     *
     * cache uniforms for later use
     * 
     * light_system_uni: the uniform to cache
     */
    void cache_uniforms ( core::struct_uniform& light_system_uni );

    /* reload_uniforms
     *
     * reload the light collections based on the currently cached uniforms
     */
    void reload_uniforms ();



private:

    /* struct for cached uniforms */
    struct cached_uniforms_struct
    {
        core::struct_uniform& light_system_uni;
    };

    /* cached uniforms */
    std::unique_ptr<cached_uniforms_struct> cached_uniforms;

};



/* LIGHT_COLLECTION IMPLEMENTATION */

/* apply
 *
 * apply the lighting to uniforms
 * 
 * size_uni/lights_uni: the uniforms to apply the lights to
 */
template<class T>
inline void glh::lighting::light_collection<T>::apply ( core::uniform& size_uni, core::struct_array_uniform& lights_uni )
{
    /* cache uniform */
    cache_uniforms ( size_uni, lights_uni );

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
 * size_uni/lights_uni: the uniforms to cache
 */
template<class T>
inline void glh::lighting::light_collection<T>::cache_uniforms ( core::uniform& size_uni, core::struct_array_uniform& lights_uni )
{
    /* if uniforms are not already cached, cache the new ones */
    if ( !cached_uniforms || cached_uniforms->size_uni != size_uni || cached_uniforms->lights_uni != lights_uni )
    {
        cached_uniforms.reset ( new cached_uniforms_struct
        {
            size_uni,
            lights_uni
        } );
    }

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