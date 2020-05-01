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



/* BUFFER_BASE IMPLEMENTATION */

/* buffer_data
 *
 * buffer data into the buffer
 * 
 * size: size of data in bytes
 * data: pointer to data
 * usage: the storage method for the data (defaults to static draw)
 */
void glh::core::buffer_base::buffer_data ( const GLsizeiptr size, const GLvoid * data, const GLenum usage ) 
{
    /* assert is not mapped */
    assert_is_not_mapped ( "buffer data" );

    /* bind the buffer */
    bind ();

    /* buffer data and change capacity */
    glBufferData ( target, size, data, usage );
    capacity = size;

    /* unbind the buffer */
    unbind ();
}

/* buffer_sub_data
 *
 * offset: offset in bytes of where to start writing
 * size: size of the data in bytes
 * data: pointer to data
 */
void glh::core::buffer_base::buffer_sub_data ( const GLintptr offset, const GLsizeiptr size, const GLvoid * data = NULL )
{
    /* assert is not mapped */
    assert_is_not_mapped ( "buffer sub data" );

    /* bind the buffer */
    bind ();

    /* buffer data */
    glBufferSubData ( target, offset, size, data );

    /* unbind the buffer */
    unbind ();
}

/* clear_data
 *
 * clear the data from the buffer
 */
void glh::core::buffer_base::clear_data ()
{
    /* assert is not mapped */
    assert_is_not_mapped ( "clear data" );

    /* bind the buffer */
    bind ();
    /* empty buffer data */
    glBufferData ( target, 0, NULL, GL_STATIC_DRAW );
    /* unbind the buffer */
    unbind ();
}

/* unmap
 *
 * unmaps the buffer, making all existing maps invalid
 */
void glh::core::buffer_base::unmap ()
{
    /* unmap only if is already mapped
     * it is not an error to call unmap multiple times
     */
    if ( is_mapped () )
    {
        /* set map_ptr to NULL and increment map_id */
        map_ptr = NULL;
        ++map_id;
        
        /* unmap the buffer, throwing if fails */
        bind ();
        if ( glUnmapBuffer ( target ) != GL_TRUE ) throw exception::buffer_mapping_exception { "failed to unmap buffer" };
        unbind ();
    }
}

/* bind
 *
 * bind the buffer
 */
void glh::core::buffer_base::bind () const
{
    /* switch for different targets */
    switch ( target )
    {
        case GL_ARRAY_BUFFER: object_manager::bind_vbo ( id ); break;
        case GL_ELEMENT_ARRAY_BUFFER: object_manager::bind_ebo ( id ); break;
        default: exception::object_management_exception { "attempted to perform bind operation on unknown buffer object" };
    }
}

/* unbind
 *
 * unbind the buffer's target
 */
void glh::core::buffer_base::unbind () const
{
    /* switch for different targets */
    switch ( target )
    {
        case GL_ARRAY_BUFFER: object_manager::unbind_vbo ( id ); break;
        case GL_ELEMENT_ARRAY_BUFFER: object_manager::unbind_ebo ( id ); break;
        default: throw exception::object_management_exception { "attempted to perform bind operation on unknown buffer object" };
    }
}

/* is_bound
 *
 * checks if the buffer is bound
 */
bool glh::core::buffer_base::is_bound () const
{
    /* switch for different targets */
    switch ( target )
    {
        case GL_ARRAY_BUFFER: return object_manager::is_vbo_bound ( id );
        case GL_ELEMENT_ARRAY_BUFFER: return object_manager::is_ebo_bound ( id );
        default: throw exception::object_management_exception { "attempted to perform bind check of unknown buffer object" };
    }
}

/* generate_map
 *
 * generate a mapping, if not already mapped
 * 
 * return: the map to the buffer, or NULL on failure
 */
GLvoid * glh::core::buffer_base::generate_map ()
{
    /* if already mapped, return that */
    if ( map_ptr ) return map_ptr;

    /* else generate the map */
    bind ();
    map_ptr = glMapBuffer ( target, GL_READ_WRITE );
    unbind ();

    /* return the map */
    return map_ptr;    
}

/* assert_is_not_mapped
 *
 * throws if the buffer is mapped
 * 
 * operation: description of the operation
 */
