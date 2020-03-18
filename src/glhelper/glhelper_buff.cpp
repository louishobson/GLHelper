/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * src/glhelper/glhelper_buff.cpp
 * 
 * implementation of include/glhelper/glhelper_buff.hpp
 * 
 */



/* INCLUDES */

/* include glhelper_buff.hpp */
#include <glhelper/glhelper_buff.hpp>



/* BUFFER IMPLEMENTATION */

/* constructor
 *
 * generates a buffer
 */
glh::buffer::buffer ()
    : object { -1 }
{
    /* allocate the buffer */
    glGenBuffers ( 1, ( unsigned * ) &id );
}

/* virtual destructor
 *
 * virtual in preparation for polymorphism
 */
glh::buffer::~buffer ()
{
    /* destroy buffer */
    destroy ();
}

/* buffer_data
 *
 * buffer data into the buffer
 * 
 * size: size of data in bytes
 * data: pointer to data
 * usage: the storage method for the data
 */
void glh::buffer::buffer_data ( const size_t size, const void * data, const GLenum usage ) 
{
    /* bind the buffer */
    bind ();
    /* buffer data */
    glBufferData ( target (), size, data, usage );
    /* unbind the buffer */
    unbind ();
}

/* clear_data
 *
 * clear the data from the buffer
 */
void glh::buffer::clear_data ()
{
    /* bind the buffer */
    bind ();
    /* empty buffer data */
    glBufferData ( target (), 0, NULL, GL_STATIC_DRAW );
    /* unbind the buffer */
    unbind ();
}

/* virtual destroy
 *
 * destroys the object, setting id to -1
 */
void glh::buffer::destroy ()
{
    /* destroy buffer */
    if ( is_valid () ) glDeleteBuffers ( 1, ( unsigned * ) &id );
    
    /* set id to -1 */
    id = -1;
}

/* bind
 *
 * bind the buffer
 * 
 * return: the target it is bound to
 */
GLenum glh::buffer::bind () const 
{ 
    /* check object is valid */
    if ( !is_valid () ) throw buffer_exception { "attempted bind operation on invalid buffer object" };
    
    /* bind the buffer */
    glBindBuffer ( target (), id );

    /* return the target */
    return target ();
}

/* unbind
 *
 * unbind the buffer's target
 * 
 * return: the target just unbound
 */
GLenum glh::buffer::unbind () const
{ 
    /* check object is valid */
    if ( !is_valid () ) throw buffer_exception { "attempted bind operation on invalid buffer object" };
    
    /* bind the buffer */
    glBindBuffer ( target (), 0 );

    /* return the target */
    return target ();
}