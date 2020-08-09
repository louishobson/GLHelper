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

/* indlude stb_image_write.h with implementation */
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>



/* IMAGE IMPLEMENTATION */

/* full constructor
 *
 * construct from path to import
 * 
 * _path: the path to the image
 * _channels: the number of channels to force the image to have
 * _v_flip: flag as to whether the image should be vertically flipped
 */
glh::core::image::image ( const std::string& _path, const unsigned _channels, const bool _v_flip )
    : path { _path }
    , channels { _channels }
    , v_flip { _v_flip }
{
    /* throw if channels > 4 */
    if ( channels > 4 ) throw exception::texture_exception { "an image cannot be imported with more than 4 channels" };

    /* set to flip vertically if necessary */
    stbi_set_flip_vertically_on_load ( _v_flip );

    /* load image */
    image_data = image_data_type { stbi_load ( _path.c_str (), &width, &height, &orig_channels, channels ), [] ( void * ptr ) { if ( ptr ) stbi_image_free ( ptr ); } };

    /* throw if the image failed to load */
    if ( !image_data ) throw exception::texture_exception { "failed to load image from file at path " + path };

    /* reset channels to orig_channels if channels == 0 */
    if ( channels == 0 ) channels = orig_channels;

    /* set definitely_opaque to true initially
     * then, if channels == 4, loop through the image data, and set to false if any alpha component does not equal 255 or 0
     * 0 counts as opaque since it translates to 'non-existent', hence does not require any blending
     */
    definitely_opaque = true;
    if ( channels == 4 ) for ( unsigned i = 0; i < width * height; ++i )
    {
        if ( reinterpret_cast<unsigned char *> ( image_data.get () ) [ ( i * channels ) + 3 ] != 255 &&
             reinterpret_cast<unsigned char *> ( image_data.get () ) [ ( i * channels ) + 3 ] != 0 )
        { 
            definitely_opaque = false; 
            break; 
        }
    }
}

/* zero-parameter constructor */
glh::core::image::image ()
    : path { "" }
    , channels { 0 }
    , width { 0 }
    , height { 0 }
    , orig_channels { 0 }
    , definitely_opaque { true }
    , v_flip { false }
    , image_data { nullptr }
{}    

/* copy constructor
 *
 * the content of other is duplicated
 */
glh::core::image::image ( const image& other )
    : path { other.path }
    , channels { other.channels }
    , width { other.width }
    , height { other.height }
    , orig_channels { other.orig_channels }
    , definitely_opaque { other.definitely_opaque }
    , v_flip { other.v_flip }
    , image_data { std::malloc ( width * height * channels ), [] ( void * ptr ) { if ( ptr ) std::free ( ptr ); } }
{
    /* memcpy into image_data */
    std::memcpy ( image_data.get (), other.image_data.get (), width * height * channels );
}

/* copy assignment operator */
glh::core::image& glh::core::image::operator= ( const image& other )
{
    /* copy parameters */
    path = other.path;
    channels = other.channels;
    width = other.width; 
    height = other.height;
    orig_channels = other.orig_channels;
    definitely_opaque = other.definitely_opaque;
    v_flip = other.v_flip;
    
    /* allocate new memory for image_data, then transfer it */
    image_data = image_data_type { std::malloc ( width * height * channels ), [] ( void * ptr ) { if ( ptr ) std::free ( ptr ); } };
    std::memcpy ( image_data.get (), other.image_data.get (), width * height * channels );

    /* return * this */
    return * this;
}



/* vertical_flip
 *
 * flips the image vertically
 */
void glh::core::image::vertical_flip ()
{
    /* swap flipped flag */
    v_flip = !v_flip;

    /* flip image */
    stbi__vertical_flip ( image_data.get (), width, height, channels );
}



/* TEXTURE_BASE IMPLEMENTATION */

/* full constructor
 *
 * set everything to defaults
 */
glh::core::texture_base::texture_base ()
{
    /* generate the texture */
    glGenTextures ( 1, &id );
}

/* destructor */
glh::core::texture_base::~texture_base ()
{
    /* destroy texture */
    glDeleteTextures ( 1, &id );
}



/* bind/unbind to texture units */
bool glh::core::texture_base::bind ( const unsigned index ) const
{
    /* if index is out of range, throw */
    if ( index >= 80 ) throw exception::texture_exception { "texture unit is out of range" };

    /* if already bound, return false, else bind and return true */
    if ( bound_texture_indices.at ( index ) == this ) return false;
    glBindTextureUnit ( index, id );
    bound_texture_indices.at ( index ) = const_cast<texture_base *> ( this );
    return true;
}
bool glh::core::texture_base::unbind ( const unsigned index ) const
{
    /* if index is out of range, throw */
    if ( index >= 80 ) throw exception::texture_exception { "texture unit is out of range" };

    /* if not bound, return false, else unbind and return true */
    if ( bound_texture_indices.at ( index ) != this ) return false;
    glBindTextureUnit ( index, 0 );
    bound_texture_indices.at ( index ) = NULL;
    return true;
}
bool glh::core::texture_base::is_bound ( const unsigned index ) const
{
    /* if index is out of range, throw */
    if ( index >= 80 ) throw exception::texture_exception { "texture unit is out of range" };

    /* return true if is bound */
    return bound_texture_indices.at ( index ) == this;
}



/* get the currently bound texture */
const glh::core::object_pointer<glh::core::texture_base>& glh::core::texture_base::get_bound_texture ( const unsigned index )
{
    /* if index is out of range, throw */
    if ( index >= 80 ) throw exception::texture_exception { "texture unit is out of range" };

    /* return bound texture pointer */
    return bound_texture_indices.at ( index );
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
    if ( bind_loop_index == 80 ) bind_loop_index = 1;

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
    if ( bind_loop_index == 1 ) return 79;
    else return bind_loop_index - 1;
}



/* get_tex_image
 *
 * extract the data in a texture
 * 
 * level: the mipmap level to sample
 * format/type: the format and type to recieve the texture in
 * buffsize: the size of the buffer to dump the texture in
 * buff: pointer to the buffer to recieve the texture
 */
