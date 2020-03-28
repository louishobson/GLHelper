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

/* include glhelper_core.hpp */
#include <glhelper/glhelper_core.hpp>

/* include glhelper_exception.hpp */
#include <glhelper/glhelper_exception.hpp>

/* include glhelper_matrix.hpp */
#include <glhelper/glhelper_matrix.hpp>

/* include glhelper_vector.hpp */
#include <glhelper/glhelper_vector.hpp>

/* include glhelper_transform.hpp */
#include <glhelper/glhelper_transform.hpp>



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
    camera_perspective ( const math::vec3& _pos, const math::vec3& _direction, const math::vec3& _world_up, const double _fov, const double _aspect, const double _near, const double _far );

    /* minimal constructor
     *
     * create a default view matrix and give parameters for perspective_fov
     */
    camera_perspective ( const double _fov, const double _aspect, const double _near, const double _far )
        : camera_perspective { math::vec3 { 0., 0., 0. }, math::vec3 { -1., 0., 0. }, math::vec3 { 0., 1., 0. }, _fov, _aspect, _near, _far } {}
        
    /* zero parameter constructor
     *
     * produce a default view and projection matrix
     */
    camera_perspective () : camera_perspective { math::rad ( 45. ), 16. / 9., 0.1, 100. } {}

    /* default copy constructor */
    camera_perspective ( const camera_perspective& other ) = default;

    /* default assignment operator */
    camera_perspective& operator= ( const camera_perspective& other ) = default;

    /* default destructor */
    ~camera_perspective () = default;



    /* move_relative
     *
     * move camera relative to its own axis
     * 
     * vec: vector for movement (+x: right, +y: up, +z: back)
     *
     * return: new position vector
     */
    const math::vec3 move_relative ( const math::vec3& vec );

    /* move_global
     *
     * move camera relative to the global axis
     * 
     * vec: vector for movement (+x: x, +y: y, +z: z)
     * 
     * return: new position vector
     */
    const math::vec3 move_global ( const math::vec3& vec );

    /* pitch/yaw/world_yaw/roll
     *
     * rotate the camera via changed in pitch/roll/yaw
     *
     * arg: the angle in radians
     * 
     * return: the new direction unit vector
     */
    const math::vec3 pitch ( const double arg );
    const math::vec3 yaw ( const double arg );
    const math::vec3 world_yaw ( const double arg );
    const math::vec3 roll ( const double arg );



    /* set_fov
     *
     * set the field of view
     */
    void set_fov ( const double _fov ) { fov = _fov; proj_change = true; }

    /* set_aspect
     *
     * set the aspect ratio
     */
    void set_aspect ( const double _aspect ) { aspect = _aspect; proj_change = true; }

    /* set_near
     *
     * set the near position of the perspective projection matrix
     */
    void set_near ( const double _near ) { near = _near; proj_change = true; }

    /* set_far
     *
     * set the far position of the perspective projection matrix
     */
    void set_far ( const double _far ) { far = _far; proj_change = true; }



    /* get_trans
     *
     * recieve the transformation
     */
    const math::mat4& get_trans () const;

private:

    /* view matrix parameters */
    math::vec3 pos;
    math::vec3 direction;
    math::vec3 world_up;
    math::vec3 right;
    math::vec3 up;

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

    /* update
     *
     * update view, proj then trans if changes to parameters have occured
     * 
     * return: bool for if any changes were applied
     */
    bool update () const;

};



/* #ifndef GLHELPER_CAMERA_HPP_INCLUDED */
#endif

