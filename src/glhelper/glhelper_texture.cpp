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
 * _width/height: width/height of the texture
 * _internal_format: the internal format of the data (e.g. specific bit arrangements)
 * _format: the format of the data (e.g. what the data will be used for)
 * _type: the type of the data stored in the texture
 */
glh::core::texture_base::texture_base ( const minor_object_type _minor_type, const unsigned _width, const unsigned _height, const GLenum _internal_format, const GLenum _format, const GLenum _type )
    : object { _minor_type }
{
    /* set parameters */
    set_texture_parameters ( _width, _height, _internal_format, _format, _type );

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



/* bind_loop
 *
 * this will keep looping around texture units, disincluding 0, and making subsequent binds to the next unit
 * this avoids binding a texture to a unit already in use
 * 
 * returns the unit just bound to
 */
unsigned glh::core::texture_base::bind_loop () const
{ 
    /* bind and get the unit bound to */
    bind ( bind_loop_index ); 
    const unsigned temp_unit = bind_loop_index++;

    /* loop if necesary */
    if ( bind_loop_index == 32 ) bind_loop_index = 1;

    /* return unit just bound to */
    return temp_unit;
}

/* bind_loop_previous
 *
 * return the previous binding of the loop
 */
unsigned glh::core::texture_base::bind_loop_previous () const
{
    /* if is 1, return 31, else return next index - 1 */
    if ( bind_loop_index == 1 ) return 31;
    else return bind_loop_index - 1;
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



/* set_texture_parameters 
 *
 * _width/height: width/height of the texture
 * _internal_format: the internal format of the data (e.g. specific bit arrangements)
 * _format: the format of the data (e.g. what the data will be used for)
 * _type: the type of the data in the texture
 */
void glh::core::texture_base::set_texture_parameters  ( const unsigned _width, const unsigned _height, const GLenum _internal_format, const GLenum _format, const GLenum _type )
{
    /* set parameters */
    width = _width; height = _height;
    internal_format = _internal_format;
    format = _format;
    type = _type;
}



/* bind_loop_next defaults to 1 */
unsigned glh::core::texture_base::bind_loop_index = 1;



/* TEXTURE2D IMPLEMENTATION */

/* full constructor
 *
 * load a 2D texture from an image file
 * default settings are applied
 * 
 * _path: path to the image for the texture
 * is_srgb: true if the texture should be corrected to linear color space (defaults to false)
 * flip_v: true if the texture should be vertically flipped
 */
glh::core::texture2d::texture2d ( const std::string& _path, const bool is_srgb, const bool flip_v )
    : texture_base { minor_object_type::GLH_TEXTURE2D_TYPE }
{
    /* set the texture */
    import_texture ( _path, is_srgb, flip_v );

    /* set mag/min options */
    set_mag_filter ( GL_LINEAR );
    set_min_filter ( GL_LINEAR_MIPMAP_LINEAR );
}

/* empty texture constructor
 *
 * create an texture of a given size with supplied data
 * 
 * _width/_height: the width and height of the texture
 * _internal_format: the internal format of the data (e.g. specific bit arrangements)
 * _format: the format of the data (e.g. what the data will be used for)
 * _type: the type of the data (specific type macro with bit arrangements)
 * data: the data to put in the texture (defaults to NULL)
 * _channels: the number of channels the texture has
 */
glh::core::texture2d::texture2d ( const unsigned _width, const unsigned _height, const GLenum _internal_format, const GLenum _format, const GLenum _type, const GLvoid * data, const unsigned _channels )
    : texture_base { minor_object_type::GLH_TEXTURE2D_TYPE }
{
    /* set the texture */
    set_texture ( _width, _height, _internal_format, _format, _type, data, _channels );

    /* set mag/min options */
    set_mag_filter ( GL_LINEAR );
    set_min_filter ( GL_LINEAR );
}



/* import_texture
 *
 * import a texture from a path
 * 
 * _path: path to the image for the texture
 * is_srgb: true if the texture should be corrected to linear color space (defaults to false)
 * flip_v: true if the texture should be vertically flipped
 */
void glh::core::texture2d::import_texture ( const std::string& _path, const bool is_srgb, const bool flip_v )
{
    /* set the path */
    path = _path;

    /* load the image */
    int _width, _height, _channels;
    unsigned char * image_data = stbi_load ( path.c_str (), &_width, &_height, &_channels, 4 );

    /* check for error */
    if ( !image_data ) throw exception::texture_exception { "failed to load texture2d texture from file at path " + path };

    /* flip if flagged */
    if ( flip_v ) stbi__vertical_flip ( image_data, _width, _height, 4 );

    /* set the parameters */
    set_texture_parameters ( _width, _height, ( is_srgb ? GL_SRGB_ALPHA : GL_RGBA ), GL_RGBA, GL_UNSIGNED_BYTE );
    channels = _channels;

    /* bind texture object */
    bind ();

    /* set texture data */
    glTexImage2D ( opengl_bind_target, 0, internal_format, width, height, 0, format, type, image_data );

    /* free image data */
    stbi_image_free ( image_data );

    /* generate mipmap */
    generate_mipmap ();
}



/* set_texture
 *
 * set the texture to binary data
 * 
 * _width/_height: the width and height of the texture
 * _internal_format: the internal format of the data (e.g. specific bit arrangements)
 * _format: the format of the data (e.g. what the data will be used for)
 * _type: the type of the pixel data (specific type macro with bit arrangements)
 * data: the data to put in the texture (defaults to NULL)
 * _channels: the number of channels the texture has (defaults to 4)
 */
void glh::core::texture2d::set_texture ( const unsigned _width, const unsigned _height, const GLenum _internal_format, const GLenum _format, const GLenum _type, const GLvoid * data, const unsigned _channels )
{
    /* set the parameters */
    set_texture_parameters ( _width, _height, _internal_format, _format, _type );
    path = "";
    channels = _channels;

    /* bind texture object */
    bind ();

    /* set texture data */
    glTexImage2D ( opengl_bind_target, 0, internal_format, width, height, 0, format, type, data );
}




/* CUBEMAP IMPLEMENTATION */

/* image constructor
 *
 * construct the cubemap from six separate images for each layer
 * the order of the paths is the same as the order of cubemap layers
 * 
 * paths: array of 6 paths to the images for the cubemap faces
 * is_srgb: true if the texture should be corrected to linear color space (defaults to false)
 * flip_v: true if the texture should be vertically flipped
 */
glh::core::cubemap::cubemap ( const std::array<std::string, 6>& paths, const bool is_srgb, const bool flip_v )
    : texture_base { minor_object_type::GLH_CUBEMAP_TYPE }
{
    /* import the cubemap */
    import_texture ( paths, is_srgb, flip_v );

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
 * flip_v: true if the texture should be vertically flipped
 */
glh::core::cubemap::cubemap ( const std::string& path, const bool is_srgb, const bool flip_v )
    : texture_base { minor_object_type::GLH_CUBEMAP_TYPE }
{
    /* import the cubemap */
    import_texture ( path, is_srgb, flip_v );

    /* set default wrapping options */
    set_wrap ( GL_REPEAT );

    /* set mag/min options */
    set_mag_filter ( GL_LINEAR );
    set_min_filter ( GL_LINEAR_MIPMAP_LINEAR );
}

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
glh::core::cubemap::cubemap ( const unsigned _width, const unsigned _height, const GLenum _internal_format, const GLenum _format, const GLenum _type, const GLvoid * data )
    : texture_base { minor_object_type::GLH_CUBEMAP_TYPE }
{
    /* set the cubemap */
    set_texture ( _width, _height, _internal_format, _format, _type, data );

    /* set default wrapping options */
    set_wrap ( GL_REPEAT );

    /* set mag/min options */
    set_mag_filter ( GL_LINEAR );
    set_min_filter ( GL_LINEAR_MIPMAP_LINEAR );
}



/* import_texture
 *
 * import from either a single, or 6 images
 * 
 * path/paths: the path(s) to the texture file(s)
 * is_srgb: true if the texture should be corrected to linear color space (defaults to false)
 * flip_v: true if the texture should be vertically flipped
 */
void glh::core::cubemap::import_texture ( const std::array<std::string, 6>& paths, const bool is_srgb, const bool flip_v )
{
    /* bind cubemap object */
    bind ();

    /* loop for each face of the cube */
    for ( unsigned i = 0; i < 6; ++i )
    {
        /* store width, height and channels */
        int _width, _height, _channels;

        /* load the image for that face */
        unsigned char * image_data = stbi_load ( paths.at ( i ).c_str (), &_width, &_height, &_channels, 4 );

        /* check for error in image loading */
        if ( !image_data ) throw exception::texture_exception { "failed to load cubemap texture from file at path " + paths.at ( i ) };

        /* flip if flagged */
        if ( flip_v ) stbi__vertical_flip ( image_data, _width, _height, 4 );

        /* if first iteration, set cubemap parameters
         * else check that the width and height of this texture is the same as previous textures
         */
        if ( i == 0 )
        {
            set_texture_parameters ( _width, _height, ( is_srgb ? GL_SRGB_ALPHA : GL_RGBA ), GL_RGBA, GL_UNSIGNED_BYTE );
        } else
        {
            if ( width != _width || height != _height ) throw exception::texture_exception { "cubemap texture dimensions at layer " + std::to_string ( i ) + " (path " + paths.at ( i ) + ") differs from the textures at previous layers" };
        }

        /* set the texture data for this face
         * although the original image may not have 4 channels, by putting the last parameter as 4 in stbi_load,
         * the image is forced to have 4 channels
         */
        glTexImage2D ( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internal_format, width, height, 0, format, type, image_data );

        /* free image data */
        stbi_image_free ( image_data );
    }

    /* generate mipmap */
    generate_mipmap ();
}
void glh::core::cubemap::import_texture ( const std::string& path, const bool is_srgb, const bool flip_v )
{
    /* bind cubemap object */
    bind ();

    /* load the image */
    int _width, _height, _channels;
    unsigned char * image_data = stbi_load ( path.c_str (), &_width, &_height, &_channels, 4 );

    /* check for error in image loading */
    if ( !image_data ) throw exception::texture_exception { "failed to load cubemap texture from file at path " + path };

    /* flip if flagged */
    if ( flip_v ) stbi__vertical_flip ( image_data, _width, _height, 4 );

    /* set the texture */
    set_texture_parameters ( _width, _height, ( is_srgb ? GL_SRGB_ALPHA : GL_RGBA ), GL_RGBA, GL_UNSIGNED_BYTE );

    /* bind cubemap object */
    bind ();

    /* loop for each face of the cube and configure that face */
    for ( unsigned i = 0; i < 6; ++i )
        glTexImage2D ( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internal_format, width, height, 0, format, type, image_data );

    /* free image data */
    stbi_image_free ( image_data );

    /* generate mipmap */
    generate_mipmap ();
}

/* set_texture
 *
 * _width/_height: the width and height of the texture
 * _internal_format: the internal format of the data (e.g. specific bit arrangements)
 * _format: the format of the data (e.g. what the data will be used for)
 * _type: the type of the pixel data (specific type macro with bit arrangements)
 * data: the data to put in the texture (defaults to NULL)
 */
void glh::core::cubemap::set_texture ( const unsigned _width, const unsigned _height, const GLenum _internal_format, const GLenum _format, const GLenum _type, const GLvoid * data )
{
    /* set parameters */
    set_texture_parameters ( _width, _height, _internal_format, _format, _type );

    /* bind cubemap object */
    bind ();

    /* loop for each face of the cube and configure that face */
    for ( unsigned i = 0; i < 6; ++i )
        glTexImage2D ( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internal_format, width, height, 0, format, type, data );
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
glh::core::texture2d_multisample::texture2d_multisample ( const unsigned _width, const unsigned _height, const GLenum _internal_format, const unsigned _samples, const bool _fixed_sample_locations )
    : texture_base { minor_object_type::GLH_TEXTURE2D_MULTISAMPLE_TYPE }
{
    /* set texture */
    set_texture ( _width, _height, _internal_format, _samples, _fixed_sample_locations );

    /* set default wrapping options */
    set_wrap ( GL_REPEAT );

    /* set mag/min options */
    set_mag_filter ( GL_LINEAR );
    set_min_filter ( GL_LINEAR );
}



/* set_texture
 *
 * _width/_height: the width and height of the texture
 * _internal_format: the internal format of the texture (e.g. specific bit arrangements)
 * _samples: the number of sampes the texture should contain
 * _fixed_sample_locations: defaults to true - I don't know what this setting does tbh
 */
void glh::core::texture2d_multisample::set_texture ( const unsigned _width, const unsigned _height, const GLenum _internal_format, const unsigned _samples, const bool _fixed_sample_locations )
{
    /* set parameters */
    set_texture_parameters ( _width, _height, _internal_format, GL_NONE, GL_NONE );
    samples = _samples;
    fixed_sample_locations = _fixed_sample_locations;

    /* bind texture object */
    bind ();

    /* set texture data */
    glTexImage2DMultisample ( opengl_bind_target, samples, internal_format, width, height, fixed_sample_locations );
}
