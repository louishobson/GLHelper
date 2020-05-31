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



/* TEXTURE_BASE IMPLEMENTATION */

/* full constructor
 *
 * only supply the texture target
 * 
 * _minor_type: the minor type of the texture
 * _internal_format: the internal format of the data (e.g. specific bit arrangements)
 * _format: the format of the data (e.g. what the data will be used for)
 * _width/height: width/height of the texture (defaults to 0)
 */
glh::core::texture_base::texture_base ( const minor_object_type _minor_type, const GLenum _internal_format, const GLenum _format, const int _width, const int _height )
    : object { _minor_type }
    , internal_format ( _internal_format )
    , format ( _format )
    , width { _width }
    , height { _height }
{
    /* assert major type is a texture */ 
    if ( major_type != major_object_type::GLH_TEXTURE_TYPE ) throw exception::texture_exception { "attempted to construct texture_base with non-texture type" }; 
}



/* unbind_all
 *
 * unbind from all targets
 * this includes all texture units
 */
bool glh::core::texture_base::unbind_all () const
{
    /* track whether anything was unbound */
    bool binding_change = false;

    /* get number of units */
    unsigned texture_units;
    if ( is_texture2d_bind_target ( bind_target ) ) texture_units = static_cast<unsigned> ( object_bind_target::__TEXTURE2D_END__ ) - static_cast<unsigned> ( object_bind_target::__TEXTURE2D_START__ ) - 1; else
    if ( is_cubemap_bind_target ( bind_target ) ) texture_units = static_cast<unsigned> ( object_bind_target::__CUBEMAP_START__ ) - static_cast<unsigned> ( object_bind_target::__CUBEMAP_END__ ) - 1; else
    if ( is_texture2d_multisample_bind_target ( bind_target ) )  texture_units = static_cast<unsigned> ( object_bind_target::__TEXTURE2D_MULTISAMPLE_START__ ) - static_cast<unsigned> ( object_bind_target::__TEXTURE2D_MULTISAMPLE_END__ ) - 1;
    else throw exception::object_exception { "attempted to perform unbind all operation to unknown target" };

    /* unbind all units */
    for ( unsigned i = 0; i < texture_units; ++i ) binding_change |= unbind ( i );

    /* return binding change */
    return binding_change;
}



/* set_mag/min_filter
 *
 * set the texture filtering parameters of magnified/minified texture
 */
void glh::core::texture_base::set_mag_filter ( const GLenum opt ) 
{ 
    /* bind, set paramater, unbind */
    bind (); 
    glTexParameteri ( opengl_bind_target, GL_TEXTURE_MAG_FILTER, opt ); 
}
void glh::core::texture_base::set_min_filter ( const GLenum opt ) 
{ 
    /* bind, set paramater */
    bind (); 
    glTexParameteri ( opengl_bind_target, GL_TEXTURE_MIN_FILTER, opt );
}

/* set_(s/t/r)_wrap
 *
 * set the wrapping options for each coordinate axis, or all at once
 */
void glh::core::texture_base::set_s_wrap ( const GLenum opt ) 
{ 
    /* bind, set paramater */
    bind (); 
    glTexParameteri ( opengl_bind_target, GL_TEXTURE_WRAP_S, opt );
}
void glh::core::texture_base::set_t_wrap ( const GLenum opt ) 
{ 
    /* bind, set paramater */
    bind (); 
    glTexParameteri ( opengl_bind_target, GL_TEXTURE_WRAP_T, opt );
}
void glh::core::texture_base::set_r_wrap ( const GLenum opt ) 
{ 
    /* bind, set paramater */
    bind (); 
    glTexParameteri ( opengl_bind_target, GL_TEXTURE_WRAP_R, opt );
}
void glh::core::texture_base::set_wrap ( const GLenum opt ) 
{
    /* bind, set paramaters */
    bind (); 
    glTexParameteri ( opengl_bind_target, GL_TEXTURE_WRAP_S, opt ); 
    glTexParameteri ( opengl_bind_target, GL_TEXTURE_WRAP_T, opt );
    glTexParameteri ( opengl_bind_target, GL_TEXTURE_WRAP_R, opt );
}

