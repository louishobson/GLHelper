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



/* CAMERA_BASE IMPLEMENTATION */

/* apply
 *
 * apply the camera to view and projection matrices
 * 
 * camera_uni: the uniform to cache and apply to
 */
void glh::camera::camera_base::apply ( core::struct_uniform& camera_uni )
{
    /* cache the uniforms */
    cache_uniforms ( camera_uni );

    /* apply */
    apply ();
}

/* apply
 *
 * apply the camera to view and projection matrices
 * 
 * view_uni: 4x4 matrix uniform for the view matrix
 * proj_uni: 4x4 matrix uniform for the projection matrix
 */
void glh::camera::camera_base::apply () const
{
    /* throw if uniforms are not already cached */
    if ( !cached_uniforms ) throw exception::uniform_exception { "attempted to apply camera without a complete uniform cache" };

    /* update parameters */
    update_parameters ();

    /* set the parameters */
    cached_uniforms->view_uni.set_matrix ( view );
    cached_uniforms->proj_uni.set_matrix ( proj );
    cached_uniforms->view_proj_uni.set_matrix ( view_proj );
    cached_uniforms->viewpos_uni.set_vector ( viewpos );
}

/* cache_uniforms
 *
 * camera_uni: the uniform to cache
 */
void glh::camera::camera_base::cache_uniforms ( core::struct_uniform& camera_uni )
{
    /* if uniforms are not already cached, cache the new ones */
    if ( !cached_uniforms || cached_uniforms->camera_uni != camera_uni )
    {
        cached_uniforms.reset ( new cached_uniforms_struct
        {
            camera_uni,
            camera_uni.get_uniform ( "view" ),
            camera_uni.get_uniform ( "proj" ),
            camera_uni.get_uniform ( "view_proj" ),
            camera_uni.get_uniform ( "viewpos" )
        } );
    }
}

/* get_view
 *
 * get the view matrix
 */
const glh::math::mat4& glh::camera::camera_base::get_view () const
{
    /* update and return view */
    update_parameters ();
    return view;
}

/* get_proj
 *
 * get the projection matrix
 */
const glh::math::mat4& glh::camera::camera_base::get_proj () const
{
    /* update and return proj */
    update_parameters ();
    return proj;
}

/* get_view_proj
 *
 * recieve the view_proj matrix
 */
const glh::math::mat4& glh::camera::camera_base::get_view_proj () const
{
    /* update and return trans */
    update_parameters ();
    return view_proj;
}




/* update_parameters
 *
 * update the camera parameters, if any need updating
 */
void glh::camera::camera_base::update_parameters () const
{
    /* if any change to view matrix, update related parameters */
    if ( view_change )
    {
        view = create_view ();
        viewpos = math::vec3 { math::inverse ( view ) * math::vec4 { 0.0, 0.0, 0.0, 1.0 } };
    }

    /* if any change to proj matrix, update related parameters */
    if ( proj_change )
    {
        proj = create_proj ();
    }

    /* if any change to either, update view_proj parameters */
    if ( view_change || proj_change )
    {
        view_proj = proj * view;
    }

    view_change = false;
    proj_change = false;
}




/* CAMERA_MOVEMENT IMPLEMENTATION */

/* full constructor
 *
 * give parameters for look towards to create view matrix
 */
glh::camera::camera_movement::camera_movement ( const math::vec3& _position, const math::vec3& _direction, const math::vec3& _world_y )
    : position { _position }
    , x { 0. }
    , y { 0. }
    , z { 0. }
    , restrict_x { 0. }
    , restrict_y { 0. }
    , restrict_z { 0. }
    , restrictive_mode { false }
{
    /* set x, y, z, restrict_x, restrict_y and restrict_z */
    z = math::normalize ( - _direction );
    x = math::cross ( math::normalize ( _world_y ), z );
    y = math::cross ( z, x );
    restrict_x = x;
    restrict_y = y;
    restrict_z = z;
}



/* enable/disable_restrictive_mode
 *
 * enables/disables restrictive movement mode
 * 
 * when restricted, roll is disabled, and movement occures irrespective of pitch
 * pitch is limited to 90 degrees up and down
 */
void glh::camera::camera_movement::enable_restrictive_mode ()
{
    /* set to true */
    restrictive_mode = true;

    /* set the values of restrict_xyz */
    restrict_x = x;
    restrict_y = y;
    restrict_z = z;
}
void glh::camera::camera_movement::disable_restrictive_mode () 
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
const glh::math::vec3& glh::camera::camera_movement::move ( const math::vec3& vec )
{
    
    /* move position
     * if non-restricted use xyz, otherwise use restricted values
    */
    if ( !restrictive_mode )
    {
        position += x * vec.at ( 0 );
        position += y * vec.at ( 1 );
        position += z * vec.at ( 2 );
    } else
    {
        position += restrict_x * vec.at ( 0 );
        position += restrict_y * vec.at ( 1 );
        position += restrict_z * vec.at ( 2 );
    } 
    

    /* set view as changed */
    view_change = true;

    /* return pos */
    return position;
}

/* move_global
 *
 * move camera relative to the global axis
 * 
 * vec: vector for movement (+x: x, +y: y, +z: z)
 * 
 * return: new position vector
 */
