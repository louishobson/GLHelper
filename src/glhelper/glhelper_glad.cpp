/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * src/glhelper/glhelper_glad.cpp
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
 */
void glh::core::glad_loader::load ()
{
    /* get the window from the current context */
    GLFWwindow * win = glfwGetCurrentContext ();

    /* produce error if no context set */
    if ( !win ) throw exception::glad_exception { "attempted to load GLAD with no context set" };

    /* if window is already loaded, return without reloading glad */
    if ( win == active_window ) return;

    /* activate glad to current context */
    if ( !gladLoadGLLoader ( ( GLADloadproc ) glfwGetProcAddress ) )
    {
        /* failed to initialise OpenGL, so produce an exception */
        throw exception::glad_exception { "GLH ERROR: failed to load glad" };
    }

    /* set new active window */
    active_window = win;
}

/* GLFWwindow * active_window
 *
 * a pointer to the currently active window
 */
const GLFWwindow * glh::core::glad_loader::active_window = NULL;