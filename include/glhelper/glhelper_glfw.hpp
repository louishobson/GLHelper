/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * src/glhelper_glfw.hpp
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
#include <iostream>
#include <string>
#include <functional>

/* include memory for shared_ptr */
#include <memory>

/* include glhelper_core.hpp */
#include <glhelper/glhelper_core.hpp>

/* include glhelper_glad.hpp */
#include <glhelper/glhelper_glad.hpp>

/* include glhelper_exception.hpp */
#include <glhelper/glhelper_exception.hpp>



/* NAMESPACE FORWARD DECLARATIONS */

namespace glh
{
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
     */
    explicit window ( GLFWwindow * _winptr );

    /* deleted copy constructor
     *
     * copying a window makes no logical sense
     */
    window ( const window& other ) = delete;

    /* move constructor
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



    /* comparison operators
     *
     * determines if two window objects refer to the same window
     * 
     * return: boolean representing equality
     */
    bool operator== ( const window& other ) const { return ( winptr.get () == other.internal_ptr () ); }
    bool operator!= ( const window& other ) const { return ( winptr.get () != other.internal_ptr () ); }



    /* set_window_size
     *
     * set the size of the window
     * 
     * width/height: width and height of the window
     */
    void set_window_size ( const int width, const int height ) { glfwSetWindowSize ( winptr.get (), width, height ); }

    /* set_viewport_size
     *
     * set the size of the viewport
     *
     * width/height: width and height of the viewport
     */
    void set_viewport_size ( const int width, const int height ) { make_current (); glViewport ( 0, 0, width, height ); }

    /* swap_buffers
     *
     * swap the GLFW buffers
     */
    void swap_buffers () { glfwSwapBuffers ( winptr.get () ); }

    /* clear
     *
     * clears the window
     *
     * r,g,b,a: rgba values of the clear colour
     */
    void clear ( const float r, const float g, const float b, const float a ) { make_current (); glClearColor ( r, g, b, a ); glClear ( GL_COLOR_BUFFER_BIT ); }




    /* internal_ptr
     *
     * returns the pointer to the GLFWwindow held by winptr
     * 
     * return: pointer held by winptr
     */
    const GLFWwindow * internal_ptr () const { return winptr.get (); }
    GLFWwindow * internal_ptr () { return winptr.get (); }



private:

    /* struct __window_deleter
     *
     * functor for deleting the smart pointer
     */
    struct __window_deleter 
    { 
        /* caller operator overload */
        void operator() ( GLFWwindow * win );
    };

    /* std::unique_ptr<GLFWwindow> winptr
     *
     * pointer to the glfw window object
     */
    std::unique_ptr<GLFWwindow, __window_deleter> winptr;

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

    /* make_current
     *
     * makes the window current
     */
    void make_current ();

};



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
    explicit glfw_exception ( const char * __what )
        : exception ( __what )
    {}

    /* zero-parameter constructor
     *
     * construct glad_exception with no descrption
     */
    explicit glfw_exception ()
        : exception { NULL }
    {}

    /* default everything else and inherits what () function */

};


/* #ifndef GLHELPER_GLFW_HPP_INCLUDED */
#endif