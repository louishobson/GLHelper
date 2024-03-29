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
 *     bool shadow_mapping_enabled;
 * 
 *     mat4 shadow_trans;
 * 
 *     float shadow_bias;
 * 
 *     float pcf_samples;
 *     float pcf_radius;
 *     mat2 pcf_rotation;
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
 *     bool shadow_mapping_enabled;
 * 
 *     float shadow_bias;
 *     float shadow_depth_range_mult;
 * 
 *     int pcf_samples;
 *     float pcf_radius;
 *     mat2 pcf_rotation;
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
 *     bool shadow_mapping_enabled;
 * 
 *     mat4 shadow_trans;
 * 
 *     float shadow_bias;
 *     float shadow_depth_range_mult;
 * 
 *     int pcf_samples;
 *     float pcf_radius;
 *     mat2 pcf_rotation;
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
 * shadow_mapping_enabled: whether the light should be shadow mapped (all types)
 * shadow_trans: the transformation matrix used for shadow mapping (only directional and spot lights)
 * shadow_bias: the bias to apply when sampling from the shadow map
 * shadow_depth_range_mult: reciprocal the side length of the perspective frustum (only point and spot lights)
 * pcf_samples: the number of pcf samples to take (all types)
 * pcf_radius: the radius of pcf samples (all types)
 * pcf_rotation: an anticlockwise rotation by 360/pcf_samples degrees (all types)
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
 * 
 *     sampler2DArrayShadow shadow_maps;
 * };
 * 
 * this structure holds multiple arrays of lights
 * this is the structure the glh::lighting::light_system class expects to be supplied with to write to
 * 
 * dirlights(_size): array of directional lights and its size
 * pointlights(_size): array of collection of point lights and its size
 * spotlights(_size): collection of spotlights and its size
 * shadow_maps: sampler for shadow maps
 * 
 */



/* HEADER GUARD */
#ifndef GLHELPER_LIGHTING_HPP_INCLUDED
#define GLHELPER_LIGHTING_HPP_INCLUDED



/* INCLUDES */

/* include core headers */
#include <array>
#include <cmath>
#include <iostream>
#include <memory>
#include <type_traits>
#include <vector>

/* include glhelper_core.hpp */
#include <glhelper/glhelper_core.hpp>

/* include glhelper_shader.hpp */
#include <glhelper/glhelper_shader.hpp>

/* include glhelper_vector.hpp */
#include <glhelper/glhelper_vector.hpp>

/* include glhelper_matrix.hpp */
#include <glhelper/glhelper_matrix.hpp>

/* include glhelper_transform.hpp */
#include <glhelper/glhelper_transform.hpp>

/* include glhelper_region.hpp */
#include <glhelper/glhelper_region.hpp>

/* include glhelper_camera.hpp */
#include <glhelper/glhelper_camera.hpp>

/* include glhelper_texture.hpp */
#include <glhelper/glhelper_texture.hpp>

/* include glhelper_framebuffer.hpp */
#include <glhelper/glhelper_framebuffer.hpp>

