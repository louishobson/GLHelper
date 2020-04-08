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
glh::camera_perspective::camera_perspective ( const math::vec3& _pos, const math::vec3& _direction, const math::vec3& _world_y, const double _fov, const double _aspect, const double _near, const double _far )
    : pos { _pos }
    , x { 0. }
    , y { 0. }
    , z { 0. }
    , restrict_x { 0. }
    , restrict_y { 0. }
    , restrict_z { 0. }
    , restrictive_mode { false }
    , fov { _fov }
    , aspect { _aspect }
    , near { _near }
    , far { _far }
    , view_change { true }
    , proj_change { true }
{
    /* set x, y, z, restrict_x, restrict_y and restrict_z */
    z = math::norm ( - _direction );
    x = math::cross ( math::norm ( _world_y ), z );
    y = math::cross ( z, x );
    restrict_x = x;
    restrict_y = y;
    restrict_z = z;

    /* update trans */
    update_trans ();
}



/* enable/disable_restrictive_mode
 *
 * enables/disables restrictive movement mode
 * 
 * when restricted, roll is disabled, and movement occures irrespective of pitch
 * pitch is limited to 90 degrees up and down
 */
void glh::camera_perspective::enable_restrictive_mode ()
{
    /* set to true */
    restrictive_mode = true;

    /* set the values of restrict_xyz */
    restrict_x = x;
    restrict_y = y;
    restrict_z = z;
}
void glh::camera_perspective::disable_restrictive_mode () 
{
    /* purely set to false */ 
    restrictive_mode = false;
}

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
const glh::math::vec3& glh::camera_perspective::move ( const math::vec3& vec )
{
    
    /* move pos
     * if non-restricted use xyz, otherwise use restricted values
    */
    if ( !restrictive_mode )
    {
        pos += x * vec.at ( 0 );
        pos += y * vec.at ( 1 );
        pos += z * vec.at ( 2 );
    } else
    {
        pos += restrict_x * vec.at ( 0 );
        pos += restrict_y * vec.at ( 1 );
        pos += restrict_z * vec.at ( 2 );
    } 
    

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
const glh::math::vec3& glh::camera_perspective::move_global ( const math::vec3& vec )
{
    /* set view as changed */
    view_change = true;
    
    /* return vec added to pos */
    return ( pos += vec );
}

/* pitch/yaw/roll
 *
 * rotate the camera via changes in pitch/yaw/roll
 * how this affects the camera is dependant on whether restrictive movement is active
 *
 * arg: the angle in radians
 * 
 * return: the position vector
 */
const glh::math::vec3& glh::camera_perspective::pitch ( const double arg )
{
    /* if non-restrictive, rotate y and z around x axis */
    if ( !restrictive_mode )
    {
        y = math::rotate ( y, arg, x );
        z = math::rotate ( z, arg, x );
    } else
    /* otherwise, rotate y and z around the restrict_x axis */
    {
        /* if trying to pitch beyond vertical, reduce arg accordingly */
        double pitch_angle = math::angle ( restrict_y, z );
        if ( pitch_angle + arg > math::rad ( 180 ) )
        {
            y = math::rotate ( y, math::rad ( 180 ) - pitch_angle, restrict_x );
            z = math::rotate ( z, math::rad ( 180 ) - pitch_angle, restrict_x );
        } else
        if ( pitch_angle + arg < math::rad ( 0 ) )
        {
            y = math::rotate ( y, math::rad ( 0 ) - pitch_angle, restrict_x );
            z = math::rotate ( z, math::rad ( 0 ) - pitch_angle, restrict_x );
        } else
        {        
            y = math::rotate ( y, arg, restrict_x );
            z = math::rotate ( z, arg, restrict_x );
        }
    }

    /* set view as changed and return */
    view_change = true;
    return pos;
}
const glh::math::vec3& glh::camera_perspective::yaw ( const double arg )
{
    /* if non-restrictive, rotate x and z around the y axis */
    if ( !restrictive_mode )
    {
        x = math::rotate ( x, arg, y );
        z = math::rotate ( z, arg, y );
    } else
    /* otherwise, rotate x, restict_x, y, z and restrict_z around the restrict_y axis */
    {
        x = math::rotate ( x, arg, restrict_y );
        restrict_x = math::rotate ( restrict_x, arg, restrict_y );
        y = math::rotate ( y, arg, restrict_y );
        z = math::rotate ( z, arg, restrict_y );
        restrict_z = math::rotate ( restrict_z, arg, restrict_y );
    }    

    /* set view as changed and return */
    view_change = true;
    return pos;
}
const glh::math::vec3& glh::camera_perspective::roll ( const double arg )
{
    /* if non-restrictive, rotate x and y around the z axis */
    if ( !restrictive_mode )
    {
        x = math::rotate ( x, arg, z );
        y = math::rotate ( y, arg, z );
    } else
    /* otherwise return pos without change */
    return pos;

    /* set view as changed and return */
    view_change = true;
    return pos;
}

/* get_view
 *
 * get the view matrix
 */
const glh::math::mat4& glh::camera_perspective::get_view () const
{
    /* update and return view */
    update_view ();
    return view;
}

/* get_proj
 *
 * get the projection matrix
 */
const glh::math::mat4& glh::camera_perspective::get_proj () const
{
    /* update and return proj */
    update_proj ();
    return proj;
}

/* get_trans
 *
 * recieve the transformation
 */
const glh::math::mat4& glh::camera_perspective::get_trans () const
{
    /* update and return trans */
    update_trans ();
    return trans;
}

/* update_view
 *
 * update the view matrix
 */
bool glh::camera_perspective::update_view () const
{
    /* if view has been changed, update */
    if ( view_change )
    {
        view = math::camera ( pos, x, y, z );
        view_change = false;
        return true;
    } else std::cout << "nochange\n";

    /* else return false */
    return false;
}

/* update_proj
 *
 * update the projection matrix
 */
bool glh::camera_perspective::update_proj () const
{
    /* if proj has been changed, update */
    if ( proj_change )
    {
        proj = math::perspective_fov ( fov, aspect, near, far );
        proj_change = false;
        return true;
    }

    /* else return false */
    return false;

}

/* update_trans
 *
 * update view, proj then trans if changes to parameters have occured
 * 
 * return: bool for if any changes were applied
 */
bool glh::camera_perspective::update_trans () const
{
    /* if any change occured in updating view and proj, update trans */
    bool view_update = update_view ();
    bool proj_update = update_proj ();
    if ( view_update || proj_update ) 
    {
        trans = proj * view;
        return true;
    }
    
    /* else return false */
    return false;
}