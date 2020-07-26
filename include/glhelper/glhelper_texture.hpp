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
 * defines constructs to handle textures
 * notable constructs include:
 * 
 * 
 * 
 * CLASS GLH::CORE::IMAGE
 * 
 * imports a 2d image from an external file
 * the format will always be 4 channel RGBA, each component an unsigned byte
 * 
 * 
 * 
 * CLASS GLH::CORE::TEXTURE_BASE
 * 
 * base class for all textures
 * implements common fucntionality such as wrapping and filtering settings
 * 
 * 
 * 
 * CLASS GLH::CORE::TEXTURE2D
 * 
 * derivation of texture_base to represent a 2d texture
 * the texture can be loaded from a file, or be initialised blank
 * blank textures are useful for using as color buffers for framebuffer objects
 * 
 * 
 * 
 * CLASS GLH::CORE::TEXTURE2D_ARRAY
 * 
 * derivation of texture_base to represent a 2d texture array
 * 
 * 
 * 
 * CLASS GLH::CORE::TEXTURE2D_MULTISAMPLE
 * 
 * derivation of texture_base to represent a 2d multisampled texture
 * this cannot be initialised to an image, since it is purely used for rendering to using an fbo
 * 
 * 
 * 
 * CLASS GLH::CORE::CUBEMAP
 * 
 * derivation of texture_base to represent a cubemap
 * 
 * 
 * 
 * CLASS GLH::CORE::CUBEMAP_ARRAY
 * 
 * derivation of texture_base to represent a cubemap texture array
 * 
 * 
 * 
 * CLASS GLH::EXCEPTION::TEXTURE_EXCEPTION
 * 
 * thrown when an error occurs in one of the texture methods (e.g. a texture file cannot be found)
 * 
 */



/* HEADER GUARD */
#ifndef GLHELPER_TEXTURE_HPP_INCLUDED
#define GLHELPER_TEXTURE_HPP_INCLUDED



/* INCLUDES */

/* include core headers */
#include <cstdlib>
#include <cstring>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <string>

/* include glhelper_core.hpp */
#include <glhelper/glhelper_core.hpp>

/* include glhelper_buffer.hpp */
#include <glhelper/glhelper_buffer.hpp>

/* include glhelper_vector.hpp */
#include <glhelper/glhelper_vector.hpp>

/* indlude stb_image.h without implementation */
#include <stb/stb_image.h>



/* NAMESPACE DECLARATIONS */

namespace glh
{
    namespace core
    {
        /* class image
         *
         * importer for external images
         */
        class image;



        /* class texture_base : object
         *
         * base class for all textures
         */
        class texture_base;

        /* class texture_multisample_base : texture_base
         *
         * base class for multisample textures
         * causes certain option-setting methods to throw
         */
        class texture_multisample_base;

        /* class texture1d : texture_base
         *
         * represents a 1D texture
         */
        class texture1d;

        /* class buffer_texture : texture_base
         *
         * wraps a buffer object in a 1D texture
         */
        class buffer_texture;

        /* class texture2d : texture_base
         *
         * represents a 2D texture
         */
        class texture2d;

        /*  class texture2d_array : texture_base
         *
         * represents an array of 2d textures
         */
        class texture2d_array;

        /* class texture2d_multisample : texture_multisample_base
         *
         * represents a multisample 2d texture
         */
        class texture2d_multisample;

        /* class cubemap : texture_base
         *
         * represents a cubemap texture
         */
        class cubemap;

        /* class cubemap_array : texture_base
         *
         * represents a cubemap array texture
         */
        class cubemap_array;
    }

    namespace exception
    {
        /* class texture_exception : exception
         *
         * exception relating to textures
         */
        class texture_exception;
    }
}



/* TEXTURE_EXCEPTION DEFINITION */

/* class texture_exception : exception
 *
 * for exceptions related to textures
 */
class glh::exception::texture_exception : public exception
{
public:

    /* full constructor
     *
     * __what: description of the exception
     */
    explicit texture_exception ( const std::string& __what )
        : exception { __what }
    {}

    /* default zero-parameter constructor
     *
     * construct texture_exception with no descrption
     */
    texture_exception () = default;