/* include glhelper_render.hpp */
#include <glhelper/glhelper_render.hpp>



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
             , const region::spherical_region<>& _shadow_region = region::spherical_region<> { math::vec3 { 0.0 }, 0.0 }
             , const bool _enabled = true, const bool _shadow_mapping_enabled = true, const double _shadow_bias = 0.0
             , const unsigned _pcf_samples = 0, const double _pcf_radius = 0.0 )
        : direction { math::normalize ( _direction ) }
        , ambient_color { _ambient_color }, diffuse_color { _diffuse_color }, specular_color { _specular_color }
        , shadow_region { _shadow_region }
        , enabled { _enabled }, shadow_mapping_enabled { _shadow_mapping_enabled }, shadow_bias { _shadow_bias }
        , pcf_samples { _pcf_samples }, pcf_radius { _pcf_radius }
        , pcf_rotation { math::rotate ( math::identity<2> (), glh::math::pi ( 2.0 ) / _pcf_samples, 0, 1 ) }
        , shadow_camera { math::vec3 { 0.0 }, _direction, math::any_perpandicular ( _direction ), math::vec3 { 0.0 }, math::vec3 { 0.0 } }
        , shadow_camera_change { true }
    {}

    /* default zero-parameter constructor */
    dirlight () = default;

    /* copy constructor */
    dirlight ( const dirlight& other )
        : direction { other.direction }
        , ambient_color { other.ambient_color }, diffuse_color { other.diffuse_color }, specular_color { other.specular_color }
        , shadow_region { other.shadow_region }
        , enabled { other.enabled }, shadow_mapping_enabled { other.shadow_mapping_enabled }, shadow_bias { other.shadow_bias }
        , pcf_samples { other.pcf_samples }, pcf_radius { other.pcf_radius }, pcf_rotation { other.pcf_rotation }
        , shadow_camera { other.shadow_camera }, shadow_camera_change { other.shadow_camera_change }
    {}

    /* default move constructor */
    dirlight ( dirlight&& other ) = default;

    /* copy assignment operator */
    dirlight& operator= ( const dirlight& other )
        { direction = other.direction
        ; ambient_color = other.ambient_color; diffuse_color = other.diffuse_color; specular_color = other.specular_color
        ; shadow_region = other.shadow_region
        ; enabled = other.enabled; shadow_mapping_enabled = other.shadow_mapping_enabled; shadow_bias = other.shadow_bias
        ; pcf_samples = other.pcf_samples; pcf_radius = other.pcf_radius; pcf_rotation = other.pcf_rotation
        ; shadow_camera = other.shadow_camera; shadow_camera_change = other.shadow_camera_change
    ; return * this; }

    /* default move assignment operator */
    dirlight& operator= ( dirlight&& other ) = default;

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

    /* get/set_shadow_region
     *
     * get/set the region the light should cast shadows over
     */
    const region::spherical_region<>& get_shadow_region () const { return shadow_region; }
    void set_shadow_region ( const region::spherical_region<>& _shadow_region ) { shadow_region = _shadow_region; shadow_camera_change = true; }

    /* enable/disable/is_enabled
     *
     * enable/disable the light or get whether is enabled
     */
    void enable () { enabled = true; }
    void disable () { enabled = false; }
    bool is_enabled () const { return enabled; }

    /* enable/disable_shadow_mapping
     * is_shadow_mapping_enabled
     * 
     * enable/disable shadow mapping or get whether shadow mapping is enabled
     */
    void enable_shadow_mapping () { shadow_mapping_enabled = true; }
    void disable_shadow_mapping () { shadow_mapping_enabled = false; }
    bool is_shadow_mapping_enabled () const { return shadow_mapping_enabled; }

    /* get/set_shadow_bias
     *
     * get/set the shadow bias
     */
    const double& get_shadow_bias () const { return shadow_bias; }
    void set_shadow_bias ( const double _shadow_bias ) { shadow_bias = _shadow_bias; }

    /* get/set_pcf_samples/radius
     *
     * get/set the pcf sample count and radius
     */
    const unsigned& get_pcf_samples () const { return pcf_samples; }
    void set_pcf_samples ( const unsigned _pcf_samples ) 
        { pcf_samples = _pcf_samples; pcf_rotation = math::rotate ( math::identity<2> (), glh::math::pi ( 2.0 ) / _pcf_samples, 0, 1 ); }
    const double& get_pcf_radius () const { return pcf_radius; }
    void set_pcf_radius ( const double _pcf_radius ) { pcf_radius = _pcf_radius; }




