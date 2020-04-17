/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * include/glhelper/glhelper_camera.hpp
 * 
 * camera movement and projection abstraction
 * 
 */



/* HEADER GUARD */
#ifndef GLHELPER_CAMERA_HPP_INCLUDED
#define GLHELPER_CAMERA_HPP_INCLUDED



/* INCLUDES */

/* include core headers */
#include <iostream>
#include <memory>

/* include glhelper_core.hpp */
#include <glhelper/glhelper_core.hpp>

/* include glhelper_exception.hpp */
#include <glhelper/glhelper_exception.hpp>

/* include glhelper_math.hpp */
#include <glhelper/glhelper_math.hpp>

/* include glhelper_shader.hpp */
#include <glhelper/glhelper_shader.hpp>



/* NAMESPACE FORWARD DECLARATIONS */

namespace glh
{
    namespace camera
    {
        /* class camera_base
         *
         * base class for all cameras
         */
        class camera_base;

        /* class camera : camera_base
         *
         * handles both the view and projection matrix
         * abstracts movement and rotation
         */
        class camera;

        /* class mirror_camera : camera_base
         *
         * takes a camera and mirror dimensions and position to make a camera for the mirror surface
         */
        class mirror_camera;
    }
}



/* CAMERA_BASE DEFINITION */

/* class camera_base
 *
 * base class for all cameras
 */
class glh::camera::camera_base
{
public:

    /* default zero-parameter constructor */
    camera_base () = default;

    /* copy constructor
     *
     * non-default as cannot copy cached uniforms
     */
    camera_base ( const camera_base& other ) {}

    /* default move constructor */
    camera_base ( camera_base&& other ) = default;

    /* copy assignment operator
     *
     * non-default for same reason as copy constructor
     */
    camera_base& operator= ( const camera_base& other ) 
    { return * this; }

    /* default move assignment operator */
    camera_base& operator= ( camera_base&& other ) = default;

    /* default destructor */
    virtual ~camera_base () = default;



    /* apply
     *
     * apply the camera to view and projection matrices
     * 
     * view_uni: 4x4 matrix uniform for the view matrix
     * proj_uni: 4x4 matrix uniform for the projection matrix
     */
    void apply ( const core::uniform& view_uni, const core::uniform& proj_uni );
    void apply () const;

    /* cache_view_uniform
     *
     * cache the view matrix uniform
     * 
     * view_uni: 4x4 matrix uniform for the view matrix
     */
    void cache_view_uniform ( const core::uniform& view_uni );

    /* cache_proj_uniform
     *
     * cache the projection matrix uniform
     * 
     * proj_uni: 4x4 matrix uniform for the projection matrix
     */
    void cache_proj_uniform ( const core::uniform& proj_uni );

    /* cache_uniforms
     *
     * cache all uniforms simultaneously
     *
     * view_uni: 4x4 matrix uniform for the view matrix
     * proj_uni: 4x4 matrix uniform for the projection matrix
     */
    void cache_uniforms ( const core::uniform& view_uni, const core::uniform& proj_uni );



    /* get_view
     *
     * get the view matrix
     */
    const math::mat4& get_view () const;

    /* get_proj
     *
     * get the projection matrix
     */
    const math::mat4& get_proj () const;

    /* get_trans
     *
     * recieve the full transformation
     * found by projection * view
     */
    const math::mat4& get_trans () const;



protected:

    /* cached uniforms */
    std::unique_ptr<core::uniform> cached_view_uniform;
    std::unique_ptr<core::uniform> cached_proj_uniform;



    /* view
     *
     * the current view matrix
     */
    mutable math::mat4 view;

    /* proj
     *
     * the current projection matrix
     */
    mutable math::mat4 proj;

    /* trans
     *
     * the current combination of the view and projection matrix
     */
    mutable math::mat4 trans;



    /* update_view
     *
     * update the view matrix
     */
    virtual bool update_view () const = 0;

    /* update_proj
     *
     * update the projection matrix
     */
    virtual bool update_proj () const = 0;

    /* update_trans
     *
     * update view, proj then trans if changes to parameters have occured
     * 
     * return: bool for if any changes were applied
     */
    virtual bool update_trans () const = 0;

};




/* CAMERA DEFINITION */

/* class camera
 *
 * handles both the view and projection matrix
 * abstracts movement and rotation
 */
class glh::camera::camera : public camera_base
{
public:

    /* full constructor
     *
     * give parameters for look_towards and perspective_fov
     */
    camera ( const math::vec3& _position, const math::vec3& _direction, const math::vec3& _world_y, const double _fov, const double _aspect, const double _near, const double _far );

    /* minimal constructor
     *
     * create a default view matrix and give parameters for perspective_fov
     */
    camera ( const double _fov, const double _aspect, const double _near, const double _far )
        : camera { math::vec3 { 0.0, 0.0, 0.0 }, math::vec3 { 0.0, 0.0, -1.0 }, math::vec3 { 0.0, 1.0, 0.0 }, _fov, _aspect, _near, _far } {}
        