    /* default everything else and inherits what () function */

};



/* IMAGE DEFINITION */

/* class image
 *
 * importer for external images
 */
class glh::core::image
{
public:

    /* full constructor
     *
     * construct from path to import
     * 
     * _path: the path to the image
     * _v_flip: flag as to whether the image should be vertically flipped
     */
    explicit image ( const std::string& _path, const bool _v_flip = false );

    /* zero-parameter constructor */
    image ();

    /* copy constructor */
    image ( const image& other );

    /* default (more efficient) move constructor */
    image ( image&& other ) = default;

    /* copy assignment operator */
    image& operator= ( const image& other );

    /* default (more efficient) move assignment operator */
    image& operator= ( image&& other ) = default;

    /* default destructor */
    ~image () = default;



    /* vertical_flip
     *
     * flips the image vertically
     */
    void vertical_flip ();



    /* resize
     *
     * resize the image using linear interpolation
     * 
     * new_width/height: the new width and height of the image
     * 
     * return: the new interpolated image
     */
    void resize ( const unsigned new_width, const unsigned new_height );



    /* get_ptr
     *
     * get a pointer to the image data
     */
    void * get_ptr () { return image_data.get (); }
    const void * get_ptr () const { return image_data.get (); }

    /* get_path
     * 
     * get the path of the image
     */
    const std::string& get_path () const { return path; }

    /* get_width/height/channels
     *
     * get the width/height/no. of channels of the image
     */
    unsigned get_width () const { return width; }
    unsigned get_height () const { return height; }
    unsigned get_channels () const { return channels; }

    /* is_vertically_flipped
     *
     * returns true if the image is vertically flipped
     */
    bool is_vertically_flipped () const { return v_flip; }

    /* has_alpha
     *
     * true if channels == 2 or 4
     */
    bool has_alpha () const { return ( channels == 2 || channels == 4 ); }

    /* get_aspect
     *
     * get the aspect ratio of the image
     */
    double get_aspect () const { return static_cast<double> ( width ) / height; }



private:

    /* path to the image */
    std::string path;

    /* width, height and original number of channels of the image */
    int width;
    int height;
    int channels;

    /* whether the image has been vertically flipped */
    bool v_flip;

    /* the type of image_data */
    using image_data_type = std::unique_ptr<void, std::function<void ( void * )>>;

    /* shared pointer to the data of the image */
    image_data_type image_data;
    
};



/* TEXTURE_BASE DEFINITION */

/* class texture_base : object
 *
 * base class for all textures
 */
class glh::core::texture_base : public object
{
public:

    /* full constructor
     *
     * set everything to defaults
     */
    texture_base ();
    
    /* deleted copy constructor */
    texture_base ( const texture_base& other ) = delete;

    /* default move constructor */
    texture_base ( texture_base&& other ) = default;

    /* deleted copy constructor */
    texture_base& operator= ( const texture_base& other ) = delete;

    /* virtual destructor */
    virtual ~texture_base ();



    /* using bind methods from base class */
    using object::bind;
    using object::unbind;
    using object::is_bound;

    /* bind/unbind to texture units */
    bool bind ( const unsigned index ) const;
    bool unbind ( const unsigned index ) const;
    bool is_bound ( const unsigned index ) const;

    /* get the currently bound texture */
    static const object_pointer<texture_base>& get_bound_texture ( const unsigned index = 0 );



    /* bind_loop
     *
     * this will keep looping around texture units, disincluding 0, and making subsequent binds to the next unit
     * this avoids binding a texture to a unit already in use
     * 
     * returns the unit just bound to
     */
    unsigned bind_loop () const;

    /* bind_loop_next
     *
     * return the next index of the bind loop without binding
     */
    unsigned bind_loop_next () const { return bind_loop_index; }

    /* bind_loop_previous
     *
     * return the previous binding of the loop
     */
    unsigned bind_loop_previous () const;



    /* set_mag/min_filter
     *
     * set the texture filtering parameters of magnified/minified texture
     */
    virtual void set_mag_filter ( const GLenum opt );
    virtual void set_min_filter ( const GLenum opt );

