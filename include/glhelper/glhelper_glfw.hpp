/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * include/glhelper/glhelper_glfw.hpp
 * 
 * declares glfw-handling constructs
 * 
 * the initiation and termination of glfw is handled automatically:
 * on first window object creation, glfw is initialised
 * when there are no window objects left, glfw is terminated implicitly
 * 
 * windows are also automatically destroyed on object destruction
 * 
 */



/* HEADER GUARD */
#ifndef GLHELPER_GLFW_HPP_INCLUDED
#define GLHELPER_GLFW_HPP_INCLUDED



/* INCLUDES */

/* include core headers */
#include <functional>
#include <iostream>
#include <string>

/* include memory for shared_ptr */
#include <memory>

/* include glhelper_core.hpp */
#include <glhelper/glhelper_core.hpp>

/* include glhelper_glad.hpp */
#include <glhelper/glhelper_glad.hpp>

/* include glhelper_exception.hpp */
#include <glhelper/glhelper_exception.hpp>

/* include glhelper_buff.hpp */
#include <glhelper/glhelper_buff.hpp>

/* include glhelper_shader.hpp */
#include <glhelper/glhelper_shader.hpp>



/* NAMESPACE FORWARD DECLARATIONS */

namespace glh
{
    /* class vao
     *
     * forward declaration for use by window class
     */
    class vao;

    /* class program
     *
     * forward declaration for use by window class
     */
    class program;

    /* glad_loader due to circular dependancy */
    class glad_loader;
     
    /* class window
     *
     * stores a handle on a glfw window object
     */
    class window;

    /* class glfw_exception : exception
     *
     * for exceptions related to glfw
     */
    class glfw_exception;
}



/* FULL DECLARATIONS */

/* class window
 *
 * stores a handle on a glfw window object
 */
class glh::window
{
public:

    /* CONSTRUCTORS AND DESTRUCTORS */

    /* full constructor
     *
     * creates a working glfw window
     * glad will be implicity loaded to the window's context
     * 
     * title: the title of the window
     * width/height: the width and height of the window
     */
    explicit window ( const std::string& title, const int width, const int height );

    /* zero-parameter constructor
     *
     * creates a glfw window based on defaults
     */
    explicit window ()
        : window { "New window", 600, 400 }
    {}

    /* from pointer constructor
     *
     * constructs from pointer to already configured GLFWwindow
     * 
     * _winptr: pointer to GLFWwindow
     * _managed: whether the window should be deleted on destruction of the object (defaults to false)
     */
    explicit window ( GLFWwindow * _winptr, const bool _managed = false );

    /* deleted copy constructor
     *
     * copying a window makes no logical sense
     */
    window ( const window& other ) = delete;

    /* default move constructor
     *
     * moving does make sense, for example returning from functions
     */
    window ( window&& other ) = default;

    /* deleted copy assignment operater
     *
     * assigning a window from another window makes no logical sense, hence the deletion
     */
    window& operator= ( const window& other ) = delete;

    /* destructor */
    ~window ();



    /* WINDOW CONTROLLING METHODS */

    /* set_window_size
     *
     * set the size of the window
     * 
     * width/height: width and height of the window
     */
    void set_window_size ( const int width, const int height );

    /* set_viewport_size
     *
     * set the size of the viewport
     *
     * width/height: width and height of the viewport
     */
    void set_viewport_size ( const int width, const int height );



    /* CALLBACK TYPEDEFS */

    /* window_size_callback_t
     *
     * the callback type for window a window resize event
     * 
     * window&: reference to the window which was resized
     * int,int: with, height
     */
    typedef void ( __window_size_callback_internal_t ) ( GLFWwindow *, int, int );
    typedef std::function<void ( GLFWwindow *, int, int )> window_size_callback_t;



    /* CALLBACK SETTING METHODS */

    /* set_window_size_callback
     *
     * set the callback for window resizing
     * 
     * callback: the callback to run on window resize event, or nullptr to remove the callback
     */
    void set_window_size_callback ( const window_size_callback_t& callback );




    /* EVENT CONTROL */

