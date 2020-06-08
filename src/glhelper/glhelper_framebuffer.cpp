/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * src/glhelper/glhelper_framebuffer.cpp
 * 
 * implementation ofinclude/glhelper/glhelper_framebuffer.hpp
 * 
 */



/* INCLUDES */

/* include glhelper_framebuffer.hpp */
#include <glhelper/glhelper_framebuffer.hpp>



/* RBO IMPLEMENTATION */

/* full constructor
 *
 * construct a renderbuffer with a given size and storage type
 * 
 * _width/_height: the width/height of the renderbuffer
 * _format: the storage format of the renderbuffer
 * _samples: the number of samples the renderbuffer should contain defaults to 0, (meaning no multisampling)
 */
glh::core::rbo::rbo ( const unsigned _width, const unsigned _height, const GLenum _format, const unsigned _samples )
    : object { minor_object_type::GLH_RBO_TYPE }
    , width { _width }
    , height { _height }
    , format { _format }
    , samples { _samples }
{
    /* bind, set the storage, unbind */
    const bool binding_change = bind ();
    if ( samples == 0 ) glRenderbufferStorage ( opengl_bind_target, format, width, height );
    else glRenderbufferStorageMultisample ( opengl_bind_target, samples, format, width, height );
    if ( binding_change ) unbind ();
}



/* FBO IMPLEMENTATION */

/* attach_texture2d
 *
 * add a texture2d attachment
 *
 * texture: the texture to attach
 * attachment: which attachment the texture should be used as
 * mipmap: the mipmap level to attach (defaults to 0)
 */
void glh::core::fbo::attach_texture2d ( const texture2d& texture, const GLenum attachment, const GLint mipmap )
{
    /* bind the fbo */
    const bool binding_change = bind ();

    /* attach the texture */
    glFramebufferTexture2D ( opengl_bind_target, attachment, texture.get_opengl_bind_taregt (), texture.internal_id (), mipmap );

    /* unbind fbo */
    if ( binding_change ) unbind ();
}
void glh::core::fbo::attach_texture2d ( const texture2d_multisample& texture, const GLenum attachment, const GLint mipmap )
{
    /* bind the fbo */
    const bool binding_change = bind ();

    /* attach the texture */
    glFramebufferTexture2D ( opengl_bind_target, attachment, texture.get_opengl_bind_taregt (), texture.internal_id (), mipmap );

    /* unbind fbo */
    if ( binding_change ) unbind ();
}

/* attach_cubemap
 *
 * add a cubemap attachment
 * 
 * _cubemap: the cubemap to attach
 * attachment: which attachment the cubemap should be used as
 * mipmap: the mipmap level to attach (defaults to 0)
 */
void glh::core::fbo::attach_cubemap ( const cubemap& _cubemap, const GLenum attachment, const GLuint mipmap )
{
    /* bind the fbo */
    const bool binding_change = bind ();

    /* attach the cubemap */
    glFramebufferTexture ( opengl_bind_target, attachment, _cubemap.internal_id (), mipmap );

    /* unbind fbo */
    if ( binding_change ) unbind ();
}

/* attach_rbo
 *
 * add a renderbuffer attachment
 *
 * _rbo: the renderbuffer to attach
 * attachment: which attachment the rbo should be used as
 */
void glh::core::fbo::attach_rbo ( const rbo& _rbo, const GLenum attachment )
{
    /* bind the fb */
    const bool binding_change = bind ();

    /* attach the rbo */
    glFramebufferRenderbuffer ( opengl_bind_target, attachment, _rbo.get_opengl_bind_taregt (), _rbo.internal_id () );

    /* unbind fbo */
    if ( binding_change ) unbind ();
}

/* is_complete
 *
 * return true if the framebuffer is complete
 */
bool glh::core::fbo::is_complete () const
{
    /* bind, check if is complete, unbind and return */
    const bool binding_change = bind ();
    const bool complete = ( glCheckFramebufferStatus ( opengl_bind_target ) == GL_FRAMEBUFFER_COMPLETE );
    if ( binding_change ) unbind ();
    return complete;
}



/* draw/read_buffer
 *
 * sets where to use as a draw/read buffer
 * by default is GL_COLOR_ATTACHMENT0
 * 
 * buff: GLenum for what buffer to use
 */
void glh::core::fbo::draw_buffer ( const GLenum buff )
{
    /* bind, set option, unbind */
    const bool binding_change = bind_draw ();
    glDrawBuffer ( buff );
    if ( binding_change ) unbind_draw ();
}
void glh::core::fbo::read_buffer ( const GLenum buff )
{
    /* bind, set option, unbind */
    const bool binding_change = bind_read ();
    glReadBuffer ( buff );
    if ( binding_change ) unbind_read ();
}



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
void glh::core::fbo::blit_copy ( const fbo& other, const unsigned srcx0, const unsigned srcy0, const unsigned srcx1, const unsigned srcy1, 
                                 const unsigned dstx0, const unsigned dsty0, const unsigned dstx1, const unsigned dsty1, const GLbitfield copy_mask, const GLenum filter )
{
    /* bind both framebuffers */
    const bool read_binding_change = other.bind_read ();
    const bool draw_binding_change = bind_draw ();

    /* copy data */
    glBlitFramebuffer ( srcx0, srcy0, srcx1, srcy1, dstx0, dsty0, dstx1, dsty1, copy_mask, filter );

    /* unbind both framebuffers */
    if ( draw_binding_change ) unbind_draw ();
    if ( read_binding_change ) other.unbind_read ();
}