    /* set_(s/t/r)_wrap
     *
     * set the wrapping options for each coordinate axis, or all at once
     */
    virtual void set_s_wrap ( const GLenum opt );
    virtual void set_t_wrap ( const GLenum opt );
    virtual void set_r_wrap ( const GLenum opt );
    virtual void set_wrap ( const GLenum opt );

    /* set_border_color
     *
     * set the color of the boarder, such that the texture can be clamped to the edge with a specific color
     */
    virtual void set_border_color ( const math::fvec4& color );

    /* set_compare_mode
     *
     * set the compare mode for the texture (GL_TEXTURE_COMPARE_MODE)
     */
    virtual void set_compare_mode ( const GLenum opt );

    /* set_compare_func
     *
     * set the comparison function for the texture
     */
    virtual void set_compare_func ( const GLenum opt );



    /* generate_mipmap
     *
     * generate texture mipmap
     */
    void generate_mipmap ();



protected:

    /* currently bound textures */
    static std::array<object_pointer<texture_base>, 80> bound_texture_indices;

    /* bind_loop_index
     *
     * the next unit to bind the texture to in the bind loop
     */
    static unsigned bind_loop_index;

};



/* TEXTURE_MULTISAMPLE_BASE DEFINITION */

/* class texture_multisample_base : texture_base
 *
 * base class for multisample textures
 * causes certain option-setting methods to throw
 */
class glh::core::texture_multisample_base : public texture_base
{
public:

    /* default zero-parameter constructor */
    texture_multisample_base () = default;
    
    /* deleted copy constructor */
    texture_multisample_base ( const texture_multisample_base& other ) = delete;

    /* default move constructor */
    texture_multisample_base ( texture_multisample_base&& other ) = default;

    /* deleted copy constructor */
    texture_multisample_base& operator= ( const texture_multisample_base& other ) = delete;

    /* virtual default destructor */
    virtual ~texture_multisample_base () = default;



    /* set_mag/min_filter
     * 
     * set the texture filtering parameters of magnified/minified texture
     */
    virtual void set_mag_filter ( const GLenum opt )
        { throw exception::texture_exception { "cannot set sampler state for multisample texture (set_mag_filter)" }; }
    virtual void set_min_filter ( const GLenum opt )
        { throw exception::texture_exception { "cannot set sampler state for multisample texture (set_min_filter)" }; }

    /* set_(s/t/r)_wrap
     *
     * set the wrapping options for each coordinate axis, or all at once
     */
    virtual void set_s_wrap ( const GLenum opt )
        { throw exception::texture_exception { "cannot set sampler state for multisample texture (set_s_wrap)" }; }
    virtual void set_t_wrap ( const GLenum opt )
        { throw exception::texture_exception { "cannot set sampler state for multisample texture (set_t_wrap)" }; }
    virtual void set_r_wrap ( const GLenum opt )
        { throw exception::texture_exception { "cannot set sampler state for multisample texture (set_r_wrap)" }; }
    virtual void set_wrap ( const GLenum opt )
        { throw exception::texture_exception { "cannot set sampler state for multisample texture (set_wrap)" }; }

    /* set_border_color
     *
     * set the color of the boarder, such that the texture can be clamped to the edge with a specific color
     */
    virtual void set_border_color ( const math::fvec4& color )
        { throw exception::texture_exception { "cannot set sampler state for multisample texture (set_border_color)" }; }

    /* set_compare_mode
     *
     * set the compare mode for the texture (GL_TEXTURE_COMPARE_MODE)
     */
    virtual void set_compare_mode ( const GLenum opt )
        { throw exception::texture_exception { "cannot set sampler state for multisample texture (set_compare_mode)" }; }

    /* set_compare_func
     *
     * set the comparison function for the texture
     */
    virtual void set_compare_func ( const GLenum opt )
        { throw exception::texture_exception { "cannot set sampler state for multisample texture (set_compare_func)" }; }
};



/* TEXTURE1D DEFINITION */

/* class texture1d : texture_base
*
* represents a 1D texture
*/
class glh::core::texture1d : public texture_base
{
public:

    /* tex_image constructor
     *
     * see tex_image for details
     */
    texture1d ( const unsigned _width, const GLenum _internal_format, const GLenum format = GL_RGBA, const GLenum type = GL_UNSIGNED_BYTE, const void * data = NULL )
        : is_immutable { false }
        { tex_image ( _width, _internal_format, format, type, data ); }


