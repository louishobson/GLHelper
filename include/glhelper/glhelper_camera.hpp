/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * include/glhelper/glhelper_camera.hpp
 * 
 * constructs for camera movement and projection abstraction
 * notable constructs include:
 * 
 * 
 * 
 * CLASS GLH::CAMERA::CAMERA_BASE
 * 
 * abstract base class for all further camera classes
 * functionality such as uniform and matrix caching and applying uniforms is common to all cameras
 * the pure virtual methods are the update_view/proj as the creation of these matrices will differ for each camera
 * this class is applied to the following uniform structure
 * 
 * 
 * 
 * GLSL STRUCT CAMERA_STRUCT
 * 
 * struct camera_struct
 * {
 *     mat4 view;
 *     mat4 proj;
 *     mat4 view_proj;
 *
 *     vec3 viewpos;
 * }
 * 
 * this struct contains the core parameters for a camera, described below
 * 
 * view/proj: the view and projection matrices
 * view_proj: equal to proj * view
 * viewpos: the viewer position
 * 
 * 
 * 
 * CLASS GLH::CAMERA::CAMERA_MOVEMENT
 * 
 * this derivation of camera_base defines how to create a view matrix based on a view position
 * methods for control of movement and direction of viewing are supplied for convenience
 * NOTE: restrictive mode is disabled by default, however often it is the desired camera mode
 * 
 * 
 * 
 * CLASS GLH::CAMERA::CAMERA_PERSPECTIVE
 * 
 * this derivation of camera_base defines how to create a perspective prpjection matrix
 * 
 * 
 * 
 * CLASS GLH::CAMERA::CAMERA_ORTHOGRAPHIC
 * 
 * this derivation of camera_base defines how to create an orthographic projection matrix
 * 
 * 
 * 
 * CLASS GLH::CAMERA::CAMERA_PERSPECTIVE_MOVEMENT
 * 
 * this is a final derivation of camera_base, through the classes camera_movement and camera_perspective
 * this camera uses the functionality from both of these direct base classes
 * 
 * 
 * 
 * CLASS GLH::CAMERA::CAMERA_ORTHOGRAPHIC_MOVEMENT
 * 
 * this is a final derivation of camera_base, through the classes camera_movement and camera_orthographic
 * this camera uses the functionality from both of these base classes
 * 
 * 
 * 
 * CLASS GLH::CAMERA::MIRROR_CAMERA
 * 
 * a niche camera specifically for mirrors
 * thie camera takes a reference to a glh::camera::camera object and produces matrices based on the position of the
 * camera and the position of a mirror
 * when applied, the image produced will be what should be rendered onto the mirror for it to function correctly
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

/* include glhelper_vector.hpp */
#include <glhelper/glhelper_vector.hpp>

/* include glhelper_matrix.hpp */
#include <glhelper/glhelper_matrix.hpp>

/* include glhelper_transform.hpp */
#include <glhelper/glhelper_transform.hpp>

/* include glhelper_shader.hpp */
#include <glhelper/glhelper_shader.hpp>



/* NAMESPACE DECLARATIONS */

namespace glh
{
    namespace camera
    {
        /* class camera_base
         *
         * base class for all cameras
         */
        class camera_base;



        /* class camera_movement : camera_base
         *
         * derivation of camera_base to add movement functionality
         * still intended to be a base class
         */
        class camera_movement;

        /* class camera_perspective : camera_base
         *
         * camera using a perspective projection matrix
         */
        class camera_perspective;

        /* class camera_orthographic : camera_base
         *
         * camera using a orthographic projection matrix
         */
        class camera_orthographic;

        /* class camera_noproject : camera_base
         *
         * camera using no projection
         */
        class camera_noproject;



        /* class camera_perspective_movement : camera_movement, camera_perspective
         *
         * camera with movement functionality and a perspective projection matrix 
         */
        class camera_perspective_movement;

        /* class camera_orthographic_movement : camera_movement, camera_orthographic
         * 
         * camera with movement functionality and an orthographic projection matrix
         */
        class camera_orthographic_movement;

        /* class camera_noproject_movement : camera_movement, camera_noproject
         *
         * camera with movement functionality and no projection matrix
         */
        class camera_noproject_movement;



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

    /* zero-parameter constructor */
    camera_base ()
        : view_change { true }
        , proj_change { true }
    {}

