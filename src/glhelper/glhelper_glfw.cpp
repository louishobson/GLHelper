/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * src/glhelper/glhelper_glfw.cpp
 * 
 * implementation of src/glhelper_glfw.hpp
 *
 */



/* INCLUDES */

/* include glhelper_glfw.hpp */
#include <glhelper/glhelper_glfw.hpp>



/* WINDOW IMPLEMENTATION */

/* CONSTRUCTORS AND DESTRUCTORS */

/* constructor
 *
 * creates a working glfw window
 * glad will be implicity loaded to the window's context
 */
glh::window::window ( const std::string& title, const int width, const int height )
    : managed { true }
{
    /* register object */
    register_object ();

    /* create window */
    winptr = glfwCreateWindow ( width, height, title.c_str (), NULL, NULL );

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
     * _managed: whether the window should be deleted on destruction of the object (defaults to true)
     */
glh::window::window ( GLFWwindow * _winptr, const bool _managed )
    : winptr { _winptr }
    , managed { _managed }
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
    /* if managed, destroy window */
    if ( managed ) glfwDestroyWindow ( winptr );

    /* unregister object */
    unregister_object ();
}



/* WINDOW CONTROLLING METHODS */

/* set_window_size
 *
 * set the size of the window
 * 
 * width/height: width and height of the window
 */
void glh::window::set_window_size ( const int width, const int height ) 
{
    /* set the window size */ 
    glfwSetWindowSize ( winptr, width, height ); 
}

/* set_viewport_size
 *
 * set the size of the viewport
 *
 * width/height: width and height of the viewport
 */
void glh::window::set_viewport_size ( const int width, const int height ) 
{ 
    /* make window current */
    make_current (); 
    /* set the viewport size */
    glViewport ( 0, 0, width, height ); 
}



/* CALLBACK SETTING METHODS */

/* set_window_size_callback
 *
 * set the callback for window resizing
 * 
 * callback: the callback to run on window resize event
 */
void glh::window::set_window_size_callback ( const window_size_callback_t& callback )
{
    /* remove callback */
    glfwSetWindowSizeCallback ( winptr, NULL );
    window_size_callback = nullptr;

    /* if callback is invalid (e.g. nullptr), return */
    if ( !callback ) return;

    /* set callback attribute */
    window_size_callback = callback;

    /* set the callback */
    glfwSetWindowSizeCallback ( winptr, * window_size_callback.target<__window_size_callback_internal_t *> () );
}



/* EVENT CONTROL */

/* poll_events
 *
 * run any callbacks set for events which have occured since the last poll
 * immediately return even if no events have occured
 */
void glh::window::poll_events ()
{
    /* make window current, poll events and return */
    make_current ();
    glfwPollEvents ();
}

/* wait_events
 *
 * wait for at least one event to have occured since the last poll, and run associated callbacks
 * if an event has already occured, this function returns immediately
 * 
 * timeout: seconds to wait for events before returning (or 0 for infinite timeout)
 */
void glh::window::wait_events ( const double timeout )
{
    /* make window current */
    make_current ();

    /* if timeout == 0, wait forever */
    if ( timeout == 0.0f ) glfwWaitEvents ();
    /* else, wait on timeout */
    else glfwWaitEventsTimeout ( timeout );
}

/* should_close
 *
 * return: boolean as to whether the window should close or not
 */
bool glh::window::should_close ()
{
    /* return if window should close */
    return glfwWindowShouldClose ( winptr );
}

/* set_should close 
 *
 * set close flag on window
 */
void glh::window::set_should_close ()
{
    /* set should close */
    glfwWindowShouldClose ( winptr );
}



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
void glh::window::draw_arrays ( const vao& _vao, const program& _program, const GLenum mode, const GLint start_index, const GLsizei count )
{
    /* make the window current */
    make_current ();
    /* use the program */
    _program.use ();
    /* bind the vao */
    _vao.bind ();

    /* draw arrays */
    glDrawArrays ( mode, start_index, count );

    /* unbind the vao */
    _vao.unbind ();
}

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
void glh::window::draw_elements ( const vao& _vao, const program& _program, const GLenum mode, const GLint count, const GLenum type, const void * start_index )
{
    /* make the window current */
    make_current ();
    /* use the program */
    _program.use ();
    /* bind the vao */
    _vao.bind ();

    /* draw elements */
    glDrawElements ( mode, count, type, start_index );

    /* unbind the vao */
    _vao.unbind ();
}


/* swap_buffers
 *
 * swap the GLFW buffers
 */
void glh::window::swap_buffers () 
{ 
    /* swap the buffers */
    glfwSwapBuffers ( winptr ); 
}

/* clear
 *
 * clears the window
 *
 * r,g,b,a: rgba values of the clear colour
 */
void glh::window::clear ( const float r, const float g, const float b, const float a ) 
{
    /* make window current */
    make_current (); 
    /* set the clear colour and clear */
    glClearColor ( r, g, b, a ); 
    glClear ( GL_COLOR_BUFFER_BIT );
}



/* WINDOW OBJECT LIFETIME MANAGEMENT */

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



/* OPENGL WINDOW MANAGEMENT */

/* make_current
 *
 * makes the window current
 */
void glh::window::make_current () 
{ 
    /* make the context current */
    glfwMakeContextCurrent ( winptr );
    /* tell the glad loader to load to the current context */
    glad_loader::load ();
}