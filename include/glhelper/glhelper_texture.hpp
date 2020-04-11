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
 * handles textures for OpenGL
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

/* indlude stb_image.h without implementation */
#include <stb/stb_image.h>



/* NAMESPACE FORWARD DECLARATIONS */

namespace glh
{
    /* class texture2c : object
     *
     * represents a 2D texture
     * automatically loads the texture given a path, and creates a mipmap
     */
    class texture2d;

    /* class texture_exception : exception
     *
     * exception relating to textures
     */
    class texture_exception;
}



/* TEXTURE2D DEFINITION */

class glh::texture2d : public object
{
public:

    /* full constructor
     *
     * load a 2D texture from an image file
     * default settings are applied
     * 
     * _path: path to the image for the texture
     * _texture_unit: the texture unit to bind to (integer 0-31)
     */
    texture2d ( const std::string& _path, const GLenum _texture_unit = 0 );

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



    /* destroy
     *
     * destroys the texture, setting its id to 0
     */
    void destroy () override;

    /* bind
     *
     * bind the texture to a texture unit
     * 
     * _texture_unit: the texture unit to bind to, or the last one
     */
    GLenum bind () const;
    GLenum bind ( const unsigned _texture_unit );

    /* is_bound
     *
     * return boolean for if the texture is bound
     */
    bool is_bound () const;



    /* get_path
     *
     * get the path the texture was originally imported from
     */
    const std::string& get_path () const { return path; }

    /* get/set_texture_unit
     *
     * get/change the texture unit and then bind to it
     * 
     * _texture_unit: the new texture unit
     */
    const unsigned& get_texture_unit () const { return texture_unit; };
    void set_texture_unit ( const unsigned _texture_unit ) { texture_unit = _texture_unit; }

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

    /* format
     *
     * the format of the texture (e.g. GL_RGB)
     */
    GLenum format;

    /* texture_unit
     *
     * the texture unit to bind to
     * not one of GL_TEXTURE0 + x, rather just the x-value
     */
    unsigned texture_unit;

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
class glh::texture_exception : public exception
{
public:

    /* full constructor
     *
     * __what: description of the exception
     */
    explicit texture_exception ( const std::string& __what )
        : exception ( __what )
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