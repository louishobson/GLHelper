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

/* include math */
#include <glhelper/glhelper_math.hpp>

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

        /* class texture2d_multisample : texture_base
         *
         * represents a multisample 2d texture
         */
        class texture2d_multisample;

        /* class cubemap : texture_base
         *
         * represents a cubemap texture
         */
        class cubemap;

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
     * 
     * _minor_type: the type of the texture
     */
    texture_base ( const minor_object_type _minor_type )
        : object { _minor_type }
    {}
    
    /* deleted copy constructor */
    texture_base ( const texture_base& other ) = delete;

    /* default move constructor */
    texture_base ( texture_base&& other ) = default;

    /* deleted copy constructor */
    texture_base& operator= ( const texture_base& other ) = delete;

    /* default virtual destructor */
    virtual ~texture_base () = default;



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

    /* set_border_color
     *
     * set the color of the boarder, such that the texture can be clamped to the edge with a specific color
     */
    void set_border_color ( const math::fvec3& color );

    /* generate_mipmap
     *
     * generate texture mipmap
     */
    void generate_mipmap ();



    /* bind/unbind
     *
     * bind the texture to a texture unit
     * 
     * unit: the texture unit to bind to/unbind from
     */
    using object::bind;
    bool bind ( const unsigned texture_unit ) const { return bind ( static_cast<object_bind_target> ( static_cast<unsigned> ( bind_target ) + texture_unit ) ); }

    /* unbind
     *
     * unbind the texture from a texture unit
     * 
     * uniy: the texture unit to unbind from
     */
    using object::unbind;
    bool unbind ( const unsigned texture_unit ) const { return unbind ( static_cast<object_bind_target> ( static_cast<unsigned> ( bind_target ) + texture_unit ) ); }

    /* unbind_all
     *
     * unbind from all targets
     * this includes all texture units
     */
    bool unbind_all () const;

    /* is_bound
     *
     * check if is bound to a texture unit
     * 
     * unit: the texture unit to check if it is bound to
     * 
     * return: boolean for if the texture is bound to the unit supplied
     */
    using object::is_bound;
    bool is_bound ( const unsigned texture_unit ) const { return is_bound ( static_cast<object_bind_target> ( static_cast<unsigned> ( bind_target ) + texture_unit ) ); }



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



protected:

    /* bind_loop_index
     *
     * the next unit to bind the texture to in the bind loop
     */
    static unsigned bind_loop_index;

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
        : texture_base { minor_object_type::GLH_TEXTURE2D_TYPE }
        , is_immutable { false }
        { tex_image ( _width, _height, _internal_format, format, type, data ); }
    explicit texture2d ( const image& _image, const bool use_srgb = false )
        : texture_base { minor_object_type::GLH_TEXTURE2D_TYPE }
        , is_immutable { false }
        { tex_image ( _image, use_srgb ); }


    /* zero-parameter constructor */
    texture2d ()
        : texture_base { minor_object_type::GLH_TEXTURE2D_TYPE }
        , width { 0 }, height { 0 }
        , internal_format { GL_NONE }
        , has_alpha_component { false }
        , is_immutable { false }
    {}

    /* deleted copy constructor */
    texture2d ( const texture2d& other ) = delete;

    /* default move copy constructor */
    texture2d ( texture2d&& other ) = default;

    /* deleted assignment operator */
    texture2d& operator= ( const texture2d& other ) = delete;

    /* default destructor */
    ~texture2d () = default;



    /* get_bound_object_pointer
     *
     * produce a pointer to the texture2d currently bound to a unit
     */
    using object::get_bound_object_pointer;
    static object_pointer<texture2d> get_bound_object_pointer ( const unsigned unit = 0 )
    { return get_bound_object_pointer<texture2d> ( static_cast<object_bind_target> ( static_cast<unsigned> ( object_bind_target::GLH_TEXTURE2D_0_TARGET ) + unit ) ); }



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
        : texture_base { minor_object_type::GLH_TEXTURE2D_ARRAY_TYPE }
        , is_immutable { false }
        { tex_image ( _width, _height, _depth, _internal_format, format, type, data ); }
    explicit texture2d_array ( std::initializer_list<image> images, const bool use_srgb = false )
        : texture_base { minor_object_type::GLH_TEXTURE2D_ARRAY_TYPE }
        , is_immutable { false }
        { tex_image ( images, use_srgb ); }

    /* zero=parameter constructor */
    texture2d_array ()
        : texture_base { minor_object_type::GLH_TEXTURE2D_ARRAY_TYPE }
        , width { 0 }, height { 0 }, depth { 0 }
        , internal_format { GL_NONE }
        , is_immutable { false }
    {}

    /* deleted copy constructor */
    texture2d_array ( const texture2d_array& other ) = delete;

    /* default move constructor */
    texture2d_array ( texture2d_array&& other ) = default;

    /* deleted copy assignment operator */
    texture2d_array& operator= ( const texture2d_array& other ) = delete;

    /* default destructor */
    ~texture2d_array () = default;



    /* get_bound_object_pointer
     *
     * produce a pointer to the texture2d_array currently bound to a unit
     */
    using object::get_bound_object_pointer;
    static object_pointer<texture2d_array> get_bound_object_pointer ( const unsigned unit = 0 )
    { return get_bound_object_pointer<texture2d_array> ( static_cast<object_bind_target> ( static_cast<unsigned> ( object_bind_target::GLH_TEXTURE2D_ARRAY_0_TARGET ) + unit ) ); }



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
     * images: an initialiser list of images to form the array from
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
     * images: an initialiser list of images to substitute into the array
     */
    void tex_sub_image ( const unsigned x_offset, const unsigned y_offset, const unsigned z_offset, const unsigned _width, const unsigned _height, const unsigned _depth, const GLenum format, const GLenum type, const void * data );
    void tex_sub_image ( const unsigned x_offset, const unsigned y_offset, const unsigned z_offset, std::initializer_list<image> images );



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

