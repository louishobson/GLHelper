/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * Uses image loader from https://github.com/nothings/stb/blob/master/stb_image.h
 * Included under Public Domain licence
 * For details, see: https://github.com/nothings/stb/blob/master/LICENSE
 * 
 * include/glhelper/glhelper_testure.hpp
 * 
 * defines constructs to handle textures
 * notable constructs include:
 * 
 * 
 * 
 * CLASS GLH::CORE::TEXTURE_BASE
 * 
 * base class for all textures
 * implements common fucntionality such as wrapping and filtering settings
 * 
 * 
 * 
 * CLASS GLH::CORE::TEXTURE2D
 * 
 * dreivation of texture_base to represent a 2d texture
 * the texture can be loaded from a file, or be initialised blank
 * blank textures are useful for using as color buffers for framebuffer objects
 * 
 * 
 * 
 * CLASS GLH::CORE::CUBEMAP
 * 
 * derivation of texture_base to represent a cubemap
 * 
 * 
 * 
 * CLASS GLH::EXCEPTION::TEXTURE_EXCEPTION
 * 
 * thrown when an error occurs in one of the texture methods (e.g. a texture file cannot be found)
 * 
 */



/* HEADER GUARD */
#ifndef GLHELPER_TEXTURE_HPP_INCLUDED
#define GLHELPER_TEXTURE_HPP_INCLUDED



/* INCLUDES */

/* include core headers */
#include <iostream>
#include <string>

/* include glhelper_core.hpp */
#include <glhelper/glhelper_core.hpp>

/* include math */
#include <glhelper/glhelper_math.hpp>

/* indlude stb_image.h without implementation */
#include <stb/stb_image.h>



/* NAMESPACE DECLARATIONS */

namespace glh
{
    namespace core
    {
        /* class texture_base : object
         *
         * base class for all textures
         */
        class texture_base;

        /* class texture2d : texture_base
         *
         * represents a 2D texture
         * automatically loads the texture given a path, and creates a mipmap
         */
        class texture2d;

        /* class cubemap : texture_base
         *
         * represents a cubemap
         */
        class cubemap;

        /* class texture2d_multisample : texture_base
         *
         * multisample2d texture
         * no image will be loaded into this texture, as it will be used as a buffer rather than to store images
         */
        class texture2d_multisample;
    }

    namespace exception
    {
        /* class texture_exception : exception
         *
         * exception relating to textures
         */
        class texture_exception;
    }
}



/* TEXTURE_BASE DEFINITION */

/* class texture_base : object
 *
 * base class for all textures
 */
class glh::core::texture_base : public object
{
public:

    /* full constructor
     *
     * only supply the texture target
     * 
     * _width/height: width/height of the texture
     * _minor_type: the minor type of the texture
     * _internal_format: the internal format of the data (e.g. specific bit arrangements)
     * _format: the format of the data (e.g. what the data will be used for)
     * _type: the type of the data in the texture
     */
    texture_base ( const minor_object_type _minor_type, const int _width, const int _height, const GLenum _internal_format, const GLenum _format, const GLenum _type );

    /* deleted zero-parameter constructor */
    texture_base () = delete;
    
    /* deleted copy constructor */
    texture_base ( const texture_base& other ) = delete;

    /* default move constructor */
    texture_base ( texture_base&& other ) = default;

    /* deleted copy constructor */
    texture_base& operator= ( const texture_base& other ) = delete;

    /* default virtual destructor */
    virtual ~texture_base () = default;



    /* set_mag/min_filter
     *
     * set the texture filtering parameters of magnified/minified texture
     */
    void set_mag_filter ( const GLenum opt );
    void set_min_filter ( const GLenum opt );

    /* set_(s/t/r)_wrap
     *
     * set the wrapping options for each coordinate axis, or all at once
     */
    void set_s_wrap ( const GLenum opt );
    void set_t_wrap ( const GLenum opt );
    void set_r_wrap ( const GLenum opt );
    void set_wrap ( const GLenum opt );

    /* set_border_color
     *
     * set the color of the boarder, such that the texture can be clamped to the edge with a specific color
     */
    void set_border_color ( const math::fvec3& color );

    /* generate_mipmap
     *
     * generate texture mipmap
     */
    void generate_mipmap ();



    /* bind/unbind
     *
     * bind the texture to a texture unit
     * 
     * unit: the texture unit to bind to/unbind from
     */
    using object::bind;
    bool bind ( const unsigned texture_unit ) const { return bind ( static_cast<object_bind_target> ( static_cast<unsigned> ( bind_target ) + texture_unit ) ); }