    /* copy constructor
     *
     * non-default as cannot copy cached uniforms
     */
    camera_base ( const camera_base& other )
        : view_change { true }
        , proj_change { true }
    {}

    /* default move constructor */
    camera_base ( camera_base&& other ) = default;

    /* copy assignment operator
     *
     * non-default for same reason as copy constructor
     */
    camera_base& operator= ( const camera_base& other ) 
    { view_change = true; proj_change = true; return * this; }

    /* default move assignment operator */
    camera_base& operator= ( camera_base&& other ) = default;

    /* default destructor */
    virtual ~camera_base () = default;



    /* apply
     *
     * apply the camera to view and projection matrices
     * 
     * camera_uni: the uniform to cache and apply to
     */
    void apply ( core::struct_uniform& camera_uni );
    void apply () const;

    /* cache_uniforms
     *
     * camera_uni: the uniform to cache
     */
    void cache_uniforms ( core::struct_uniform& camera_uni );



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

    /* get_view_proj
     *
     * recieve the view_proj matrix
     */
    const math::mat4& get_view_proj () const;



protected:

    /* change in the view matrix */
    mutable bool view_change;

    /* changes to the matrices in the camera */
    mutable bool proj_change;



private:

    /* the current core matrices */
    mutable math::mat4 view;
    mutable math::mat4 proj;
    mutable math::mat4 view_proj;

    /* viewing position */
    mutable math::vec3 viewpos;



    /* struct for cached uniforms */
    struct cached_uniforms_struct
    {
        core::struct_uniform camera_uni;
        core::uniform& view_uni;
        core::uniform& proj_uni;
        core::uniform& view_proj_uni;
        core::uniform& viewpos_uni;
    };

    /* cached uniforms */
    std::unique_ptr<cached_uniforms_struct> cached_uniforms;



    /* update_parameters
     *
     * update the camera parameters, if any need updating
     */
    void update_parameters () const;



    /* create_view
     *
     * create the view matrix
     */
    virtual math::mat4 create_view () const = 0;

    /* create_proj
     *
     * create the projection matrix
     */
    virtual math::mat4 create_proj () const = 0;

};



/* CAMERA_MOVEMENT DEFINITION */

/* class camera_movement : camera_base
 *
 * derivation of camera_base to add movement functionality
 * still intended to be a base class
 */
class glh::camera::camera_movement : public virtual camera_base
{
public:

    /* full constructor
     *
     * give parameters for look towards to create view matrix
     */
    camera_movement ( const math::vec3& _position, const math::vec3& _direction, const math::vec3& _world_y );

    /* zero-parameter constuctor
     *
     * create view matrix from default parameters
     */
    camera_movement ()
        : camera_movement { math::vec3 { 0.0, 0.0, 0.0 }, math::vec3 { 0.0, 0.0, -1.0 }, math::vec3 { 0.0, 1.0, 0.0 } }
    {}

    /* default copy constructor */
    camera_movement ( const camera_movement& other ) = default;

    /* default copy assignment operator */
    camera_movement& operator= ( const camera_movement& other ) = default;

    /* default virtual destructor */
    virtual ~camera_movement () = default;



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



    /* get/set_position
     *
     * get/set the current viewing position
     */
    const math::vec3& get_position () const { return position; }
    void set_position ( const math::vec3& _position ) { position = _position; view_change = true; }

    /* get/set_direction
     *
     * get/set the direction of the camera
     */
    math::vec3 get_direction () const { return -z; }
    void set_direction ( const math::vec3& direction, const math::vec3& world_y );

    /* get_x/y/z
     *
     * get the current coordinate axis of the camera
     */
    const math::vec3& get_x () const { return x; }
    const math::vec3& get_y () const { return y; }
    const math::vec3& get_z () const { return z; }




protected:

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



    /* create_view
     *
     * create the view matrix
     */
    math::mat4 create_view () const final;

};



/* CAMERA_PERSPECTIVE DEFINITION */

/* class camera_perspective : camera_base
 *
 * camera using a perspective projection matrix
 */
class glh::camera::camera_perspective : public virtual camera_base
{
public:

    /* full constructor
     *
     * give parameters for perspective_fov
     */
    camera_perspective ( const double _fov, const double _aspect, const double _near, const double _far );
        
    /* zero parameter constructor
     *
     * produce a default projection matrix
     */
    camera_perspective () : camera_perspective { math::rad ( 60.0 ), 16.0 / 9.0, 0.1, 200.0 } {}