private:

    /* direction of the light */
    math::vec3 direction;

    /* colors of light */
    math::vec3 ambient_color;
    math::vec3 diffuse_color;
    math::vec3 specular_color;

    /* whether the light is enabled */
    bool enabled;

    /* whether the light should be shadow mapped */
    bool shadow_mapping_enabled;

    /* the current shadow bias */
    double shadow_bias;

    /* the pcf sample count and radius */
    unsigned pcf_samples;
    double pcf_radius;

    /* pcf rotation matrix */
    math::mat2 pcf_rotation;

    /* struct for cached uniforms */
    struct cached_uniforms_struct
    {
        core::struct_uniform& light_uni;
        core::uniform& direction_uni;
        core::uniform& ambient_color_uni;
        core::uniform& diffuse_color_uni;
        core::uniform& specular_color_uni;
        core::uniform& enabled_uni;
        core::uniform& shadow_mapping_enabled_uni;
        core::uniform& shadow_trans_uni;
        core::uniform& shadow_bias_uni;
        core::uniform& pcf_samples_uni;
        core::uniform& pcf_radius_uni;
        core::uniform& pcf_rotation_uni;
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
               , const region::spherical_region<>& _shadow_region = region::spherical_region<> { math::vec3 { 0.0 }, 0.0 }
               , const bool _enabled = true, const bool _shadow_mapping_enabled = true, const double _shadow_bias = 0.0
               , const unsigned _pcf_samples = 0, const double _pcf_radius = 0.0 )
        : position { _position }, att_const { _att_const }, att_linear { _att_linear }, att_quad { _att_quad }
        , ambient_color { _ambient_color }, diffuse_color { _diffuse_color }, specular_color { _specular_color }
        , enabled { _enabled }, shadow_mapping_enabled { _shadow_mapping_enabled }, shadow_bias { _shadow_bias }
        , pcf_samples { _pcf_samples }, pcf_radius { _pcf_radius }
        , pcf_rotation { math::rotate ( math::identity<2> (), glh::math::pi ( 2.0 ) / _pcf_samples, 0, 1 ) }
        , shadow_region { _shadow_region }
    {}

    /* default zero-parameter constructor */
    pointlight () = default;

    /* copy constructor */
    pointlight ( const pointlight& other )
        : position { other.position }
        , att_const { other.att_const }, att_linear { other.att_linear }, att_quad { other.att_quad }
        , ambient_color { other.ambient_color }, diffuse_color { other.diffuse_color }, specular_color { other.specular_color }
        , shadow_region { other.shadow_region }
        , enabled { other.enabled }, shadow_mapping_enabled { other.shadow_mapping_enabled }, shadow_bias { other.shadow_bias }
        , pcf_samples { other.pcf_samples }, pcf_radius { other.pcf_radius }, pcf_rotation { other.pcf_rotation }
    {}

    /* default move constructor */
    pointlight ( pointlight&& other ) = default;

    /* copy assignment operator */
    pointlight& operator= ( const pointlight& other ) 
        { position = other.position;
        ; att_const = other.att_const; att_linear = other.att_linear; att_quad = other.att_quad
        ; ambient_color = other.ambient_color; diffuse_color = other.diffuse_color; specular_color = other.specular_color
        ; shadow_region = other.shadow_region
        ; enabled = other.enabled; shadow_mapping_enabled = other.shadow_mapping_enabled; shadow_bias = other.shadow_bias
    ; return * this; }

    /* default move assignment operator */
    pointlight& operator= ( pointlight&& other ) = default;

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
    void set_position ( const math::vec3& _position ) { position = _position; }

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

    /* get/set_shadow_region
     *
     * get/set the region the light should cast shadows over
     */
    const region::spherical_region<>& get_shadow_region () const { return shadow_region; }
    void set_shadow_region ( const region::spherical_region<>& _shadow_region ) { shadow_region = _shadow_region; }

    /* enable/disable/is_enabled
     *
     * enable/disable the light or get whether is enabled
     */
    void enable () { enabled = true; }
    void disable () { enabled = false; }
    bool is_enabled () const { return enabled; }

    /* enable/disable_shadow_mapping
     * is_shadow_mapping_enabled
     * 
     * enable/disable shadow mapping or get whether shadow mapping is enabled
     */
    void enable_shadow_mapping () { shadow_mapping_enabled = true; }
    void disable_shadow_mapping () { shadow_mapping_enabled = false; }
    bool is_shadow_mapping_enabled () const { return shadow_mapping_enabled; }

    /* get/set_shadow_bias
     *
     * get/set the shadow bias
     */
    const double& get_shadow_bias () const { return shadow_bias; }
    void set_shadow_bias ( const double _shadow_bias ) { shadow_bias = _shadow_bias; }

    /* get/set_pcf_samples/radius
     *
     * get/set the pcf sample count and radius
     */
    const unsigned& get_pcf_samples () const { return pcf_samples; }
    void set_pcf_samples ( const unsigned _pcf_samples ) 
        { pcf_samples = _pcf_samples; pcf_rotation = math::rotate ( math::identity<2> (), glh::math::pi ( 2.0 ) / _pcf_samples, 0, 1 ); }
    const double& get_pcf_radius () const { return pcf_radius; }
    void set_pcf_radius ( const double _pcf_radius ) { pcf_radius = _pcf_radius; }



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

    /* whether the light should be shadow mapped */
    bool shadow_mapping_enabled;

    /* the current shadow bias */
    double shadow_bias;

    /* the pcf sample count and radius */
    unsigned pcf_samples;
    double pcf_radius;

    /* pcf rotation matrix */
    math::mat2 pcf_rotation;

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
        core::uniform& shadow_mapping_enabled_uni;
        core::uniform& shadow_bias_uni;
        core::uniform& shadow_depth_range_mult_uni;
        core::uniform& pcf_samples_uni;
        core::uniform& pcf_radius_uni;
        core::uniform& pcf_rotation_uni;
    };

    /* cached uniforms */
    std::unique_ptr<cached_uniforms_struct> cached_uniforms;

    /* the last shadow region used */
    region::spherical_region<> shadow_region;

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
              , const region::spherical_region<>& _shadow_region = region::spherical_region<> { math::vec3 { 0.0 }, 0.0 }
              , const bool _enabled = true, const bool _shadow_mapping_enabled = true, const double _shadow_bias = 0.0
              , const unsigned _pcf_samples = 0, const double _pcf_radius = 0.0 )
        : position { _position }, direction { math::normalize ( _direction ) }
        , inner_cone { _inner_cone }, outer_cone { _outer_cone }, att_const { _att_const }, att_linear { _att_linear }, att_quad { _att_quad }
        , ambient_color { _ambient_color }, diffuse_color { _diffuse_color }, specular_color { _specular_color }
        , enabled { _enabled }, shadow_mapping_enabled { _shadow_mapping_enabled }, shadow_bias { _shadow_bias }
        , pcf_samples { _pcf_samples }, pcf_radius { _pcf_radius }
        , pcf_rotation { math::rotate ( math::identity<2> (), glh::math::pi ( 2.0 ) / _pcf_samples, 0, 1 ) }
        , shadow_camera { _position, _direction, math::any_perpandicular ( _direction ), _outer_cone, 1.0, 0.1, 0.1 }
        , shadow_region { _shadow_region }
        , shadow_camera_change { true }
    {}

    /* default zero-parameter constructor */
    spotlight () = default;

    /* copy constructor */
    spotlight ( const spotlight& other )
        : position { other.position }, direction { other.direction }
        , inner_cone { other.inner_cone }, outer_cone { other.outer_cone }, att_const { other.att_const }, att_linear { other.att_linear }, att_quad { other.att_quad }
        , ambient_color { other.ambient_color }, diffuse_color { other.diffuse_color }, specular_color { other.specular_color }
        , shadow_region { other.shadow_region }
        , enabled { other.enabled }, shadow_mapping_enabled { other.shadow_mapping_enabled }, shadow_bias { other.shadow_bias }
        , pcf_samples { other.pcf_samples }, pcf_radius { other.pcf_radius }, pcf_rotation { other.pcf_rotation }
        , shadow_camera { other.shadow_camera }, shadow_camera_change { other.shadow_camera_change }
    {}

    /* default move constructor */
    spotlight ( spotlight&& other ) = default;

    /* copy assignment operator */
    spotlight& operator= ( const spotlight& other )
        { position = other.position; direction = other.direction
        ; inner_cone = other.inner_cone; outer_cone = other.outer_cone; att_const = other.att_const; att_linear = other.att_linear; att_quad = other.att_quad
        ; ambient_color = other.ambient_color; diffuse_color = other.diffuse_color; specular_color = other.specular_color
        ; shadow_region = other.shadow_region
        ; enabled = other.enabled; shadow_mapping_enabled = other.shadow_mapping_enabled; shadow_bias = other.shadow_bias
        ; pcf_samples = other.pcf_samples; pcf_radius = other.pcf_radius; pcf_rotation = other.pcf_rotation
        ; shadow_camera = other.shadow_camera; shadow_camera_change = other.shadow_camera_change
    ; return * this; }

    /* default move assignment operator */
    spotlight& operator= ( spotlight&& other ) = default;

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

    /* get/set_shadow_region
     *
     * get/set the region the light should cast shadows over
     */
    const region::spherical_region<>& get_shadow_region () const { return shadow_region; }
    void set_shadow_region ( const region::spherical_region<>& _shadow_region ) { shadow_region = _shadow_region; shadow_camera_change = true; }

    /* enable/disable/is_enabled
     *
     * enable/disable the light or get whether is enabled
     */
    void enable () { enabled = true; }
    void disable () { enabled = false; }
    bool is_enabled () const { return enabled; }

    /* enable/disable_shadow_mapping
     * is_shadow_mapping_enabled
     * 
     * enable/disable shadow mapping or get whether shadow mapping is enabled
     */
    void enable_shadow_mapping () { shadow_mapping_enabled = true; }
    void disable_shadow_mapping () { shadow_mapping_enabled = false; }
    bool is_shadow_mapping_enabled () const { return shadow_mapping_enabled; }

    /* get/set_shadow_bias
     *
     * get/set the shadow bias
     */
    const double& get_shadow_bias () const { return shadow_bias; }
    void set_shadow_bias ( const double _shadow_bias ) { shadow_bias = _shadow_bias; }

    /* get/set_pcf_samples/radius
     *
     * get/set the pcf sample count and radius
     */
    const unsigned& get_pcf_samples () const { return pcf_samples; }
    void set_pcf_samples ( const unsigned _pcf_samples ) 
        { pcf_samples = _pcf_samples; pcf_rotation = math::rotate ( math::identity<2> (), glh::math::pi ( 2.0 ) / _pcf_samples, 0, 1 ); }
    const double& get_pcf_radius () const { return pcf_radius; }
    void set_pcf_radius ( const double _pcf_radius ) { pcf_radius = _pcf_radius; }



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

    /* whether the light should be shadow mapped */
    bool shadow_mapping_enabled;

    /* the current shadow bias */
    double shadow_bias;

    /* the pcf sample count and radius */
    unsigned pcf_samples;
    double pcf_radius;

    /* pcf rotation matrix */
    math::mat2 pcf_rotation;

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
        core::uniform& shadow_mapping_enabled_uni;
        core::uniform& shadow_trans_uni;
        core::uniform& shadow_bias_uni;
        core::uniform& shadow_depth_range_mult_uni;
        core::uniform& pcf_samples_uni;
        core::uniform& pcf_radius_uni;
        core::uniform& pcf_rotation_uni;
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



