/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * include/glhelper/glhelper_framebuffer.hpp
 * 
 * declares framebuffer handling constructs
 * notable constructs include:
 * 
 * 
 * 
 * CLASS GLH::CORE::RBO
 * 
 * renderbuffer class for use within a framebuffer
 * no more complicated than that
 * 
 * 
 * 
 * CLASS GLH::CORE::FBO
 * 
 * framebuffer class
 * can attach renderbuffers and textures through the attach_rbo and attach_texture2d methods
 * one can check if the fbo is complete through the is_complete method
 * 
 */



/* HEADER GUARD */
#ifndef GLHELPER_FRAMEBUFFER_HPP_INCLUDED
#define GLHELPER_FRAMEBUFFER_HPP_INCLUDED



/* INCLUDES */

/* include core headers */
#include <algorithm>
#include <array>
#include <iostream>
#include <string>

/* include glhelper_core.hpp */
#include <glhelper/glhelper_core.hpp>

/* include glhelper_texture.hpp */
#include <glhelper/glhelper_texture.hpp>



/* NAMESPACE DECLARATIONS */

namespace glh
{
    namespace core
    {
        /* class rbo : object
         *
         * renderbuffer class
         */
        class rbo;

        /* class fbo : object
         *
         * framebuffer class
         */
        class fbo;
    }
}



/* RBO DEFINITION */

/* class rbo : object
 *
 * renderbuffer class
 */
class glh::core::rbo : public object
{
public:

    /* full constructor
     *
     * construct a renderbuffer with a given size and storage type
     * 
     * _width/_height: the width/height of the renderbuffer
     * _format: the storage format of the renderbuffer
     * _samples: the number of samples the renderbuffer should contain (defaults to 0, meaning no multisampling)
     */
    rbo ( const unsigned _width, const unsigned _height, const GLenum _format, const unsigned _samples = 0 );

    /* deleted zero-parameter constructor */
    rbo () = delete;

    /* deleted copy constructor */
    rbo ( const rbo& other ) = delete;

    /* default move constructor */
    rbo ( rbo&& other ) = default;

    /* deleted copy assignment operator */
    rbo& operator= ( const rbo& other ) = delete;

    /* default estructor */
    ~rbo () = default;



    /* default bind/unbind the rbo */
    bool bind () const;
    bool unbind () const;
    bool is_bound () const { return bound_rbo == this; }

    /* get the currently bound rbo */
    static const object_pointer<rbo>& get_bound_rbo () { return bound_rbo; }



    /* get_width/height
     *
     * get the width and height of the texture
     */
    const unsigned& get_width () const { return width; }
    const unsigned& get_height () const { return height; }

    /* get_samples
     * 
     * get the number of samples the texture has
     */
    const unsigned& get_samples () const { return samples; }

private:

    /* the currently bound rbo */
    static object_pointer<rbo> bound_rbo;

    /* width and height of the rbo */
    const unsigned width;
    const unsigned height;

    /* the format of the data in the rbo */
    const GLenum format;

    /* the number of samples the rbo contains */
    const unsigned samples;

};



/* FBO DEFINITION */

/* class fbo : object
 *
 * framebuffer class
 */
class glh::core::fbo : public object
{
public: 

    /* zero-parameter constructor
     *
     * construct empty framebuffer
     */
    fbo ();

    /* deleted copy constructor */
    fbo ( const fbo& other ) = delete;

    /* default move constructor */
    fbo ( fbo&& other ) = default;

    /* deleted copy assignment operator */
    fbo& operator= ( const fbo& other ) = delete;

    /* default destructor */
    ~fbo () = default;



    /* default bind/unbind the fbo
     *
     * equivalent to binding/unbinding from read and draw targets simultaneously
     */
    bool bind () const;
    bool unbind () const;
    bool is_bound () const { return bound_read_fbo == this && bound_draw_fbo == this; }

    /* bind to read/draw targets separately */
    bool bind_read () const;
    bool bind_draw () const;

    /* unbind from read/draw targets */
    bool unbind_read () const;
    bool unbind_draw () const;

    /* get if bound to read or draw targets */
    bool is_read_bound () const { return bound_read_fbo == this; }
    bool is_draw_bound () const { return bound_draw_fbo == this; }

    /* bind the default framebuffer set up by the window manager */
    static bool bind_default_framebuffer ();
    static bool is_default_framebuffer_bound ();

    /* get the bound read/draw fbos */
    static const object_pointer<fbo>& get_bound_read_fbo () { return bound_read_fbo; }
    static const object_pointer<fbo>& get_bound_draw_fbo () { return bound_draw_fbo; }



    /* attach_texture
     *
     * add a texture attachment
     *
     * texture: the texture to attach
     * attachment: which attachment the texture should be used as
     * mipmap: the mipmap level to attach (defaults to 0)
     */
    void attach_texture ( const texture_base& texture, const GLenum attachment, const GLint mipmap = 0 );

    /* attach_rbo
     *
     * add a renderbuffer attachment
     *
     * _rbo: the renderbuffer to attach
     * attachment: which attachment the rbo should be used as
     */
    void attach_rbo ( const rbo& _rbo, const GLenum attachment );

    

    /* is_complete
     *
     * return true if the framebuffer is complete
     */
    bool is_complete () const;



    /* draw/read_buffer
     *
     * sets which buffer to use as the draw/read buffer
     * 
     * buff: GLenum for what buffer to use
     */
    void draw_buffer ( const GLenum buff );
    void read_buffer ( const GLenum buff );

    /* draw_buffers
     *
     * sets multiple buffers to be used as the read/draw buffer
     * 
     * buffs...: a list of GLenums for which buffers to use
     */
    template<class ...Ts> void draw_buffers ( const Ts... buffs ); 



    /* blit_copy
     *
     * copy a region FROM ANOTHER FBO INTO THIS FBO
     * 
     * other: the other fbo to copy from
     * srcx0, srcy0, srcx1, srcy1: the x and y positions in the other fbo to read from
     * dstx0, dsty0, dstx1, dsty1: the x and y positions in this fbo to write to
     * copy_mask: mask for which buffers to copy
     * filter: the interpolation settings for any stretches applied
     */
    void blit_copy ( const fbo& other, const unsigned srcx0, const unsigned srcy0, const unsigned srcx1, const unsigned srcy1, 
                     const unsigned dstx0, const unsigned dsty0, const unsigned dstx1, const unsigned dsty1, const GLbitfield copy_mask, const GLenum filter );



private:

    /* bound read/draw fbos */
    static object_pointer<fbo> bound_read_fbo;
    static object_pointer<fbo> bound_draw_fbo;

};



/* FBO TEMPLATE METHOD IMPLEMENTATIONS */

/* draw_buffers
 *
 * sets multiple buffers to be used as the read/draw buffer
 * 
 * buffs...: a list of GLenums for which buffers to use
 */
template<class ...Ts> void glh::core::fbo::draw_buffers ( const Ts... buffs )
{
    /* create array of buffs */
    std::array<const GLenum, sizeof...( Ts )> buff_array { buffs... };

    /* bind, set options, unbind */
    const bool binding_change = bind_draw ();
    glDrawBuffers ( sizeof...( Ts ), buff_array.data () );
    if ( binding_change ) unbind_draw ();
}



/* #ifndef GLHELPER_FRAMEBUFFER_HPP_INCLUDED */
#endif