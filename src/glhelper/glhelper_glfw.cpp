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
    glfwSetWindowSize ( winptr, width, height );

    /* poll events to generate previous dimensions/mouseinfo/etc storage */
    poll_events ();

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



/* EVENT CONTROL */

/* poll_events
 *
 * run any callbacks set for events which have occured since the last poll
 * immediately return even if no events have occured
 */
void glh::window::poll_events ()
{
    /* set prev_... storage */
    prev_dimensions = get_dimensions ();
    prev_mouseinfo = get_mouseinfo ();
    prev_timeinfo = get_poll_timeinfo ();

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
    /* set prev_... storage */
    prev_dimensions = get_dimensions ();
    prev_mouseinfo = get_mouseinfo ();
    prev_timeinfo = get_poll_timeinfo ();

    /* make window current */
    make_current ();

    /* if timeout == 0, wait forever */
    if ( timeout == 0. ) glfwWaitEvents ();
    /* else, wait on timeout */
    else glfwWaitEventsTimeout ( timeout );
}

/* post_empty_event
 *
 * post an empty event to cause wait event function to return
 */
void glh::window::post_empty_event ()
{
    /* make window current */
    make_current ();

    /* post empty event and return */
    glfwPostEmptyEvent ();
}

/* should_close
 *
 * return: boolean as to whether the window should close or not
 */
bool glh::window::should_close () const
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

/* get_dimensions
 *
 * get the dimensions of the window
 * 
 * return: dimensions_t containing dimensions info
 */
glh::window::dimensions_t glh::window::get_dimensions () const
{
    /* generate dimension info */
    dimensions_t dimensions;
    glfwGetWindowPos ( winptr, &dimensions.xpos, &dimensions.ypos );
    glfwGetWindowSize ( winptr, &dimensions.width, &dimensions.height );
    dimensions.deltaxpos = dimensions.xpos - prev_dimensions.xpos;
    dimensions.deltaypos = dimensions.ypos - prev_dimensions.ypos;
    dimensions.deltawidth = dimensions.width - prev_dimensions.width;
    dimensions.deltaheight = dimensions.height - prev_dimensions.height;

    /* return dimensions */
    return dimensions;
}

/* get_key
 *
 * test to see if a key was pressed
 * 
 * key: the GLFW code for the key
 * 
 * return: GLFW_TRUE/FALSE for if the key has been pressed since last poll
 */
glh::window::keyinfo_t glh::window::get_key ( const int key ) const
{
    /* generate the keyinfo */
    keyinfo_t keyinfo;
    keyinfo.key = key;
    keyinfo.scancode = glfwGetKeyScancode ( key );
    keyinfo.action = glfwGetKey ( winptr, key );
    keyinfo.mods = 0;

    /* return the keyinfo */
    return keyinfo;
}

/* get_mouseinfo
 *
 * get info about the mouse position and its change
 * 
 * return: mouseinfo_t containing mouse info
 */
glh::window::mouseinfo_t glh::window::get_mouseinfo () const
{
    /* generate raw mouseinfo */
    mouseinfo_t mouseinfo;
    glfwGetCursorPos ( winptr, &mouseinfo.xpos, &mouseinfo.ypos );
    mouseinfo.deltaxpos = mouseinfo.xpos - prev_mouseinfo.xpos;
    mouseinfo.deltaypos = mouseinfo.ypos - prev_mouseinfo.ypos;

    /* get the viewport dimensions */
    dimensions_t dimensions = get_dimensions ();

    /* set fractions in mouse info */
    mouseinfo.xfrac = mouseinfo.xpos / dimensions.width;
    mouseinfo.yfrac = mouseinfo.ypos / dimensions.height;
    mouseinfo.deltaxfrac = mouseinfo.deltaxpos / dimensions.width;
    mouseinfo.deltayfrac = mouseinfo.deltaypos / dimensions.width;

    /* return the mouse info */
    return mouseinfo;
}

/* get_timeinfo
 *
 * get info about the timings of polls
 *
 * return: timeinfo_t containing info about poll times
 */
glh::window::timeinfo_t glh::window::get_timeinfo () const
{
    /* generate timeinfo */
    timeinfo_t timeinfo;
    timeinfo.now = glfwGetTime ();
    timeinfo.poll = prev_timeinfo.poll;
    timeinfo.lastpoll = prev_timeinfo.lastpoll;
    timeinfo.deltapoll = timeinfo.poll - timeinfo.lastpoll;

    /* return timeinfo */
    return timeinfo;
}



/* OTHER INPUT METHODS */

/* set_input_mode
 *
 * set the settings of an input mode
 * 
 * mode: the mode to change
 * value: the value to change it to
 */
void glh::window::set_input_mode ( const int mode, const int value )
{
    /* set the input mode */
    glfwSetInputMode ( winptr, mode, value );
}



/* OPENGL WINDOW MANAGEMENT */

/* make_current
 *
 * makes the window current
 */
void glh::window::make_current () const
{ 
    /* if window not already current */
    if ( !is_current () )
    {
        /* make the context current */
        glfwMakeContextCurrent ( winptr );
        /* tell the glad loader to load to the current context */
        glad_loader::load ();
    }
}

/* is_current
 *
 * checks if the window is current
 */
bool glh::window::is_current () const
{
    /* return true of context is current */
    return ( glfwGetCurrentContext () == winptr );
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



/* PEVIOUS INFO STORAGE */

/* get_poll_timeinfo
 *
 * set the timeinfo at a poll
 */
glh::window::timeinfo_t glh::window::get_poll_timeinfo () const
{
    /* generate timeinfo */
    timeinfo_t timeinfo;
    timeinfo.now = glfwGetTime ();
    timeinfo.poll = glfwGetTime ();
    timeinfo.lastpoll = prev_timeinfo.poll;
    timeinfo.deltapoll = timeinfo.poll - timeinfo.lastpoll;

    /* return timeinfo */
    return timeinfo;
}