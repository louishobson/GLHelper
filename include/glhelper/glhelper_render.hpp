/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * include/glhelper/glhelper_render.hpp
 * 
 * defines renderer class to change and keep track of rendering settings
 * notable constructs include:
 * 
 * 
 * 
 * CLASS GLH::CORE::RENDERER
 * 
 * class containing static methods to control rendering and rendering options
 * various settings are tracked to reduce duplicate calls to set the same setting
 * 
 */



/* HEADER GUARD */
#ifndef GLHELPER_RENDER_HPP_INCLUDED
#define GLHELPER_RENDER_HPP_INCLUDED



/* INCLUDES */

/* include core headers */
#include <iostream>
#include <vector>

/* include glhelper_core.hpp */
#include <glhelper/glhelper_core.hpp>

/* include glhelper_vector.hpp */
#include <glhelper/glhelper_vector.hpp>

/* include glhelper_buffer.hpp */
#include <glhelper/glhelper_buffer.hpp>

/* include glhelper_shader.hpp */
#include <glhelper/glhelper_shader.hpp>



/* NAMESPACE DECLARATIONS */

namespace glh
{
    namespace core
    {
        /* class renderer
         *
         * contains static methods to control rendering
         */
        class renderer;
    }
}



/* RENDERER DEFINITION */

/* class renderer
 *
 * contains static methods to control rendering
 */
class glh::core::renderer
{
public:

    /* deleted constructor */
    renderer () = delete;

    /* deleted copy constructor */
    renderer ( const renderer& other ) = delete;

    /* deleted copy assignment operator */
    renderer& operator= ( const renderer& other ) = delete;

    /* deleted destructor */
    ~renderer () = delete;



    /* draw_arrays
     *
     * draw vertices straight from a vbo (via a vao)
     * all ebo data is ignored
     * 
     * mode: the primative to render
     * start_index: the start index of the buffered data
     * count: number of vertices to draw
     * instances: number of instances to draw (defaults to 1)
     */
    static void draw_arrays ( const GLenum mode, const GLint start_index, const GLsizei count, const unsigned instances = 1 );



    /* draw_elements
     *
     * draw vertices from an ebo (via a vao)
     * 
     * mode: the primative to render
     * count: number of vertices to draw
     * type: the type of the data in the ebo
     * start_index: the start index of the elements
     * instances: number of instances to draw (defaults to 1)
     */
    static void draw_elements ( const GLenum mode, const GLsizei count, const GLenum type, const GLsizeiptr start_index, const unsigned instances = 1 );



    /* get/set_clear_color
     *
     * get.set the clear color
     * 
     * color: fvec4 containing rgba components of clear color
     */
    static const math::fvec4& get_clear_color () { return clear_color; }
    static void set_clear_color ( const math::fvec4& color );

    /* clear
     *
     * clears the screen
     *
     * buffer_bits: bitfield for the buffers to clear (or all by default)
     */
    static void clear ( const GLbitfield buffer_bits = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT )
    { glClear ( buffer_bits ); }



    /* enable/disable_depth_test
     * 
     * enable or disable depth testing
     */
    static void enable_depth_test ();
    static void disable_depth_test ();

    /* depth_test_enabled
     *
     * returns true if depth testing is enabled
     */
    static bool depth_test_enabled () { return depth_test_state; }

    /* get/set_depth_mask
     *
     * get/set the depth mask
     * 
     * mask: boolean defining if the depth buffer is written to for each fragment
     */
    static GLboolean get_depth_mask () { return depth_mask; }
    static void set_depth_mask ( const GLboolean mask );

    /* set_depth_func
     *
     * set the function to use for depth testing
     * 
     * func: the function to use (GL_LESS is the default)
     */
    static void set_depth_func ( const GLenum func ) { glDepthFunc ( func ); }

    

    /* enable/disable_stencil_test
     *
     * enavle or disable stencil testing
     */
    static void enable_stencil_test ();
    static void disable_stencil_test ();

    /* stencil_test_enabled
     *
     * return true is stencil test is enabled
     */
    static bool stencil_test_enabled () { return stencil_test_state; }

    /* get/set_stencil_mask
     *
     * get/set the stencil mask
     * 
     * mask: a bit mask to define which bits are writen to the stencil buffer
     */
    static GLuint get_stencil_mask () { return stencil_mask; }
    static void set_stencil_mask ( const GLuint mask );

    /* stencil_func
     *
     * set the function to use for stencil testing
     * 
     * func: the function to use (GL_LESS is the default)
     * ref: the value to compare the stencil buffer value against
     * mask: another bit mask applied to both the stencil buffer and ref value
     */
    static void stencil_func ( const GLenum func, const GLint ref, const GLuint mask )
    { glStencilFunc ( func, ref, mask ); }

    /* stencil_op
     *
     * set the options under which the stencil buffer should be updated
     *
     * sfail: what to do when the stencil test fails
     * dpfail: what to do when the stencil test passes, but depth test fails
     * dppass: what to do when the stencil and depth tests pass
     */
    static void stencil_op ( const GLenum sfail, const GLenum dpfail, const GLenum dppass )
    { glStencilOp ( sfail, dpfail, dppass ); }