/* LIGHT_SYSTEM DEFINITION */

/* class light_system
 *
 * several collection of lights for each type
 */
class glh::lighting::light_system
{
public:

    /* full constructor
     *
     * shadow_map_width: an optional initial width for the shadow maps (defaults to 1024)
     */
    light_system ( const unsigned _shadow_map_width = 1024 );

    /* deleted copy constructor */
    light_system ( const light_system& other ) = delete;

    /* default move constructor */
    light_system ( light_system&& other ) = default;

    /* deleted copy assignment operator */
    light_system& operator= ( const light_system& other ) = delete;

    /* default destructor */
    ~light_system () = default;



    /* add_...light
     *
     * add a light to the system
     */
    void add_dirlight ( const math::vec3& direction
                      , const math::vec3& ambient_color, const math::vec3& diffuse_color, const math::vec3& specular_color
                      , const region::spherical_region<>& _shadow_region = region::spherical_region<> { math::vec3 { 0.0 }, 0.0 }
                      , const bool enabled = true, const bool shadow_mapping_enabled = true, const double shadow_bias = 0.0
                      , const unsigned pcf_samples = 0, const double pcf_radius = 0.0 )
        { dirlights.emplace_back ( direction, ambient_color, diffuse_color, specular_color, _shadow_region, enabled, shadow_mapping_enabled, shadow_bias, pcf_samples, pcf_radius ); }
    void add_pointlight ( const math::vec3& position
                        , const double att_const, const double att_linear, const double att_quad
                        , const math::vec3& ambient_color, const math::vec3& diffuse_color, const math::vec3& specular_color
                        , const region::spherical_region<>& _shadow_region = region::spherical_region<> { math::vec3 { 0.0 }, 0.0 }
                        , const bool enabled = true, const bool shadow_mapping_enabled = true, const double shadow_bias = 0.0
                        , const unsigned pcf_samples = 0, const double pcf_radius = 0.0 )
        { pointlights.emplace_back ( position, att_const, att_linear, att_quad, ambient_color, diffuse_color, specular_color, _shadow_region, enabled, shadow_mapping_enabled, shadow_bias, pcf_samples, pcf_radius ); }
    void add_spotlight ( const math::vec3& position, const math::vec3& direction
                       , const double inner_cone, const double outer_cone, const double att_const, const double att_linear, const double att_quad
                       , const math::vec3& ambient_color, const math::vec3& diffuse_color, const math::vec3& specular_color
                       , const region::spherical_region<>& _shadow_region = region::spherical_region<> { math::vec3 { 0.0 }, 0.0 }
                       , const bool enabled = true, const bool shadow_mapping_enabled = true, const double shadow_bias = 0.0
                       , const unsigned pcf_samples = 0, const double pcf_radius = 0.0 )
        { spotlights.emplace_back ( position, direction, inner_cone, outer_cone, att_const, att_linear, att_quad, ambient_color, diffuse_color, specular_color, _shadow_region, enabled, shadow_mapping_enabled, shadow_bias, pcf_samples, pcf_radius ); }



