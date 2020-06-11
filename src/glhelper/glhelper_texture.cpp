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



/* IMAGE IMPLEMENTATION */

/* full constructor
 *
 * construct from path to import
 * 
 * _path: the path to the image
 * _v_flip: flag as to whether the image should be vertically flipped
 */
glh::core::image::image ( const std::string& _path, const bool _v_flip )
    : path { _path }
    , v_flip { _v_flip }
    , image_data { stbi_load ( _path.c_str (), &width, &height, &channels, 4 ), [] ( void * ptr ) { if ( ptr ) stbi_image_free ( ptr ); } }
{
    /* throw if the image failed to load */
    if ( !image_data ) throw exception::texture_exception { "failed to load image from file at path " + path };

    /* flip vertically if necessary */
    if ( v_flip ) stbi__vertical_flip ( image_data.get (), width, height, 4 );
}

/* zero-parameter constructor */
glh::core::image::image ()
    : width { 0 }
    , path { "" }
    , height { 0 }
    , channels { 0 }
    , v_flip { false }
    , image_data { nullptr }
{}    

/* copy constructor
 *
 * the content of other is duplicated
 */
glh::core::image::image ( const image& other )
    : path { other.path }
    , width { other.width }
    , height { other.height }
    , channels { other.channels }
    , v_flip { other.v_flip }
    , image_data { std::malloc ( width * height * 4 ), [] ( void * ptr ) { if ( ptr ) std::free ( ptr ); } }
{
    /* memcpy into image_data */
    std::memcpy ( image_data.get (), other.image_data.get (), width * height * 4 );
}

/* copy assignment operator */
glh::core::image& glh::core::image::operator= ( const image& other )
{
    /* copy parameters */
    path = other.path;
    width = other.width; 
    height = other.height;
    channels = other.channels;
    v_flip = other.v_flip;
    
    /* allocate new memory for image_data, then transfer it */
    image_data = std::unique_ptr<void, std::function<void ( void * )>> { std::malloc ( width * height * 4 ), [] ( void * ptr ) { if ( ptr ) std::free ( ptr ); } };
    std::memcpy ( image_data.get (), other.image_data.get (), width * height * 4 );

    /* return * this */
    return * this;
}



/* vertical_flip
 *
 * flips the texture vertically
 */
void glh::core::image::vertical_flip ()
{
    /* swap flipped flag */
    v_flip = !v_flip;

    /* flip image */
    stbi__vertical_flip ( image_data.get (), width, height, 4 );
}



/* TEXTURE_BASE IMPLEMENTATION */

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



/* bind_loop_next defaults to 1 */
unsigned glh::core::texture_base::bind_loop_index = 1;



/* TEXTURE2D IMPLEMENTATION */

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
    /* set the paraameters */
    width = _width; height = _height;
    internal_format = _internal_format;
    has_alpha_component = true;

    /* call glTexImage2D */
    bind ();
    glTexImage2D ( opengl_bind_target, 0, internal_format, width, height, 0, format, type, data );
}
void glh::core::texture2d::tex_image ( const image& _image, const bool use_srgb )
{
    /* set the paraameters */
    width = _image.get_width (); height = _image.get_height ();
    internal_format = ( use_srgb ? GL_SRGB_ALPHA : GL_RGBA );
    has_alpha_component = _image.has_alpha ();

    /* call glTexImage2D */
    bind ();
    glTexImage2D ( opengl_bind_target, 0, internal_format, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _image.get_ptr () );
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
    has_alpha_component = true;

    /* call glTesSubImage2D */
    bind ();
    glTexSubImage2D ( opengl_bind_target, 0, x_offset, y_offset, _width, _height, format, type, data );
}
void glh::core::texture2d::tex_sub_image ( const unsigned x_offset, const unsigned y_offset, const image& _image )
{
    /* check offsets and dimensions */
    if ( x_offset + _image.get_width () > width || y_offset + _image.get_height () > height )
        throw exception::texture_exception { "attempted to call tex_sub_image on texture2d with offsets and dimensions which are out of range" };

    /* change has_alpha_component */
    if ( !has_alpha_component ) has_alpha_component = _image.has_alpha (); else
    if ( x_offset == 0 && y_offset == 0 && _image.get_width () == width && _image.get_height () == height ) has_alpha_component = _image.has_alpha ();

    /* call glTesSubImage2D */
    bind ();
    glTexSubImage2D ( opengl_bind_target, 0, x_offset, y_offset, _image.get_width (), _image.get_height (), GL_RGBA, GL_UNSIGNED_BYTE, _image.get_ptr () );
}



/* TEXTURE2D_ARRAY IMPLEMENTATION */

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
 * images: an initialiser list of images to form the array from
 * use_srgb: true if the textures should be srgb
 */
