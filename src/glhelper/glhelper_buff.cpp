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
glh::buffer::buffer ( const GLenum _target, const GLenum _reverse_target )
    : target { _target }
    , reverse_target { _reverse_target }
{
    /* allocate the buffer */
    glGenBuffers ( 1, &id );
}



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



/* destroy
 *
 * destroys the object, setting id to 0
 */
void glh::buffer::destroy ()
{
    /* destroy buffer */
    if ( is_valid () ) glDeleteBuffers ( 1, &id );
    
    /* set id to 0 */
    id = 0;
}

/* bind
 *
 * bind the buffer
 * 
 * return: the target it is bound to
 */
const GLenum& glh::buffer::bind () const 
{ 
    /* check object is valid */
    if ( !is_valid () ) throw buffer_exception { "attempted bind operation on invalid buffer object" };
    
    /* bind the buffer, if not bound already */
    if ( !is_bound () ) glBindBuffer ( target, id );

    /* return the target */
    return target;
}

/* unbind
 *
 * unbind the buffer's target
 * 
 * return: the target just unbound
 */
const GLenum& glh::buffer::unbind () const
{ 
    /* check object is valid */
    if ( !is_valid () ) throw buffer_exception { "attempted bind operation on invalid buffer object" };
    
    /* unbind the buffer, if bound */
    if ( is_bound () ) glBindBuffer ( target, 0 );

    /* return the target */
    return target;
}

/* is_bound
 *
 * checks if the buffer is bound
 */
bool glh::buffer::is_bound () const
{
    /* get currently bound buffer */
    GLint bound_buffer;
    glGetIntegerv ( reverse_target, &bound_buffer ); 
    /* return boolean for if is valid and is bound */
    return ( is_valid () && bound_buffer == id ); 
}



/* VAO IMPLEMENTATION */

/* constructor
 *
 * creates a vertex array object without any vbo or ebo bound
 */
glh::vao::vao ()
{
    /* generate vao */
    glGenVertexArrays ( 1, &id );
}



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



/* destroy
 *
 * destroys the object, setting id to 0
 */
void glh::vao::destroy ()
{
    /* if object is valid */
    if ( is_valid () )
    {
        /* delete vao */
        glDeleteVertexArrays ( 1, &id );

        /* set id to 0 */
        id = 0;
    }
}



/* bind
 *
 * bind the vertex array object
 */
void glh::vao::bind () const
{
    /* check object is valid */
    if ( !is_valid () ) throw buffer_exception { "attempted bind operation on invalid vertex array object" };

    /* bind the vao, if not bound already */
    if ( !is_bound () ) glBindVertexArray ( id );
}

/* unbind
 *
 * unbind the vertex array object
 */
void glh::vao::unbind () const
{
    /* check object is valid */
    if ( !is_valid () ) throw buffer_exception { "attempted bind operation on invalid vertex array object" };

    /* unbind the vao, if bound */
    if ( is_bound () ) glBindVertexArray ( 0 );
}

/* is_bound
 *
 * checks if the vao is bound
 */
bool glh::vao::is_bound () const
{
    /* get currently bound vao */
    GLint bound_vao;
    glGetIntegerv ( GL_VERTEX_ARRAY_BINDING, &bound_vao ); 
    /* return boolean for if is valid and is bound */
    return ( is_valid () && bound_vao == id ); 
}