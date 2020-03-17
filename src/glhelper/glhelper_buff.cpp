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
    if ( id >= 0 ) glDeleteBuffers ( 1, ( unsigned * ) &id );
}

/* buffer_data
 *
 * buffer data into the buffer
 */
void glh::buffer::buffer_data ( const size_t size, const void * data, const GLenum usage ) 
{ 
    /* bind the buffer */
    bind ();
    /* buffer data */
    glBufferData ( type (), size, data, usage );
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
    if ( id >= 0 ) glDeleteBuffers ( 1, ( unsigned * ) &id );
    
    /* set id to -1 */
    id = -1;
}
