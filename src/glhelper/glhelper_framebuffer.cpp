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
    : width { _width }
    , height { _height }
    , format { _format }
    , samples { _samples }
{
    /* generate rbo */
    glGenRenderbuffers ( 1, &id );

    /* bind, set the storage, unbind */
    const bool binding_change = bind ();
    if ( samples == 0 ) glNamedRenderbufferStorage ( id, format, width, height );
    else glNamedRenderbufferStorageMultisample ( id, samples, format, width, height );
    if ( binding_change ) unbind ();
}

/* destructor */
glh::core::rbo::~rbo ()
{
    /* destroy object */
    if ( id != 0 ) glDeleteRenderbuffers ( 1, &id );
}



/* default bind/unbind the rbo */
bool glh::core::rbo::bind () const
{
    /* if already bound, return false, else bind and return true */
    if ( bound_rbo == this ) return false;
    glBindRenderbuffer ( GL_RENDERBUFFER, id );
    bound_rbo = const_cast<rbo *> ( this );
    return false;
}
bool glh::core::rbo::unbind () const
{
    /* if not bound, return false, else unbind and return true */
    if ( bound_rbo != this ) return false;
    glBindRenderbuffer ( GL_RENDERBUFFER, 0 );
    bound_rbo = NULL;
    return false;
}



/* the currently bound rbo */
glh::core::object_pointer<glh::core::rbo> glh::core::rbo::bound_rbo {};



/* FBO IMPLEMENTATION */

/* zero-parameter constructor
*
* construct empty framebuffer
*/
glh::core::fbo::fbo ()
{
    /* generate fbo */
    glCreateFramebuffers ( 1, &id );
}

/* destructor */
glh::core::fbo::~fbo ()
{
    /* destroy fbo */
    glDeleteFramebuffers ( 1, &id );
}



/* default bind/unbind the fbo
 *
 * equivalent to binding/unbinding from read and draw targets simultaneously
 */
bool glh::core::fbo::bind () const
{
    /* if bound, return false, else bind and return true */
    if ( bound_read_fbo == this && bound_draw_fbo == this ) return false;
    glBindFramebuffer ( GL_FRAMEBUFFER, id );
    bound_read_fbo = const_cast<fbo *> ( this );
    bound_draw_fbo = const_cast<fbo *> ( this );
    return true;
}
bool glh::core::fbo::unbind () const
{
    /* if not bound, return false, else unbind and return true */
    if ( bound_read_fbo != this && bound_draw_fbo != this ) return false;
    glBindFramebuffer ( GL_FRAMEBUFFER, 0 );
    bound_read_fbo = NULL;
    bound_draw_fbo = NULL;
    return true;
}

/* bind to read/draw targets separately */
bool glh::core::fbo::bind_read () const
{
    /* if bound, return false, else bind and return true */
    if ( bound_read_fbo == this ) return false;
    glBindFramebuffer ( GL_READ_FRAMEBUFFER, id );
    bound_read_fbo = const_cast<fbo *> ( this );
    return true;
}
bool glh::core::fbo::bind_draw () const
{
    /* if bound, return false, else bind and return true */
    if ( bound_draw_fbo == this ) return false;
    glBindFramebuffer ( GL_DRAW_FRAMEBUFFER, id );
    bound_draw_fbo = const_cast<fbo *> ( this );
    return true;
}

/* unbind from read/draw targets */
bool glh::core::fbo::unbind_read () const
{
    /* if not bound, return false, else unbind and return true */
    if ( bound_read_fbo != this ) return false;
    glBindFramebuffer ( GL_READ_FRAMEBUFFER, 0 );
    bound_read_fbo = NULL;
    return true;
}
bool glh::core::fbo::unbind_draw () const
{
    /* if not bound, return false, else unbind and return true */
    if ( bound_draw_fbo != this ) return false;
    glBindFramebuffer ( GL_DRAW_FRAMEBUFFER, 0 );
    bound_draw_fbo = NULL;
    return true;
}

/* bind the default framebuffer set up by the window manager */
bool glh::core::fbo::bind_default_framebuffer ()
{
    /* if bound, return false, else bind and return true */
    if ( bound_read_fbo.internal_id () == 0 && bound_draw_fbo.internal_id () == 0 ) return false;
    glBindFramebuffer ( GL_FRAMEBUFFER, 0 );
    bound_read_fbo = NULL;
    bound_draw_fbo = NULL;
    return true;
}
bool glh::core::fbo::is_default_framebuffer_bound ()
{
    /* return true if bound framebuffer has internal id of 0 */
    return bound_read_fbo.internal_id () == 0 && bound_draw_fbo.internal_id () == 0;
}



/* attach_texture
 *
 * add a texture attachment
 *
 * texture: the texture to attach
 * attachment: which attachment the texture should be used as
 * mipmap: the mipmap level to attach (defaults to 0)
 */