    /* default copy constructor */
    camera_perspective ( const camera_perspective& other ) = default;

    /* default assignment operator */
    camera_perspective& operator= ( const camera_perspective& other ) = default;

    /* default virtual destructor */
    virtual ~camera_perspective () = default;



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




protected:

    /* perspective projection matrix parameters */
    double fov;
    double aspect;
    double near;
    double far;

    

    /* create_proj
     *
     * create the projection matrix
     */
    math::mat4 create_proj () const final;

};




/* CAMERA_ORTHOGRAPHIC DEFINITION */

/* class camera_orthographic : camera_base
 *
 * camera using a orthographic projection matrix
 */
class glh::camera::camera_orthographic : public virtual camera_base
{
public:

    /* full constructor
     *
     * give parameters for orthographic matrix production
     */
    camera_orthographic ( const math::vec3& _lbn, const math::vec3& _rtf );
        
    /* zero parameter constructor
     *
     * produce a default orthographic matrix
     */
    camera_orthographic () : camera_orthographic { math::vec3 { -50.0, -50.0, 0.0 }, math::vec3 { 50, 50, 200 } } {}

    /* default copy constructor */
    camera_orthographic ( const camera_orthographic& other ) = default;

    /* default assignment operator */
    camera_orthographic& operator= ( const camera_orthographic& other ) = default;

    /* default virtual destructor */
    virtual ~camera_orthographic () = default;



    /* get/set_lbn
     *
     * get/set the lbn point of the cuboid
     */
    const math::vec3& get_lbn () const { return lbn; }
    void set_lbn ( const math::vec3& _lbn ) { lbn = _lbn; proj_change = true; }

    /* get/set_rtf
     *
     * get/set the rtf point of the cuboid
     */
    const math::vec3& get_rtf () const { return rtf; }
    void set_rtf ( const math::vec3& _rtf ) { rtf = _rtf; proj_change = true; }




protected:

    /* orthographic projection matrix parameters */
    math::vec3 lbn;
    math::vec3 rtf;



    /* create_proj
     *
     * create the projection matrix
     */
    math::mat4 create_proj () const final;

};



/* CAMERA_NOPROJECT DEFINITION */

/* class camera_noproject : camera_base
 *
 * camera using no projection
 */
class glh::camera::camera_noproject : public virtual camera_base
{
public:

    /* default everything */
    camera_noproject () = default;
    camera_noproject ( const camera_noproject& other ) = default;
    camera_noproject& operator= ( const camera_noproject& other ) = default;
    ~camera_noproject () = default;

private:

    /* create_proj
     *
     * create the projection matrix
     */
    math::mat4 create_proj () const final { return identity; }

    /* identity matrix */
    static const math::mat4 identity;

};




/* CAMERA_PERSPECTIVE_MOVEMENT DEFINITION */

/* class camera_perspective_movement : camera_movement, camera_perspective
 *
 * camera with movement functionality and a perspective projection matrix 
 */
class glh::camera::camera_perspective_movement : public camera_movement, public camera_perspective
{
public:

    /* full constructor
     *
     * construct based on parameters of movement and perspective cameras
     */
    camera_perspective_movement ( const math::vec3& _position, const math::vec3& _direction, const math::vec3& _world_y, const double _fov, const double _aspect, const double _near, const double _far )
        : camera_movement { _position, _direction, _world_y }
        , camera_perspective { _fov, _aspect, _near, _far }
    {}

    /* default zero-parameter constructor */
    camera_perspective_movement () = default;

    /* default copy constructor */
    camera_perspective_movement ( const camera_perspective_movement& other ) = default;

    /* default copy assignment operator */
    camera_perspective_movement& operator= ( const camera_perspective_movement& other ) = default;

    /* default destructor */
    ~camera_perspective_movement () = default;

};



/* CAMERA_ORTHOGRAPHIC_MOVEMENT DEFINITION */

/* class camera_orthographic_movement : camera_movement, camera_orthographic
 * 
 * camera with movement functionality and an orthographic projection matrix
 */
class glh::camera::camera_orthographic_movement : public camera_movement, public camera_orthographic
{
public:

    /* full constructor
     *
     * construct based on parameters of movement and perspective cameras
     */
    camera_orthographic_movement ( const math::vec3& _position, const math::vec3& _direction, const math::vec3& _world_y, const math::vec3& _lbn, const math::vec3& _rtf )
        : camera_movement { _position, _direction, _world_y }
        , camera_orthographic { _lbn, _rtf, }
    {}

    /* default zero-parameter constructor */
    camera_orthographic_movement () = default;

    /* default copy constructor */
    camera_orthographic_movement ( const camera_orthographic_movement& other ) = default;

    /* default copy assignment operator */
    camera_orthographic_movement& operator= ( const camera_orthographic_movement& other ) = default;

    /* default destructor */
    ~camera_orthographic_movement () = default;

};



/* CAMERA_NOPROJECT_MOVEMENT */

/* class camera_noproject_movement : camera_movement, camera_noproject
 *
 * camera with movement functionality and no projection matrix
 */
class glh::camera::camera_noproject_movement : public camera_movement, public camera_noproject
{
public:

    /* full constructor
     *
     * construct based on parameters of movement and perspective cameras
     */
    camera_noproject_movement ( const math::vec3& _position, const math::vec3& _direction, const math::vec3& _world_y )
        : camera_movement { _position, _direction, _world_y }
    {}

    /* default zero-parameter constructor */
    camera_noproject_movement () = default;

    /* default copy constructor */
    camera_noproject_movement ( const camera_noproject_movement& other ) = default;

    /* default copy assignment operator */
    camera_noproject_movement& operator= ( const camera_noproject_movement& other ) = default;

    /* default destructor */
    ~camera_noproject_movement () = default;

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
     * takes another camera and mirror positioning to form a second camera from the mirror's perspective
     * the normal should be pointing out of the side the texture will be rendered on
     * given this, the mirror will act as normal when looking from the side of the normal, and be transparent on the other
     * take care with winding orders, as when looking at either side of the mirror, the winding order is reversed for its contents
     *
     * _cam: the viewer camera
     * _position: the position of the centre of the mirror
     * _normal: the normal unit vector to the mirror
     * _ytan: a tangent unit vector to the vertical axis of the mirror
     * _width: the width of the mirror
     * _height: the height of the mirror
     */
    mirror_camera ( const camera_perspective_movement& _cam, const math::vec3& _position, const math::vec3& _normal, const math::vec3& _ytan, const double _width, const double _height )
        : cam { _cam }, position { _position }, normal { math::normalize ( _normal ) }, ytan { math::normalize ( _ytan ) }, half_width { _width / 2.0 }, half_height { _height / 2.0 }
    {}

    /* deleted zero-parameter constructor */
    mirror_camera () = delete;

    /* default copy constructor */
    mirror_camera ( const mirror_camera& other ) = default;

    /* deleted copy assignment operator
     *
     * it is too unclear what should be copied from one to another
     */
    mirror_camera& operator= ( const mirror_camera& other ) = delete;

    /* default destructor */
    ~mirror_camera () = default;



    /* get/set_position
     *
     * get/set the position of the mirror
     */
    const math::vec3& get_position () const { return position; }
    void set_position ( const math::vec3& _position ) { position = _position; }
    
    /* get/set_normal
     *
     * get/set the normal vector of the mirror
     */
    const math::vec3& get_normal () const { return normal; }
    void set_normal ( const math::vec3& _normal ) { normal = math::normalize ( _normal ); }

    /* get/set_ytan
     *
     * get/set the y tangent of the mirror
     */
    const math::vec3& get_ytan () const { return ytan; }
    void set_ytan ( const math::vec3& _ytan ) { ytan = math::normalize ( _ytan ); }

    /* get/set_width/height
     *
     * get/set the width/height of the mirror
     */
    double get_width () const { return half_width * 2; }
    void set_width ( const double _width ) { half_width = _width / 2.0; }
    double get_height () const { return half_height * 2; }
    void set_height ( const double _height ) { half_height = _height / 2.0; }



protected:

    /* the camera the user is looking through */
    const camera_perspective_movement& cam;

    /* parameters form the mirror */
    math::vec3 position;
    math::vec3 normal;
    math::vec3 ytan;
    double half_width;
    double half_height;



    /* create_view
     *
     * create the view matrix
     */
    math::mat4 create_view () const final;

    /* create_proj
     *
     * create the projection matrix
     */
    math::mat4 create_proj () const;

};



/* #ifndef GLHELPER_CAMERA_HPP_INCLUDED */
#endif