/* set_border_color
 *
 * set the color of the boarder, such that the texture can be clamped to the edge with a specific color
 */
void glh::core::texture_base::set_border_color ( const math::fvec3& color )
{
    /* bind, set paramater */
    bind (); 
    glTexParameterfv ( opengl_bind_target, GL_TEXTURE_BORDER_COLOR, color.internal_ptr () );
}

/* generate_mipmap
 *
 * generate texture mipmap
 */
void glh::core::texture_base::generate_mipmap ()
{
    /* bind and generate mipmap */
    bind ();
    glGenerateMipmap ( opengl_bind_target );
}



/* TEXTURE2D IMPLEMENTATION */

/* full constructor
 *
 * load a 2D texture from an image file
 * default settings are applied
 * 
 * _path: path to the image for the texture
 * is_srgb: true if the texture should be corrected to linear color space (defaults to false)
 */
glh::core::texture2d::texture2d ( const std::string& _path, const bool is_srgb )
    : texture_base { minor_object_type::GLH_TEXTURE2D_TYPE, static_cast<GLenum> ( is_srgb ? GL_SRGB_ALPHA : GL_RGBA ), GL_RGBA }
    , path { _path }
{
    /* load the image */
    unsigned char * image_data = stbi_load ( path.c_str (), &width, &height, &channels, 4 );

    /* check for error */
    if ( !image_data ) throw exception::texture_exception { "failed to load texture2d texture from file at path " + path };

    /* bind texture object */
    bind ();

    /* set the texture data
     * although the original image may not have 4 channels, by putting the last parameter as 4 in stbi_load,
     * the image is forced to have 4 channels
     */
    glTexImage2D ( opengl_bind_target, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, image_data );
    
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
glh::core::texture2d::texture2d ( const unsigned _width, const unsigned _height, const GLenum _internal_format, const GLenum _format, const GLenum _type, const GLvoid * data )
    : texture_base { minor_object_type::GLH_TEXTURE2D_TYPE, _internal_format, _format, ( int ) _width, ( int ) _height }
    , path {}
    , channels { 4 }
{
    /* bind texture object */
    bind ();

    /* set texture data */
    glTexImage2D ( opengl_bind_target, 0, internal_format, width, height, 0, format, _type, NULL );

    /* set default wrapping options */
    set_wrap ( GL_REPEAT );

    /* set mag/min options */
    set_mag_filter ( GL_LINEAR );
    set_min_filter ( GL_LINEAR );
}



/* CUBEMAP IMPLEMENTATION */

/* image constructor
 *
 * construct the cubemap from six separate images for each layer
 * the order of the paths is the same as the order of cubemap layers
 * 
 * paths: array of 6 paths to the images for the cubemap faces
 * is_srgb: true if the texture should be corrected to linear color space (defaults to false)
 */
glh::core::cubemap::cubemap ( const std::array<std::string, 6>& paths, const bool is_srgb )
    : texture_base { minor_object_type::GLH_CUBEMAP_TYPE, static_cast<GLenum> ( is_srgb ? GL_SRGB_ALPHA : GL_RGBA ), GL_RGBA }
{
    /* bind cubemap object */
    bind ();

    /* loop for each face of the cube */
    for ( unsigned i = 0; i < 6; ++i )
    {
        /* store width and height */
        int _width, _height;

        /* save the path and load the image for that face */
        face_textures.at ( i ).path = paths.at ( i );
        unsigned char * image_data = stbi_load ( face_textures.at ( i ).path.c_str (), &_width, &_height, &face_textures.at ( i ).channels, 4 );

        /* check for error in image loading */
        if ( !image_data ) throw exception::texture_exception { "failed to load cubemap texture from file at path " + face_textures.at ( i ).path };

        /* if first iteration, set cubemap width and height
         * else check that the width and height of this texture is the same as previous textures
         */
        if ( i == 0 )
        {
            width = _width;
            height = _height;
        } else
        {
            if ( width != _width || height != _height ) throw exception::texture_exception { "cubemap texture dimensions at layer " + std::to_string ( i ) + " (path " + face_textures.at ( i ).path + ") differs from the textures at previous layers" };
        }

        /* set the texture data for this face
         * although the original image may not have 4 channels, by putting the last parameter as 4 in stbi_load,
         * the image is forced to have 4 channels
         */
        glTexImage2D ( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, image_data );

        /* free image data */
        stbi_image_free ( image_data );
    }

    /* generate mipmap */
    generate_mipmap ();

    /* set default wrapping options */
    set_wrap ( GL_REPEAT );

    /* set mag/min options */
    set_mag_filter ( GL_LINEAR );
    set_min_filter ( GL_LINEAR_MIPMAP_LINEAR );
}

/* 1-image constructor
 * 
 * construct the cubemap width one image for all six sides
 *
 * path: path to the image 
 * is_srgb: true if the texture should be corrected to linear color space (defaults to false)
 */
glh::core::cubemap::cubemap ( const std::string& path, const bool is_srgb )
    : texture_base { minor_object_type::GLH_CUBEMAP_TYPE, static_cast<GLenum> ( is_srgb ? GL_SRGB_ALPHA : GL_RGBA ), GL_RGBA }
{
    /* bind cubemap object */
    bind ();

    /* store the number of channels */
    int _channels;

    /* load the image */
    unsigned char * image_data = stbi_load ( path.c_str (), &width, &height, &_channels, 4 );

    /* check for error in image loading */
    if ( !image_data ) throw exception::texture_exception { "failed to load cubemap texture from file at path " + path };

    /* loop for each face of the cube */
    for ( unsigned i = 0; i < 6; ++i )
    {
        /* set the info about the face */
        face_textures.at ( i ).path = path;
        face_textures.at ( i ).channels = _channels;

        /* set the texture data for this face
         * although the original image may not have 4 channels, by putting the last parameter as 4 in stbi_load,
         * the image is forced to have 4 channels
         */
        glTexImage2D ( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, image_data );
    }

    /* free image data */
    stbi_image_free ( image_data );

    /* generate mipmap */
    generate_mipmap ();

    /* set default wrapping options */
    set_wrap ( GL_REPEAT );

    /* set mag/min options */
    set_mag_filter ( GL_LINEAR );
    set_min_filter ( GL_LINEAR_MIPMAP_LINEAR );
}



/* TEXTURE2D_MULTISAMPLE IMPLEMENTATION */

/* empty texture constructor
 * 
 * constructs a multisample texture with a given size and number of samples 
 * 
 * _width/_height: the width and height of the texture
 * _internal_format: the internal format of the texture (e.g. specific bit arrangements)
 * _samples: the number of sampes the texture should contain
 * _fixed_sample_locations: defaults to true - I don't know what this setting does tbh
 */
glh::core::texture2d_multisample::texture2d_multisample ( const int _width, const int _height, const GLenum _internal_format, const unsigned _samples, const bool _fixed_sample_locations )
    : texture_base { minor_object_type::GLH_TEXTURE2D_MULTISAMPLE_TYPE, _internal_format, GL_NONE, _width, _height }
    , samples { _samples }
    , fixed_sample_locations { _fixed_sample_locations }
{
    /* bind texture object */
    bind ();

    /* set texture data */
    glTexImage2DMultisample ( opengl_bind_target, samples, internal_format, width, height, fixed_sample_locations );

    /* set default wrapping options */
    set_wrap ( GL_REPEAT );

    /* set mag/min options */
    set_mag_filter ( GL_LINEAR );
    set_min_filter ( GL_LINEAR );
}