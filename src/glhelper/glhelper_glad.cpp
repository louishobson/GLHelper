/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * src/glhelper_glad.cpp
 * 
 * implementation of src/glhelper_glad.hpp
 * 
 */



/* INCLUDES */

/* include glhelper_glad.hpp */
#include <glhelper/glhelper_glad.hpp>



/* GLAD_LOADER IMPLEMENTATION */

/* load_to_window
 *
 * win: window to load OpenGL to
 * 
 * return: true for success, false for failure
 */
bool glh::glad_loader::load ()
{
    /* get the window from the current context */
    GLFWwindow * win = glfwGetCurrentContext ();

    /* produce error if no context set */
    __GLH_PARSE_ERROR_EVENT__ ( GLH_ERROR_HANDLE_GLAD_LOAD_ON_EMPTY_CONTEXT, glad_exception, "tried to load GLAD with no context set", false );

    /* if window is already loaded, return true without reloading glad */
    if ( win == active_window ) return true;

    /* activate glad to current context */
    if ( !gladLoadGLLoader ( ( GLADloadproc ) glfwGetProcAddress ) )
    {
        /* failed to initialise OpenGL, so produce an exception */
        __GLH_PARSE_ERROR_EVENT__ ( GLH_ERROR_HANDLE_GLAD_LOAD_FAILURE, glad_exception, "failed to load GLAD", false )
    }

    /* set new active window */
    active_window = win;

    /* return true for success */
    return true;
}