void glh::core::texture_base::get_tex_image ( const unsigned level, const GLenum format, const GLenum type, const unsigned buffsize, void * buff ) const
{
    /* get the texture data */
    glGetTextureImage ( id, level, format, type, buffsize, buff );
}



/* set_mag/min_filter
 *
 * set the texture filtering parameters of magnified/minified texture
 */
void glh::core::texture_base::set_mag_filter ( const GLenum opt ) 
{ 
    /* set paramater, unbind */
    glTextureParameteri ( id, GL_TEXTURE_MAG_FILTER, opt ); 
}
void glh::core::texture_base::set_min_filter ( const GLenum opt ) 
{ 
    /* set paramater */
    glTextureParameteri ( id, GL_TEXTURE_MIN_FILTER, opt );
}

/* set_(s/t/r)_wrap
 *
 * set the wrapping options for each coordinate axis, or all at once
 */
void glh::core::texture_base::set_s_wrap ( const GLenum opt ) 
{ 
    /* set paramater */
    glTextureParameteri ( id, GL_TEXTURE_WRAP_S, opt );
}
void glh::core::texture_base::set_t_wrap ( const GLenum opt ) 
{ 
    /* set paramater */
    glTextureParameteri ( id, GL_TEXTURE_WRAP_T, opt );
}
void glh::core::texture_base::set_r_wrap ( const GLenum opt ) 
{ 
    /* set paramater */
    glTextureParameteri ( id, GL_TEXTURE_WRAP_R, opt );
}
void glh::core::texture_base::set_wrap ( const GLenum opt ) 
{
    /* set paramaters */
    glTextureParameteri ( id, GL_TEXTURE_WRAP_S, opt ); 
    glTextureParameteri ( id, GL_TEXTURE_WRAP_T, opt );
    glTextureParameteri ( id, GL_TEXTURE_WRAP_R, opt );
}

/* set_border_color
 *
 * set the color of the boarder, such that the texture can be clamped to the edge with a specific color
 */
void glh::core::texture_base::set_border_color ( const math::fvec4& color )
{
    /* set paramater */
    glTextureParameterfv ( id, GL_TEXTURE_BORDER_COLOR, color.internal_ptr () );
}

/* set_compare_mode
 *
 * set the compare mode for the texture (GL_TEXTURE_COMPARE_MODE)
 */
void glh::core::texture_base::set_compare_mode ( const GLenum opt )
{
    /* set parameter */
    glTextureParameteri ( id, GL_TEXTURE_COMPARE_MODE, opt );
}

/* set_compare_func
 *
 * set the comparison function for the texture
 */
void glh::core::texture_base::set_compare_func ( const GLenum opt )
{
    /* set parameter */
    glTextureParameteri ( id, GL_TEXTURE_COMPARE_FUNC, opt );
}

/* generate_mipmap
 *
 * generate texture mipmap
 */
void glh::core::texture_base::generate_mipmap ()
{
    /* generate mipmap */
    glGenerateTextureMipmap ( id );
}



/* channels_to_internal_format
 *
 * change a number of channels to an internal format
 */
GLenum glh::core::texture_base::channels_to_internal_format ( const unsigned channels, const bool use_srgb ) const
{
    /* return format */
    if ( use_srgb )
    {
        if ( channels == 3 ) return GL_SRGB8; else
        if ( channels == 4 ) return GL_SRGB8_ALPHA8;
    } else
    {
        if ( channels == 1 ) return GL_R8; else
        if ( channels == 2 ) return GL_RG8; else
        if ( channels == 3 ) return GL_RGB8; else
        if ( channels == 4 ) return GL_RGBA8;
    }

    /* throw, since invalid channel or channel-srgb combination */
    throw exception::texture_exception { "invalid channel or srgb-channel combination" };
}

/* channels_to_format
 *
 * change a number of channels to a format format
 */
GLenum glh::core::texture_base::channels_to_format ( const unsigned channels, const bool use_srgb ) const
{
    /* return format */
    if ( use_srgb )
    {
        if ( channels == 3 ) return GL_SRGB; else
        if ( channels == 4 ) return GL_SRGB_ALPHA;
    } else
    {
        if ( channels == 1 ) return GL_RED; else
        if ( channels == 2 ) return GL_RG; else
        if ( channels == 3 ) return GL_RGB; else
        if ( channels == 4 ) return GL_RGBA;
    }

    /* throw, since invalid channel or channel-srgb combination */
    throw exception::texture_exception { "invalid channel or srgb-channel combination" };
}



/* currently bound textures */
std::array<glh::core::object_pointer<glh::core::texture_base>, 80> glh::core::texture_base::bound_texture_indices {};

/* bind_loop_next defaults to 1 */
unsigned glh::core::texture_base::bind_loop_index { 1 };



/* TEXTURE1D IMPLEMENTATION */

/* default bind/unbind the texture */
bool glh::core::texture1d::bind () const
{
    /* if already bound, return false, else bind and return true */
    if ( bound_texture_indices.at ( 0 ) == this ) return false;
    glBindTexture ( GL_TEXTURE_1D, id );
    bound_texture_indices.at ( 0 ) = const_cast<texture1d *> ( this );
    return true;
}
bool glh::core::texture1d::unbind () const
{
    /* if not bound, return false, else unbind and return true */
    if ( bound_texture_indices.at ( 0 ) != this ) return false;
    glBindTexture ( GL_TEXTURE_1D, 0 );
    bound_texture_indices.at ( 0 ) = NULL;
    return true;
}



/* tex_storage
 *
 * set up the texture using immutable storage
 * 
 * _width: the width of the texture
 * _internal_format: the internal format of the texture
 * mipmap_levels: the number of mipmap levels to allocate (defaults to 0, which will allocate the maximum)
 */
void glh::core::texture1d::tex_storage ( const unsigned _width, const GLenum _internal_format, const unsigned mipmap_levels )
{
    /* throw if immutable */
    if ( is_immutable ) throw exception::texture_exception { "attempted to modify an immutable texture1d" };

    /* assert that all size parameters are more than 0 */
    if ( _width == 0 )
        throw exception::texture_exception { "cannot call tex_storage on texture1d with any size parameter as 0" };

    /* set the parameters */
    width = _width;
    internal_format = _internal_format;
    is_immutable = true;

    /* set the storage */
    glTextureStorage1D ( id, ( mipmap_levels > 0 ? mipmap_levels : std::log2 ( width ) + 1 ), internal_format, width );
}

