/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * include/glhelper/glhelper_glfw.hpp
 * 
 * declares glfw-handling constructs
 * 
 * the initiation and termination of glfw is handled automatically:
 * on first window object creation, glfw is initialised
 * when there are no window objects left, glfw is terminated implicitly
 * 
 * windows are also automatically destroyed on object destruction
 * 
 */



/* HEADER GUARD */
#ifndef GLHELPER_GLFW_HPP_INCLUDED
#define GLHELPER_GLFW_HPP_INCLUDED



/* INCLUDES */

/* include core headers */
#include <functional>
#include <iostream>
#include <string>

/* include glhelper_core.hpp */
#include <glhelper/glhelper_core.hpp>

/* include glhelper_glad.hpp */
#include <glhelper/glhelper_glad.hpp>

/* include glhelper_exception.hpp */
#include <glhelper/glhelper_exception.hpp>

/* include glhelper_shader.hpp */
#include <glhelper/glhelper_shader.hpp>



/* NAMESPACE FORWARD DECLARATIONS */

namespace glh
{
    /* glad_loader due to circular dependancy */
    class glad_loader;
     
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



/* WINDOW DEFINITION */

/* class window
 *
 * stores a handle on a glfw window object
 */
class glh::window
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
     */
    window ( const std::string& title, const int width, const int height );

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
        int xpos;
        int ypos;
        int width;
        int height;
        int deltaxpos;
        int deltaypos;
        int deltawidth;
        int deltaheight;
    };

    /* keyinfo_t
     *
     * information about a key
     */
    struct keyinfo_t
    {
        int key;
        int scancode;
        int action;
        int mods;
    };

    /* mouseinfo_t
     *
     * information about mouse positioning
     */
    struct mouseinfo_t
    {
        double xpos;
        double ypos;
        double xfrac;
        double yfrac;
        double deltaxpos;
        double deltaypos;
        double deltaxfrac;
        double deltayfrac;
    };

    /* timeinfo_t
     *
     * information about poll timings
     */
    struct timeinfo_t
    {
        double now;
        double last;
        double delta;
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
     * return: dimensions_t containing dimensions info
     */
    dimensions_t get_dimensions () const;

    /* get_key
     *
     * test to see if a key was pressed
     * 
     * key: the GLFW code for the key
     * 
     * return: keyinfo_t containing info on the key
     */
    keyinfo_t get_key ( const int key ) const;

    /* get_mouseinfo
     *
     * get info about the mouse position and its change
     * 
     * return: mouseinfo_t containing mouse info
     */
    mouseinfo_t get_mouseinfo () const;

    /* get_timeinfo
     *
     * get info about the timings of polls
     *
     * return: timeinfo_t containing info about poll times
     */
    timeinfo_t get_timeinfo () const;



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
bool inline operator== ( const glh::window& lhs, const glh::window& rhs ) { return ( lhs.internal_ptr () == rhs.internal_ptr () ); }
bool inline operator!= ( const glh::window& lhs, const glh::window& rhs ) { return ( lhs.internal_ptr () != rhs.internal_ptr () ); }



/* class glfw_exception : exception
 *
 * for exceptions related to glfw
 */
class glh::glfw_exception : public exception
{
public:

    /* full constructor
     *
     * __what: description of the exception
     */
    explicit glfw_exception ( const std::string& __what )
        : exception ( __what )
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