    /* zero-parameter constructor */
    texture1d ()
        : width { 0 }
        , internal_format { GL_NONE }
        , is_immutable { false }
    { bind (); }

    /* deleted copy constructor */
    texture1d ( const texture1d& other ) = delete;

    /* default move copy constructor */
    texture1d ( texture1d&& other ) = default;

    /* deleted assignment operator */
    texture1d& operator= ( const texture1d& other ) = delete;

    /* default destructor */
    ~texture1d () = default;



    /* default bind/unbind the texture */
    bool bind () const;
    bool unbind () const;
    bool is_bound () const { return bound_texture_indices.at ( 0 ) == this; }



    /* tex_storage
     *
     * set up the texture using immutable storage
     * 
     * _width: the width of the texture
     * _internal_format: the internal format of the texture
     * mipmap_levels: the number of mipmap levels to allocate (defaults to 0, which will allocate the maximum)
     */
    void tex_storage ( const unsigned _width, const GLenum _internal_format, const unsigned mipmap_levels = 0 );

    /* tex_image
     *
     * set up the texture based on provided parameters
     * 
     * _width: the width of the texture
     * _internal_format: the internal format of the texture
     * format/type: the format and component type of the input data for the texture (defaults to GL_RGBA and GL_UNSIGNED_BYTE)
     * data: the actual data to put into the texture (defaults to NULL)
     */
    void tex_image ( const unsigned _width, const GLenum _internal_format, const GLenum format = GL_RGBA, const GLenum type = GL_UNSIGNED_BYTE, const void * data = NULL );

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
    void tex_sub_image ( const unsigned x_offset, const unsigned _width, const GLenum format, const GLenum type, const void * data );
    


    /* get_width
     *
     * get the width of the texture
     */
    const unsigned& get_width () const { return width; }

    /* get_internal_format
     *
     * get the internal format of the texture
     */
    const GLenum& get_internal_format () const { return internal_format; }

private:

    /* width
     *
     * the width of the texture
     */
    unsigned width;

    /* internal_format
     * 
     * the format of the data stored in the texture
     */
    GLenum internal_format;

    /* is_immutable
     *
     * true if the texture has been set by tex_storage, and is now immutable
     */
    bool is_immutable;

};



/* BUFFER_TEXTURE DEFINITION */

/* class buffer_texture : texture_base
 *
 * wraps a buffer object in a 1D texture
 */
class glh::core::buffer_texture : public texture_base
{
public:

    /* zero-parameter constructor */
    buffer_texture () 
        : assoc_buffer { NULL }
        , internal_format { GL_NONE }
    { bind (); }



    /* default bind/unbind the texture */
    bool bind () const;
    bool unbind () const;
    bool is_bound () const { return bound_texture_indices.at ( 0 ) == this; }



    /* tex_buffer
     *
     * associates a buffer with the texture
     * 
     * buff: the new buffer to associate
     * _internal_format: the internal format of the buffer
     */
    void tex_buffer ( const buffer& buff, const GLenum _internal_format );



    /* get_size
     *
     * gets the size of the texture (i.e. the buffer)
     */
    unsigned get_size () const { return ( assoc_buffer ? assoc_buffer->get_size () : 0 ); }

    /* get_internal_format
     *
     * gets the internal format of the texture
     */
    const GLenum& get_internal_format () const { return internal_format; }

    /* get_assoc_buffer
     *
     * get the buffer associated with the texture
     */
    const const_object_pointer<buffer>& get_assoc_buffer () const { return assoc_buffer; }

private:

    /* the currently associated buffer */
    const_object_pointer<buffer> assoc_buffer;

    /* internal_format
     * 
     * the format of the data stored in the texture
     */
    GLenum internal_format;

};



/* TEXTURE2D DEFINITION */

/* class texture2d : texture_base
 *
 * represents a 2D texture
 */
class glh::core::texture2d : public texture_base
{
public:

    /* tex_image constructor
     *
     * see tex_image for details
     */
    texture2d ( const unsigned _width, const unsigned _height, const GLenum _internal_format, const GLenum format = GL_RGBA, const GLenum type = GL_UNSIGNED_BYTE, const void * data = NULL )
        : is_immutable { false }
        { tex_image ( _width, _height, _internal_format, format, type, data ); }
    explicit texture2d ( const image& _image, const bool use_srgb = false )
        : is_immutable { false }
        { tex_image ( _image, use_srgb ); }


    /* zero-parameter constructor */
    texture2d ()
        : width { 0 }, height { 0 }
        , internal_format { GL_NONE }
        , has_alpha_component { false }
        , is_immutable { false }
    { bind (); }

    /* deleted copy constructor */
    texture2d ( const texture2d& other ) = delete;

    /* default move copy constructor */
    texture2d ( texture2d&& other ) = default;

    /* deleted assignment operator */
    texture2d& operator= ( const texture2d& other ) = delete;

    /* default destructor */
    ~texture2d () = default;



    /* default bind/unbind the texture */
    bool bind () const;
    bool unbind () const;
    bool is_bound () const { return bound_texture_indices.at ( 0 ) == this; }



    /* tex_storage
     *
     * set up the texture using immutable storage
     * 
     * _width/_height: the width and height of the texture
     * _internal_format: the internal format of the texture
     * mipmap_levels: the number of mipmap levels to allocate (defaults to 0, which will allocate the maximum)
     */
    void tex_storage ( const unsigned _width, const unsigned _height, const GLenum _internal_format, const unsigned mipmap_levels = 0 );

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
    void tex_image ( const unsigned _width, const unsigned _height, const GLenum _internal_format, const GLenum format = GL_RGBA, const GLenum type = GL_UNSIGNED_BYTE, const void * data = NULL );
    void tex_image ( const image& _image, const bool use_srgb = false );

    /* tex_sub_image
     *
     * substitiute data into the texture
     * changes are made to mipmap level 0, so remember to regenerate mipmaps if necessary
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
    void tex_sub_image ( const unsigned x_offset, const unsigned y_offset, const unsigned _width, const unsigned _height, const GLenum format, const GLenum type, const void * data );
    void tex_sub_image ( const unsigned x_offset, const unsigned y_offset, const image& _image );



    /* get_width/height
     *
     * get the width and height of the texture
     */
    const unsigned& get_width () const { return width; }
    const unsigned& get_height () const { return height; }

    /* get_internal_format
     *
     * get the internal format of the texture
     */
    const GLenum& get_internal_format () const { return internal_format; }

    /* has_alpha
     *
     * returns has_alpha_component
     */
    bool has_alpha () const { return has_alpha_component; }



private:

    /* width/height
     *
     * the width and height of the texture
     */
    unsigned width;
    unsigned height;

    /* internal_format
     * 
     * the format of the data stored in the texture
     */
    GLenum internal_format;

    /* has_alpha_component
     *
     * always true, unless created from an imported image without alpha components
     */
    bool has_alpha_component;

    /* is_immutable
     *
     * true if the texture has been set by tex_storage, and is now immutable
     */
    bool is_immutable;

};



/* TEXTURE2D_ARRAY DEFINITION */

/*  class texture2d_array : texture_base
 *
 * represents an array of 2d textures
 */
class glh::core::texture2d_array : public texture_base
{
public:

    /* tex_image constructor
     *
     * see tex_image for details
     */
    texture2d_array ( const unsigned _width, const unsigned _height, const unsigned _depth, const GLenum _internal_format, const GLenum format = GL_RGBA, const GLenum type = GL_UNSIGNED_BYTE, const void * data = NULL )
        : is_immutable { false }
        { tex_image ( _width, _height, _depth, _internal_format, format, type, data ); }
    explicit texture2d_array ( std::initializer_list<image> images, const bool use_srgb = false )
        : is_immutable { false }
        { tex_image ( images, use_srgb ); }

    /* zero=parameter constructor */
    texture2d_array ()
        : width { 0 }, height { 0 }, depth { 0 }
        , internal_format { GL_NONE }
        , is_immutable { false }
    { bind (); }

    /* deleted copy constructor */
    texture2d_array ( const texture2d_array& other ) = delete;

