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

/* include glhelper_exception.hpp */
#include <glhelper/glhelper_exception.hpp>

/* include glhelper_manager.hpp */
#include <glhelper/glhelper_manager.hpp>

/* indlude stb_image.h without implementation */
#include <stb/stb_image.h>



/* NAMESPACE FORWARD DECLARATIONS */

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
     * _target: the target for the texture (e.g. GL_TEXTURE_2D)
     * _internal_format: the internal format of the data (e.g. specific bit arrangements)
     * _format: the format of the data (e.g. what the data will be used for)
     * _width/height: width/height of the texture (defaults to 0)
     */
    texture_base ( const GLenum _target, const GLenum _internal_format, const GLenum _format, const int _width = 0, const int _height = 0 )
        : object { object_manager::generate_texture () }
        , target { _target }
        , internal_format ( _internal_format )
        , format ( _format )
        , width { _width }
        , height { _height }
    {}

    /* deleted copy constructor */
    texture_base ( const texture_base& other ) = delete;

    /* default move constructor */
    texture_base ( texture_base&& other ) = default;

    /* deleted copy constructor */
    texture_base& operator= ( const texture_base& other ) = delete;

    /* virtual destructor */
    virtual ~texture_base () { destroy (); }



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

    /* generate_mipmap
     *
     * generate texture mipmap
     */
    void generate_mipmap ();



    /* destroy
     *
     * destroys the texture, setting its id to 0
     */
    void destroy () { object_manager::destroy_texture ( id ); id = 0; }

    /* bind
     *
     * bind the texture to a texture unit
     * 
     * texture_unit: the texture unit to bind to, defaulting to 0
     */
    void bind ( const unsigned texture_unit = 0 ) const;

    /* unbind
     *
     * unbind the texture from a texture unit
     * 
     * texture_unit: the texture unit to unbind from, defaulting to 0
     */
    void unbind ( const unsigned texture_unit = 0 ) const;

    /* is_bound
     *
     * texture_unit: the texture unit to check if it is bound to, defaulting to 0
     * 
     * return: boolean for if the texture is bound
     */
    bool is_bound ( const unsigned texture_unit = 0 ) const;



    /* get_target
     *
     * get the target in which the texture is bound
     */
    const GLenum& get_target () const { return target; }



protected:

    /* target of the texture */
    const GLenum target;

    /* (internal_)format
     *
     * the (internal) format of the texture (e.g. GL_RGB)
     */
    const GLenum internal_format;
    const GLenum format;

    /* width/height of each face of the cube map */
    int width;
    int height;

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
     */
    texture2d ( const std::string& _path );

    /* empty texture constructor
     *
     * create an texture of a given size with supplied data
     * 
     * _width/_height: the width and height of the texture
     * __internal_format: the internal format of the data (e.g. specific bit arrangements)
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



    /* get_path
     *
     * get the path the texture was originally imported from
     */
    const std::string& get_path () const { return path; }

    /* get_width/height
     *
     * get the width and height of the texture
     */
    const int& get_width () const { return width; }
    const int& get_height () const { return height; }

    /* get_channels
     *
     * get the number of channels the texture orginally had
     */
    const int& get_channels () const { return channels; }



private:

    /* path
     *
     * path to the shader
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
     */
    explicit cubemap ( const std::array<std::string, 6>& paths );

    /* 1-image constructor
     *
     * construct the cubemap width one image for all six sides
     *
     * path: path to the image 
     */
    explicit cubemap ( const std::string& path );

    /* deleted zero-parameter constructor */
    cubemap () = delete;

    /* deleted copy constructor */
    cubemap ( const cubemap& other ) = delete;

    /* default move constructor */
    cubemap ( cubemap&& other ) = default;

    /* deleted copy assignment operator */
    cubemap& operator= ( const cubemap& other ) = delete;



    /* get_path
     *
     * get the path the face at layer i was originally imported from
     */
    const std::string& get_path ( const unsigned i ) const { return face_textures.at ( i ).path; }

    /* get_width/height
     *
     * get the width and height of the texture
     */
    const int& get_width () const { return width; }
    const int& get_height () const { return height; }

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