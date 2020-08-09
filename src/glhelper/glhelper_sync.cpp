/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * src/glhelper/glhelper_sync.cpp
 * 
 * implementation of include/glhelper/glhelper_model.hpp
 *
 */



/* INCLUDES */

/* include glhelper_sync.hpp */
#include <glhelper/glhelper_sync.hpp>



/* SYNC_OBJECT IMPLEMENTATION */

/* virtual destructor */
glh::core::sync_object::~sync_object ()
{
    /* if not already deleted */
    if ( handle )
    {
        /* delete sync */
        glDeleteSync ( handle );

        /* set to NULL */
        handle = NULL;
    }
}

/* client_wait_sync
 *
 * pause cpu operations until the condition supplied is met or the timeout reached
 * 
 * flags: defaults to GL_SYNC_FLUSH_COMMANDS_BIT, but the only other value possible is 0
 * timeout: the timeout in nanoseconds before returning (defaults to wait forever)
 */
GLenum glh::core::sync_object::client_wait_sync ( const GLbitfield flags, const long unsigned timeout ) const
{
    /* wait for the sync or timeout */
    return glClientWaitSync ( handle, flags, timeout );
}

/* wait_sync
 *
 * this does not pause the cpu, rather stops any commands from being added to the gpu queue until the sync condition
 */
void glh::core::sync_object::wait_sync () const
{
    /* wait for the sync */
    glWaitSync ( handle, 0, GL_TIMEOUT_IGNORED );
}



/* SYNC IMPLEMENTATION */

/* finish_queue
 *
 * pauses the cpu until the gpu queue has been emptied
 */
void glh::core::sync::finish_queue ()
{
    /* call glFinish */
    glFinish ();
}

/* flush_queue
 *
 * causes the gpu to complete all issued commands within a finite time
 */
void glh::core::sync::flush_queue ()
{
    /* call glFlush */
    glFlush ();
}