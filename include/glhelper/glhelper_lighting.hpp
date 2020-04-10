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
 * LIGHTING_STRUCT
 * 
 * struct lighting_struct
 * {
 *     int ditlights_size;
 *     light_struct dirlights [];
 * 
 *     int pointlights_size;
 *     light_struct pointlights [];
 * 
 *     int spotlights_size;
 *     light_struct spotlights [];
 * };
 * 
 * this structure holds arrays of each type of light
 * the size of the arrays for each type must be large enough to store as many lights as required
 * 
 * x_size & x[]: the size of the array of a type of light, and the actual array
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

    /* class lighting
     *
     * class to store multiple of different types of light
     */
    class lighting;

    /* class lighting_exception : exception
     *
     * exception relating to lighting
     */
    class lighting_exception;
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

    /* pure virtual destructor */
    virtual ~light () = 0;



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

/* make destructor default */
inline glh::light::~light () = default;



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



/* LIGHTING DEFINITION */

/* class lighting
 *
 * class to store multiple of different types of light
 */
class glh::lighting
{
public:

    /* default constructor
     *
     * an empty set of lights
     */
    lighting () = default;

    /* copy constructor
     *
     * not default as cannot copy cached uniforms
     */
    lighting ( const lighting& other )
        : dirlights { other.dirlights }
        , pointlights { other.pointlights }
        , spotlights { other.spotlights }
    {}

    /* default move constructor */
    lighting ( lighting&& other ) = default;

    /* copy assignment operator
     *
     * not default for the same reason as copy constructor
     */
    lighting& operator= ( const lighting& other );

    /* default move assignment operator */
    lighting& operator= ( lighting&& other ) = default;

    /* default destructor */
    ~lighting () = default;



    /* at_dirlight
     *
     * get a dirlight at an index
     */
    dirlight& at_dirlight ( const unsigned i ) { return dirlights.at ( i ); }
    const dirlight& at_dirlight ( const unsigned i ) const { return dirlights.at ( i ); }

    /* at_pointlight
     *
     * get a pointlight at an index
     */
    pointlight& at_pointlight ( const unsigned i ) { return pointlights.at ( i ); }
    const pointlight& at_pointlight ( const unsigned i ) const { return pointlights.at ( i ); }

    /* at_spotlight
     *
     * get a spotlight at an index
     */
    spotlight& at_spotlight ( const unsigned i ) { return spotlights.at ( i ); }
    const spotlight& at_spotlight ( const unsigned i ) const { return spotlights.at ( i ); }



    /* add_dirlight
     *
     * add a dirlight to the lighting system
     * overloads are based on constructors of dirlight
     */
    void add_dirlight () { dirlights.push_back ( dirlight {} ); }
    void add_dirlight ( const dirlight& _dirlight ) { dirlights.push_back ( _dirlight ); }
    void add_dirlight ( dirlight&& _dirlight ) { dirlights.push_back ( _dirlight ); }
    void add_dirlight ( const math::vec3& _direction
                      , const math::vec3& _ambient_color, const math::vec3& _diffuse_color, const math::vec3& _specular_color
                      , const bool _enabled = true )
    { dirlights.push_back ( dirlight { _direction, _ambient_color, _diffuse_color, _specular_color, _enabled } ); }

    /* add_pointlight
     *
     * add a pointlight to the lighting system
     * overloads are based on the constructors of pointlight
     */
    void add_pointlight () { pointlights.push_back ( pointlight {} ); }
    void add_pointlight ( const pointlight& _pointlight ) { pointlights.push_back ( _pointlight ); }
    void add_pointlight ( pointlight&& _pointlight ) { pointlights.push_back ( _pointlight ); }
    void add_pointlight ( const math::vec3& _position
                      , const double _att_const, const double _att_linear, const double _att_quad
                      , const math::vec3& _ambient_color, const math::vec3& _diffuse_color, const math::vec3& _specular_color
                      , const bool _enabled = true )
    { pointlights.push_back ( pointlight { _position, _att_const, _att_linear, _att_quad, _ambient_color, _diffuse_color, _specular_color, _enabled } ); }

    /* add_spotlight
     * 
     * add a spotlight to the lighting system
     * overloads are based on the constructors of spotlight
     */
    void add_spotlight () { spotlights.push_back ( spotlight {} ); }
    void add_spotlight ( const spotlight& _spotlight ) { spotlights.push_back ( _spotlight ); }
    void add_spotlight ( spotlight&& _spotlight ) { spotlights.push_back ( _spotlight ); }
    void add_spotlight ( const math::vec3& _position, const math::vec3& _direction
                       , const double _inner_cone, const double _outer_cone,const double _att_const, const double _att_linear, const double _att_quad
                       , const math::vec3& _ambient_color, const math::vec3& _diffuse_color, const math::vec3& _specular_color
                       , const bool _enabled = true )
    { spotlights.push_back ( spotlight { _position, _direction, _inner_cone, _outer_cone, _att_const, _att_linear, _att_quad, _ambient_color, _diffuse_color, _specular_color, _enabled } ); }



    /* erase_dirlight
     *
     * remove a dirlight at an index
     */
    void erase_dirlight ( const unsigned i ) { dirlights.erase ( dirlights.begin () + i ); }
    
    /* erase_pointlight
     *
     * remove a pointlight at an index
     */
    void erase_pointlight ( const unsigned i ) { pointlights.erase ( pointlights.begin () + i ); }
    
    /* erase_spotlight
     *
     * remove a spotlight at an index
     */
    void erase_spotlight ( const unsigned i ) { spotlights.erase ( spotlights.begin () + i ); }



    /* apply
     *
     * apply the lighting to a uniform
     * the uniform should be of type lighting_struct (see top)
     * 
     * lightint_uni: the uniform to apply the lights to (which will be cached)
     */
    void apply ( const struct_uniform& lighting_uni ) const;
    void apply () const;

    /* cache_uniforms
     *
     * cache light uniforms for later use
     * 
     * lightint_uni: the uniform to cache
     */
    void cache_uniforms ( const struct_uniform& lighting_uni ) const;



private:

    /* dirlights/pointlights/spotlights
     *
     * arrays of each type of light
     */
    std::vector<dirlight> dirlights;
    std::vector<pointlight> pointlights;
    std::vector<spotlight> spotlights;

    /* struct for cached uniforms */
    struct cached_uniforms_struct
    {
        struct_uniform lighting_uni;
        uniform dirlights_size_uni;
        array_uniform<struct_uniform> dirlights_uni;
        uniform pointlights_size_uni;
        array_uniform<struct_uniform> pointlights_uni;
        uniform spotlights_size_uni;
        array_uniform<struct_uniform> spotlights_uni;
    };

    /* cached uniforms */
    mutable std::unique_ptr<cached_uniforms_struct> cached_uniforms;

};



/* LIGHTING_EXCEPTION DEFINITION */

/* class lighting_exception : exception
 *
 * exception relating to lighting
 */
class glh::lighting_exception : public exception
{
public:

    /* full constructor
     *
     * __what: description of the exception
     */
    explicit lighting_exception ( const std::string& __what )
        : exception ( __what )
    {}

    /* default zero-parameter constructor
     *
     * construct lighting_exception with no descrption
     */
    lighting_exception () = default;

    /* default everything else and inherits what () function */

};



/* #ifndef GLHELPER_LIGHTING_HPP_INCLUDED */
#endif