    /* ...light_at
     *
     * get a light at an index
     */
    dirlight& dirlight_at ( const unsigned index ) { return dirlights.at ( index ); }
    const dirlight& dirlight_at ( const unsigned index ) const { return dirlights.at ( index ); }
    pointlight& pointlight_at ( const unsigned index ) { return pointlights.at ( index ); }
    const pointlight& pointlight_at ( const unsigned index ) const { return pointlights.at ( index ); }
    spotlight& spotlight_at ( const unsigned index ) { return spotlights.at ( index ); }
    const spotlight& spotlightt_at ( const unsigned index ) const { return spotlights.at ( index ); }



    /* remove_...light
     *
     * remove a light at an index
     */
    void remove_dirlight ( const unsigned index ) { dirlights.erase ( dirlights.begin () + index ); }
    void remove_pointlight ( const unsigned index ) { pointlights.erase ( pointlights.begin () + index ); }
    void remove_spotlight ( const unsigned index ) { spotlights.erase ( spotlights.begin () + index ); }



    /* ...light_count
     *
     * get the number of lights
     */
    unsigned dirlight_count () const { return dirlights.size (); }
    unsigned pointlight_count () const { return pointlights.size (); }
    unsigned spotlight_count () const { return spotlights.size (); }



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

    /* recache_uniforms
     *
     * recache the light collections based on the currently cached uniforms
     */
    void recache_uniforms ();



