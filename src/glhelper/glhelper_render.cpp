/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * src/glhelper/glhelper_render.hpp
 * 
 * implementation of include/glhelper/glhelper_render.hpp
 * 
 */



/* INCLUDES */

/* include glhelper_render.hpp */
#include <glhelper/glhelper_render.hpp>



/* RENDERER IMPLEMENTATION */

/* get/set_clear_color
 *
 * get.set the clear color
 * 
 * color: vec4 containing rgba components of clear color
 */
void glh::renderer::set_clear_color ( const math::vec4& color )
{ 
    /* if is a new clear color run the function */
    if ( color != clear_color )
    {
        glClearColor ( color.at ( 0 ), color.at ( 1 ), color.at ( 2 ), color.at ( 3 ) ); 
        clear_color = color;
    }
}

/* enable/disable_depth_test
 * 
 * enable or disable depth testing
 */
void glh::renderer::enable_depth_test () 
{
    /* enable if not already enabled */
    if ( !depth_test_state )
    {
        glEnable ( GL_DEPTH_TEST ); 
        depth_test_state = true; 
    } 
}
void glh::renderer::disable_depth_test ()
{
    /* disable if not already disabled */
    if ( depth_test_state )
    {
        glDisable ( GL_DEPTH_TEST ); 
        depth_test_state = false; 
    } 
}

/* get/set_depth_mask
 *
 * get/set the depth mask
 * 
 * mask: boolean defining if the depth buffer is written to for each fragment
 */
void glh::renderer::set_depth_mask ( const GLboolean mask )
{ 
    /* if is a different mask */
    if ( mask != depth_mask )
    {
        glDepthMask ( mask ); 
        depth_mask = mask;
    }
}

/* enable/disable_stencil_test
 *
 * enavle or disable stencil testing
 */
void glh::renderer::enable_stencil_test ()
{
    /* enable if not already enabled */
    if ( !stencil_test_state )
    {
        glEnable ( GL_STENCIL_TEST ); 
        stencil_test_state = true; 
    } 
}
void glh::renderer::disable_stencil_test ()
{
    /* disable if not already disabled */
    if ( stencil_test_state )
    {
        glDisable ( GL_STENCIL_TEST ); 
        stencil_test_state = false; 
    } 
}

/* get/set_stencil_mask
 *
 * get/set the stencil mask
 * 
 * mask: a bit mask to define which bits are writen to the stencil buffer
 */

void glh::renderer::set_stencil_mask ( const GLuint mask )
{
    /* if mask differs, set new mask */
    if ( mask != stencil_mask )
    {
        glStencilMask ( mask );
        stencil_mask = stencil_mask;
    }
}

/* enable/disable_blend
 *
 * enable/disable blending
 */
void glh::renderer::enable_blend ()
{
    /* enable if not already enabled */
    if ( !blend_state )
    {
        glEnable ( GL_BLEND ); 
        blend_state = true; 
    } 
}
void glh::renderer::disable_blend ()
{
    /* disable if not already disabled */
    if ( blend_state )
    {
        glDisable ( GL_BLEND ); 
        blend_state = false; 
    }
}

/* enable/disable_face_culling
 *
 * enable/disable face culling
 */
void glh::renderer::enable_face_culling ()
{
    /* enable if not already enabled */
    if ( !face_culling_state )
    {
        glEnable ( GL_CULL_FACE ); 
        face_culling_state = true; 
    } 
}
void glh::renderer::disable_face_culling ()
{
    /* disable if not already disable */
    if ( face_culling_state )
    {
        glDisable ( GL_CULL_FACE ); 
        face_culling_state = false; 
    }
}

/* get/set_cull_face
 *
 * get/set the face currently being culled
 * 
 * face: the face to be culled
 */
void glh::renderer::set_cull_face ( const GLenum face )
{
    /* if a different face, set the new face */
    if ( face != cull_face )
    {
        glCullFace ( face );
        cull_face = face;
    }
}

/* get/set_front_face
 *
 * set whether the front face is defined by a clockwise or counter clockwise winding order
 */
void glh::renderer::set_front_face ( const GLenum winding )
{
    /* if a different front face, set the new face */
    if ( winding != front_face )
    {
        glFrontFace ( winding );
        front_face = winding;
    }
}



/* RENDERER STATIC MEMBERS DEFINITIONS */

/* clear color is black by default */
glh::math::vec4 glh::renderer::clear_color { 0.0, 0.0, 0.0, 1.0 };

/* depth testing disabled by default */
bool glh::renderer::depth_test_state { false };

/* depth mask is GL_TRUE by default */
GLboolean glh::renderer::depth_mask { GL_TRUE };

/* stencil testing disabled by default */
bool glh::renderer::stencil_test_state { false };

/* stencil mask is 0xff by default */
GLuint glh::renderer::stencil_mask { 0xff };

/* blending is disabled by default */
bool glh::renderer::blend_state { false };

/* face culling is disabled by default */
bool glh::renderer::face_culling_state { false };

/* cull face is GL_BACK by default */
GLenum glh::renderer::cull_face { GL_BACK };

/* front face is GL_CCW by default */
GLenum glh::renderer::front_face ( GL_CCW );