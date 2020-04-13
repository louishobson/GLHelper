/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * src/glhelper/glhelper_texture.cpp
 * 
 * implements include/glhelper/glhelper_texture.hpp
 * 
 */



/* INCLUDES */

/* include glhelper_texture.hpp */
#include <glhelper/glhelper_texture.hpp>

/* indlude stb_image.h with implementation */
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>



/* TEXTURE2D IMPLEMENTATION */

/* full constructor
 *
 * load a 2D texture from an image file
 * default settings are applied
 * 
 * _path: path to the image for the texture
 */
glh::texture2d::texture2d ( const std::string& _path )
    : object { glh::object_manager::generate_texture () }
    , path { _path }
    , internal_format ( GL_RGBA )
    , format { GL_RGBA }
    , type { GL_UNSIGNED_BYTE }
{
    /* load the image */
    unsigned char * image_data = stbi_load ( path.c_str (), &width, &height, &channels, 4 );

    /* check for error */
    if ( !image_data ) throw texture_exception { "failed to load texture from file at path " + path };

    /* bind texture object */
    bind ();

    /* set the texture data
     * although the original image may not have 4 channels, by putting the last parameter as 4 in stbi_load,
     * the image is forced to have 4 channels
     */
    glTexImage2D ( GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, type, image_data );
    
    /* generate mipmap */
    generate_mipmap ();

    /* free image data */
    stbi_image_free ( image_data );

    /* set default wrapping options */
    set_wrap ( GL_REPEAT );

    /* set mag/min options */
    set_mag_filter ( GL_LINEAR );
    set_min_filter ( GL_LINEAR_MIPMAP_LINEAR );
}

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
glh::texture2d::texture2d ( const unsigned _width, const unsigned _height, const GLenum _internal_format, const GLenum _format, const GLenum _type, const GLvoid * data )
    : object { glh::object_manager::generate_texture () }
    , path {}
    , internal_format { _internal_format }
    , format { _format }
    , type { _type }
{
    /* bind texture object */
    bind ();

    /* set texture data */
    glTexImage2D ( GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, type, data );

    /* set mag/min options */
    set_mag_filter ( GL_LINEAR );
    set_min_filter ( GL_LINEAR );
}


/* set_mag/min_filter
 *
 * set the texture filtering parameters of magnified/minified texture
 */
void glh::texture2d::set_mag_filter ( const GLenum opt ) 
{ 
    /* bind, set paramater, unbind */
    bind (); 
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, opt ); 
}
void glh::texture2d::set_min_filter ( const GLenum opt ) 
{ 
    /* bind, set paramater, unbind */
    bind (); 
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, opt );
}

/* set_(s/t/r)_wrap
 *
 * set the wrapping options for each coordinate axis, or all at once
 */
void glh::texture2d::set_s_wrap ( const GLenum opt ) 
{ 
    /* bind, set paramater, unbind */
    bind (); 
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, opt );
}
void glh::texture2d::set_t_wrap ( const GLenum opt ) 
{ 
    /* bind, set paramater, unbind */
    bind (); 
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, opt );
}
void glh::texture2d::set_r_wrap ( const GLenum opt ) 
{ 
    /* bind, set paramater, unbind */
    bind (); 
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, opt );
}
void glh::texture2d::set_wrap ( const GLenum opt ) 
{
    /* bind, set paramaters, unbind */
    bind (); 
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, opt ); 
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, opt );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, opt );
}

/* generate_mipmap
 *
 * generate texture mipmap
 */
void glh::texture2d::generate_mipmap ()
{
    /* bind and generate mipmap */
    bind ();
    glGenerateMipmap ( GL_TEXTURE_2D );
}