    /* bind_shadow_maps_fbo
     *
     * reallocates the 2d texture array to size max ( dirlights + pointlights * 2 + spotlights, 1 ), then binds the shadow map fbo
     * also resizes the viewport to the size of the shadow map
     */
    void bind_shadow_maps_fbo () const;



    /* requires_shadow_mapping
     *
     * true if there are lights which require shadow mapping
     */
    bool requires_shadow_mapping () const;



    /* get_shadow_map_width
     *
     * get the width of the shadow maps
     */
    const unsigned& get_shadow_map_width () const { return shadow_map_width; }



private:

    /* arrays of types of lights */
    std::vector<dirlight> dirlights;
    std::vector<pointlight> pointlights;
    std::vector<spotlight> spotlights;

    /* texture2d array for shadow maps */
    mutable core::texture2d_array shadow_maps;

    /* framebuffer for shadow mapping */
    core::fbo shadow_maps_fbo;

    /* the size of the shadow maps */
    const unsigned shadow_map_width;

    /* struct for cached uniforms */
    struct cached_uniforms_struct
    {
        core::struct_uniform& light_system_uni;
        core::uniform& dirlights_size_uni;
        core::struct_array_uniform& dirlights_uni;
        core::uniform& pointlights_size_uni;
        core::struct_array_uniform& pointlights_uni;
        core::uniform& spotlights_size_uni;
        core::struct_array_uniform& spotlights_uni;
        core::uniform& shadow_maps_uni;
    };

    /* cached uniforms */
    std::unique_ptr<cached_uniforms_struct> cached_uniforms;

};



/* #ifndef GLHELPER_LIGHTING_HPP_INCLUDED */
#endif