/* tex_image
 *
 * set up the texture based on provided parameters
 * 
 * _width: the width of the texture
 * _internal_format: the internal format of the texture
 * format/type: the format and component type of the input data for the texture (defaults to GL_RGBA and GL_UNSIGNED_BYTE)
 * data: the actual data to put into the texture (defaults to NULL)
 */
void glh::core::texture1d::tex_image ( const unsigned _width, const GLenum _internal_format, const GLenum format, const GLenum type, const void * data )
{
    /* throw if immutable */
    if ( is_immutable ) throw exception::texture_exception { "attempted to modify an immutable texture1d" };

    /* set the parameters */
    width = _width;
    internal_format = _internal_format;

    /* call glTexImage2D */
    bind ();
    glTexImage1D ( GL_TEXTURE_1D, 0, internal_format, width, 0, format, type, data );
}

/* tex_sub_image
 *
 * substitiute data into the texture
 * changes are made to mipmap level 0, so remember to regenerate mipmaps if necessary
 * 
 * x_offset: the x offset to begin substitution
 * _width: the width of the area to substitute
 * format/type: the format and component type of the input data
 * data: the data to substitute
 */
void glh::core::texture1d::tex_sub_image ( const unsigned x_offset, const unsigned _width, const GLenum format, const GLenum type, const void * data )
{
    /* check offsets and dimensions */
    if ( x_offset + _width > width )
        throw exception::texture_exception { "attempted to call tex_sub_image on texture1d with offsets and dimensions which are out of range" };

    /* call glTesSubImage2D */
    bind ();
    glTexSubImage1D ( GL_TEXTURE_1D, 0, x_offset, _width, format, type, data );
}



/* BUFFER_TEXTURE IMPLEMENTATION */

/* default bind/unbind the texture */
bool glh::core::buffer_texture::bind () const
{
    /* if already bound, return false, else bind and return true */
    if ( bound_texture_indices.at ( 0 ) == this ) return false;
    glBindTexture ( GL_TEXTURE_BUFFER, id );
    bound_texture_indices.at ( 0 ) = const_cast<buffer_texture *> ( this );
    return true;
}
bool glh::core::buffer_texture::unbind () const
{
    /* if not bound, return false, else unbind and return true */
    if ( bound_texture_indices.at ( 0 ) != this ) return false;
    glBindTexture ( GL_TEXTURE_BUFFER, 0 );
    bound_texture_indices.at ( 0 ) = NULL;
    return true;
}



/* tex_buffer
 *
 * associates a buffer with the texture
 * 
 * buff: the new buffer to associate
 * _internal_format: the internal format of the buffer
 */
void glh::core::buffer_texture::tex_buffer ( const buffer& buff, const GLenum _internal_format )
{
    /* set the parameters */
    assoc_buffer = buff;
    internal_format = _internal_format;

    /* associate the buffer */
    glTextureBuffer ( id, internal_format, buff.internal_id () );
}



/* TEXTURE2D IMPLEMENTATION */

/* default bind/unbind the texture */
bool glh::core::texture2d::bind () const
{
    /* if already bound, return false, else bind and return true */
    if ( bound_texture_indices.at ( 0 ) == this ) return false;
    glBindTexture ( GL_TEXTURE_2D, id );
    bound_texture_indices.at ( 0 ) = const_cast<texture2d *> ( this );
    return true;
}
bool glh::core::texture2d::unbind () const
{
    /* if not bound, return false, else unbind and return true */
    if ( bound_texture_indices.at ( 0 ) != this ) return false;
    glBindTexture ( GL_TEXTURE_2D, 0 );
    bound_texture_indices.at ( 0 ) = NULL;
    return true;
}

/* tex_storage
 *
 * set up the texture using immutable storage
 * 
 * _width/_height: the width and height of the texture
 * _internal_format: the internal format of the texture
 * mipmap_levels: the number of mipmap levels to allocate (defaults to 0, which will allocate the maximum)
 */
void glh::core::texture2d::tex_storage ( const unsigned _width, const unsigned _height, const GLenum _internal_format, const unsigned mipmap_levels )
{
    /* throw if immutable */
    if ( is_immutable ) throw exception::texture_exception { "attempted to modify an immutable texture2d" };

    /* assert that all size parameters are more than 0 */
    if ( _width == 0 || _height == 0 )
        throw exception::texture_exception { "cannot call tex_storage on texture2d with any size parameter as 0" };

    /* set the parameters */
    width = _width; height = _height;
    internal_format = _internal_format;
    is_immutable = true;

    /* set the storage */
    glTextureStorage2D ( id, ( mipmap_levels > 0 ? mipmap_levels : std::log2 ( std::max ( width, height ) ) + 1 ), internal_format, width, height );
}

/* tex_image
 *
 * set up the texture based on provided parameters
 * 
 * EITHER:
 * 
 * _width/_height: the width and height of the texture
 * _internal_format: the internal format of the texture
 * format/type: the format and component type of the input data for the texture (defaults to GL_RGBA and GL_UNSIGNED_BYTE)
 * data: the actual data to put into the texture (defaults to NULL)
 * 
 * OR:
 * 
 * _image: the image to load the texture to
 * use_srgb: true if srgb texture should be used
 */
void glh::core::texture2d::tex_image ( const unsigned _width, const unsigned _height, const GLenum _internal_format, const GLenum format, const GLenum type, const void * data )
{
    /* throw if immutable */
    if ( is_immutable ) throw exception::texture_exception { "attempted to modify an immutable texture2d" };

    /* set the parameters */
    width = _width; height = _height;
    internal_format = _internal_format;
    definitely_opaque = false;

    /* call glTexImage2D */
    bind ();
    glTexImage2D ( GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, type, data );
}
void glh::core::texture2d::tex_image ( const image& _image, const bool use_srgb )
{
    /* throw if immutable */
    if ( is_immutable ) throw exception::texture_exception { "attempted to modify an immutable texture2d" };

    /* set the paraameters */
    width = _image.get_width (); height = _image.get_height ();
    internal_format = channels_to_internal_format ( _image.get_channels (), use_srgb );
    definitely_opaque = _image.is_definitely_opaque ();

    /* call glTexImage2D */
    bind ();
    glTexImage2D ( GL_TEXTURE_2D, 0, internal_format, width, height, 0, channels_to_format ( _image.get_channels () ), GL_UNSIGNED_BYTE, _image.get_ptr () );
}

