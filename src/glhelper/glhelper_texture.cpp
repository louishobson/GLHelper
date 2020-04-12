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
    , format { GL_RGBA }
{
    /* load the image */
    unsigned char * image_data = stbi_load ( path.c_str (), &width, &height, &channels, 4 );

    /* check for error */
    if ( !image_data ) throw texture_exception { "failed to load texture from file at path " + path };

    /* generate texture object and bind it */
    bind ();

    /* set the texture and generate mipmap
     * although the original image may not have 4 channels, by putting the last parameter as 4 in stbi_load,
     * the image is forced to have 4 channels
     */
    glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data );
    glGenerateMipmap ( GL_TEXTURE_2D );

    /* free image data */
    stbi_image_free ( image_data );

    /* set default wrapping options */
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT );

    /* set mag/min options */
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
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