void glh::core::texture2d_array::tex_image ( const unsigned _width, const unsigned _height, const unsigned _depth, const GLenum _internal_format, const GLenum format, const GLenum type, const void * data )
{
    /* set the parameters */
    width = _width, height = _height, depth = _depth;
    internal_format = _internal_format;

    /* set up the texture array */
    bind ();
    glTexImage3D ( opengl_bind_target, 0, internal_format, width, height, depth, 0, format, type, data );
}
void glh::core::texture2d_array::tex_image ( std::initializer_list<std::reference_wrapper<const image>> images, const bool use_srgb )
{
    /* if images has size of 0, set width and height to 0 and create empty texture */
    if ( images.size () == 0 )
    {
        width = 0; height = 0; depth = 0;
    } else
    /* otherwise process the images */
    {
        /* set width and height to the first image's width and height, and the depth to the number of images supplied */
        width = images.begin ()->get ().get_width ();
        height = images.begin ()->get ().get_height ();
        depth = images.size ();

        /* assert that all of the images are the same size */
        for ( const image& _image: images ) if ( _image.get_width () != width || _image.get_height () != height )
            throw exception::texture_exception { "attempted to call tex_image on texture2d_array without all the images supplied being of the same dimensions" };
    }

    /* set internal_format to RGBA or sRGBA */
    internal_format = ( use_srgb ? GL_RGBA : GL_SRGB_ALPHA );

    /* first set the size of the texture array, then substitute the images in */
    bind ();
    glTexImage3D ( opengl_bind_target, 0, internal_format, width, height, depth, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
    auto images_it = images.begin ();
    for ( unsigned i = 0; i < depth; ++i, ++images_it )
        glTexSubImage3D ( opengl_bind_target, 0, 0, 0, i, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, images_it->get ().get_ptr () );
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
* images: an initialiser list of images to substitute into the array
*/
void glh::core::texture2d_array::tex_sub_image ( const unsigned x_offset, const unsigned y_offset, const unsigned z_offset, const unsigned _width, const unsigned _height, const unsigned _depth, const GLenum format, const GLenum type, const void * data )
{
    /* check offsets and dimensions */
    if ( x_offset + _width > width || y_offset + _height > height || z_offset + _depth > depth )
        throw exception::texture_exception { "attempted to call tex_sub_image on texture2d_array with offsets and dimensions which are out of range" };

    /* substitute the image data */
    bind ();
    glTexSubImage3D ( opengl_bind_target, 0, x_offset, y_offset, z_offset, _width, _height, _depth, format, type, data );
}
void glh::core::texture2d_array::tex_sub_image ( const unsigned x_offset, const unsigned y_offset, const unsigned z_offset, std::initializer_list<std::reference_wrapper<const image>> images )
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
        glTexSubImage3D ( opengl_bind_target, 0, x_offset, y_offset, z_offset + i, images_it->get ().get_width (), images_it->get ().get_width (), 1, GL_RGBA, GL_UNSIGNED_BYTE, images_it->get ().get_ptr () );
}



/* TEXTURE2D_MULTISAMPLE IMPLEMENTATION */

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
    /* set the parameters */
    width = _width; height = _height;
    samples = _samples;
    internal_format = _internal_format;
    fixed_sample_locations = _fixed_sample_locations;

    /* set up the texture */
    bind ();
    glTexImage2DMultisample ( opengl_bind_target, samples, internal_format, width, height, fixed_sample_locations );
}



/* CUBEMAP IMPLEMENTATION */

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
 * images: initialiser list of images that must be six elements large
 * use_srgb: true if srgb texture should be used
 */
void glh::core::cubemap::tex_image ( const unsigned _width, const unsigned _height, const GLenum _internal_format, const GLenum format, const GLenum type, const void * data )
{
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
    /* set the paraameters */
    width = _image.get_width (); height = _image.get_height ();
    internal_format = ( use_srgb ? GL_SRGB_ALPHA : GL_RGBA );

    /* call glTexImage2D on each face */
    bind ();
    for ( unsigned i = 0; i < 6; ++i )
        glTexImage2D ( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internal_format, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _image.get_ptr () );
}
void glh::core::cubemap::tex_image ( std::initializer_list<std::reference_wrapper<const image>> images, const bool use_srgb )
{
    /* assert only six images */
    if ( images.size () != 6 ) throw exception::texture_exception { "attempted to call tex_image on cubemap with " + std::to_string ( images.size () ) + " images supplied" };

    /* set the width and height to that of the first image */
    width = images.begin ()->get ().get_width ();
    height = images.begin ()->get ().get_width ();

    /* assert that the sizes of the images are all the same */
    for ( const image& _image: images ) if ( _image.get_width () != width || _image.get_height () != height )
        throw exception::texture_exception { "attempted to call tex_image on cubemap without all the images supplied being of the same dimensions" };

    /* set the internal format */
    internal_format = ( use_srgb ? GL_SRGB_ALPHA : GL_RGBA );

    /* call glTexImage2D on each face */
    bind ();
    auto images_it = images.begin ();
    for ( unsigned i = 0; i < 6; ++i, ++images_it )
        glTexImage2D ( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internal_format, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, images_it->get ().get_ptr () );
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
 * images: initialiser list of images that must be six elements large
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
        glTexSubImage2D ( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, x_offset, y_offset, _image.get_width (), _image.get_height (), GL_RGBA, GL_UNSIGNED_BYTE, _image.get_ptr () );
}
void glh::core::cubemap::tex_sub_image ( const unsigned face, const unsigned x_offset, const unsigned y_offset, const image& _image )
{
    /* check offsets and dimensions */
    if ( x_offset + _image.get_width () > width || y_offset + _image.get_height () > height )
        throw exception::texture_exception { "attempted to call tex_sub_image on cubemap with offsets and dimensions which are out of range" };

    /* substitute data on the face supplied */
    bind ();
    glTexSubImage2D ( GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, x_offset, y_offset, _image.get_width (), _image.get_height (), GL_RGBA, GL_UNSIGNED_BYTE, _image.get_ptr () );
}
void glh::core::cubemap::tex_sub_image ( const unsigned x_offset, const unsigned y_offset, std::initializer_list<std::reference_wrapper<const image>> images )
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
        glTexSubImage2D ( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, x_offset, y_offset, images_it->get ().get_width (), images_it->get ().get_height (), GL_RGBA, GL_UNSIGNED_BYTE, images_it->get ().get_ptr () );
}