    /* poll_events
     *
     * run any callbacks set for events which have occured since the last poll
     * immediately return even if no events have occured
     */
    void poll_events ();

    /* wait_events
     *
     * wait for at least one event to have occured since the last poll, and run associated callbacks
     * if an event has already occured, this function returns immediately
     * 
     * timeout: seconds to wait for events before returning (or 0 for infinite timeout)
     */
    void wait_events ( const double timeout );

    /* should_close
     *
     * return: boolean as to whether the window should close or not
     */
    bool should_close ();

    /* set_should close 
     *
     * set close flag on window
     */
    void set_should_close ();



    /* DRAWING METHODS */

    /* draw_arrays
     *
     * draw vertices straight from a vbo (via a vao)
     * all ebo data is ignored
     * 
     * _vao: the vao to draw from
     * _program: shader program to use to draw the vertices
     * mode: the primative to render
     * start_index: the start index of the buffered data
     * count: number of vertices to draw
     */
    void draw_arrays ( const vao& _vao, const program& _program, const GLenum mode, const GLint start_index, const GLsizei count );

    /* draw_elements
     *
     * draw vertices from an ebo (via a vao)
     * 
     * _vao: the vao to draw from
     * _program: shader program to use to draw the vertices
     * mode: the primative to render
     * count: number of vertices to draw
     * type: the type of the data in the ebo
     * start_index: the start index of the elements
     */
    void draw_elements ( const vao& _vao, const program& _program, const GLenum mode, const GLint count, const GLenum type, const GLvoid * start_index );

    /* swap_buffers
     *
     * swap the GLFW buffers
     */
    void swap_buffers ();

    /* clear
     *
     * clears the window
     *
     * r,g,b,a: rgba values of the clear colour
     */
    void clear ( const GLfloat r, const GLfloat g, const GLfloat b, const GLfloat a );



    /* INTERNAL DATA GETTING METHODS */

    /* internal_ptr
     *
     * returns the pointer to the GLFWwindow held by winptr
     * 
     * return: pointer held by winptr
     */
    const GLFWwindow * internal_ptr () const { return winptr; }
    GLFWwindow * internal_ptr () { return winptr; }



private:

    /* CALLBACK STORAGE */

    /* window_resize_callback
     *
     * callback for window resizing
     */
    window_size_callback_t window_size_callback;



    /* WINDOW OBJECT LIFETIME MANAGEMENT */

    /* GLFWwindow * winptr
     *
     * pointer to the glfw window object
     */
    GLFWwindow * winptr;

    /* const bool managed
     *
     * whether the window should be destroyed on object destruction
     * defaults to true
     */
    const bool managed;

    /* static int object_count
     *
     * static integer, keeping track of how many objects are currently in existance
     * is used to know when to initialise or terminate glfw
     * automatically initialised to 0
     */
    static int object_count;

    /* register_object
     *
     * increment object_count and initialise glfw if necessary
     */
    static void register_object ();

    /* unregister_object
     *
     * decrement object_count and terminate glfw if necessary
     */
    static void unregister_object ();



    /* OPENGL WINDOW MANAGEMENT */

    /* make_current
     *
     * makes the window current
     */
    void make_current ();

};

/* comparison operators
 *
 * determines if two window objects refer to the same window
 * 
 * return: boolean representing equality
 */
bool operator== ( const glh::window& lhs, const glh::window& rhs ) { return ( lhs.internal_ptr () == rhs.internal_ptr () ); }
bool operator!= ( const glh::window& lhs, const glh::window& rhs ) { return ( lhs.internal_ptr () != rhs.internal_ptr () ); }



/* class glfw_exception : exception
 *
 * for exceptions related to glfw
 */
class glh::glfw_exception : public exception
{
public:

    /* full constructor
     *
     * __what: description of the exception
     */
    explicit glfw_exception ( const std::string& __what )
        : exception ( __what )
    {}

    /* default zero-parameter constructor
     *
     * construct glfw_exception with no descrption
     */
    explicit glfw_exception () = default;

    /* default everything else and inherits what () function */

};


/* #ifndef GLHELPER_GLFW_HPP_INCLUDED */
#endif