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
glh::glfw::window::window ( const std::string& title, const unsigned width, const unsigned height, const unsigned msaa_samples )
    : managed { true }
{
    /* register object */
    register_object ();

    /* hint at number of samples */
    glfwWindowHint ( GLFW_SAMPLES, msaa_samples );

    /* create window */
    winptr = glfwCreateWindow ( width, height, title.c_str (), NULL, NULL );

    /* check success */
    if ( !winptr )
    {
        /* failed to create window, so produce error */
        throw exception::glfw_exception { "GLH ERROR: failed to create GLFW window" };
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
glh::glfw::window::window ( GLFWwindow * _winptr, const bool _managed )
    : winptr { _winptr }
    , managed { _managed }
{
    /* register object */
    register_object ();

    /* check window */
    if ( !winptr )
    {
        /* window is invalid, so produce exception */
        throw exception::glfw_exception { "GLH ERROR: failed to copy valid GLFW window" };
    }    
}

/* destructor */
glh::glfw::window::~window ()
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
void glh::glfw::window::poll_events ()
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
void glh::glfw::window::wait_events ( const double timeout )
{
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
void glh::glfw::window::post_empty_event ()
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
bool glh::glfw::window::should_close () const
{
    /* return if window should close */
    return glfwWindowShouldClose ( winptr );
}

/* set_should close 
 *
 * set close flag on window
 */
void glh::glfw::window::set_should_close ()
{
    /* set should close */
    glfwSetWindowShouldClose ( winptr, true );
}

/* get_dimensions
 *
 * get the dimensions of the window
 * 
 * return: dimensions_t containing dimensions info
 */
glh::glfw::window::dimensions_t glh::glfw::window::get_dimensions () const
{
    /* generate dimension info */
    dimensions_t dimensions;
    glfwGetWindowPos ( winptr, &dimensions.xpos, &dimensions.ypos );
    glfwGetWindowSize ( winptr, &dimensions.width, &dimensions.height );
    dimensions.deltaxpos = dimensions.xpos - prev_dimensions.xpos;
    dimensions.deltaypos = dimensions.ypos - prev_dimensions.ypos;
    dimensions.deltawidth = dimensions.width - prev_dimensions.width;
    dimensions.deltaheight = dimensions.height - prev_dimensions.height;

    /* set prev_dimensions */
    prev_dimensions = dimensions;

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
glh::glfw::window::keyinfo_t glh::glfw::window::get_key ( const int key ) const
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
glh::glfw::window::mouseinfo_t glh::glfw::window::get_mouseinfo () const
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
    mouseinfo.deltayfrac = mouseinfo.deltaypos / dimensions.height;

    /* set previous mouseinfo */
    prev_mouseinfo = mouseinfo;

    /* return the mouse info */
    return mouseinfo;
}

/* get_timeinfo
 *
 * get info about the timings of polls
 *
 * return: timeinfo_t containing info about poll times
 */
glh::glfw::window::timeinfo_t glh::glfw::window::get_timeinfo () const
{
    /* generate timeinfo */
    timeinfo_t timeinfo;
    timeinfo.now = glfwGetTime ();
    timeinfo.last = prev_timeinfo.now;
    timeinfo.delta = timeinfo.now - timeinfo.last;
    
    /* set prev_timeinfo */
    prev_timeinfo = timeinfo;

    /* return timeinfo */
    return timeinfo;
}

/* get_gamepadinfo
 *
 * get info about a gamepad
 * 
 * joystick: the joystick to get info on
 * 
 * return gamepadinfo_t containing info about the gamepad
 */
glh::glfw::window::gamepadinfo_t glh::glfw::window::get_gamepadinfo ( const int joystick ) const
{
    /* create empty struct */
    gamepadinfo_t gamepadinfo;
    std::memset ( &gamepadinfo, 0, sizeof ( gamepadinfo ) );

    /* return if gamepad not present */
    if ( !glfwJoystickPresent ( joystick ) ) return gamepadinfo;

    /* if can be mapped to a gamepad, use that */
    if ( glfwJoystickIsGamepad ( joystick ) )
    {
        /* generate gamepadinfo */
        GLFWgamepadstate glfw_gamepadinfo;
        glfwGetGamepadState ( joystick, &glfw_gamepadinfo );

        /* generate gamepadinfo */
        gamepadinfo.button_a = glfw_gamepadinfo.buttons [ GLFW_GAMEPAD_BUTTON_A ];
        gamepadinfo.button_b = glfw_gamepadinfo.buttons [ GLFW_GAMEPAD_BUTTON_B ];
        gamepadinfo.button_x = glfw_gamepadinfo.buttons [ GLFW_GAMEPAD_BUTTON_X ];
        gamepadinfo.button_y = glfw_gamepadinfo.buttons [ GLFW_GAMEPAD_BUTTON_Y ];

        gamepadinfo.button_lh_bumper = glfw_gamepadinfo.buttons [ GLFW_GAMEPAD_BUTTON_LEFT_BUMPER ];
        gamepadinfo.button_rh_bumper = glfw_gamepadinfo.buttons [ GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER ];

        gamepadinfo.button_back = glfw_gamepadinfo.buttons [ GLFW_GAMEPAD_BUTTON_BACK ];
        gamepadinfo.button_start = glfw_gamepadinfo.buttons [ GLFW_GAMEPAD_BUTTON_START ];

        gamepadinfo.button_lh_thumb = glfw_gamepadinfo.buttons [ GLFW_GAMEPAD_BUTTON_LEFT_THUMB ];
        gamepadinfo.button_rh_thumb = glfw_gamepadinfo.buttons [ GLFW_GAMEPAD_BUTTON_RIGHT_THUMB ];

        gamepadinfo.button_dpad_up = glfw_gamepadinfo.buttons [ GLFW_GAMEPAD_BUTTON_DPAD_UP ];
        gamepadinfo.button_dpad_right = glfw_gamepadinfo.buttons [ GLFW_GAMEPAD_BUTTON_DPAD_RIGHT ];
        gamepadinfo.button_dpad_down = glfw_gamepadinfo.buttons [ GLFW_GAMEPAD_BUTTON_DPAD_DOWN ];
        gamepadinfo.button_dpad_left = glfw_gamepadinfo.buttons [ GLFW_GAMEPAD_BUTTON_DPAD_LEFT ];

        gamepadinfo.axis_lh_x = glfw_gamepadinfo.axes [ GLFW_GAMEPAD_AXIS_LEFT_X ];
        gamepadinfo.axis_lh_y = glfw_gamepadinfo.axes [ GLFW_GAMEPAD_AXIS_LEFT_Y ];
        gamepadinfo.axis_rh_x = glfw_gamepadinfo.axes [ GLFW_GAMEPAD_AXIS_RIGHT_X ];
        gamepadinfo.axis_rh_y = glfw_gamepadinfo.axes [ GLFW_GAMEPAD_AXIS_RIGHT_Y ];

        gamepadinfo.axis_lh_trigger = glfw_gamepadinfo.axes [ GLFW_GAMEPAD_AXIS_LEFT_TRIGGER ];
        gamepadinfo.axis_rh_trigger = glfw_gamepadinfo.axes [ GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER ];
    } else
    /* otherwise take a guess as to which axis and buttons map to which */
    {
        /* get buttons and set them */
        int button_count;
        const unsigned char * buttons = glfwGetJoystickButtons ( joystick, &button_count );
        if ( button_count >= 4 )
        {
            gamepadinfo.button_a = buttons [ 0 ];
            gamepadinfo.button_b = buttons [ 1 ];
            gamepadinfo.button_x = buttons [ 2 ];
            gamepadinfo.button_y = buttons [ 3 ];
        }
        if ( button_count >= 6 )
        {
            gamepadinfo.button_lh_bumper = buttons [ 4 ];
            gamepadinfo.button_rh_bumper = buttons [ 5 ];
        }
        if ( button_count >= 8 )
        {
            gamepadinfo.button_back = buttons [ 6 ];
            gamepadinfo.button_start = buttons [ 7 ];
        }

        /* get axis and set them */
        int axis_count;
        const float * axis = glfwGetJoystickAxes ( joystick, &axis_count );
        if ( axis_count >= 2 )
        {
            gamepadinfo.axis_lh_x = axis [ 0 ];
            gamepadinfo.axis_lh_y = axis [ 1 ];            
        }
        if ( axis_count >= 4 )
        {
            gamepadinfo.axis_rh_x = axis [ 2 ];
            gamepadinfo.axis_rh_y = axis [ 3 ];     
        }
        if ( axis_count >= 6 )
        {
            gamepadinfo.axis_lh_trigger = axis [ 4 ];
            gamepadinfo.axis_rh_trigger = axis [ 5 ];     
        }

        /* get hats and set them */
        int hat_count;
        const unsigned char * hats = glfwGetJoystickHats ( joystick, &hat_count );
        if ( hat_count >= 1 )
        {
            gamepadinfo.button_dpad_up = hats [ 0 ] & GLFW_HAT_UP;
            gamepadinfo.button_dpad_right = hats [ 0 ] & GLFW_HAT_RIGHT;
            gamepadinfo.button_dpad_down = hats [ 0 ] & GLFW_HAT_DOWN;
            gamepadinfo.button_dpad_left = hats [ 0 ] & GLFW_HAT_LEFT;
        }
    }

    /* return info */
    return gamepadinfo;    
}



/* OTHER INPUT METHODS */

/* set_input_mode
 *
 * set the settings of an input mode
 * 
 * mode: the mode to change
 * value: the value to change it to
 */
void glh::glfw::window::set_input_mode ( const int mode, const int value )
{
    /* set the input mode */
    glfwSetInputMode ( winptr, mode, value );
}



/* OPENGL WINDOW MANAGEMENT */

/* make_current
 *
 * makes the window current
 */
void glh::glfw::window::make_current () const
{ 
    /* if window not already current */
    if ( !is_current () )
    {
        /* make the context current */
        glfwMakeContextCurrent ( winptr );
        /* tell the glad loader to load to the current context */
        core::glad_loader::load ();
    }
}

/* is_current
 *
 * checks if the window is current
 */
bool glh::glfw::window::is_current () const
{
    /* return true of context is current */
    return ( glfwGetCurrentContext () == winptr );
}

/* swap_buffers
 *
 * swap the GLFW buffers
 */
void glh::glfw::window::swap_buffers () 
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
int glh::glfw::window::object_count = 0;

/* register_object
 *
 * increment object_count and initialise glfw if necessary
 */
void glh::glfw::window::register_object ()
{
    /* increment object count and initialise glfw if was zero */
    if ( object_count++ == 0 ) 
    {
        /* init glfw */
        glfwInit ();
        /* set version to 3, and only use core OpenGL functionality */
        glfwWindowHint ( GLFW_CONTEXT_VERSION_MAJOR, 4 );
        glfwWindowHint ( GLFW_CONTEXT_VERSION_MINOR, 4 );
        glfwWindowHint ( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    }
}

/* unregister_object
 *
 * decrement object_count and terminate glfw if necessary
 */
void glh::glfw::window::unregister_object ()
{
    /* decrement object count and terminate glfw if is now zero */
    if ( --object_count == 0 ) 
    {
        /* terminate glfw */
        glfwTerminate ();
    }
}