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

/* indlude stb_image.h */
#define STB_IMAGE_IMPLEMENTATION
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



/* FULL DECLARATIONS */

class glh::texture2d : public object
{
public:

    /* full constructor
     *
     * load a 2D texture from an image file
     * default settings are applied
     * 
     * _path: path to the image for the texture
     * _format: the format of the image (e.g. GL_RGB)
     * _texture_unit: the texture unit to bind to
     */
    texture2d ( const std::string& _path, const GLenum _format, const GLenum _texture_unit = GL_TEXTURE0 );

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
    texture2d ( const texture2d& other ) = delete;

    /* virtual destructor */
    virtual ~texture2d () { destroy (); }



    /* set_texture_unit
     *
     * change the texture unit and then bind to it
     * 
     * _texture_unit: the new texture unit
     */
    void set_texture_unit ( const GLenum _texture_unit ) { texture_unit = _texture_unit }

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
     * bind the texture to the current texture unit
     */
    GLenum bind () const;

    /* unbind
     *
     * unbind the texture from the current texture unit
     */
    GLenum unbind () const;



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
    const GLenum format;

    /* texture_unit
     *
     * the texture unit to bind to
     */
    GLenum texture_unit;

    /* widith, height, channels
     *
     * stores the width, height, and number of channels of the texture
     */
    int width;
    int height;
    int channels;

};



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
    explicit texture_exception () = default;

    /* default everything else and inherits what () function */

};



/* #ifndef GLHELPER_TEXTURE_HPP_INCLUDED */
#endif