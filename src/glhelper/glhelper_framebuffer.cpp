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
 * width/height: the width/height of the renderbuffer
 * format: the storage format of the renderbuffer
 */
glh::core::rbo::rbo ( const unsigned width, const unsigned height, const GLenum format )
    : object { minor_object_type::GLH_RBO_TYPE }
{
    /* bind, set the storage, unbind */
    const bool binding_change = bind ();
    glRenderbufferStorage ( opengl_bind_target, format, width, height );
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
void glh::core::fbo::attach_texture2d ( const texture2d& texture, const GLenum attachment, GLint mipmap )
{
    /* bind the fbo */
    const bool binding_change = bind ();

    /* attach the rbo */
    glFramebufferTexture2D ( opengl_bind_target, attachment, texture.get_opengl_bind_taregt (), texture.internal_id (), mipmap );

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