    /* unbind
     *
     * unbind the texture from a texture unit
     * 
     * uniy: the texture unit to unbind from
     */
    using object::unbind;
    bool unbind ( const unsigned texture_unit ) const { return unbind ( static_cast<object_bind_target> ( static_cast<unsigned> ( bind_target ) + texture_unit ) ); }

    /* unbind_all
     *
     * unbind from all targets
     * this includes all texture units
     */
    bool unbind_all () const;

    /* is_bound
     *
     * check if is bound to a texture unit
     * 
     * unit: the texture unit to check if it is bound to
     * 
     * return: boolean for if the texture is bound to the unit supplied
     */
    using object::is_bound;
    bool is_bound ( const unsigned texture_unit ) const { return is_bound ( static_cast<object_bind_target> ( static_cast<unsigned> ( bind_target ) + texture_unit ) ); }



    /* bind_loop
     *
     * this will keep looping around texture units, disincluding 0, and making subsequent binds to the next unit
     * this avoids binding a texture to a unit already in use
     * 
     * returns the unit just bound to
     */
    unsigned bind_loop () const;

    /* bind_loop_next
     *
     * return the next index of the bind loop without binding
     */
    unsigned bind_loop_next () const { return bind_loop_index; }

    /* bind_loop_previous
     *
     * return the previous binding of the loop
     */
    unsigned bind_loop_previous () const;



    /* get_width/height
     *
     * get the width and height of the texture
     */
    const int& get_width () const { return width; }
    const int& get_height () const { return height; }

protected:

    /* width/height of each face of the cube map */
    int width;
    int height;

    /* (internal_)format
     *
     * the (internal) format of the texture (e.g. GL_RGB)
     */
    const GLenum internal_format;
    const GLenum format;

    /* type
     *
     * the type of the data in the texture
     */
    const GLenum type;



    /* bind_loop_index
     *
     * the next unit to bind the texture to in the bind loop
     */
    static unsigned bind_loop_index;

};



/* TEXTURE2D DEFINITION */

/* class texture2d : texture_base
 *
 * represents a 2D texture
 * automatically loads the texture given a path, and creates a mipmap
 */
class glh::core::texture2d : public texture_base
{
public:

    /* image constructor
     *
     * load a 2D texture from an image file
     * default settings are applied
     * 
     * _path: path to the image for the texture
     * is_srgb: true if the texture should be corrected to linear color space (defaults to false)
     */
    explicit texture2d ( const std::string& _path, const bool is_srgb = false );

    /* empty texture constructor
     *
     * create an texture of a given size with supplied data
     * 
     * _width/_height: the width and height of the texture
     * _internal_format: the internal format of the data (e.g. specific bit arrangements)
     * _format: the format of the data (e.g. what the data will be used for)
     * _type: the type of the pixel data (specific type macro with bit arrangements)
     * data: the data to put in the texture (defaults to NULL)
     */
    texture2d ( const unsigned _width, const unsigned _height, const GLenum _internal_format, const GLenum _format, const GLenum _type, const GLvoid * data = NULL );

    /* deleted zero-parameter constructor
     *
     * a texture must be loaded to an image
     */
    texture2d () = delete;

    /* deleted copy constructor */
    texture2d ( const texture2d& other ) = delete;

    /* default move copy constructor */
    texture2d ( texture2d&& other ) = default;

    /* deleted assignment operator */
    texture2d& operator= ( const texture2d& other ) = delete;

    /* default destructor */
    ~texture2d () = default;



    /* get_bound_object_pointer
     *
     * produce a pointer to the texture2d currently bound to a unit
     */
    using object::get_bound_object_pointer;
    static object_pointer<texture2d> get_bound_object_pointer ( const unsigned unit = 0 )
    { return get_bound_object_pointer<texture2d> ( static_cast<object_bind_target> ( static_cast<unsigned> ( object_bind_target::GLH_TEXTURE2D_0_TARGET ) + unit ) ); }



    /* get_path
     *
     * get the path the texture was originally imported from
     */
    const std::string& get_path () const { return path; }

    /* get_channels
     *
     * get the number of channels the texture orginally had
     */
    const int& get_channels () const { return channels; }

private:

    /* path
     *
     * path to the texture
     */
    const std::string path;

    /* channels
     *
     * stores number of channels of the texture
     */
    int channels;

};



/* CUBEMAP DEFINITION */

/* class cubemap : texture_base
 *
 * represents a cubemap
 */
class glh::core::cubemap : public texture_base
{
public:

    /* 6-image constructor
     *
     * construct the cubemap from six separate images for each layer
     * the order of the paths is the same as the order of cubemap layers
     * 
     * paths: array of 6 paths to the images for the cubemap faces
     * is_srgb: true if the texture should be corrected to linear color space (defaults to false)
     */
    explicit cubemap ( const std::array<std::string, 6>& paths, const bool is_srgb = false );

