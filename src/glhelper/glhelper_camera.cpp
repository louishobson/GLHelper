/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * src/glhelper/glhelper_camera.cpp
 * 
 * implementation of include/glhelper/glhelper_camera.hpp
 * 
 */



/* INCLUDES */

/* include glhelper_camera.hpp */
#include <glhelper/glhelper_camera.hpp>



/* CAMERA_PERSPECTIVE IMPLEMENTATION */

/* full constructor
 *
 * give parameters for look_at and perspective_fov
 */
glh::camera_perspective::camera_perspective ( const math::vec3& _pos, const math::vec3& _direction, const math::vec3& _world_up, const double _fov, const double _aspect, const double _near, const double _far )
    : pos { _pos }
    , z { math::norm ( - _direction ) }
    , world_up { math::norm ( _world_up ) }
    , x { math::cross ( world_up, z ) }
    , y { math::cross ( z, x ) }
    , fov { _fov }
    , aspect { _aspect }
    , near { _near }
    , far { _far }
    , view_change { true }
    , proj_change { true }
{
    /* update */
    update ();
}

/* move_relative
 *
 * move camera relative to its own axis
 * 
 * vec: vector for movement (+x: right, +y: up, +z: back)
 *
 * return: new position vector
 */
const glh::math::vec3 glh::camera_perspective::move_relative ( const math::vec3& vec )
{
    /* move pos */
    pos += x * vec.at ( 0 );
    pos += y * vec.at ( 1 );
    pos += z * vec.at ( 2 );

    /* set view as changed */
    view_change = true;

    /* return pos */
    return pos;
}

/* move_global
 *
 * move camera relative to the global axis
 * 
 * vec: vector for movement (+x: x, +y: y, +z: z)
 * 
 * return: new position vector
 */
const glh::math::vec3 glh::camera_perspective::move_global ( const math::vec3& vec )
{
    /* set view as changed */
    view_change = true;
    
    /* return vec added to pos */
    return ( pos += vec );
}

/* pitch/yaw/world_yaw/roll
 *
 * rotate the camera via changed in pitch/roll/yaw
 *
 * arg: the angle in radians
 * 
 * return: the new direction unit vector
 */
const glh::math::vec3 glh::camera_perspective::pitch ( const double arg )
{
    /* rotate y and z around x axis */
    y = math::rotate ( y, arg, x );
    z = math::rotate ( z, arg, x );

    /* set view as changed and return */
    view_change = true;
    return pos;
}
const glh::math::vec3 glh::camera_perspective::yaw ( const double arg )
{
    /* rotate x and z around the y axis */
    x = math::rotate ( x, arg, y );
    z = math::rotate ( z, arg, y );

    /* set view as changed and return */
    view_change = true;
    return pos;
}
const glh::math::vec3 glh::camera_perspective::world_yaw ( const double arg )
{
    /* rotate x, y and z around the world_up axis */
    x = math::rotate ( x, arg, world_up );
    z = math::rotate ( z, arg, world_up );
    y = math::rotate ( y, arg, world_up );

    /* set view as changed and return */
    view_change = true;
    return pos;
}
const glh::math::vec3 glh::camera_perspective::roll ( const double arg )
{
    /* rotate x and y around the z axis */
    x = math::rotate ( x, arg, z );
    y = math::rotate ( y, arg, z );

    /* set view as changed and return */
    view_change = true;
    return pos;
}

/* get_trans
 *
 * recieve the transformation
 */
const glh::math::mat4& glh::camera_perspective::get_trans () const
{
    /* update and return trans */
    update ();
    return trans;
}

/* update
 *
 * update view, proj then trans if changes to parameters have occured
 * 
 * return: bool for if any changes were applied
 */
bool glh::camera_perspective::update () const
{
    /* true if any change is to occur */
    bool change_occured = view_change || proj_change;

    /* if view has been changed, update */
    if ( view_change )
    {
        view = math::camera ( pos, x, y, z );
        view_change = false;
    }
    
    /* if proj has been changed, update */
    if ( proj_change )
    {
        proj = math::perspective_fov ( fov, aspect, near, far );
        proj_change = false;
    }

    /* if any change occured, update trans */
    if ( change_occured ) trans = proj * view;

    /* return change_occured */
    return change_occured;
}