/* tex_sub_image
 *
 * substitiute data into the texture
 * 
 * EITHER:
 * 
 * x/y_offset: the x and y offsets to begin substitution
 * _width/_height: the width and height of the area to substitute
 * format/type: the format and component type of the input data
 * data: the data to substitute
 * 
 * OR:
 * 
 * x/y_offset: the x and y offsets to begin substitution
 * _image: the image to substitute into the texture
 */
void glh::core::texture2d::tex_sub_image ( const unsigned x_offset, const unsigned y_offset, const unsigned _width, const unsigned _height, const GLenum format, const GLenum type, const void * data )
{
    /* check offsets and dimensions */
    if ( x_offset + _width > width || y_offset + _height > height )
        throw exception::texture_exception { "attempted to call tex_sub_image on texture2d with offsets and dimensions which are out of range" };

    /* now could have an alpha component */
    definitely_opaque = false;

    /* call glTesSubImage2D */
    bind ();
    glTexSubImage2D ( GL_TEXTURE_2D, 0, x_offset, y_offset, _width, _height, format, type, data );
}
void glh::core::texture2d::tex_sub_image ( const unsigned x_offset, const unsigned y_offset, const image& _image )
{
    /* check offsets and dimensions */
    if ( x_offset + _image.get_width () > width || y_offset + _image.get_height () > height )
        throw exception::texture_exception { "attempted to call tex_sub_image on texture2d with offsets and dimensions which are out of range" };

    /* change definitely_opaque */
    if ( definitely_opaque ) definitely_opaque = _image.is_definitely_opaque (); else
    if ( x_offset == 0 && y_offset == 0 && _image.get_width () == width && _image.get_height () == height ) definitely_opaque = _image.is_definitely_opaque ();

    /* call glTesSubImage2D */
    bind ();
    glTexSubImage2D ( GL_TEXTURE_2D, 0, x_offset, y_offset, _image.get_width (), _image.get_height (), channels_to_format ( _image.get_channels () ), GL_UNSIGNED_BYTE, _image.get_ptr () );
}



/* TEXTURE2D_ARRAY IMPLEMENTATION */

/* default bind/unbind the texture */
bool glh::core::texture2d_array::bind () const
{
    /* if already bound, return false, else bind and return true */
    if ( bound_texture_indices.at ( 0 ) == this ) return false;
    glBindTexture ( GL_TEXTURE_2D_ARRAY, id );
    bound_texture_indices.at ( 0 ) = const_cast<texture2d_array *> ( this );
    return true;
}
bool glh::core::texture2d_array::unbind () const
{
    /* if not bound, return false, else unbind and return true */
    if ( bound_texture_indices.at ( 0 ) != this ) return false;
    glBindTexture ( GL_TEXTURE_2D_ARRAY, 0 );
    bound_texture_indices.at ( 0 ) = NULL;
    return true;
}

/* tex_storage
 *
 * set up the texture using immutable storage
 * 
 * _width/_height/_depth: the width, height and number of textures in the array
 * _internal_format: the internal format of the texture
 * mipmap_levels: the number of mipmap levels to allocate (defaults to 0, which will allocate the maximum)
 */
void glh::core::texture2d_array::tex_storage ( const unsigned _width, const unsigned _height, const unsigned _depth, const GLenum _internal_format, const unsigned mipmap_levels )
{
    /* throw if immutable */
    if ( is_immutable ) throw exception::texture_exception { "attempted to modify an immutable texture2d_array" };

    /* assert that all size parameters are more than 0 */
    if ( _width == 0 || _height == 0 || _depth == 0 )
        throw exception::texture_exception { "cannot call tex_storage on texture2d_array with any size parameter as 0" };

    /* set the parameters */
    width = _width, height = _height, depth = _depth;
    internal_format = _internal_format;
    is_immutable = true;

    /* set up the storage */
    glTextureStorage3D ( id, ( mipmap_levels > 0 ? mipmap_levels : std::log2 ( std::max ( width, height ) ) + 1 ), internal_format, width, height, depth );
} 

/* tex_image
 *
 * set up the texture based on provided parameters
 * 
 * EITHER:
 * 
 * _width/_height/_depth: the width, height and number of textures in the array
 * _internal_format: the internal format of the textures
 * format/type: the format and type of the input data (defaults to GL_RGBA and GL_UNSIGNED_BYTE)
 * data: the data to put into the textures (defaults to NULL)
 *
 * OR:
 * 
 * images: an initializer list of images to form the array from
 * use_srgb: true if the textures should be srgb
 */
