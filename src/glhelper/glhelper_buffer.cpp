/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * src/glhelper/glhelper_buffer.cpp
 * 
 * implementation of include/glhelper/glhelper_buffer.hpp
 * 
 */



/* INCLUDES */

/* include glhelper_buffer.hpp */
#include <glhelper/glhelper_buffer.hpp>



/* BUFFER IMPLEMENTATION */

/* buffer_data
 *
 * buffer data into the buffer
 * 
 * size: size of data in bytes
 * data: pointer to data
 * usage: the storage method for the data
 */
void glh::buffer::buffer_data ( const GLsizeiptr size, const GLvoid * data, const GLenum usage ) 
{
    /* bind the buffer */
    bind ();
    /* buffer data */
    glBufferData ( target, size, data, usage );
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
    glBufferData ( target, 0, NULL, GL_STATIC_DRAW );
    /* unbind the buffer */
    unbind ();
}

/* bind
 *
 * bind the buffer
 */
void glh::buffer::bind () const
{
    /* split for different targets */
    if ( target == GL_ARRAY_BUFFER ) object_manager::bind_vbo ( id ); else
    if ( target == GL_ELEMENT_ARRAY_BUFFER ) object_manager::bind_ebo ( id ); else
    throw object_management_exception { "attempted to bind unknown buffer object" };
}

/* unbind
 *
 * unbind the buffer's target
 */
void glh::buffer::unbind () const
{
    /* split for different targets */
    if ( target == GL_ARRAY_BUFFER ) object_manager::unbind_vbo ( id ); else
    if ( target == GL_ELEMENT_ARRAY_BUFFER ) object_manager::unbind_ebo ( id ); else
    throw object_management_exception { "attempted to unbind unknown buffer object" };
}

/* is_bound
 *
 * checks if the buffer is bound
 */
bool glh::buffer::is_bound () const
{
    /* split for different targets */
    if ( target == GL_ARRAY_BUFFER ) return object_manager::is_vbo_bound ( id ); else
    if ( target == GL_ELEMENT_ARRAY_BUFFER ) return object_manager::is_ebo_bound ( id ); else
    throw object_management_exception { "attempted to check binding of unknown buffer object" };
}




/* VAO IMPLEMENTATION */

/* set_vertex_attrib
 *
 * configures a vertex attribute of the vao
 * also implicitly enables the vertex attribute
 * 
 * attrib: the attribute to configure (>=0)
 * buff: the vertex buffer object to bind to the attribute
 * size: components per vertex (1, 2, 3 or 4)
 * type: the type of each component of each vertex
 * norm: boolean as to whether to normalise the vertex data
 * stride: offset between consecutive vertices in bytes
 * offset: the offset from the start of the vertex data in bytes
 */
void glh::vao::set_vertex_attrib ( const GLuint attrib, const vbo& buff, const GLint size, const GLenum type, const GLboolean norm, const GLsizei stride, const GLvoid * offset )
{
    /* bind vao */
    bind ();
    /* bind vbo */
    buff.bind ();

    /* set attribute pointer */
    glVertexAttribPointer ( attrib, size, type, norm, stride, offset );
    /* enable attribute */
    glEnableVertexAttribArray ( attrib );

    /* unbind vao */
    unbind ();
    /* unbind vbo */
    buff.unbind ();
}

/* enable_vertex_attrib
 *
 * enable a vertex attribute
 * 
 * attrib: the attribute to configure (>=0)
 */
void glh::vao::enable_vertex_attrib ( const GLuint attrib )
{
    /* bind vao, enable, unbind */
    bind ();
    glEnableVertexAttribArray ( attrib );
    unbind ();
}

/* disable_vertex_attrib
 *
 * disable a vertex attribute
 * 
 * attrib: the attribute to configure (>=0)
 */
void glh::vao::disable_vertex_attrib ( const GLuint attrib )
{
    /* bind vao, disable, unbind */
    bind ();
    glDisableVertexAttribArray ( attrib );
    unbind ();
}

/* bind_ebo
 *
 * binds an element buffer object to the vao
 *
 * buff: the ebo to bind
 */
void glh::vao::bind_ebo ( const ebo& buff )
{
    /* bind vao, then ebo */
    bind ();
    buff.bind ();

    /* unbind vao, then ebo */
    unbind ();
    buff.unbind ();
}