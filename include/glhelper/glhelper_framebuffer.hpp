/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * include/glhelper/glhelper_framebuffer.hpp
 * 
 * declares framebuffer handling constructs
 * 
 */



/* HEADER GUARD */
#ifndef GLHELPER_FRAMEBUFFER_HPP_INCLUDED
#define GLHELPER_FRAMEBUFFER_HPP_INCLUDED



/* INCLUDES */

/* include core headers */
#include <iostream>
#include <string>

/* include glhelper_core.hpp */
#include <glhelper/glhelper_core.hpp>

/* include glhelper_exception.hpp */
#include <glhelper/glhelper_exception.hpp>

/* include glhelper_manager.hpp */
#include <glhelper/glhelper_manager.hpp>

/* include glhelper_texture.hpp */
#include <glhelper/glhelper_texture.hpp>



/* NAMESPACE FORWARD DECLARATIONS */

namespace glh
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



/* RBO DEFINITION */

/* class rbo : object
 *
 * renderbuffer class
 */
class glh::rbo : public object
{
public:

    /* full constructor
     *
     * construct a renderbuffer with a given size and storage type
     * 
     * width/height: the width/height of the renderbuffer
     * format: the storage format of the renderbuffer
     */
    rbo ( const unsigned width, const unsigned height, const GLenum format );

    /* deleted zero-parameter constructor */
    rbo () = delete;

    /* deleted copy constructor */
    rbo ( const rbo& other ) = delete;

    /* default move constructor */
    rbo ( rbo&& other ) = default;

    /* deleted copy assignment operator */
    rbo& operator= ( const rbo& other ) = delete;

    /* destructor */
    ~rbo () { destroy (); }



    /* destroy 
     *
     * destroy the renderbuffer
     */
    void destroy () { glh::object_manager::destroy_rbo ( id ); id = 0; }

    /* bind
     *
     * bind the renderbuffer
     */
    void bind () const { glh::object_manager::bind_rbo ( id ); }

    /* unbind
     *
     * unbind the renderbuffer, if already bound
     */
    void unbind () const { glh::object_manager::unbind_rbo ( id ); }

    /* is_bound
     *
     * return true if the renderbuffer is bound
     */
    bool is_bound () const { return glh::object_manager::is_rbo_bound ( id ); }
};



/* FBO DEFINITION */

/* class fbo : object
 *
 * framebuffer class
 */
class glh::fbo : public object
{
public: 

    /* zero-parameter constructor
     *
     * construct empty framebuffer
     */
    fbo ()
        : object { glh::object_manager::generate_fbo () }
    {}

    /* deleted copy constructor */
    fbo ( const fbo& other ) = delete;

    /* default move constructor */
    fbo ( fbo&& other ) = default;

    /* deleted copy assignment operator */
    fbo& operator= ( const fbo& other ) = delete;

    /* destructor */
    ~fbo () { destroy (); }



    /* attach_texture2d
     *
     * add a texture2d attachment
     *
     * texture: the texture to attach
     * attachment: which attachment the texture should be used as
     * mipmap: the mipmap level to attach (defaults to 0)
     */
    void attach_texture2d ( const texture2d& texture, const GLenum attachment, GLint mipmap = 0 );

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



    /* destroy
     *
     * destroys the object, setting its id to 0
     */
    void destroy () { glh::object_manager::destroy_fbo ( id ); id = 0; }

    /* bind
     *
     * bind the fbo
     */
    void bind () const { glh::object_manager::bind_fbo ( id ); }

    /* unbind
     *
     * unbind the fbo, if bound
     */
    void unbind () const { glh::object_manager::unbind_fbo ( id ); }

    /* is_bound
     *
     * return true if the fbo is bound
     */
    bool is_bound () const { return glh::object_manager::is_fbo_bound ( id ); }

};




/* #ifndef GLHELPER_FRAMEBUFFER_HPP_INCLUDED */
#endif