void glh::core::texture2d_array::tex_image ( const unsigned _width, const unsigned _height, const unsigned _depth, const GLenum _internal_format, const GLenum format, const GLenum type, const void * data )
{
    /* throw if immutable */
    if ( is_immutable ) throw exception::texture_exception { "attempted to modify an immutable texture2d_array" };

    /* set the parameters */
    width = _width, height = _height, depth = _depth;
    internal_format = _internal_format;

    /* set up the texture array */
    bind ();
    glTexImage3D ( GL_TEXTURE_2D_ARRAY, 0, internal_format, width, height, depth, 0, format, type, data );
}
void glh::core::texture2d_array::tex_image ( std::initializer_list<image> images, const bool use_srgb )
{
    /* throw if immutable */
    if ( is_immutable ) throw exception::texture_exception { "attempted to modify an immutable texture2d_array" };

    /* save format */
    GLenum format;

    /* if images has size of 0, set width, height and depth to 0, set format to GL_RED and create empty texture */
    if ( images.size () == 0 )
    {
        width = 0; height = 0; depth = 0; internal_format = GL_R8; format = GL_RED;
    } else
    /* otherwise process the images */
    {
        /* set width, height and internal_format to the first image's width, height and format, and set the depth to the number of images supplied */
        width = images.begin ()->get_width ();
        height = images.begin ()->get_height ();
        depth = images.size ();
        internal_format = channels_to_internal_format ( images.begin ()->get_channels (), use_srgb );
        format = channels_to_format ( images.begin ()->get_channels () ); 

        /* assert that all of the images are the same size and have the same format */
        for ( const image& _image: images ) if ( _image.get_width () != width || _image.get_height () != height || channels_to_internal_format ( _image.get_channels (), use_srgb ) != internal_format )
            throw exception::texture_exception { "attempted to call tex_image on texture2d_array without all the images supplied being of the same dimensions or formats" };
    }

    /* first set the size of the texture array, then substitute the images in */
    bind ();
    glTexImage3D ( GL_TEXTURE_2D_ARRAY, 0, internal_format, width, height, depth, 0, format, GL_UNSIGNED_BYTE, NULL );
    auto images_it = images.begin ();
    for ( unsigned i = 0; i < depth; ++i, ++images_it )
        glTexSubImage3D ( GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1, format, GL_UNSIGNED_BYTE, images_it->get_ptr () );
}



/* tex_sub_image
*
* substitiute data into the texture
* changes are made to mipmap level 0, so remember to regenerate mipmaps if necessary
* 
* EITHER:
* 
* x/y/z_offset: x, y and z-offsets for substituting image data
* _width/_height/_depth: the width height and depth of the data to substitute
* format/type: the format and type of the image data to substitute (defaults to GL_RGBA and GL_UNSIGNED_BYTE)
* data: the data to substitute (defaults to GL_RGBA and GL_UNSIGNED_BYTE)
* 
* OR:
* 
* x/y/z_offset: x, y and z-offsets for substituting image data
* images: an initializer list of images to substitute into the array
*/
void glh::core::texture2d_array::tex_sub_image ( const unsigned x_offset, const unsigned y_offset, const unsigned z_offset, const unsigned _width, const unsigned _height, const unsigned _depth, const GLenum format, const GLenum type, const void * data )
{
    /* check offsets and dimensions */
    if ( x_offset + _width > width || y_offset + _height > height || z_offset + _depth > depth )
        throw exception::texture_exception { "attempted to call tex_sub_image on texture2d_array with offsets and dimensions which are out of range" };

    /* substitute the image data */
    bind ();
    glTexSubImage3D ( GL_TEXTURE_2D_ARRAY, 0, x_offset, y_offset, z_offset, _width, _height, _depth, format, type, data );
}
void glh::core::texture2d_array::tex_sub_image ( const unsigned x_offset, const unsigned y_offset, const unsigned z_offset, std::initializer_list<image> images )
{
    /* check offsets and dimensions */
    if ( z_offset + images.size () > depth )
        throw exception::texture_exception { "attempted to call tex_sub_image on texture2d_array with offsets and dimensions which are out of range" };
    for ( const image& _image: images ) if ( x_offset + _image.get_width () > width || y_offset + _image.get_width () > height )
        throw exception::texture_exception { "attempted to call tex_sub_image on texture2d_array with offsets and dimensions which are out of range" };

    /* substitute the image data */
    bind ();
    auto images_it = images.begin ();
    for ( unsigned i = 0; i < images.size (); ++i, ++images_it )
        glTexSubImage3D ( GL_TEXTURE_2D_ARRAY, 0, x_offset, y_offset, z_offset + i, images_it->get_width (), images_it->get_width (), 1, channels_to_format ( images_it->get_channels () ), GL_UNSIGNED_BYTE, images_it->get_ptr () );
}



/* TEXTURE2D_MULTISAMPLE IMPLEMENTATION */

/* default bind/unbind the texture */
bool glh::core::texture2d_multisample::bind () const
{
    /* if already bound, return false, else bind and return true */
    if ( bound_texture_indices.at ( 0 ) == this ) return false;
    glBindTexture ( GL_TEXTURE_2D_MULTISAMPLE, id );
    bound_texture_indices.at ( 0 ) = const_cast<texture2d_multisample *> ( this );
    return true;
}
bool glh::core::texture2d_multisample::unbind () const
{
    /* if not bound, return false, else unbind and return true */
    if ( bound_texture_indices.at ( 0 ) != this ) return false;
    glBindTexture ( GL_TEXTURE_2D_MULTISAMPLE, 0 );
    bound_texture_indices.at ( 0 ) = NULL;
    return true;
}

/* tex_storage
 *
 * set up the texture using immutable storage 
 * 
 * _width/_height: the width and height of the texture
 * _samples: the number of samples
 * _internal_format: the internal format of the texture
 * _fixed_sample_locations: whether to use fixed sample locations (defaults to true)
 */
void glh::core::texture2d_multisample::tex_storage ( const unsigned _width, const unsigned _height, const unsigned _samples, const GLenum _internal_format, const bool _fixed_sample_locations )
{
    /* throw if immutable */
    if ( is_immutable ) throw exception::texture_exception { "attempted to modify an immutable texture2d_multisample" };

    /* assert that all size parameters are more than 0 */
    if ( _width == 0 || _height == 0 )
        throw exception::texture_exception { "cannot call tex_storage on texture2d_multisample with any size parameter as 0" };

    /* set the parameters */
    width = _width; height = _height;
    samples = _samples;
    internal_format = _internal_format;
    fixed_sample_locations = _fixed_sample_locations;
    is_immutable = true;

    /* set up the texture */
    glTextureStorage2DMultisample ( id, samples, internal_format, width, height, fixed_sample_locations );
}

/* tex_image
 * 
 * set up the 2d multisample texture
 * 
 * _width/_height: the width and height of the texture
 * _samples: the number of samples
 * _internal_format: the internal texture format
 * _fixed_sample_locations: whether to use fixed sample locations (defaults to true)
 */
