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
 * windows are also automatically destroyed when the LAST window object referring to it is destroyed
 * 
 */



/* HEADER GUARD */
#ifndef GLHELPER_GLFW_HPP_INCLUDED
#define GLHELPER_GLFW_HPP_INCLUDED



/* INCLUDES */

/* include core headers */
#include <iostream>

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

    /* constructor
     *
     * creates a working glfw window
     * glad will be implicity loaded to the window's context
     */
    window ();

    /* from pointer constructor
     *
     * constructs from pointer to already configured GLFWwindow
     * 
     * _winptr: pointer to GLFWwindow
     */
    window ( GLFWwindow * _winptr );

    /* copy constructor
     *
     * creates a duplicate handle on the same window
     */
    window ( const window& other );

    /* deleted copy assignment operater
     *
     * assigning a window from another window makes no logical sense, hence the deletion
     */
    window& operator= ( const window& other ) = delete;

    /* destructor */
    ~window ();



    /* make_current
     *
     * makes the window be currently loaded into the OpenGL context
     * synonymous to glh::glad.load_to_window ( object )
     * 
     * return: true for success, false for failure
     */
    bool make_current () { return glad.load_to_window ( * this ); }

    /* internal_ptr
     *
     * returns a reference to the shared pointer storing the GLFWwindow pointer
     * 
     * return: reference to winptr
     */
    const std::shared_ptr<GLFWwindow>& internal_ptr () const { return winptr; }

    /* comparison operators
     *
     * determines if two window objects refer to the same window
     * 
     * return: boolean representing equality
     */
    bool operator== ( const window& other ) const { return ( winptr == other.internal_ptr () ); }



private:

    /* std::shared_ptr<GLFWwindow> winptr
     *
     * pointer to the glfw window object
     */
    std::shared_ptr<GLFWwindow> winptr;

    /* static int object_count
     *
     * static integer, keeping track of how many objects are currently in existance
     * is used to know when to initialise or terminate glfw
     * automatically initialised to 0
     */
    static int object_count;

};


/* #ifndef GLHELPER_GLFW_HPP_INCLUDED */
#endif