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



    /* load_to_window
     *
     * win: window to load OpenGL to
     */
    void load_to_window ( const window& win );

    /* is_window_loaded
     *
     * checks whether window is currently loaded to glad
     * 
     * win: window to check
     * 
     * return: boolean representing if is currently loaded
     */
    bool is_window_loaded ( const window& win ) const { return ( win == active_window ); }



private:

    /* window active_window
     *
     * a copy of the handle of the currently loaded window
     */
    window active_window;

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