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



/* TEXTURE2D IMPLEMENTATION */

/* full constructor
 *
 * load a 2D texture from an image file
 * default settings are applied
 * 
 * _path: path to the image for the texture
 * _format: the format of the image (e.g. GL_RGB)
 * _texture_unit: the texture unit to bind to
 */
glh::texture2d::texture2d ( const std::string& _path, const GLenum _format, const GLenum _texture_unit )
    : path { _path }
    , format { _format }
    , texture_unit { _texture_unit }
{
    /* load the image */
    unsigned char * image_data = stbi_load ( path.c_str (), &width, &height, &channels, 0 );

    /* check for error */
    if ( !image_data ) throw texture_exception { "failed to load texture from file" };

    /* generate texture object and bind it */
    glGenTextures ( 1, &id );
    bind ();

    /* set the texture and generate mipmap */
    glTexImage2D ( GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image_data );
    glGenerateMipmap ( GL_TEXTURE_2D );z

    /* free image data */
    stbi_image_free ( image_data );

    /* set default wrapping options */
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT );

    /* set mag/min options */
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );

    /* leave bound */
}

/* bind
 *
 * bind the texture
 */
GLenum glh::texture2d::bind () const
{
    /* check the object is valid */
    if ( !is_valid () )  throw texture_exception { "attempted bind operation on invalid 2D texture object" };    

    /* activate the texture unit */
    glActiveTexture ( texture_unit );
    /* bind the texture */
    glBindTexture ( GL_TEXTURE_2D, id );

    /* return the target */
    return GL_TEXTURE_2D;
}

/* unbind
 *
 * unbind the texture
 */
GLenum glh::texture2d::unbind () const
{
    /* check the object is valid */
    if ( !is_valid () )  throw texture_exception { "attempted bind operation on invalid 2D texture object" };    

    /* activate the texture unit */
    glActiveTexture ( texture_unit );
    /* unbind the texture */
    glBindTexture ( GL_TEXTURE_2D, 0 );

    /* return the target */
    return GL_TEXTURE_2D;
}