void glh::core::texture2d_multisample::tex_image ( const unsigned _width, const unsigned _height, const unsigned _samples, const GLenum _internal_format, const bool _fixed_sample_locations )
{
    /* throw if immutable */
    if ( is_immutable ) throw exception::texture_exception { "attempted to modify an immutable texture2d_multisample" };

    /* set the parameters */
    width = _width; height = _height;
    samples = _samples;
    internal_format = _internal_format;
    fixed_sample_locations = _fixed_sample_locations;

    /* set up the texture */
    bind ();
    glTexImage2DMultisample ( GL_TEXTURE_2D_MULTISAMPLE, samples, internal_format, width, height, fixed_sample_locations );
}



/* CUBEMAP IMPLEMENTATION */

/* default bind/unbind the texture */
bool glh::core::cubemap::bind () const
{
    /* if already bound, return false, else bind and return true */
    if ( bound_texture_indices.at ( 0 ) == this ) return false;
    glBindTexture ( GL_TEXTURE_CUBE_MAP, id );
    bound_texture_indices.at ( 0 ) = const_cast<cubemap *> ( this );
    return true;
}
bool glh::core::cubemap::unbind () const
{
    /* if not bound, return false, else unbind and return true */
    if ( bound_texture_indices.at ( 0 ) != this ) return false;
    glBindTexture ( GL_TEXTURE_CUBE_MAP, 0 );
    bound_texture_indices.at ( 0 ) = NULL;
    return true;
}

/* tex_storage
 *
 * set up the texture using immutable storage
 * 
 * _width/_height: the width and height of the texture
 * _internal_format: the internal format of the texture
 * mipmap_levels: the number of mipmap levels to allocate (defaults to 0, which will allocate the maximum)
 */
void glh::core::cubemap::tex_storage ( const unsigned _width, const unsigned _height, const GLenum _internal_format, const unsigned mipmap_levels )
{
    /* throw if immutable */
    if ( is_immutable ) throw exception::texture_exception { "attempted to modify an immutable cubemap" };

    /* assert that all size parameters are more than 0 */
    if ( _width == 0 || _height == 0 )
        throw exception::texture_exception { "cannot call tex_storage on cubemap with any size parameter as 0" };

    /* set the parameters */
    width = _width; height = _height;
    internal_format = _internal_format;
    is_immutable = true;

    /* set storage */
    glTextureStorage2D ( 0, ( mipmap_levels > 0 ? mipmap_levels : std::log2 ( std::max ( width, height ) ) + 1 ), internal_format, width, height );
}

/* tex_image
 *
 * set up the cubemap based on provided parameters
 * 
 * EITHER:
 * 
 * _width/_height: the width and height of all of the faces of the cubemap
 * _internal_format: the internal format of the cubemap
 * format/type: the format and component type of the input data for the cubemap (defaults to GL_RGBA and GL_UNSIGNED_BYTE)
 * data: the data to apply to each face of the cubemap (defaults to NULL)
 * 
 * OR:
 * 
 * _image: the image to load each face of the cubemap to
 * use_srgb: true if srgb texture should be used
 * 
 * OR:
 * 
 * images: initializer list of images that must be six elements large
 * use_srgb: true if srgb texture should be used
 */
void glh::core::cubemap::tex_image ( const unsigned _width, const unsigned _height, const GLenum _internal_format, const GLenum format, const GLenum type, const void * data )
{
    /* throw if immutable */
    if ( is_immutable ) throw exception::texture_exception { "attempted to modify an immutable cubemap" };

    /* set the parameters */
    width = _width; height = _height;
    internal_format = _internal_format;

    /* call glTexImage2D on each face */
    bind ();
    for ( unsigned i = 0; i < 6; ++i )
        glTexImage2D ( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internal_format, width, height, 0, format, type, data );
}
void glh::core::cubemap::tex_image ( const image& _image, const bool use_srgb )
{
    /* throw if immutable */
    if ( is_immutable ) throw exception::texture_exception { "attempted to modify an immutable cubemap" };

    /* set the paraameters */
    width = _image.get_width (); height = _image.get_height ();
    internal_format = channels_to_internal_format ( _image.get_channels (), use_srgb );

    /* call glTexImage2D on each face */
    bind ();
    for ( unsigned i = 0; i < 6; ++i )
        glTexImage2D ( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internal_format, width, height, 0, channels_to_format ( _image.get_channels () ), GL_UNSIGNED_BYTE, _image.get_ptr () );
}
void glh::core::cubemap::tex_image ( std::initializer_list<image> images, const bool use_srgb )
{
    /* throw if immutable */
    if ( is_immutable ) throw exception::texture_exception { "attempted to modify an immutable cubemap" };

    /* assert only six images */
    if ( images.size () != 6 ) throw exception::texture_exception { "attempted to call tex_image on cubemap with " + std::to_string ( images.size () ) + " images supplied" };

    /* set the width, height and internal format to that of the first image */
    width = images.begin ()->get_width ();
    height = images.begin ()->get_width ();
    internal_format = channels_to_internal_format ( images.begin ()->get_channels (), use_srgb );

    /* assert that the sizes and formats of the images are all the same */
    for ( const image& _image: images ) if ( _image.get_width () != width || _image.get_height () != height || channels_to_internal_format ( _image.get_channels (), use_srgb ) != internal_format )
        throw exception::texture_exception { "attempted to call tex_image on cubemap without all the images supplied being of the same dimensions or formats" };

    /* call glTexImage2D on each face */
    bind ();
    auto images_it = images.begin ();
    for ( unsigned i = 0; i < 6; ++i, ++images_it )
        glTexImage2D ( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internal_format, width, height, 0, channels_to_format ( images.begin ()->get_channels () ), GL_UNSIGNED_BYTE, images_it->get_ptr () );
}

