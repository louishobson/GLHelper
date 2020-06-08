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



        /* class shadow_map_2d
         *
         * stores the objects necessary for a 2d shadow map
         */
        class shadow_map_2d;

        /* class shadow_map_cube
         *
         * stores the objects necessary for a cubemap shadow map
         */
        class shadow_map_cube;



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
template<class T> struct glh::meta::is_light<T, std::enable_if_t<std::is_base_of<glh::lighting::light, T>::value>> : std::true_type {};



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
                , math::vec3 { 0.0 }, math::vec3 { 0.0 }, math::vec3 {}}
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

    /* default pure virtual destructor */
    virtual ~light () = 0;



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



    /* apply
     *
     * apply the lighting to a uniform
     * the uniform should be of type light_struct (see top)
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
    


    /* enable/disable/is_enabled
     *
     * enable/disable the light or get whether is enabled
     */
    void enable () { enabled = true; }
    void disable () { enabled = false; }
    bool is_enabled () const { return enabled; }



private:

    /* whether the light is enables */
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

};

/* make destructor default */
glh::lighting::light::~light () = default;



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



    /* shadow_camera
     *
     * produce a camera for shadow mapping based on a region of space to capture
     * returns an orthographic movement camera capturing the whole scene
     * 
     * capture_region: a spherical region of what to capture in the shadow map
     */
    camera::camera_orthographic_movement shadow_camera ( const region::spherical_region<>& capture_region ) const;    



private:

    /* make position private */
    using light::position;

    /* make attenuation private */
    using light::att_const;
    using light::att_linear;
    using light::att_quad;

    /* make inner/outer_cone private */
    using light::inner_cone;
    using light::outer_cone;

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



    /* shadow_camera
     *
     * produce a camera for shadow mapping based on a region of space to capture
     * returns a perspective movement camera in the +ve-x direction
     * 90 degree rotation can be applied to the view matrix to aquire different directions
     * 
     * capture_region: a spherical region of what to capture in the shadow map
     */
    camera::camera_perspective_movement shadow_camera ( const region::spherical_region<>& capture_region ) const;



private:

    /* make direction private */
    using light::direction;

    /* make inner/outer_cone private */
    using light::inner_cone;
    using light::outer_cone;

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



    /* shadow_camera
     *
     * produce a camera for shadow mapping based on a region of space to capture
     * returns a perspective movement camera, aspect ratio 1:1, capturing all up to the outer cone
     * 
     * capture_region: a spherical region of what to capture in the shadow map
     */
    camera::camera_perspective_movement shadow_camera ( const region::spherical_region<>& capture_region ) const;    

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
    void add_light ( const T& _light ) { lights.push_back ( _light ); }
    void add_light ( T&& _light ) { lights.push_back ( _light ); }

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



/* SHADOW_MAP_2D DEFINITION */

/* class shadow_map_2d
 *
 * stores the objects necessary for a 2d shadow map
 */
class glh::lighting::shadow_map_2d
{
public:

    /* full constructor
     *
     * initialise the texture to a given size and attach it to the fbo
     * 
     * width: the width and height to set the texture to
     */
    shadow_map_2d ( const unsigned width );

    /* deleted zero-paramater constructor */
    shadow_map_2d () = delete;

    /* deleted copy constructor */
    shadow_map_2d ( const shadow_map_2d& other ) = delete;

    /* default move constructor */
    shadow_map_2d ( shadow_map_2d&& other ) = default;

    /* deleted copy assignment operator */
    shadow_map_2d& operator= ( const shadow_map_2d& other ) = delete;

    /* default destructor */
    ~shadow_map_2d () = default;



private:

    /* the texture2d that will be used for the depth map */
    core::texture2d depth_texture;

    /* the fbo that will be used for generating the depth map */
    core::fbo shadow_fbo;

};



/* SHADOW_MAP_CUBE DEFINITION */

/* class shadow_map_cube
 *
 * stores the objects necessary for a cubemap shadow map
 */
class glh::lighting::shadow_map_cube
{
public:

    /* full constructor
     *
     * initialise the cubemap to a given size and attach it to the fbo
     * 
     * width: the width and height to set the cubemap to
     */
    shadow_map_cube ( const unsigned width );

    /* deleted zero-parameter constructor */
    shadow_map_cube () = delete;

    /* deleted copy constructor */
    shadow_map_cube ( const shadow_map_cube& other ) = delete;

    /* default move constructor */
    shadow_map_cube ( shadow_map_cube&& other ) = default;

    /* deleted copy assignment operator */
    shadow_map_cube& operator= ( const shadow_map_cube& other ) = delete;

    /* default destructor */
    ~shadow_map_cube () = default;



private:

    /* the cubemap that will be used for the depth map */
    core::cubemap depth_texture;

    /* the fbo that will be used for generating the depth map */
    core::fbo shadow_fbo;

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
        : dirlights { other.dirlights }
        , pointlights { other.pointlights }
        , spotlights { other.spotlights }
    {}

    /* default move constructor */
    light_system ( light_system&& other ) = default;

    /* copy assignment operator 
     *
     * not default for same reason as copy constructor
     */
    light_system& operator= ( const light_system& other )
    { dirlights = other.dirlights; pointlights = other.pointlights; spotlights = other.spotlights; return * this; }

    /* default move assignment operator */
    light_system& operator= ( light_system&& other ) = default;

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
    void add_light ( const dirlight& _light ) { dirlights.add_light ( _light ); }
    void add_light ( dirlight&& _light ) { dirlights.add_light ( _light ); }
    void add_light ( const pointlight& _light ) { pointlights.add_light ( _light ); }
    void add_light ( pointlight&& _light ) { pointlights.add_light ( _light ); }  
    void add_light ( const spotlight& _light ) { spotlights.add_light ( _light ); }
    void add_light ( spotlight&& _light ) { spotlights.add_light ( _light ); }      



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