    /* zero parameter constructor
     *
     * produce a default view and projection matrix
     */
    camera () : camera { math::rad ( 60.0 ), 16.0 / 9.0, 0.1, 200.0 } {}

    /* default copy constructor */
    camera ( const camera& other ) = default;

    /* default assignment operator */
    camera& operator= ( const camera& other ) = default;

    /* default destructor */
    ~camera () = default;



    /* enable/disable_restrictive_mode
     *
     * enables/disables restrictive movement mode
     * 
     * when restricted, roll is disabled, and movement occures irrespective of pitch
     * pitch is limited to 90 degrees up and down
     */
    void enable_restrictive_mode ();
    void disable_restrictive_mode ();

    /* move
     *
     * move camera relative to its own axis
     * when unrestricted, the camera moves completely based on its own axis
     * when restricted, y movement is global, but x and z are dependant on yaw
     * 
     * vec: vector for movement
     *
     * return: new position vector
     */
    const math::vec3& move ( const math::vec3& vec );

    /* move_global
     *
     * move camera relative to the global axis
     * 
     * vec: vector for movement (+x: x, +y: y, +z: z)
     * 
     * return: new position vector
     */
    const math::vec3& move_global ( const math::vec3& vec );

    /* pitch/yaw/roll
     *
     * rotate the camera via changes in pitch/yaw/roll
     * how this affects the camera is dependant on whether restrictive movement is active
     *
     * arg: the angle in radians
     * 
     * return: the position vector
     */
    const math::vec3& pitch ( const double arg );
    const math::vec3& yaw ( const double arg );
    const math::vec3& roll ( const double arg );



    /* get/set_fov
     *
     * get/set the field of view
     */
    const double& get_fov () const { return fov; }
    void set_fov ( const double _fov ) { fov = _fov; proj_change = true; }

    /* get/set_aspect
     *
     * get/set the aspect ratio
     */
    const double& get_aspect () const { return aspect; }
    void set_aspect ( const double _aspect ) { aspect = _aspect; proj_change = true; }

    /* get/set_near
     *
     * get/set the near position of the perspective projection matrix
     */
    const double& get_near () const { return near; }
    void set_near ( const double _near ) { near = _near; proj_change = true; }

    /* get/set_far
     *
     * get/set the far position of the perspective projection matrix
     */
    const double& get_far () const { return far; }
    void set_far ( const double _far ) { far = _far; proj_change = true; }

    /* get/set_position
     *
     * get/set the current viewing position
     */
    const math::vec3& get_position () const { return position; }
    void set_position ( const math::vec3& _position ) { position = _position; view_change = true; }

    /* get_direction
     *
     * get the current viewing direction
     */
    math::vec3 get_direction () const { return -z; }



private:

    /* view matrix parameters */
    math::vec3 position;
    math::vec3 x;
    math::vec3 y;
    math::vec3 z;
    math::vec3 restrict_x;
    math::vec3 restrict_y;
    math::vec3 restrict_z;

    /* movement restriction */
    bool restrictive_mode;

    /* perspective projection matrix parameters */
    double fov;
    double aspect;
    double near;
    double far;

    /* changes to the matrices in the camera */
    mutable bool view_change;
    mutable bool proj_change;

    

    /* update_view
     *
     * update the view matrix
     */
    bool update_view () const;

    /* update_proj
     *
     * update the projection matrix
     */
    bool update_proj () const;

    /* update_trans
     *
     * update view, proj then trans if changes to parameters have occured
     * 
     * return: bool for if any changes were applied
     */
    bool update_trans () const;

};



/* MIRROR_CAMERA DEFINITION */

/* class mirror_camera
 *
 * takes a camera and mirror dimensions and position to make a camera for the mirror surface
 */
class glh::camera::mirror_camera : public camera_base
{
public:

    /* full constructor
     *
     * takes another camera and mirror positioning
     *
     * _cam: the viewer camera
     * _position: the position of the centre of the mirror
     * _normal: the normal to the mirror
     * _width: the width of the mirror
     * _height: the height of the mirror
     * _wup: the world up vector (defaults to 0,1,0)
     */
    mirror_camera ( const camera& _cam, const math::vec3& _position, const math::vec3& _normal, const double _width, const double _height, const math::vec3& _wup = math::vec3 { 0.0, 1.0, 0.0 })
        : cam { _cam }, position { _position }, normal { math::normalise ( _normal ) }, half_width { _width / 2.0 }, half_height { _height / 2.0 }, wup { math::normalise ( _wup ) }
    {}



private:

    /* the camera the user is looking through */
    const camera& cam;

    /* parameters form the mirror */
    math::vec3 position;
    math::vec3 normal;
    double half_width;
    double half_height;
    math::vec3 wup;



    /* update_view
     *
     * update the view matrix
     */
    bool update_view () const;

    /* update_proj
     *
     * update the projection matrix
     */
    bool update_proj () const;

    /* update_trans
     *
     * update view, proj then trans if changes to parameters have occured
     * 
     * return: bool for if any changes were applied
     */
    bool update_trans () const;

};



/* #ifndef GLHELPER_CAMERA_HPP_INCLUDED */
#endif

