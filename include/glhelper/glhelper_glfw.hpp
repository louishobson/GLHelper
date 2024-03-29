/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * include/glhelper/glhelper_glfw.hpp
 * 
 * declares glfw-handling constructs
 * notable constructs include:
 * 
 * 
 * 
 * CLASS GLH::GLFW::WINDOW
 * 
 * class handling a GLFW window 
 * the initiation and termination of GLFW handled automatically:
 * on first window object creation, GLFW is implicitly initialised, and GLAD is loaded to its context
 * when there are no window objects left, GLFW is implicitly terminated
 * all input handling is found as members of the window class
 * all rendering handling is found in the glh::core::renderer class
 * that being said, window's swap_buffers method must be run for any rendering to be applied
 * 
 * 
 * 
 * CLASS GLH::EXCEPTION::GLFW_EXCEPTION
 * 
 * thrown when an error occurs in one of the window methods (e.g. invalid GLFW window objects)
 * 
 */



/* HEADER GUARD */
#ifndef GLHELPER_GLFW_HPP_INCLUDED
#define GLHELPER_GLFW_HPP_INCLUDED



/* INCLUDES */

/* include core headers */
#include <cstring>
#include <functional>
#include <iostream>
#include <string>

/* include glhelper_core.hpp */
#include <glhelper/glhelper_core.hpp>

/* include glhelper_glad.hpp */
#include <glhelper/glhelper_glad.hpp>

/* include glhelper_framebuffer */
#include <glhelper/glhelper_framebuffer.hpp>



/* NAMESPACE DECLARATIONS */

namespace glh
{
    namespace core
    {
        /* glad_loader due to circular dependancy */
        class glad_loader;
    }
     
    namespace glfw
    {
        /* class window
         *
         * stores a handle on a glfw window object
         */
        class window;
    }

    namespace exception
    {
        /* class glfw_exception : exception
         *
         * for exceptions related to glfw
         */
        class glfw_exception;
    }
}



/* WINDOW DEFINITION */

/* class window
 *
 * stores a handle on a glfw window object
 */
class glh::glfw::window
{
public:

    /* CONSTRUCTORS AND DESTRUCTORS */

    /* full constructor
     *
     * creates a working glfw window
     * glad will be implicity loaded to the window's context
     * 
     * title: the title of the window
     * width/height: the width and height of the window
     * msaa_samples: the number of MSAA samples (defaults to 0)
     */
    window ( const std::string& title, const unsigned width = 600, const unsigned height  = 400, const unsigned msaa_samples = 0 );

    /* zero-parameter constructor
     *
     * creates a glfw window based on defaults
     */
    window ()
        : window { "New window", 600, 400 }
    {}

    /* from pointer constructor
     *
     * constructs from pointer to already configured GLFWwindow
     * 
     * _winptr: pointer to GLFWwindow
     * _managed: whether the window should be deleted on destruction of the object (defaults to false)
     */
    window ( GLFWwindow * _winptr, const bool _managed = false );

    /* deleted copy constructor
     *
     * copying a window makes no logical sense
     */
    window ( const window& other ) = delete;

    /* default move constructor
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



    /* TRIVIAL STRUCTURES FOR INPUT INFO */

    /* dimensions_t
     *
     * stores window dimensions
     */
    struct dimensions_t
    {
        /* x and y pos of the screen */
        int xpos;
        int ypos;

        /* the width and height of the screen */
        int width;
        int height;

        /* the change in x and y pos */
        int deltaxpos;
        int deltaypos;

        /* the change in width and height */
        int deltawidth;
        int deltaheight;
    };

    /* keyinfo_t
     *
     * information about a key
     */
    struct keyinfo_t
    {
        /* the key in question */
        int key;

        /* its scancode */
        int scancode;

        /* the action applied */
        int action;

        /* modifier bits during the action */
        int mods;
    };

    /* mouseinfo_t
     *
     * information about mouse positioning
     */
    struct mouseinfo_t
    {
        /* the x and y pos of the mouse */
        double xpos;
        double ypos;

        /* the x and y pos as fractions of the screen dimensions */
        double xfrac;
        double yfrac;

        /* the change in x and y pos */
        double deltaxpos;
        double deltaypos;

        /* the change in x and y as a fraction of the screen dimensions */
        double deltaxfrac;
        double deltayfrac;
    };

    /* timeinfo_t
     *
     * information about poll timings
     */
    struct timeinfo_t
    {
        /* the current time */
        double now;

        /* the time when the timeinfo was last generated */
        double last;

        /* the change in time */
        double delta;
    };

    /* gamepadinfo_t
     *
     * information about a gamepad
     */
    struct gamepadinfo_t
    {
        /* the id of the joystick (1-16) */
        int joystick_id;

        /* the right-hand buttons */
        int button_a;
        int button_b;
        int button_x;
        int button_y;

        /* back bumpers */
        int button_lh_bumper;
        int button_rh_bumper;

        /* back and start buttons */
        int button_back;
        int button_start;

        /* left and right thumbs */
        int button_lh_thumb;
        int button_rh_thumb;

