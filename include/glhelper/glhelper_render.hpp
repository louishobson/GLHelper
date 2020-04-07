/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * include/glhelper/glhelper_render.hpp
 * 
 * constructs for rendering to a window in OpenGL
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

/* include glhelper_exception.hpp */
#include <glhelper/glhelper_exception.hpp>

/* include glhelper_glfw.hpp */
#include <glhelper/glhelper_glfw.hpp>

/* include glhelper_buff.hpp */
#include <glhelper/glhelper_buff.hpp>

/* include glhelper_shader.hpp */
#include <glhelper/glhelper_shader.hpp>

/* include glhelper_texture.hpp */
#include <glhelper/glhelper_texture.hpp>



/* NAMESPACE FORWARD DECLARATIONS */

namespace glh
{
    /* class renderer
     *
     * base class for renderable objects
     * indented to be inherited from to create more complex classes
     * contains static methods to configure OpenGL
     */
    class renderer;

}



/* RENDERER DEFINITION */

/* class renderer
 *
 * base class for renderable objects
 * indented to be inherited from to create more complex classes
 * contains static methods to configure OpenGL
 */
class glh::renderer
{
public:

    /* default constructor */
    renderer () = default;

    /* default copy constructor */
    renderer ( const renderer& other ) = default;

    /* default copy assignment operator */
    renderer& operator= ( const renderer& other ) = default;

    /* default destructor */
    ~renderer () = default;



    /* draw_arrays
     *
     * draw vertices straight from a vbo (via a vao)
     * all ebo data is ignored
     * 
     * mode: the primative to render
     * start_index: the start index of the buffered data
     * count: number of vertices to draw
     */
    static void draw_arrays ( const GLenum mode, const GLint start_index, const GLsizei count ) 
    { glDrawArrays ( mode, start_index, count ); }

    /* draw_elements
     *
     * draw vertices from an ebo (via a vao)
     * 
     * mode: the primative to render
     * count: number of vertices to draw
     * type: the type of the data in the ebo
     * start_index: the start index of the elements
     */
    static void draw_elements ( const GLenum mode, const GLint count, const GLenum type, const GLvoid * start_index )
    { glDrawElements ( mode, count, type, start_index ); }

    /* clear_color
     *
     * set the clear color
     * 
     * r,g,b,a: components for the new clear color
     */
    static void clear_color ( const double r, const double g, const double b, const double a )
    { glClearColor ( r, g, b, a ); }

    /* clear
     *
     * clears the screen
     */
    static void clear ()
    { glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); }

    /* enable/disable_depth_test
     * 
     * enable or disable depth testing
     */
    static void enable_depth_test () { glEnable ( GL_DEPTH_TEST ); }
    static void disable_depth_test () { glDisable ( GL_DEPTH_TEST ); }

    /* viewport
     *
     * set the viewport size
     */
    static void viewport ( GLint x, GLint y, GLsizei width, GLsizei height )
    { glViewport ( x, y, width, height ); }

};



/* #ifndef GLHELPER_RENDER_HPP_INCLUDED */
#endif