    /* default move constructor */
    texture2d_array ( texture2d_array&& other ) = default;

    /* deleted copy assignment operator */
    texture2d_array& operator= ( const texture2d_array& other ) = delete;

    /* default destructor */
    ~texture2d_array () = default;



    /* default bind/unbind the texture */
    bool bind () const;
    bool unbind () const;
    bool is_bound () const { return bound_texture_indices.at ( 0 ) == this; }



    /* tex_storage
     *
     * set up the texture using immutable storage
     * 
     * _width/_height/_depth: the width, height and number of textures in the array
     * _internal_format: the internal format of the texture
     * mipmap_levels: the number of mipmap levels to allocate (defaults to 0, which will allocate the maximum)
     */
    void tex_storage ( const unsigned _width, const unsigned _height, const unsigned _depth, const GLenum _internal_format, const unsigned mipmap_levels = 0 );

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
    void tex_image ( const unsigned _width, const unsigned _height, const unsigned _depth, const GLenum _internal_format, const GLenum format = GL_RGBA, const GLenum type = GL_UNSIGNED_BYTE, const void * data = NULL );
    void tex_image ( std::initializer_list<image> images, const bool use_srgb = false );



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
    void tex_sub_image ( const unsigned x_offset, const unsigned y_offset, const unsigned z_offset, const unsigned _width, const unsigned _height, const unsigned _depth, const GLenum format, const GLenum type, const void * data );
    void tex_sub_image ( const unsigned x_offset, const unsigned y_offset, const unsigned z_offset, std::initializer_list<image> images );



    /* get_width/height/depth
     *
     * get the width, height and depth of the texture
     */
    const unsigned& get_width () const { return width; }
    const unsigned& get_height () const { return height; }
    const unsigned& get_depth () const { return depth; }

    /* get_internal_format
     *
     * get the internal format of the texture
     */
    const GLenum& get_internal_format () const { return internal_format; }



private:

    /* width, height, depth
     *
     * the width height of each texture and the number of textures
     */
    unsigned width;
    unsigned height;
    unsigned depth;

    /* internal_format
     * 
     * the format of the data stored in the texture
     */
    GLenum internal_format;

    /* is_immutable
     *
     * true if the texture has been set by tex_storage, and is now immutable
     */
    bool is_immutable;
    
};



/* TEXTURE2D_MULTISAMPLE DEFINITION */

/* class texture2d_multisample : texture_multisample_base
 *
 * representsb a multisample 2D texture
 */
class glh::core::texture2d_multisample : public texture_multisample_base
{
public:

    /* tex_image constructor
     *
     * see tex_image for details
     */
    texture2d_multisample ( const unsigned _width, const unsigned _height, const unsigned _samples, const GLenum _internal_format, const bool _fixed_sample_locations = GL_TRUE )
        : is_immutable { false }
    { tex_image ( _width, _height, _samples, _internal_format, _fixed_sample_locations ); }

    /* zero-parameter constructor */
    texture2d_multisample ()
        : width { 0 }, height { 0 }
        , samples { 0 }
        , internal_format { GL_NONE }
        , fixed_sample_locations { GL_TRUE }
        , is_immutable { false }
    { bind (); }

    /* deleted copy constructor */
    texture2d_multisample ( const texture2d_multisample& other ) = delete;

    /* default move constructor */
    texture2d_multisample ( texture2d_multisample&& other ) = default;

    /* default destructor */
    ~texture2d_multisample () = default;



    /* default bind/unbind the texture */
    bool bind () const;
    bool unbind () const;
    bool is_bound () const { return bound_texture_indices.at ( 0 ) == this; }



    /* tex_storage
     *
     * set up the texture using immutable storage
     * 
     * _width/_height: the width and height of the texture
     * _samples: the number of samples
     * _internal_format: the internal format of the texture
     * _fixed_sample_locations: whether to use fixed sample locations (defaults to true)
     */
    void tex_storage ( const unsigned _width, const unsigned _height, const unsigned _samples, const GLenum _internal_format, const bool _fixed_sample_locations = GL_TRUE );

