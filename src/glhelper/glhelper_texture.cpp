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
 * _texture_unit: the texture unit to bind to
 */
glh::texture2d::texture2d ( const std::string& _path, const GLenum _texture_unit )
    : path { _path }
    , format { GL_NONE }
    , texture_unit { _texture_unit }
{
    /* load the image */
    unsigned char * image_data = stbi_load ( path.c_str (), &width, &height, &channels, 0 );

    /* check for error */
    if ( !image_data ) throw texture_exception { "failed to load texture from file at path " + path };

    /* generate texture object and bind it */
    glGenTextures ( 1, &id );
    bind ();

    /* get the format of the texture based on the number of channels */
    if ( channels == 1 ) format = GL_RED; else
    if ( channels == 2 ) format = GL_RG; else
    if ( channels == 3 ) format = GL_RGB; else
    if ( channels == 4 ) format = GL_RGBA; else
    {
        throw texture_exception { "failed to set texture format for texture with " + std::to_string ( channels ) + " channels" };
    }
    

    /* set the texture and generate mipmap */
    glTexImage2D ( GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image_data );
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

    /* unbind */
    unbind ();
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
    unbind ();
}
void glh::texture2d::set_min_filter ( const GLenum opt ) 
{ 
    /* bind, set paramater, unbind */
    bind (); 
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, opt );
    unbind ();
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
    unbind ();
}
void glh::texture2d::set_t_wrap ( const GLenum opt ) 
{ 
    /* bind, set paramater, unbind */
    bind (); 
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, opt );
    unbind ();
}
void glh::texture2d::set_r_wrap ( const GLenum opt ) 
{ 
    /* bind, set paramater, unbind */
    bind (); 
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, opt );
    unbind ();
}
void glh::texture2d::set_wrap ( const GLenum opt ) 
{
    /* bind, set paramaters, unbind */
    bind (); 
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, opt ); 
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, opt );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, opt );
    unbind ();
}



/* destroy
 *
 * destroys the texture, setting its id to 0
 */
void glh::texture2d::destroy ()
{
    /* if is valid */
    if ( is_valid () )
    {
        /* destroy and set id to 0 */
        glDeleteShader ( id );
        id = 0;
    }    
}



/* bind
 *
 * bind the texture
 */
GLenum glh::texture2d::bind () const
{
    /* check the object is valid */
    if ( !is_valid () )  throw texture_exception { "attempted bind operation on invalid 2D texture object imported from path " + path };    

    /* bind the texture, if not already bound
     * texture unit gets activated by is_bound ()
     */
    if ( !is_bound () ) glBindTexture ( GL_TEXTURE_2D, id );

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
    if ( !is_valid () )  throw texture_exception { "attempted bind operation on invalid 2D texture object imported from path " + path };    

    /* unbind the texture, if not already unbound
     * texture unit gets activated by is_bound ()
     */
    if ( is_bound () ) glBindTexture ( GL_TEXTURE_2D, 0 );

    /* return the target */
    return GL_TEXTURE_2D;
}

/* is_bound
 *
 * return boolean for if the texture is bound
 */
bool glh::texture2d::is_bound () const
{
    /* activate the texture unit */
    glActiveTexture ( texture_unit );
    /* get texture currently bound to unit */
    GLint bound_texture;
    glGetIntegerv ( GL_TEXTURE_BINDING_2D, &bound_texture ); 
    /* return boolean for if is valid and is in use */
    return ( is_valid () && bound_texture == id ); 
}