        /* dpad buttons */
        int button_dpad_up;
        int button_dpad_right;
        int button_dpad_down;
        int button_dpad_left;

        /* left and right axis */
        double axis_lh_x;
        double axis_lh_y;
        double axis_rh_x;
        double axis_rh_y;

        /* left and right triggers */
        double axis_lh_trigger;
        double axis_rh_trigger;
    };



    /* EVENT CONTROL */

    /* poll_events
     *
     * run any callbacks set for events which have occured since the last poll
     * immediately return even if no events have occured
     */
    void poll_events ();

    /* wait_events
     *
     * wait for at least one event to have occured since the last poll, and run associated callbacks
     * if an event has already occured, this function returns immediately
     * 
     * timeout: seconds to wait for events before returning (or 0 for infinite timeout)
     */
    void wait_events ( const double timeout );

    /* post_empty_event
     *
     * post an empty event to cause wait event function to return
     */
    void post_empty_event ();

    /* should_close
     *
     * return: boolean as to whether the window should close or not
     */
    bool should_close () const;

    /* set_should close 
     *
     * set close flag on window
     */
    void set_should_close ();

    /* get_dimensions
     *
     * get the dimensions of the window
     * 
     * get_window_pos: set to true if position information is required
     * 
     * return: dimensions_t containing dimensions info
     */
    dimensions_t get_dimensions ( const bool get_window_pos = false ) const;

    /* get_key
     *
     * test to see if a key was pressed
     * 
     * key: the GLFW code for the key
     * get_scancode: set to true if the scancode is required
     * 
     * return: keyinfo_t containing info on the key
     */
    keyinfo_t get_key ( const int key, const bool get_scancode = false ) const;

    /* get_mouseinfo
     *
     * get info about the mouse position and its change
     * 
     * get_fractions: set to true if fraction movement relative to window size is required
     * 
     * return: mouseinfo_t containing mouse info
     */
    mouseinfo_t get_mouseinfo ( const bool get_fractions = false ) const;

    /* get_timeinfo
     *
     * get info about the timings of polls
     *
     * return: timeinfo_t containing info about poll times
     */
    timeinfo_t get_timeinfo () const;

    /* get_gamepadinfo
     *
     * get info about a gamepad
     * 
     * joystick: the joystick to get info on
     * 
     * return gamepadinfo_t containing info about the gamepad
     */
    gamepadinfo_t get_gamepadinfo ( const int joystick ) const;



    /* OTHER INPUT METHODS */

    /* set_input_mode
     *
     * set the settings of an input mode
     * 
     * mode: the mode to change
     * value: the value to change it to
     */
    void set_input_mode ( const int mode, const int value );



    /* OPENGL WINDOW MANAGEMENT */

    /* bind_framebuffer 
     *
     * bind the window's framebuffer as well as resize the viewport
     * 
     * returns true if a change in binding occured
     */
    bool bind_framebuffer () const { return core::fbo::bind_default_framebuffer (); }

    /* make_current
     *
     * makes the window current
     */
    void make_current () const;

    /* is_current
     *
     * checks if the window is current
     */
    bool is_current () const;

    /* swap_buffers
     *
     * swap the GLFW buffers
     */
    void swap_buffers ();



    /* INTERNAL DATA GETTING METHODS */

    /* internal_ptr
     *
     * returns the pointer to the GLFWwindow held by winptr
     * 
     * return: pointer held by winptr
     */
    const GLFWwindow * internal_ptr () const { return winptr; }



private:

    /* WINDOW OBJECT LIFETIME MANAGEMENT */

    /* GLFWwindow * winptr
     *
     * pointer to the glfw window object
     */
    GLFWwindow * winptr;

    /* const bool managed
     *
     * whether the window should be destroyed on object destruction
     * defaults to true
     */
    const bool managed;

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



    /* PEVIOUS INFO STORAGE */

    /* prev_dimensions/mouseinfo/timeinfo
     *
     * previous dimension, mouse and time info to calculate changes
     */
    mutable dimensions_t prev_dimensions;
    mutable mouseinfo_t prev_mouseinfo;
    mutable timeinfo_t prev_timeinfo;

};

/* comparison operators
 *
 * determines if two window objects refer to the same window
 * 
 * return: boolean representing equality
 */
bool inline operator== ( const glh::glfw::window& lhs, const glh::glfw::window& rhs ) { return ( lhs.internal_ptr () == rhs.internal_ptr () ); }
bool inline operator!= ( const glh::glfw::window& lhs, const glh::glfw::window& rhs ) { return ( lhs.internal_ptr () != rhs.internal_ptr () ); }



/* class glfw_exception : exception
 *
 * for exceptions related to glfw
 */
class glh::exception::glfw_exception : public exception
{
public:

    /* full constructor
     *
     * __what: description of the exception
     */
    explicit glfw_exception ( const std::string& __what )
        : exception { __what }
    {}

    /* default zero-parameter constructor
     *
     * construct glfw_exception with no descrption
     */
    glfw_exception () = default;

    /* default everything else and inherits what () function */

};


/* #ifndef GLHELPER_GLFW_HPP_INCLUDED */
#endif