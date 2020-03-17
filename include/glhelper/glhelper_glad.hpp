/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * src/glhelper_glad.hpp
 * 
 * declares glad-handling constructs
 * 
 */



/* HEADER GUARD */
#ifndef GLHELPER_GLAD_HPP_INCLUDED
#define GLHELPER_GLAD_HPP_INCLUDED



/* INCLUDES */

/* include core headers */
#include <iostream>

/* include glhelper_core.hpp */
#include <glhelper/glhelper_core.hpp>

/* include glhelper_glfw.hpp */
#include <glhelper/glhelper_glfw.hpp>

/* include glhelper_exception.hpp */
#include <glhelper/glhelper_exception.hpp>



/* MACROS */

/* GLH_ERROR_HANDLE_GLAD_LOAD_ON_EMPTY_CONTEXT
 *
 * error handle for when glad is loaded with no OpenGL context set
 */
#ifndef GLH_ERROR_HANDLE_GLAD_LOAD_ON_EMPTY_CONTEXT
#define GLH_ERROR_HANDLE_GLAD_LOAD_ON_EMPTY_CONTEXT 3
#endif

/* GLH_ERROR_HANDLE_GLAD_LOAD_FAILURE
 *
 * error handle for when glad loading fails
 */
#ifndef GLH_ERROR_HANDLE_GLAD_LOAD_FAILURE
#define GLH_ERROR_HANDLE_GLAD_LOAD_FAILURE 3
#endif




/* NAMESPACE FORWARD DECLARATIONS */

namespace glh
{
    /* class glad_loader, object glad
     *
     * controls glad
     */
    class glad_loader;

    /* class glad_exception : exception
     *
     * for exceptions related to glad
     */
    class glad_exception;
}



/* FULL DECLARATIONS */

/* class glad_loader, object glad
 *
 * controls glad
 */
class glh::glad_loader
{
public:    

    /* constructor
     *
     * creates an inactivated instance of glad
     */
    explicit glad_loader ()
        : active_window { NULL }
    {}

    /* deleted copy constructor */
    glad_loader ( const glad_loader& other ) = delete;

    /* deleted assignement operator */
    glad_loader& operator= ( const glad_loader& other ) = delete;

    /* default destructor */



    /* load
     *
     * loads glad to the current context
     * 
     * return: true for success, false for failure
     */
    bool load ();

    /* is_window_loaded
     *
     * checks whether window is currently loaded to glad
     * 
     * win: window to check
     * 
     * return: boolean representing if is currently loaded
     */
    bool is_window_loaded ( const window& win ) const { return ( win.internal_ptr () == active_window ); }



private:

    /* GLFWwindow * active_window
     *
     * a pointer to the currently active window
     */
    GLFWwindow * active_window;

} glad;



/* class glad_exception : [glh::]exception
 *
 * for exceptions related to glad
 */
class glh::glad_exception : public exception
{
public:

    /* full constructor
     *
     * __what: description of the exception
     */
    explicit glad_exception ( const char * __what )
        : exception ( __what )
    {}

    /* zero-parameter constructor
     *
     * construct glad_exception with no descrption
     */
    explicit glad_exception ()
        : exception { NULL }
    {}

    /* default everything else and inherits what () function */

};



/* #ifndef GLHELPER_GLAD_HPP_INCLUDED */
#endif