    /* tex_image
     *
     * set up the 2d multisample texture
     * 
     * _width/_height: the width and height of the texture
     * _samples: the number of samples
     * _internal_format: the internal texture format
     * _fixed_sample_locations: whether to use fixed sample locations (defaults to true)
     */
    void tex_image ( const unsigned _width, const unsigned _height, const unsigned _samples, const GLenum _internal_format, const bool _fixed_sample_locations = GL_TRUE );

    

    /* get_width/height
     *
     * get the width and height of the texture
     */
    const unsigned& get_width () const { return width; }
    const unsigned& get_height () const { return height; }

    /* get_internal_format
     *
     * get the internal format of the texture
     */
    const GLenum& get_internal_format () const { return internal_format; }

    /* get_samples
     * 
     * get the number of samples the texture has
     */
    const unsigned& get_samples () const { return samples; }



private:

    /* width/height
     *
     * the width and height of the texture
     */
    unsigned width;
    unsigned height;

    /* number of samples of the texture */
    unsigned samples;

    /* internal_format
     * 
     * the format of the data stored in the texture
     */
    GLenum internal_format;

    /* whether fixed sample locations are being used */
    bool fixed_sample_locations;

    /* is_immutable
     *
     * true if the texture has been set by tex_storage, and is now immutable
     */
    bool is_immutable;

};



/* CUBEMAP DEFINITION */

/* class cubemap : texture_base
 *
 * represents a cubemap texture
 */
class glh::core::cubemap : public texture_base
{
public:

    /* tex_image constructor
     *
     * see tex_image for details
     */
    cubemap ( const unsigned _width, const unsigned _height, const GLenum _internal_format, const GLenum format = GL_RGBA, const GLenum type = GL_UNSIGNED_BYTE, const void * data = NULL )
        : is_immutable { false }
        { tex_image ( _width, _height, _internal_format, format, type, data ); }
    explicit cubemap ( const image& _image, const bool use_srgb = false )
        : is_immutable { false }
        { tex_image ( _image, use_srgb ); }
    explicit cubemap ( std::initializer_list<image> images, const bool use_srgb = false )
        : is_immutable { false }
        { tex_image ( images, use_srgb ); }

    /* zero-parameter constructor */
    cubemap ()
        : width { 0 }, height { 0 }
        , internal_format { GL_NONE }
        , is_immutable { false }
    { bind (); }

    /* deleted copy constructor */
    cubemap ( const cubemap& other ) = delete;

    /* default move constructor */
    cubemap ( cubemap&& other ) = default;

    /* deleted copy assignment operator */
    cubemap& operator= ( const cubemap& other ) = delete;

    /* default destructor */
    ~cubemap () = default;



    /* default bind/unbind the texture */
    bool bind () const;
    bool unbind () const;
    bool is_bound () const { return bound_texture_indices.at ( 0 ) == this; }



    /* tex_storage
     *
     * set up the texture using immutable storage
     * 
     * _width/_height: the width and height of the texture
     * _internal_format: the internal format of the texture
     * mipmap_levels: the number of mipmap levels to allocate (defaults to 0, which will allocate the maximum)
     */
    void tex_storage ( const unsigned _width, const unsigned _height, const GLenum _internal_format, const unsigned mipmap_levels = 0 );

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
    void tex_image ( const unsigned _width, const unsigned _height, const GLenum _internal_format, const GLenum format = GL_RGBA, const GLenum type = GL_UNSIGNED_BYTE, const void * data = NULL );
    void tex_image ( const image& _image, const bool use_srgb = false );
    void tex_image ( std::initializer_list<image> images, const bool use_srgb = false );

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
    void tex_sub_image ( const unsigned x_offset, const unsigned y_offset, const unsigned _width, const unsigned _height, const GLenum format, const GLenum type, const void * data );
    void tex_sub_image ( const unsigned face, const unsigned x_offset, const unsigned y_offset, const unsigned _width, const unsigned _height, const GLenum format, const GLenum type, const void * data );
    void tex_sub_image ( const unsigned x_offset, const unsigned y_offset, const image& _image );
    void tex_sub_image ( const unsigned face, const unsigned x_offset, const unsigned y_offset, const image& _image );
    void tex_sub_image ( const unsigned x_offset, const unsigned y_offset, std::initializer_list<image> images );



