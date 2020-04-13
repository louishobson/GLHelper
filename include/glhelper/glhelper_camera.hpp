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
    /* class camera_perspective
     *
     * encapsulates a camera and perspective matrix, abstracting movement control
     */
    class camera_perspective;
}



/* CAMERA_PERSPECTIVE DEFINITION */

/* class camera_perspective
 *
 * encapsulates a camera and perspective matrix, abstracting movement control
 */
class glh::camera_perspective
{
public:

    /* full constructor
     *
     * give parameters for look_towards and perspective_fov
     */
    camera_perspective ( const math::vec3& _position, const math::vec3& _direction, const math::vec3& _world_y, const double _fov, const double _aspect, const double _near, const double _far );

    /* minimal constructor
     *
     * create a default view matrix and give parameters for perspective_fov
     */
    camera_perspective ( const double _fov, const double _aspect, const double _near, const double _far )
        : camera_perspective { math::vec3 { 0.0, 0.0, 0.0 }, math::vec3 { 0.0, 0.0, -1.0 }, math::vec3 { 0.0, 1.0, 0.0 }, _fov, _aspect, _near, _far } {}
        
    /* zero parameter constructor
     *
     * produce a default view and projection matrix
     */
    camera_perspective () : camera_perspective { math::rad ( 60.0 ), 16.0 / 9.0, 0.1, 200.0 } {}

    /* default copy constructor */
    camera_perspective ( const camera_perspective& other ) = default;

    /* default assignment operator */
    camera_perspective& operator= ( const camera_perspective& other ) = default;

    /* default destructor */
    ~camera_perspective () = default;



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



    /* apply
     *
     * apply the camera to view and projection matrices
     * 
     * view_uni: 4x4 matrix uniform for the view matrix
     * proj_uni: 4x4 matrix uniform for the projection matrix
     */
    void apply ( const uniform& view_uni, const uniform& proj_uni );
    void apply () const;

    /* cache_view_uniform
     *
     * cache the view matrix uniform
     * 
     * view_uni: 4x4 matrix uniform for the view matrix
     */
    void cache_view_uniform ( const uniform& view_uni );

    /* cache_proj_uniform
     *
     * cache the projection matrix uniform
     * 
     * proj_uni: 4x4 matrix uniform for the projection matrix
     */
    void cache_proj_uniform ( const uniform& proj_uni );

    /* cache_uniforms
     *
     * cache all uniforms simultaneously
     *
     * view_uni: 4x4 matrix uniform for the view matrix
     * proj_uni: 4x4 matrix uniform for the projection matrix
     */
    void cache_uniforms ( const uniform& view_uni, const uniform& proj_uni );




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



    /* get_position
     *
     * get the current viewing position
     */
    const math::vec3& get_position () const { return position; }

    /* get_direction
     *
     * get the current viewing direction
     */
    math::vec3 get_direction () const { return -z; }

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

    /* view/proj_change
     *
     * booleans to represent if view or proj have been changed
     */
    mutable bool view_change;
    mutable bool proj_change;

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

   

    /* cached uniforms */
    std::unique_ptr<uniform> cached_view_uniform;
    std::unique_ptr<uniform> cached_proj_uniform;

    

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

