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
 * CLASS GLH::CORE::TEXTURE2D
 * 
 * class for a 2d texture object
 * the texture can be loaded from a file, or be initialised blank
 * blank textures are useful for using as color buffers for framebuffer objects
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
        /* class texture2c : object
         *
         * represents a 2D texture
         * automatically loads the texture given a path, and creates a mipmap
         */
        class texture2d;
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



/* TEXTURE2D DEFINITION */

class glh::core::texture2d : public object
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
     * _type: the type of the data (specific type macro with bit arrangements)
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

    /* virtual destructor */
    virtual ~texture2d () { destroy (); }



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
     * texture_unit: the texture unit to bind to, or the last one
     */
    void bind ( const unsigned texture_unit = 0 ) const { object_manager::bind_texture ( id, texture_unit ); }

    /* is_bound
     *
     * texture_unit: the texture unit to check if it is bound to
     * 
     * return boolean for if the texture is bound
     */
    bool is_bound ( const unsigned texture_unit = 0 ) const { return object_manager::is_texture_bound ( id, texture_unit ); }



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

    /* (internal_)format
     *
     * the (internal) format of the texture (e.g. GL_RGB)
     */
    GLenum internal_format;
    GLenum format;

    /* type
     *
     * the type of the data stored in the texture
     */
    GLenum type;

    /* widith, height, channels
     *
     * stores the width, height, and number of channels of the texture
     */
    int width;
    int height;
    int channels;

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