void glh::core::buffer_base::assert_is_not_mapped ( const std::string& operation ) const
{
    /* if mapped, throw */
    if ( is_mapped () ) 
    {
        if ( operation.size () > 0 ) throw exception::buffer_mapping_exception { "attempted to perform " + operation + " operation while buffer is mapped" };
        else throw exception::buffer_mapping_exception { "attempted to perform operation while buffer is mapped" };
    }
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
void glh::core::vao::set_vertex_attrib ( const GLuint attrib, const vbo& buff, const GLint size, const GLenum type, const GLboolean norm, const GLsizei stride, const GLsizeiptr offset )
{
    /* bind vao */
    bind ();
    /* bind vbo */
    buff.bind ();

    /* set attribute pointer */
    glVertexAttribPointer ( attrib, size, type, norm, stride, reinterpret_cast<GLvoid *> ( offset ) );
    /* enable attribute */
    glEnableVertexAttribArray ( attrib );

    /* add the attribute to vertex_attribs */
    if ( attrib >= vertex_attribs.size () ) vertex_attribs.resize ( attrib + 1, { 0, GL_NONE, GL_NONE, 0, 0, NULL, false } );
    vertex_attribs.at ( attrib ) = { size, type, norm, stride, offset, &buff, true };

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
void glh::core::vao::enable_vertex_attrib ( const GLuint attrib )
{
    /* bind vao, enable, unbind */
    bind ();
    glEnableVertexAttribArray ( attrib );
    vertex_attribs.at ( attrib ).enabled = true;
    unbind ();
}

/* disable_vertex_attrib
 *
 * disable a vertex attribute
 * 
 * attrib: the attribute to configure (>=0)
 */
void glh::core::vao::disable_vertex_attrib ( const GLuint attrib )
{
    /* bind vao, disable, unbind */
    bind ();
    glDisableVertexAttribArray ( attrib );
    vertex_attribs.at ( attrib ).enabled = false;
    unbind ();
}

/* bind_ebo
 *
 * binds an element buffer object to the vao
 *
 * buff: the ebo to bind
 */
void glh::core::vao::bind_ebo ( const ebo& buff )
{
    /* bind vao, then ebo */
    bind ();
    buff.bind ();

    /* set the ebo */
    bound_ebo = &buff;

    /* unbind vao, then ebo */
    unbind ();
    buff.unbind ();
}

/* is_draw_arrays_valid
 *
 * returns true if it is safe to draw arrays from the vao
 * may be false if attributes are invalid, or a buffer is mapped
 */
bool glh::core::vao::is_draw_arrays_valid () const
{   
    /* return false if invalid object */
    if ( !is_object_valid () ) return false;

    /* loop through enabled vertex attributes and check buffers are valid */
    for ( const auto& att: vertex_attribs )
    {
        if ( att.enabled ) if ( !att.buff || !att.buff->is_object_valid () || att.buff->is_mapped () ) return false;
    }

    /* else return true */
    return true;
}

/* is_draw_elements_valid
 *
 * returns true if it is safe to draw arrays from the vao
 * may be false if attributes are invalid, or a buffer is mapped
 */
bool glh::core::vao::is_draw_elements_valid () const
{
    /* check is draw arrays valid and ebo is a valid object */
    if ( !is_draw_arrays_valid () || !bound_ebo || !bound_ebo->is_object_valid () || bound_ebo->is_mapped () );

    /* else return true */
    return true;
}


/* assert_is_draw_arrays_valid
 *
 * throws if the vao is not draw arrays valid
 * 
 * operation: description of the operation
 */
void glh::core::vao::assert_is_draw_arrays_valid ( const std::string& operation ) const
{
    /* throw if invalid object */
    assert_is_object_valid ( operation );

    /* loop through enabled vertex attributes and check buffers are valid */
    for ( const auto& att: vertex_attribs )
    {
        if ( att.enabled ) 
        {
            att.buff->assert_is_object_valid ( operation ); 
            att.buff->assert_is_not_mapped ();
        }
    }
}

/* assert_is_draw_elements_valid
 *
 * throws if the vao is not draw elements valid
 * 
 * operation: description of the operation
 */
void glh::core::vao::assert_is_draw_elements_valid ( const std::string& operation ) const
{
    /* throw if not draw arrays valid */
    assert_is_draw_arrays_valid ( operation );
    
    /* throw if bound ebo is not valid */
    if ( !bound_ebo )
    {
        if ( operation.size () > 0 ) throw exception::invalid_object_exception { "attempted to perform " + operation + " operation without an ebo being associated with a vao" };
        else throw exception::invalid_object_exception { "attempted to perform operation without an ebo being associated with a vao" };
    }
    bound_ebo->assert_is_object_valid ( operation );
    bound_ebo->assert_is_not_mapped ( operation );
}