/* class texture2d_multisample : texture_base
 *
 * representsb a multisample 2D texture
 */
class glh::core::texture2d_multisample : public texture_base
{
public:

    /* tex_image constructor
     *
     * see tex_image for details
     */
    texture2d_multisample ( const unsigned _width, const unsigned _height, const unsigned _samples, const GLenum _internal_format, const bool _fixed_sample_locations = GL_TRUE )
        : texture_base { minor_object_type::GLH_TEXTURE2D_MULTISAMPLE_TYPE }
        , is_immutable { false }
    { tex_image ( _width, _height, _samples, _internal_format, _fixed_sample_locations ); }

    /* zero-parameter constructor */
    texture2d_multisample ()
        : texture_base { minor_object_type::GLH_TEXTURE2D_MULTISAMPLE_TYPE }
        , width { 0 }, height { 0 }
        , samples { 0 }
        , internal_format { GL_NONE }
        , fixed_sample_locations { GL_TRUE }
        , is_immutable { false }
    {}

    /* deleted copy constructor */
    texture2d_multisample ( const texture2d_multisample& other ) = delete;

    /* default move constructor */
    texture2d_multisample ( texture2d_multisample&& other ) = default;

    /* default destructor */
    ~texture2d_multisample () = default;



    /* get_bound_object_pointer
     *
     * produce a pointer to the texture2d_multisample currently bound to a unit
     */
    using object::get_bound_object_pointer;
    static object_pointer<texture2d_multisample> get_bound_object_pointer ( const unsigned unit = 0 )
    { return get_bound_object_pointer<texture2d_multisample> ( static_cast<object_bind_target> ( static_cast<unsigned> ( object_bind_target::GLH_TEXTURE2D_MULTISAMPLE_0_TARGET ) + unit ) ); }



    /* tex_storage
     *
     * set up the texture using immutable storage
     * 
     * _width/_height: the width and height of the texture
     * _samples: the number of samples
     * _internal_format: the internal format of the texture
     * _fixed_sample_locations: whether to use fixed sample locations (defaults to true)
     */
    void tex_storage ( const unsigned _width, const unsigned _height, const unsigned _samples, const GLenum _internal_format, const bool _fixed_sample_locations );

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
        : texture_base { minor_object_type::GLH_CUBEMAP_TYPE }
        , is_immutable { false }
        { tex_image ( _width, _height, _internal_format, format, type, data ); }
    explicit cubemap ( const image& _image, const bool use_srgb = false )
        : texture_base { minor_object_type::GLH_CUBEMAP_TYPE }
        , is_immutable { false }
        { tex_image ( _image, use_srgb ); }
    explicit cubemap ( std::initializer_list<image> images, const bool use_srgb = false )
        : texture_base { minor_object_type::GLH_CUBEMAP_TYPE }
        , is_immutable { false }
        { tex_image ( images, use_srgb ); }

    /* zero-parameter constructor */
    cubemap ()
        : texture_base { minor_object_type::GLH_CUBEMAP_TYPE }
        , width { 0 }, height { 0 }
        , internal_format { GL_NONE }
        , is_immutable { false }
    {}

    /* deleted copy constructor */
    cubemap ( const cubemap& other ) = delete;

    /* default move constructor */
    cubemap ( cubemap&& other ) = default;

    /* deleted copy assignment operator */
    cubemap& operator= ( const cubemap& other ) = delete;



    /* get_bound_object_pointer
     *
     * produce a pointer to the cubemap currently bound to a unit
     */
    using object::get_bound_object_pointer;
    static object_pointer<cubemap> get_bound_object_pointer ( const unsigned unit = 0 )
    { return get_bound_object_pointer<cubemap> ( static_cast<object_bind_target> ( static_cast<unsigned> ( object_bind_target::GLH_CUBEMAP_0_TARGET ) + unit ) ); }



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
     * images: initialiser list of images that must be six elements large
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
     * images: initialiser list of images that must be six elements large
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



/* #ifndef GLHELPER_TEXTURE_HPP_INCLUDED */
#endif