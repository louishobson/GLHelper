/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * src/glhelper_glfw.cpp
 * 
 * implementation of src/glhelper_glfw.hpp
 *
 */



/* INCLUDES */

/* include glhelper_glfw.hpp */
#include <glhelper/glhelper_glfw.hpp>



/* WINDOW IMPLEMENTATION */

/* constructor
 *
 * creates a working glfw window
 * glad will be implicity loaded to the window's context
 */
glh::window::window ( const std::string& title, const int width, const int height )
    : winptr {}
{
    /* register object */
    register_object ();

    /* create window */
    winptr.reset ( glfwCreateWindow ( width, height, title.c_str (), NULL, NULL ) );

    /* check success */
    if ( !winptr )
    {
        /* failed to create window, so produce error */
        throw glfw_exception { "GLH ERROR: failed to create GLFW window" };
    }

    /* make window current */
    make_current ();

    /* set current viewport size */
    set_window_size ( width, height );
    set_viewport_size ( width, height );

    /* clear the viewport */
    clear ( 0., 0., 0., 0. );

    /* finally swap the buffer */
    swap_buffers ();
}

/* from pointer constructor
 *
 * constructs from pointer to already configured GLFWwindow
 * 
 * _winptr: pointer to GLFWwindow
 */
glh::window::window ( GLFWwindow * _winptr )
    : winptr { _winptr }
{
    /* register object */
    register_object ();

    /* check window */
    if ( !winptr )
    {
        /* window is invalid, so produce exception */
        throw glfw_exception { "GLH ERROR: failed to copy valid GLFW window" };
    }    
}

/* destructor */
glh::window::~window ()
{
    /* reset winptr */
    winptr.reset ();

    /* unregister object */
    unregister_object ();
}



/* static int object_count
 *
 * static integer, keeping track of how many objects are currently in existance
 * is used to know when to initialise or terminate glfw
 * automatically initialised to 0
 */
int glh::window::object_count = 0;

/* register_object
 *
 * increment object_count and initialise glfw if necessary
 */
void glh::window::register_object ()
{
    /* increment object count and initialise glfw if was zero */
    if ( object_count++ == 0 ) 
    {
        /* init glfw */
        glfwInit ();
        /* set version to 3, and only use core OpenGL functionality */
        glfwWindowHint ( GLFW_CONTEXT_VERSION_MAJOR, 3 );
        glfwWindowHint ( GLFW_CONTEXT_VERSION_MINOR, 3 );
        glfwWindowHint ( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    }
}

/* unregister_object
 *
 * decrement object_count and terminate glfw if necessary
 */
void glh::window::unregister_object ()
{
    /* decrement object count and terminate glfw if is now zero */
    if ( --object_count == 0 ) 
    {
        /* terminate glfw */
        glfwTerminate ();
    }
}

/* make_current
 *
 * makes the window current
 */
void glh::window::make_current () 
{ 
    /* make the context current */
    glfwMakeContextCurrent ( winptr.get () );
    /* tell the glad loader to load to the current context */
    glad_loader::load ();
}

/* struct __window_deleter
 *
 * functor for deleting the smart pointer
 */
/* caller operator overload */
void glh::window::__window_deleter::operator() ( GLFWwindow * win )
{
    /* if win is non-NULL, destroy the window */
    if ( win ) glfwDestroyWindow ( win );
}