void glh::core::fbo::attach_texture ( const texture_base& texture, const GLenum attachment, const unsigned mipmap )
{
    /* attach the texture */
    glNamedFramebufferTexture ( id, attachment, texture.internal_id (), mipmap );
}

/* attach_texture_layer
 *
 * add a layer of a texture as an attachment attachment
 * 
 * texture: the texture to attach
 * layer: the layer to attach
 * attachment: which attachment the texture should be used as
 * mipmap: the mipmap level to attach (defaults to 0)
 */
void glh::core::fbo::attach_texture_layer ( const texture_base& texture, const unsigned layer, const GLenum attachment, const unsigned mipmap )
{
    /* attach the texture */
    glNamedFramebufferTextureLayer ( id, attachment, texture.internal_id (), mipmap, layer );
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
    /* bind the rbo */
    const bool binding_change = _rbo.bind ();

    /* attach the rbo */
    glNamedFramebufferRenderbuffer ( id, attachment, GL_RENDERBUFFER, _rbo.internal_id () );

    /* unbind rbo */
    if ( binding_change ) _rbo.unbind ();
}



/* is_complete
 *
 * return true if the framebuffer is complete
 */
bool glh::core::fbo::is_complete () const
{
    /* return completeness */
    return glCheckNamedFramebufferStatus ( id, GL_FRAMEBUFFER ) == GL_FRAMEBUFFER_COMPLETE;
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
    /* set option */
    glNamedFramebufferDrawBuffer ( id, buff );
}
void glh::core::fbo::read_buffer ( const GLenum buff )
{
    /* set option */
    glNamedFramebufferReadBuffer ( id, buff );
}



/* set_default_dimensions
 *
 * sets the default width, height and layers of a framebuffer with no color attachment
 * this causes rasterisation to occur based on these dimensions
 * 
 * width/height: the default width and height
 * layers: default layers of a framebuffer (defaults to 0)
 */
void glh::core::fbo::set_default_dimensions ( const unsigned width, const unsigned height, const unsigned layers )
{
    /* set options */
    glad_glNamedFramebufferParameteri ( id, GL_FRAMEBUFFER_DEFAULT_WIDTH, width );
    glad_glNamedFramebufferParameteri ( id, GL_FRAMEBUFFER_DEFAULT_HEIGHT, height );
    glad_glNamedFramebufferParameteri ( id, GL_FRAMEBUFFER_DEFAULT_LAYERS, layers );
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
    /* copy data */
    glBlitNamedFramebuffer ( other.internal_id (), id, srcx0, srcy0, srcx1, srcy1, dstx0, dsty0, dstx1, dsty1, copy_mask, filter );
}

/* blit_copy_from_default
 *
 * copy a region FROM THE DEFAULT FBO INTO THIS FBO
 * 
 * srcx0, srcy0, srcx1, srcy1: the x and y positions in the default fbo to read from
 * dstx0, dsty0, dstx1, dsty1: the x and y positions in this fbo to write to
 * copy_mask: mask for which buffers to copy
 * filter: the interpolation settings for any stretches applied
 */
void glh::core::fbo::blit_copy_from_default ( const unsigned srcx0, const unsigned srcy0, const unsigned srcx1, const unsigned srcy1, 
                                            const unsigned dstx0, const unsigned dsty0, const unsigned dstx1, const unsigned dsty1, const GLbitfield copy_mask, const GLenum filter )
{
    /* copy data */
    glBlitNamedFramebuffer ( 0, id, srcx0, srcy0, srcx1, srcy1, dstx0, dsty0, dstx1, dsty1, copy_mask, filter );
}


/* blit_copy_to_default
 *
 * copy a region FROM THIS FBO INTO THE DEFAULT FBO
 * 
 * srcx0, srcy0, srcx1, srcy1: the x and y positions in this fbo to read from
 * dstx0, dsty0, dstx1, dsty1: the x and y positions in the default fbo to write to
 * copy_mask: mask for which buffers to copy
 * filter: the interpolation settings for any stretches applied
 */
void glh::core::fbo::blit_copy_to_default ( const unsigned srcx0, const unsigned srcy0, const unsigned srcx1, const unsigned srcy1, 
                                            const unsigned dstx0, const unsigned dsty0, const unsigned dstx1, const unsigned dsty1, const GLbitfield copy_mask, const GLenum filter ) const
{
    /* copy data */
    glBlitNamedFramebuffer ( id, 0, srcx0, srcy0, srcx1, srcy1, dstx0, dsty0, dstx1, dsty1, copy_mask, filter );
}



/* bound read/draw fbos */
glh::core::object_pointer<glh::core::fbo> glh::core::fbo::bound_read_fbo {};
glh::core::object_pointer<glh::core::fbo> glh::core::fbo::bound_draw_fbo {};