/* tex_sub_image
 *
 * substitiute data into the cubemap
 * changes are made to mipmap level 0, so remember to regenerate mipmaps if necessary
 * 
 * EITHER: applied to every face
 * 
 * x/y_offset: the x and y offsets to begin substitution
 * _width/_height: the width and height of the area to substitute
 * format/type: the format and component type of the input data
 * data: the data to substitute
 * 
 * OR: applied to the face supplied
 * 
 * face: the face to substitute data into
 * x/y_offset: the x and y offsets to begin substitution
 * _width/_height: the width and height of the area to substitute
 * format/type: the format and component type of the input data
 * data: the data to substitute
 * 
 * OR: applied to every face
 * 
 * x/y_offset: the x and y offsets to begin substitution
 * _image: the image to substitute into the texture
 * 
 * OR: applied to the face supplied
 * 
 * face: the face to substitute data into
 * x/y_offset: the x and y offsets to begin substitution
 * _image: the image to substitute into the texture
 * 
 * OR: applied to every face with different textures
 * 
 * x/y_offset: the x and y offsets to begin substitution
 * images: initializer list of images that must be six elements large
 */
void glh::core::cubemap::tex_sub_image ( const unsigned x_offset, const unsigned y_offset, const unsigned _width, const unsigned _height, const GLenum format, const GLenum type, const void * data )
{
    /* check offsets and dimensions */
    if ( x_offset + _width > width || y_offset + _height > height )
        throw exception::texture_exception { "attempted to call tex_sub_image on cubemap with offsets and dimensions which are out of range" };

    /* substitute data on each face */
    bind ();
    for ( unsigned i = 0; i < 6; ++i )
        glTexSubImage2D ( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, x_offset, y_offset, _width, _height, format, type, data );
}
void glh::core::cubemap::tex_sub_image ( const unsigned face, const unsigned x_offset, const unsigned y_offset, const unsigned _width, const unsigned _height, const GLenum format, const GLenum type, const void * data )
{
    /* check offsets and dimensions */
    if ( x_offset + _width > width || y_offset + _height > height )
        throw exception::texture_exception { "attempted to call tex_sub_image on cubemap with offsets and dimensions which are out of range" };

    /* substitute data on the face supplied */
    bind ();
    glTexSubImage2D ( GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, x_offset, y_offset, _width, _height, format, type, data );
}
void glh::core::cubemap::tex_sub_image ( const unsigned x_offset, const unsigned y_offset, const image& _image )
{
    /* check offsets and dimensions */
    if ( x_offset + _image.get_width () > width || y_offset + _image.get_height () > height )
        throw exception::texture_exception { "attempted to call tex_sub_image on cubemap with offsets and dimensions which are out of range" };

    /* substitute data on each face */
    bind ();
    for ( unsigned i = 0; i < 6; ++i )
        glTexSubImage2D ( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, x_offset, y_offset, _image.get_width (), _image.get_height (), channels_to_format ( _image.get_channels () ), GL_UNSIGNED_BYTE, _image.get_ptr () );
}
void glh::core::cubemap::tex_sub_image ( const unsigned face, const unsigned x_offset, const unsigned y_offset, const image& _image )
{
    /* check offsets and dimensions */
    if ( x_offset + _image.get_width () > width || y_offset + _image.get_height () > height )
        throw exception::texture_exception { "attempted to call tex_sub_image on cubemap with offsets and dimensions which are out of range" };

    /* substitute data on the face supplied */
    bind ();
    glTexSubImage2D ( GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, x_offset, y_offset, _image.get_width (), _image.get_height (), channels_to_format ( _image.get_channels () ), GL_UNSIGNED_BYTE, _image.get_ptr () );
}
void glh::core::cubemap::tex_sub_image ( const unsigned x_offset, const unsigned y_offset, std::initializer_list<image> images )
{
    /* assert only six images */
    if ( images.size () != 6 ) throw exception::texture_exception { "attempted to call tex_sub_image on cubemap with " + std::to_string ( images.size () ) + " images supplied" };

    /* check offsets and dimensions */
    for ( const image& _image: images ) if ( x_offset + _image.get_width () > width || y_offset + _image.get_height () > height )
        throw exception::texture_exception { "attempted to call tex_sub_image on cubemap with offsets and dimensions which are out of range" };

    /* substitute data on each face */
    bind ();
    auto images_it = images.begin ();
    for ( unsigned i = 0; i < 6; ++i, ++images_it )
        glTexSubImage2D ( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, x_offset, y_offset, images_it->get_width (), images_it->get_height (), channels_to_format ( images_it->get_channels () ), GL_UNSIGNED_BYTE, images_it->get_ptr () );
}



/* CUBEMAP_ARRAY IMPLEMENTATION */

/* default bind/unbind the texture */
bool glh::core::cubemap_array::bind () const
{
    /* if already bound, return false, else bind and return true */
    if ( bound_texture_indices.at ( 0 ) == this ) return false;
    glBindTexture ( GL_TEXTURE_CUBE_MAP_ARRAY, id );
    bound_texture_indices.at ( 0 ) = const_cast<cubemap_array *> ( this );
    return true;
}
bool glh::core::cubemap_array::unbind () const
{
    /* if not bound, return false, else unbind and return true */
    if ( bound_texture_indices.at ( 0 ) != this ) return false;
    glBindTexture ( GL_TEXTURE_CUBE_MAP_ARRAY, 0 );
    bound_texture_indices.at ( 0 ) = NULL;
    return true;
}

/* tex_storage
 *
 * set up the cubemap array with immutable storage
 *
 * _width/_height/_depth: the width and height of each face and the number of layer-faces in the cubemap array (so must be multiple of six)
 * _internal_format: the internal format of the cubemap array
 * mipmap_levels: the number of mipmap levels to allocate (defaults to 0, which will allocate the maximum)
 */
void glh::core::cubemap_array::tex_storage ( const unsigned _width, const unsigned _height, const unsigned _depth, const GLenum _internal_format, const unsigned mipmap_levels )
{
    /* throw if immutable */
    if ( is_immutable ) throw exception::texture_exception { "attempted to modify an immutable cubemap_array" };

    /* assert that all size parameters are more than 0 */
    if ( _width == 0 || _height == 0 || _depth == 0 )
        throw exception::texture_exception { "cannot call tex_storage on cubemap_array with any size parameter as 0" };

    /* assert depth is a multiple of 6 */
    if ( _depth % 6 != 0 ) throw exception::texture_exception { "cannot call tex_storage on cubemap_array with a depth that is not a multiple of 6" };

    /* set the parameters */
    width = _width; height = _height; depth = _depth;
    internal_format = _internal_format;
    is_immutable = true;

    /* set storage */
    glTextureStorage3D ( id, ( mipmap_levels > 0 ? mipmap_levels : std::log2 ( std::max ( width, height ) ) + 1 ), internal_format, width, height, depth );
}