    /* enable/disable_blend
     *
     * enable/disable blending either as a whole or for a specific render target
     */
    static void enable_blend ()
    { glEnable ( GL_BLEND ); }
    static void enable_blend ( const unsigned render_target )
    { glEnablei ( GL_BLEND, render_target ); }
    static void disable_blend ()
    { glDisable ( GL_BLEND ); }
    static void disable_blend ( const unsigned render_target )
    { glDisablei ( GL_BLEND, render_target ); }

    /* blend_func
     *
     * set where to revieve the factors from for blending
     * 
     * sfactor: where to recieve the source factor from
     * dfactor: where to recieve the destination factor from
     */
    static void blend_func ( const GLenum sfactor, const GLenum dfactor )
    { glBlendFunc ( sfactor, dfactor ); }
    static void blend_func ( const unsigned render_target, const GLenum sfactor, const GLenum dfactor )
    { glBlendFunci ( render_target, sfactor, dfactor ); }

    /* blend_func_separate
     *
     * set where to revieve the factors from for blending
     * different values can be set for the rgb and alpha components
     *
     * srgbfact: where to recieve the source factor for rgb components
     * drgbfact: where to recieve the destination factor for rgb components
     * salphafact: where to recieve the source factor for the alpha component
     * dalphafact: where to recieve the destination factor for the alpha component
     */
    static void blend_func_separate ( const GLenum srgbfact, const GLenum drgbfact, const GLenum salphafact, const GLenum dalphafact )
    { glBlendFuncSeparate ( srgbfact, drgbfact, salphafact, dalphafact ); }
    static void blend_func_separate ( const unsigned render_target, const GLenum srgbfact, const GLenum drgbfact, const GLenum salphafact, const GLenum dalphafact )
    { glBlendFuncSeparatei ( render_target, srgbfact, drgbfact, salphafact, dalphafact ); }

    /* blend_equation
     *
     * set the equation to use for blending
     * 
     * equ: the equation to use
     */
    static void blend_equation ( const GLenum equ ) { glBlendEquation ( equ ); }
    static void blend_equation ( const unsigned render_target, const GLenum equ ) 
    { glBlendEquationi ( render_target, equ ); }



    /* enable/disable_face_culling
     *
     * enable/disable face culling
     */
    static void enable_face_culling ();
    static void disable_face_culling ();

    /* face_culling_enabled
     *
     * return true if face culling is enabled
     */
    static bool face_culling_enabled () { return face_culling_state; }

    /* get/set_cull_face
     *
     * get/set the face currently being culled
     * 
     * face: the face to be culled
     */
    static GLenum get_cull_face () { return cull_face; }
    static void set_cull_face ( const GLenum face );

    /* get/set_front_face
     *
     * set whether the front face is defined by a clockwise or counter clockwise winding order
     */
    static GLenum get_front_face () { return front_face; }
    static void set_front_face ( const GLenum winding );



    /* viewport
     *
     * set the viewport size
     */
    static void viewport ( GLint x, GLint y, GLsizei width, GLsizei height )
    { glViewport ( x, y, width, height ); }



    /* enable/disable_multisample
     *
     * enable/disable MSAA
     */
    static void enable_multisample ();
    static void disable_multisample ();

    /* multisample_enabled
     *
     * return true if MSAA is enabled
     */
    static bool multisample_enabled () { return multisample_state; }



    /* enable/disable_framebuffer_srgb
     *
     * enable/disable implicit conversion to srgb in framebuffer color buffer attachments
     */
    static void enable_framebuffer_srgb ();
    static void disable_framebuffer_srgb ();

    /* framebuffer_srg_enabled
     *
     * return true if framebuffer srgb is enabled
     */
    static bool framebuffer_srg_enabled () { return framebuffer_srgb_state; }





private:

    /* clear_color
     *
     * the current clear color for the screen
     * defaults to black
     */
    static math::fvec4 clear_color;

    /* depth_test_state
     *
     * whether depth testing is enabled
     * defaults to false
     */
    static bool depth_test_state;

    /* depth_mask
     *
     * the mask currently being used for depth testing
     * defaults to GL_TRUE
     */
    static GLboolean depth_mask;

    /* stencil_test_state
     *
     * whether stencil testing is enabled
     * defaults to false
     */
    static bool stencil_test_state;

    /* stencil_mask
     *
     * the mask currently being used for stencil testing
     * defaults to 0xff
     */
    static GLuint stencil_mask;

    /* face_culling_state
     *
     * whether face culling is enabled
     * defaults to false
     */
    static bool face_culling_state;

    /* cull_face
     *
     * the face(s) currently being culled
     * defaults to GL_BACK
     */
    static GLenum cull_face;

    /* front_face
     *
     * which winding order to use to define the dront face
     * defaults to GL_CCW
     */
    static GLenum front_face;

    /* multisample_state
     *
     * whether multisampling is enabled
     * defaults to false
     */
    static bool multisample_state;

    /* framebuffer_srgb_state
     *
     * whether srgb is enabled for framebuffers
     * defaults to false
     */
    static bool framebuffer_srgb_state;


};



/* #ifndef GLHELPER_RENDER_HPP_INCLUDED */
#endif