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

    /* default estructor */
    ~rbo () = default;



    /* get_bound_object_pointer
     *
     * produce a pointer to the rbo currently bound
     * NULL is returned if no object is bound to the bind point
     */
    using object::get_bound_object_pointer;
    static rbo * get_bound_object_pointer () { return dynamic_cast<rbo *> ( get_bound_object_pointer ( object_bind_target::GLH_RBO_TARGET ) ); }

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

};




/* #ifndef GLHELPER_FRAMEBUFFER_HPP_INCLUDED */
#endif