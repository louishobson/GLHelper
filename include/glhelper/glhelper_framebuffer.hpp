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
     * _samples: the number of samples the renderbuffer should contain (defaults to 1)
     */
    rbo ( const unsigned _width, const unsigned _height, const GLenum _format, const unsigned _samples = 1 );

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



    /* get_bound_object_pointer
     *
     * produce a pointer to the rbo currently bound
     * NULL is returned if no object is bound to the bind point
     */
    using object::get_bound_object_pointer;
    static rbo * get_bound_object_pointer () { return dynamic_cast<rbo *> ( get_bound_object_pointer ( object_bind_target::GLH_RBO_TARGET ) ); }



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
    fbo ()
        : object { minor_object_type::GLH_FBO_TYPE }
    {}

    /* deleted copy constructor */
    fbo ( const fbo& other ) = delete;

    /* default move constructor */
    fbo ( fbo&& other ) = default;

    /* deleted copy assignment operator */
    fbo& operator= ( const fbo& other ) = delete;

    /* default destructor */
    ~fbo () = default;



    /* get_bound_object_pointer
     *
     * produce a pointer to the fbo currently bound
     * NULL is returned if no object is bound to the bind point
     */
    using object::get_bound_object_pointer;
    static fbo * get_bound_object_pointer () { return dynamic_cast<fbo *> ( get_bound_object_pointer ( object_bind_target::GLH_FBO_TARGET ) ); }



    /* bind_read/draw
     *
     * bind to the read/draw targets
     * 
     * returns true if a change in binding occured
     */
    bool bind_read () const { return bind ( object_bind_target::GLH_READ_FBO_TARGET ); }
    bool bind_draw () const { return bind ( object_bind_target::GLH_DRAW_FBO_TARGET ); }

    /* unbind_read/draw
     *
     * unbind from the read/draw targets
     * 
     * returns true if a change in binding occured
     */
    bool unbind_read () const { return unbind ( object_bind_target::GLH_READ_FBO_TARGET ); }
    bool unbind_draw () const { return unbind ( object_bind_target::GLH_DRAW_FBO_TARGET ); }

    /* is_read/draw_bound
     *
     * return true if is bound to read/draw target
     */
    bool is_read_bound () const { return is_bound ( object_bind_target::GLH_READ_FBO_TARGET ); }
    bool is_draw_bound () const { return is_bound ( object_bind_target::GLH_DRAW_FBO_TARGET ); }

    /* unbind_all
     *
     * unbind from all bind points, including read and sraw targets
     * 
     * return true if a change in binding occured
     */
    bool unbind_all () const { return ( unbind () | unbind_read () | unbind_draw () ); }



    /* attach_texture2d
     *
     * add a texture2d attachment
     *
     * texture: the texture to attach
     * attachment: which attachment the texture should be used as
     * mipmap: the mipmap level to attach (defaults to 0)
     */
    void attach_texture2d ( const texture2d& texture, const GLenum attachment, GLint mipmap = 0 );
    void attach_texture2d ( const texture2d_multisample& texture, const GLenum attachment, GLint mipmap = 0 );

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

};



/* #ifndef GLHELPER_FRAMEBUFFER_HPP_INCLUDED */
#endif