    /* 1-image constructor
     *
     * construct the cubemap width one image for all six sides
     *
     * path: path to the image 
     * is_srgb: true if the texture should be corrected to linear color space (defaults to false)
     */
    explicit cubemap ( const std::string& path, const bool is_srgb = false );

    /* empty cubemap constructor
     *
     * all of the sizes are initialised to the same parameters
     * 
     * _width/_height: the width and height of the texture
     * _internal_format: the internal format of the data (e.g. specific bit arrangements)
     * _format: the format of the data (e.g. what the data will be used for)
     * _type: the type of the pixel data (specific type macro with bit arrangements)
     * data: the data to put in the texture (defaults to NULL)
     */
    cubemap ( const unsigned _width, const unsigned _height, const GLenum _internal_format, const GLenum _format, const GLenum _type, const GLvoid * data = NULL );

    /* deleted zero-parameter constructor */
    cubemap () = delete;

    /* deleted copy constructor */
    cubemap ( const cubemap& other ) = delete;

    /* default move constructor */
    cubemap ( cubemap&& other ) = default;

    /* deleted copy assignment operator */
    cubemap& operator= ( const cubemap& other ) = delete;



    /* get_bound_object_pointer
     *
     * produce a pointer to the cubemap currently bound to a unit
     */
    using object::get_bound_object_pointer;
    static object_pointer<cubemap> get_bound_object_pointer ( const unsigned unit = 0 )
    { return get_bound_object_pointer<cubemap> ( static_cast<object_bind_target> ( static_cast<unsigned> ( object_bind_target::GLH_CUBEMAP_0_TARGET ) + unit ) ); }




    /* get_path
     *
     * get the path the face at layer i was originally imported from
     */
    const std::string& get_path ( const unsigned i ) const { return face_textures.at ( i ).path; }

    /* get_channels
     *
     * get the number of channels the face at layer i orginally had
     */
    const int& get_channels ( const unsigned i ) const { return face_textures.at ( i ).channels; }



private:

    /* struct to represent a texture face of the cube */
    struct face_texture
    {
        std::string path;
        int channels;
    };

    /* array of face_texture structs to store info on each face texture */
    std::array<face_texture, 6> face_textures;

};



/* TEXTURE2D_MULTISAMPLE DEFINITION */

/* class texture2d_multisample : texture_base
 *
 * multisample2d texture
 * no image will be loaded into this texture, as it will be used as a buffer rather than to store images
 */
class glh::core::texture2d_multisample : public texture_base
{
public:

    /* empty texture constructor
     * 
     * constructs a multisample texture with a given size and number of samples 
     * 
     * _width/_height: the width and height of the texture
     * _internal_format: the internal format of the texture (e.g. specific bit arrangements)
     * _samples: the number of sampes the texture should contain
     * _fixed_sample_locations: defaults to true - I don't know what this setting does tbh
     */
    texture2d_multisample ( const int _width, const int _height, const GLenum _internal_format, const unsigned _samples, const bool _fixed_sample_locations = GL_TRUE );
    
    /* deleted zero-parameter constructor */
    texture2d_multisample () = delete;

    /* deleted copy comstructor */
    texture2d_multisample ( const texture2d_multisample& other ) = delete;

    /* default move constructor */
    texture2d_multisample ( texture2d_multisample&& other ) = default;

    /* default destructor */
    ~texture2d_multisample () = default;



    /* get_bound_object_pointer
     *
     * produce a pointer to the texture2d_multisample currently bound to a unit
     */
    using object::get_bound_object_pointer;
    static object_pointer<texture2d_multisample> get_bound_object_pointer ( const unsigned unit = 0 )
    { return get_bound_object_pointer<texture2d_multisample> ( static_cast<object_bind_target> ( static_cast<unsigned> ( object_bind_target::GLH_TEXTURE2D_MULTISAMPLE_0_TARGET ) + unit ) ); }




    /* get_samples
     * 
     * get the number of samples the texture has
     */
    const unsigned& get_samples () const { return samples; }

private:

    /* number of samples of the texture */
    const unsigned samples;

    /* whether fixed sample locations are being used */
    const bool fixed_sample_locations;

};



/* TEXTURE_EXCEPTION DEFINITION */

/* class texture_exception : exception
 *
 * for exceptions related to textures
 */
class glh::exception::texture_exception : public exception
{
public:

    /* full constructor
     *
     * __what: description of the exception
     */
    explicit texture_exception ( const std::string& __what )
        : exception { __what }
    {}

    /* default zero-parameter constructor
     *
     * construct texture_exception with no descrption
     */
    texture_exception () = default;

    /* default everything else and inherits what () function */

};



/* #ifndef GLHELPER_TEXTURE_HPP_INCLUDED */
#endif