const glh::math::vec3& glh::camera::camera_movement::move_global ( const math::vec3& vec )
{
    /* set view as changed */
    view_change = true;
    
    /* return vec added to position */
    return ( position += vec );
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
const glh::math::vec3& glh::camera::camera_movement::pitch ( const double arg )
{
    /* if non-restrictive, rotate y and z around x axis */
    if ( !restrictive_mode )
    {
        y = math::rotate3d ( y, arg, x );
        z = math::rotate3d ( z, arg, x );
    } else
    /* otherwise, rotate y and z around the restrict_x axis */
    {
        /* if trying to pitch beyond vertical, reduce arg accordingly */
        double pitch_angle = math::angle ( restrict_y, z );
        if ( pitch_angle + arg > math::rad ( 180.0 ) )
        {
            y = math::rotate3d ( y, math::rad ( 180.0 ) - pitch_angle, restrict_x );
            z = math::rotate3d ( z, math::rad ( 180.0 ) - pitch_angle, restrict_x );
        } else
        if ( pitch_angle + arg < math::rad ( 0.0 ) )
        {
            y = math::rotate3d ( y, math::rad ( 0.0 ) - pitch_angle, restrict_x );
            z = math::rotate3d ( z, math::rad ( 0.0 ) - pitch_angle, restrict_x );
        } else
        {        
            y = math::rotate3d ( y, arg, restrict_x );
            z = math::rotate3d ( z, arg, restrict_x );
        }
    }

    /* set view as changed and return */
    view_change = true;
    return position;
}
const glh::math::vec3& glh::camera::camera_movement::yaw ( const double arg )
{
    /* if non-restrictive, rotate x and z around the y axis */
    if ( !restrictive_mode )
    {
        x = math::rotate3d ( x, arg, y );
        z = math::rotate3d ( z, arg, y );
    } else
    /* otherwise, rotate x, restict_x, y, z and restrict_z around the restrict_y axis */
    {
        x = math::rotate3d ( x, arg, restrict_y );
        restrict_x = math::rotate3d ( restrict_x, arg, restrict_y );
        y = math::rotate3d ( y, arg, restrict_y );
        z = math::rotate3d ( z, arg, restrict_y );
        restrict_z = math::rotate3d ( restrict_z, arg, restrict_y );
    }    

    /* set view as changed and return */
    view_change = true;
    return position;
}
const glh::math::vec3& glh::camera::camera_movement::roll ( const double arg )
{
    /* if non-restrictive, rotate x and y around the z axis */
    if ( !restrictive_mode )
    {
        x = math::rotate3d ( x, arg, z );
        y = math::rotate3d ( y, arg, z );
    }
    /* otherwise return position without change */
    else return position;

    /* set view as changed and return */
    view_change = true;
    return position;
}

/* get/set_direction
 *
 * get/set the direction of the camera
 */
void glh::camera::camera_movement::set_direction ( const math::vec3& direction, const math::vec3& world_y )
{
    /* set z */
    z = math::normalize ( -direction );
    restrict_z = z;

    /* find x from cross product */
    x = math::cross ( math::normalize ( world_y ), z );
    restrict_x = x;

    /* find y from cross product again */
    y = math::cross ( z, x );
    restrict_y = y;

    /* set view as changed */
    view_change = true;
}

/* create_view
 *
 * create the view matrix
 */
glh::math::mat4 glh::camera::camera_movement::create_view () const
{
    /* return view matrix using math::camera */
    return math::camera ( position, x, y, z );
}



/* CAMERA_PERSPECTIVE IMPLEMENTATION */

/* full constructor
 *
 * give parameters for perspective_fov
 */
glh::camera::camera_perspective::camera_perspective ( const double _fov, const double _aspect, const double _near, const double _far )
    : fov { _fov }
    , aspect { _aspect }
    , near { _near }
    , far { _far }
{}

/* create_proj
 *
 * create the projection matrix
 */
glh::math::mat4 glh::camera::camera_perspective::create_proj () const
{
    /* return projection matrix using math::projection_fov */
    return math::perspective_fov ( fov, aspect, near, far );
}



/* CAMERA_ORTHOGRAPHIC IMPLEMENTATION */

/* full constructor
 *
 * give parameters for orthographic matrix production
 */
glh::camera::camera_orthographic::camera_orthographic ( const math::vec3& _lbn, const math::vec3& _rtf )
    : lbn { _lbn }
    , rtf { _rtf }
{}

/* create_proj
 *
 * update the projection matrix
 */
glh::math::mat4 glh::camera::camera_orthographic::create_proj () const
{
    /* return projection matrix using math::orthographic */
    return math::orthographic ( lbn.at ( 0 ), rtf.at ( 0 ), lbn.at ( 1 ), rtf.at ( 1 ), lbn.at ( 2 ), rtf.at ( 2 ) );
}



/* MIRROR_CAMERA IMPLEMENTATION */

/* create_view
 *
 * create the view matrix
 */
glh::math::mat4 glh::camera::mirror_camera::create_view () const
{
    /* true means cam pos is same side as normal to mirror */
    const bool sign = !std::signbit ( math::dot ( normal, cam.get_position () - position ) );

    /* if cam pos is on the same side as the normal,
     * reflect cam pos in the plane found by the normal of the mirror */
    math::vec3 pos { cam.get_position () };
    if ( sign ) pos = math::reflect3d ( pos, normal, position );

    /* return view matrix created by reflecting position in the mirror and looking towards it */
    return math::look_along ( pos, normal, ytan );
}

/* create_proj
 *
 * create the projection matrix
 */
glh::math::mat4 glh::camera::mirror_camera::create_proj () const
{
    /* get the view matrix */
    math::mat4 view_matrix { get_view () };

    /* apply view matrix to the mirror position */
    math::vec3 mirror_pos { view_matrix * glh::math::vec4 { position, 1.0 } };

    /* return projection matrix
     * notice the sign of the half_widths vs the half_heights
     * we reflect the texture in the x-axis
     */
    return math::perspective ( mirror_pos.at ( 0 ) + half_width, mirror_pos.at ( 0 ) - half_width, mirror_pos.at ( 1 ) - half_height, mirror_pos.at ( 1 ) + half_height, -mirror_pos.at ( 2 ), cam.get_far () );

}