/* tex_image
 *
 * set up the cubemap array with mutable storage
 * 
 * EITHER: 
 * 
 * _width/_height/_depth: the width and height of each face and the number of layer-faces in the cubemap array (so must be multiple of six)
 * _internal_format: the internal format of the cubemap array
 * format/type: the format and component type of the input data for the cubemap (defaults to GL_RGBA and GL_UNSIGNED_BYTE)
 * data: the data to apply to to the cubemap (defaults to NULL)
 * 
 * OR:
 * 
 * images: an initializer list of images which must be a multiple of six in size, where each image is applied to every face of the cubemap array
 * use_srgb: true if srgb textures should be used
 */
void glh::core::cubemap_array::tex_image ( const unsigned _width, const unsigned _height, const unsigned _depth, const GLenum _internal_format, const GLenum format, const GLenum type, const void * data )
{
    /* throw if immutable */
    if ( is_immutable ) throw exception::texture_exception { "attempted to modify an immutable cubemap_array" };

    /* assert depth is a multiple of 6 */
    if ( _depth % 6 != 0 ) throw exception::texture_exception { "cannot call tex_storage on cubemap_array with a depth that is not a multiple of 6" };

    /* set the parameters */
    width = _width; height = _height; depth = _depth;
    internal_format = _internal_format;

    /* set the storage */
    bind ();
    glTexImage3D ( GL_TEXTURE_CUBE_MAP_ARRAY, 0, internal_format, width, height, depth, 0, format, type, data );
}
void glh::core::cubemap_array::tex_image ( std::initializer_list<image> images, const bool use_srgb )
{
    /* throw if immutable */
    if ( is_immutable ) throw exception::texture_exception { "attempted to modify an immutable cubemap_array" };

    /* store the format */
    GLenum format;

    /* if images has size of 0, set width, height and depth to 0, internal_format to GL_R8, format to GL_RED and create empty texture */
    if ( images.size () == 0 )
    {
        width = 0; height = 0; depth = 0; internal_format = GL_R8; format = GL_RED;
    } else
    /* otherwise process the images */
    {
        /* assert that there are a multiple of 6 images */
        if ( images.size () % 6 != 0 ) throw exception::texture_exception { "attempted to call tex_image on a cubemap_array with a non-multiple of 6 number of images supplied" };

        /* set width, height and internal_format to the first image's width, height and internal format, and set the depth to the number of images supplied */
        width = images.begin ()->get_width ();
        height = images.begin ()->get_height ();
        depth = images.size ();
        internal_format = channels_to_internal_format ( images.begin ()->get_channels (), use_srgb );
        format = channels_to_format ( images.begin ()->get_channels () );

        /* assert that all of the images are the same size and of the same format */
        for ( const image& _image: images ) if ( _image.get_width () != width || _image.get_height () != height || channels_to_internal_format ( _image.get_channels (), use_srgb ) != internal_format )
            throw exception::texture_exception { "attempted to call tex_image on texture2d_array without all the images supplied being of the same dimensions or format" };
    }

    /* first set the size of the texture array, then substitute the images in */
    bind ();
    glTexImage3D ( GL_TEXTURE_CUBE_MAP_ARRAY, 0, internal_format, width, height, depth, 0, format, GL_UNSIGNED_BYTE, NULL );
    auto images_it = images.begin ();
    for ( unsigned i = 0; i < depth; ++i, ++images_it )
        glTexSubImage3D ( GL_TEXTURE_CUBE_MAP_ARRAY, 0, 0, 0, i, width, height, 1, format, GL_UNSIGNED_BYTE, images_it->get_ptr () );
}

/* tex_sub_image
 *
 * substitute data into the cubemap array
 * changes are made to mipmap level 0, so remember to regenerate mipmaps if necessary
 * 
 * EIETHER: 
 * 
 * x/y/z_offset: the x, y and z offsets to begin substitution
 * _width/_height/_depth: the width, height and depth of the area to substitute
 * format/type: the format and type of the input data
 * data: the data to substitute
 * 
 * OR:
 * 
 * x/y/z_offset: the x, y and z offsets to begin substitution
 * images: initializer list of images to substitute
 */
void glh::core::cubemap_array::tex_sub_image ( const unsigned x_offset, const unsigned y_offset, const unsigned z_offset, const unsigned _width, const unsigned _height, const unsigned _depth, const GLenum format, const GLenum type, const void * data )
{
    /* check offsets and dimensions */
    if ( x_offset + _width > width || y_offset + _height > height || z_offset + _depth > depth )
        throw exception::texture_exception { "attempted to call tex_sub_image on texture2d_array with offsets and dimensions which are out of range" };

    /* substitute the image data */
    bind ();
    glTexSubImage3D ( GL_TEXTURE_CUBE_MAP_ARRAY, 0, x_offset, y_offset, z_offset, _width, _height, _depth, format, type, data );
}
void glh::core::cubemap_array::tex_sub_image ( const unsigned x_offset, const unsigned y_offset, const unsigned z_offset, std::initializer_list<image> images )
{
    /* check offsets and dimensions */
    if ( z_offset + images.size () > depth )
        throw exception::texture_exception { "attempted to call tex_sub_image on texture2d_array with offsets and dimensions which are out of range" };
    for ( const image& _image: images ) if ( x_offset + _image.get_width () > width || y_offset + _image.get_width () > height )
        throw exception::texture_exception { "attempted to call tex_sub_image on texture2d_array with offsets and dimensions which are out of range" };

    /* substitute the image data */
    bind ();
    auto images_it = images.begin ();
    for ( unsigned i = 0; i < images.size (); ++i, ++images_it )
        glTexSubImage3D ( GL_TEXTURE_CUBE_MAP_ARRAY, 0, x_offset, y_offset, z_offset + i, images_it->get_width (), images_it->get_width (), 1, channels_to_format ( images_it->get_channels () ), GL_UNSIGNED_BYTE, images_it->get_ptr () );
}