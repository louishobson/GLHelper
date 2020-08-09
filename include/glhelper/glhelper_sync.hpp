/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * include/glhelper/glhelper_sync.hpp
 * 
 * constructs to assist synchronisation between the CPU and the GPU
 *
 */



/* HEADER GUARD */
#ifndef GLHELPER_SYNC_HPP_INCLUDED
#define GLHELPER_SYNC_HPP_INCLUDED



/* INCLUDES */

/* include core headers */
#include <iostream>

/* include glhelper_core.hpp */
#include <glhelper/glhelper_core.hpp>



/* NAMESPACE DECLARATIONS */

namespace glh
{
    namespace core
    {
        /* class sync_object
         *
         * a generic class for a sync object
         */
        class sync_object;
    
        /* class fence_sync : sync_object
         *
         * creates a fence sync
         */
        class fence_sync;

        /* class sync
         *
         * contains static methods for controlling the gpu queue
         */
        class sync;
    }
}



/* SYNC_OBJECT DEFINITION */

/* class sync_object
 *
 * a generic class for a sync object
 */
class glh::core::sync_object
{
public:

    /* sync object pointer constructor */
    sync_object ( GLsync _handle )
        : handle { _handle }
    {}

    /* deleted zero-parameter constructor */
    sync_object () = delete;

    /* deleted copy constructor */
    sync_object ( const sync_object& other ) = delete;

    /* move constructor */
    sync_object ( sync_object&& other )
        : handle { other.handle }
    { other.handle = NULL; }

    /* deleted copy assignment operator */
    sync_object& operator= ( const sync_object& other ) = delete;

    /* virtual destructor */
    virtual ~sync_object ();



    /* client_wait_sync
     *
     * pause cpu operations until the condition supplied is met or the timeout reached
     * 
     * flags: defaults to GL_SYNC_FLUSH_COMMANDS_BIT, but the only other value possible is 0
     * timeout: the timeout in nanoseconds before returning (defaults to wait forever)
     */
    GLenum client_wait_sync ( const GLbitfield flags = GL_SYNC_FLUSH_COMMANDS_BIT, const long unsigned timeout = GL_TIMEOUT_IGNORED ) const;

    /* wait_sync
     *
     * this does not pause the cpu, rather stops any commands from being added to the gpu queue until the sync condition
     */
    void wait_sync () const;



private:

    /* the sync object handle */
    GLsync handle;

};



/* FENCE_SYNC DEFINITION */

/* class fence_sync : sync_object
 *
 * creates a fence sync
 */
class glh::core::fence_sync : public sync_object
{
public:

    /* zero-parameter constructor */
    fence_sync ()
        : sync_object { glFenceSync ( GL_SYNC_GPU_COMMANDS_COMPLETE, 0 ) }
    {}

    /* deleted copy constructor */
    fence_sync ( const fence_sync& other ) = delete;

    /* default move constructor */
    fence_sync ( fence_sync&& other ) = default;

    /* deleted copy assignment operator */
    fence_sync& operator= ( const fence_sync& other ) = delete;

};



/* SYNC DEFINITION */

/* class sync
 *
 * contains static methods for controlling the gpu queue
 */
class glh::core::sync
{
public:

    /* finish_queue
     *
     * pauses the cpu until the gpu queue has been emptied
     */
    static void finish_queue ();

    /* flush_queue
     *
     * causes the gpu to complete all issued commands within a finite time
     */
    static void flush_queue ();

};



/* #ifndef GLHELPER_SYNC_HPP_INCLUDED */
#endif