    /* get_width/height
     *
     * get the width and height of the texture
     */
    const unsigned& get_width () const { return width; }
    const unsigned& get_height () const { return height; }

    /* get_internal_format
     *
     * get the internal format of the texture
     */
    const GLenum& get_internal_format () const { return internal_format; }



private:

    /* width/height
     *
     * the width and height of the texture
     */
    unsigned width;
    unsigned height;

    /* internal_format
     * 
     * the format of the data stored in the texture
     */
    GLenum internal_format;

    /* is_immutable
     *
     * true if the texture has been set by tex_storage, and is now immutable
     */
    bool is_immutable;

};



/* CUBEMAP_ARRAY DEFINITION */

/* class cubemap_array : texture_base
 *
 * represents a cubemap array texture
 */
class glh::core::cubemap_array : public texture_base
{
public:

    /* tex_image constructor
     *
     * see tex_image for details
     */
    cubemap_array ( const unsigned _width, const unsigned _height, const unsigned _depth, const GLenum _internal_format, const GLenum format = GL_RGBA, const GLenum type = GL_UNSIGNED_BYTE, const void * data = NULL )
        : is_immutable { false }
        { tex_image ( _width, _height, _depth, _internal_format, format, type, data ); }
    explicit cubemap_array ( std::initializer_list<image> images, const bool use_srgb = false )
        : is_immutable { false }
        { tex_image ( images, use_srgb ); }

    /* zero-parameter constructor */
    cubemap_array ()
        : width { 0 }, height { 0 }, depth { 0 }
        , internal_format { GL_NONE }
        , is_immutable { false }
    { bind (); }

    /* deleted copy constructor */
    cubemap_array ( const cubemap_array& other ) = delete;

    /* default move constructor */
    cubemap_array ( cubemap_array&& other ) = default;

    /* deleted copy assignment operator */
    cubemap_array& operator= ( const cubemap_array& other ) = delete;

    /* default destructor */
    ~cubemap_array () = default;



    /* default bind/unbind the texture */
    bool bind () const;
    bool unbind () const;
    bool is_bound () const { return bound_texture_indices.at ( 0 ) == this; }



    /* tex_storage
     *
     * set up the cubemap array with immutable storage
     *
     * _width/_height/_depth: the width and height of each face and the number of layer-faces in the cubemap array (so must be multiple of six)
     * _internal_format: the internal format of the cubemap array
     * mipmap_levels: the number of mipmap levels to allocate (defaults to 0, which will allocate the maximum)
     */
    void tex_storage ( const unsigned _width, const unsigned _height, const unsigned _depth, const GLenum _internal_format, const unsigned mipmap_levels = 0 );

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
    void tex_image ( const unsigned _width, const unsigned _height, const unsigned _depth, const GLenum _internal_format, const GLenum format = GL_RGBA, const GLenum type = GL_UNSIGNED_BYTE, const void * data = NULL );
    void tex_image ( std::initializer_list<image> images, const bool use_srgb = false );

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
    void tex_sub_image ( const unsigned x_offset, const unsigned y_offset, const unsigned z_offset, const unsigned _width, const unsigned _height, const unsigned _depth, const GLenum format, const GLenum type, const void * data );
    void tex_sub_image ( const unsigned x_offset, const unsigned y_offset, const unsigned z_offset, std::initializer_list<image> images );



    /* get_width/height/depth
     *
     * get the width, height and depth of the texture
     */
    const unsigned& get_width () const { return width; }
    const unsigned& get_height () const { return height; }
    const unsigned& get_depth () const { return depth; }

    /* get_internal_format
     *
     * get the internal format of the texture
     */
    const GLenum& get_internal_format () const { return internal_format; }



private:

    /* width/height/depth
     *
     * the width and height and number of levels of the texture
     */
    unsigned width;
    unsigned height;
    unsigned depth;

    /* internal_format
     * 
     * the format of the data stored in the texture
     */
    GLenum internal_format;

    /* is_immutable
     *
     * true if the texture has been set by tex_storage, and is now immutable
     */
    bool is_immutable;

};



/* #ifndef GLHELPER_TEXTURE_HPP_INCLUDED */
#endif