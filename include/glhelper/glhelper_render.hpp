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

/* include glhelper_glad.hpp */
#include <glhelper/glhelper_glad.hpp>

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
    /* class renderable
     *
     * base class for renderable objects
     * indented to be inherited from to create more complex classes
     */
    class renderable;

    /* class simple_renderable : renderable
     *
     * holds references to a window, a vao, a program, and textures
     */
    class static_renderable;
}



/* RENDERABLE DEFINITION */

/* renderable
 *
 * base class for renderable objects
 * indented to be inherited from to create more complex classes
 */
class glh::renderable
{
public:

    /* default constructor */
    renderable () = default;

    /* default copy constructor */
    renderable ( const renderable& other ) = default;

    /* default copy assignment operator */
    renderable& operator= ( const renderable& other ) = default;

    /* default destructor */
    virtual ~renderable () = default;



    /* virtual (un)prepare
     *
     * so that inherited classes can be correcly (un)prepared
     */
    virtual void prepare () {}
    virtual void unprepare () {};



    /* draw_arrays
     *
     * draw vertices straight from a vbo (via a vao)
     * all ebo data is ignored
     * 
     * mode: the primative to render
     * start_index: the start index of the buffered data
     * count: number of vertices to draw
     */
    void draw_arrays ( const GLenum mode, const GLint start_index, const GLsizei count ) 
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
    void draw_elements ( const GLenum mode, const GLint count, const GLenum type, const GLvoid * start_index )
    { glDrawElements ( mode, count, type, start_index ); }

    /* clear_colour
     *
     * set the clear colour
     * 
     * r,g,b,a: components for the new clear colour
     */
    void clear_colour ( const double r, const double g, const double b, const double a )
    { glClearColor ( r, g, b, a ); }

    /* clear
     *
     * clears the screen
     */
    void clear ()
    { glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); }

    /* enable/disable_depth_test
     * 
     * enable or disable depth testing
     */
    void enable_depth_test () { glEnable ( GL_DEPTH_TEST ); }
    void disable_depth_test () { glDisable ( GL_DEPTH_TEST ); }

    /* viewport
     *
     * set the viewport size
     */
    void viewport ( GLint x, GLint y, GLsizei width, GLsizei height )
    { glViewport ( x, y, width, height ); }

};



/* STATIC_RENDERABLE DEFINITION */

/* class static_renderable : renderable
 *
 * holds a program, vbo, ebo, vao and 2D textures
 */
class glh::static_renderable : public renderable
{
public:

    /* constructor */
    template<class... Ts>
    static_renderable ( const window& __window, const vao& __vao, const program& __program, const Ts&... __texture2d )
        : _window { __window }
        , _vao { __vao }
        , _program { __program }
        , _texture2d { { &__texture2d... } }
    {}

    /* deleted zero parameter constructor */
    static_renderable () = delete;

    /* default copy constructor */
    static_renderable ( const static_renderable& other ) = default;

    /* default copy assignment operator */
    static_renderable& operator= ( const static_renderable& other ) = default;

    /* default destructor */
    ~static_renderable () = default;



    /* prepare
     *
     * perform all binds etc. ready for rendering
     */
    void prepare () override;

    /* unprepare
     *
     * undo all binds etc
     */
    void unprepare () override;



private:

    /* const window& _window
     *
     * the window to render onto
     */
    const window& _window;

    /* const vao& _vao
     *
     * the vao to use for vertex and buffer data
     */
    const vao& _vao;

    /* const program& _program
     *
     * the program to use for rendering
     */
    const program& _program;

    /* std::vector _texture2d
     *
     * array of pointers to 2D textures
     */
    std::vector<const texture2d *> _texture2d;

};


/* #ifndef GLHELPER_RENDER_HPP_INCLUDED */
#endif