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



/* buffer IMPLEMENTATION */

/* constructor
 *
 * generates a buffer
 * 
 * _minor_type: the minor type of the buffer
 */
glh::core::buffer::buffer ( const minor_object_type _minor_type )
    : object { _minor_type }
    , capacity { 0 }
    , map_ptr { NULL }
    , map_id { 0 }
{ 
    /* assert major type is a buffer */ 
    if ( major_type != major_object_type::GLH_BUFFER_TYPE ) throw exception::buffer_exception { "attempted to construct buffer with non-buffer type" }; 
}

/* construct and immediately buffer data
 *
 * generates a buffer and immediately buffers data
 * 
 * _minor_type: the minor type of the buffer
 * size: size of data in bytes
 * data: pointer to data
 * usage: the storage method for the data
 */
glh::core::buffer::buffer ( const minor_object_type _minor_type, const GLsizeiptr size, const GLvoid * data, const GLenum usage )
    : object { _minor_type }
    , capacity { 0 }
    , map_ptr { NULL }
    , map_id { 0 }
{ 
    /* assert major type is a buffer */ 
    if ( major_type != major_object_type::GLH_BUFFER_TYPE ) throw exception::buffer_exception { "attempted to construct buffer with non-buffer type" }; 
    
    /* now buffer data */
    buffer_data ( size, data, usage );
}



/* buffer_data
 *
 * buffer data into the buffer
 * 
 * size: size of data in bytes
 * data: pointer to data
 * usage: the storage method for the data (defaults to static draw)
 */
void glh::core::buffer::buffer_data ( const GLsizeiptr size, const GLvoid * data, const GLenum usage ) 
{
    /* unmap */
    unmap_buffer ();
    ++map_id;

    /* bind the buffer */
    const bool binding_change = bind ();

    /* buffer data and change capacity */
    glBufferData ( gl_target, size, data, usage );
    capacity = size;

    /* unbind the buffer */
    if ( binding_change ) unbind ();
}

/* buffer_sub_data
 *
 * offset: offset in bytes of where to start writing
 * size: size of the data in bytes
 * data: pointer to data
 */
void glh::core::buffer::buffer_sub_data ( const GLintptr offset, const GLsizeiptr size, const GLvoid * data = NULL )
{
    std::cout << "hi\n";

    /* unmap */
    unmap_buffer ();

    /* bind the buffer */
    const bool binding_change = bind ();

    /* check offsets */
    if ( offset + size > capacity )
    throw exception::buffer_exception { "attempted to perform buffer sub data operation with incompatible paramaters for buffer capacity" };

    /* buffer data */
    glBufferSubData ( gl_target, offset, size, data );

    /* unbind the buffer */
    if ( binding_change ) unbind ();
}

/* copy_sub_data
 *
 * copy data FROM ANOTHER BUFFER INTO THIS BUFFER
 * a call to buffer_data is required to first resize this buffer to the capacity required
 *
 * read_buff: the buffer to read from
 * read/write_offset: the offsets for reading and writing
 * size: the number of bytes to copy
 */
void glh::core::buffer::copy_sub_data ( const buffer& read_buff, const GLintptr read_offset, const GLintptr write_offset, const GLsizeiptr size )
{
    /* unmap */
    unmap_buffer ();

    /* bind buffers */
    const bool read_binding_change = read_buff.bind_copy_read ();
    const bool write_binding_change = bind_copy_write ();

    /* check offsets */
    if ( read_offset + size > read_buff.get_capacity () || write_offset + size > capacity )
    throw exception::buffer_exception { "attempted to perform copy buffer sub data operation with incompatible paramaters for buffer capacities" };

    /* copy sub data */
    glCopyBufferSubData ( GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, read_offset, write_offset, size );

    /* unbind buffers */
    if ( write_binding_change ) unbind_copy_write ();
    if ( read_binding_change ) read_buff.unbind_copy_read ();
}

/* clear_data
 *
 * clear the data from the buffer
 */
void glh::core::buffer::clear_data ()
{
    /* unmap */
    unmap_buffer ();
    ++map_id;

    /* bind the buffer */
    const bool binding_change = bind ();

    /* empty buffer data */
    glBufferData ( gl_target, 0, NULL, GL_STATIC_DRAW );

    /* unbind the buffer */
    if ( binding_change ) unbind ();
}



/* bind_copy_read/write
 *
 * bind the buffer to the copy read/write targets
 * 
 * returns true if a change in binding occured
 */
bool glh::core::buffer::bind_copy_read () const
{
    /* throw if not valid */
    assert_is_object_valid ( "bind" );

    /* get the index of the copy read target */
    const unsigned copy_read_target_index = static_cast<unsigned> ( object_bind_target::GLH_COPY_READ_BUFFER_TARGET );

    /* if already bound, return false */
    if ( object_bindings.at ( copy_read_target_index ) == id ) return false;

    /* bind object */
    glBindBuffer ( GL_COPY_READ_BUFFER, id );

    /* record the binding */
    object_bindings.at ( copy_read_target_index ) = id;

    /* return true */
    return true;
}
bool glh::core::buffer::bind_copy_write () const
{
    /* throw if not valid */
    assert_is_object_valid ( "bind" );

    /* get the index of the copy write target */
    const unsigned copy_write_target_index = static_cast<unsigned> ( object_bind_target::GLH_COPY_WRITE_BUFFER_TARGET );

    /* if already bound, return false */
    if ( object_bindings.at ( copy_write_target_index ) == id ) return false;

    /* bind object */
    glBindBuffer ( GL_COPY_WRITE_BUFFER, id );

    /* record the binding */
    object_bindings.at ( copy_write_target_index ) = id;

    /* return true */
    return true;
}



/* unbind_copy_read/write
 *
 * unbind the buffer to the copy read/write targets
 * 
 * returns true if a change in binding occured
 */
bool glh::core::buffer::unbind_copy_read () const
{
    /* throw if not valid */
    assert_is_object_valid ( "unbind" );

    /* get the index of the copy read target */
    const unsigned copy_read_target_index = static_cast<unsigned> ( object_bind_target::GLH_COPY_READ_BUFFER_TARGET );

    /* if not bound, return false */
    if ( object_bindings.at ( copy_read_target_index ) != id ) return false;

    /* un bind object */
    glBindBuffer ( GL_COPY_READ_BUFFER, 0 );

    /* record the unbinding */
    object_bindings.at ( copy_read_target_index ) = 0;

    /* return true */
    return true;
}
bool glh::core::buffer::unbind_copy_write () const
{
    /* throw if not valid */
    assert_is_object_valid ( "unbind" );

    /* get the index of the copy write target */
    const unsigned copy_write_target_index = static_cast<unsigned> ( object_bind_target::GLH_COPY_WRITE_BUFFER_TARGET );

    /* if not bound, return false */
    if ( object_bindings.at ( copy_write_target_index ) != id ) false;

    /* unbind object */
    glBindBuffer ( GL_COPY_WRITE_BUFFER, 0 );

    /* record the unbinding */
    object_bindings.at ( copy_write_target_index ) = 0;

    /* return true */
    return true;
}



/* unbind_all
 *
 * unbind from all targets
 * this includes copy read/write targets
 * 
 * returns true if a change in binding occured
 */
bool glh::core::buffer::unbind_all () const
{
    /* unbind normally, as well as unbinding from any copy read/write targets */
    return ( unbind () | unbind_copy_read () | unbind_copy_write () );
}



/* is_copy_read/write_bound
 *
 * check if the buffer is bound to the copy read/write targets
 * 
 * returns true if a change in binding occured
 */
bool glh::core::buffer::is_copy_read_bound () const
{
    /* return true if is valid and is bound */
    return ( is_object_valid () && object_bindings.at ( static_cast<unsigned> ( object_bind_target::GLH_COPY_READ_BUFFER_TARGET ) ) == id );
}
bool glh::core::buffer::is_copy_write_bound () const
{
    /* return true if is valid and is bound */
    return ( is_object_valid () && object_bindings.at ( static_cast<unsigned> ( object_bind_target::GLH_COPY_WRITE_BUFFER_TARGET ) ) == id );
}



/* map_buffer
 *
 * generate a mapping, if not already mapped
 * 
 * return: the map to the buffer, or NULL on failure
 */
GLvoid * glh::core::buffer::map_buffer () const
{
    /* if already mapped, return that */
    if ( map_ptr ) return map_ptr;

    /* else generate the map */
    const bool binding_change = bind ();
    map_ptr = glMapBuffer ( gl_target, GL_READ_WRITE );
    if ( binding_change ) unbind ();

    /* return the map */
    return map_ptr;    
}

/* unmap_buffer
 *
 * unmaps the buffer, although the buffer will be remapped when an iterator is dereferenced
 */
void glh::core::buffer::unmap_buffer () const
{
    /* if not mapped, return */
    if ( !map_ptr ) return;

    /* otherwise destroy the map */
    const bool binding_change = bind ();
    glUnmapBuffer ( gl_target );
    if ( binding_change ) unbind ();

    /* set to NULL */
    map_ptr = NULL;
}



/* assert_not_is_buffer_mapped
 *
 * throws if the buffer is mapped
 * 
 * operationL the operation being performed
 */
void glh::core::buffer::assert_not_is_buffer_mapped ( const std::string& operation ) const
{
    /* throw if is mapped */
    if ( is_buffer_mapped () )
    {
        if ( operation.size () > 0 ) throw exception::buffer_exception { "attempted to perform " + operation + " operation while buffer is mapped" };
        else throw exception::buffer_exception { "attempted to perform operation while buffer is mapped" };
    }
}



/* UBO IMPLEMENTATION */

/* get_index_bound_ubo_pointer
 *
 * produce a pointer to the ubo currently bound to an index bind point
 * NULL is returned if no UBO is bound to that index
 * 
 * index: the index to produce the pointer from
 */
glh::core::ubo * glh::core::ubo::get_index_bound_ubo_pointer ( const unsigned index )
{
    /* if object is bound, return pointer to it */
    if ( ubo_indexed_bindings.size () > index && ubo_indexed_bindings.at ( index ) > 0 )
    {
        return dynamic_cast<ubo *> ( object_pointers.at ( static_cast<unsigned> ( major_object_type::GLH_BUFFER_TYPE ) ).at ( ubo_indexed_bindings.at ( index ) ) );
    }

    /* else return NULL */
    else return NULL;
}

/* bind/unbind_buffer_base
 *
 * special indexed bindings for ubos
 * 
 * returns true if a change in binding occured
 */
bool glh::core::ubo::bind_buffer_base ( const unsigned index )
{
    /* resize vector if necessary */
    if ( ubo_indexed_bindings.size () <= index ) ubo_indexed_bindings.resize ( index + 1, 0 );

    /* if already bound to index, return false */
    if ( ubo_indexed_bindings.at ( index ) == id ) return false;

    /* otherwise bind ubo to index */
    glBindBufferBase ( GL_UNIFORM_BUFFER, index, id );

    /* record binding */
    ubo_indexed_bindings.at ( index ) = id;

    /* return true */
    return true;
}
bool glh::core::ubo::unbind_buffer_base ( const unsigned index )
{
    /* if not bound to index, return false */
    if ( ubo_indexed_bindings.size () <= index || ubo_indexed_bindings.at ( index ) != id ) return false;

    /* otherwise unbind ubo from index */
    glBindBufferBase ( GL_UNIFORM_BUFFER, index, 0 );

    /* record binding */
    ubo_indexed_bindings.at ( index ) = 0;

    /* return false */
    return false;
}

/* is_bound_buffer_base
 *
 * returns true if is bound to the ubo index supplied
 */
bool glh::core::ubo::is_bound_buffer_base ( const unsigned index )
{
    /* return true if is valid and is bound to index */
    return ( is_object_valid () && ubo_indexed_bindings.size () > index && ubo_indexed_bindings.at ( index ) == id );
}




/* UBO STATIC MEMBERS DEFINITIONS */

/* records of ubo indexed bindings */
std::vector<GLuint> glh::core::ubo::ubo_indexed_bindings {};



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
    const bool vao_binding_change = bind ();
    /* bind vbo */
    const bool vbo_binding_change = buff.bind ();

    /* set attribute pointer */
    glVertexAttribPointer ( attrib, size, type, norm, stride, reinterpret_cast<GLvoid *> ( offset ) );
    /* enable attribute */
    glEnableVertexAttribArray ( attrib );

    /* add the attribute to vertex_attribs */
    if ( attrib >= vertex_attribs.size () ) vertex_attribs.resize ( attrib + 1, { 0, GL_NONE, GL_NONE, 0, 0, NULL, false } );
    vertex_attribs.at ( attrib ) = { size, type, norm, stride, offset, &buff, true };

    /* unbind vao */
    if ( vao_binding_change ) unbind ();
    /* unbind vbo */
    if ( vbo_binding_change ) buff.unbind ();
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
    const bool binding_change = bind ();
    glEnableVertexAttribArray ( attrib );
    vertex_attribs.at ( attrib ).enabled = true;
    if ( binding_change ) unbind ();
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
    const bool binding_change = bind ();
    glDisableVertexAttribArray ( attrib );
    vertex_attribs.at ( attrib ).enabled = false;
    if ( binding_change ) unbind ();
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
    const bool vao_binding_change = bind ();
    const bool ebo_binding_change = buff.bind ();

    /* set the ebo */
    bound_ebo = &buff;

    /* unbind vao, then ebo */
    if ( vao_binding_change ) unbind ();
    if ( ebo_binding_change ) buff.unbind ();
}



/* prepare_arrays
 * 
 * prepare for drawing from vertex arrays, not using an ebo
 * will not bind the vao, however
 * will throw if fails to prepare
 */
void glh::core::vao::prepare_arrays () const
{
    /* throw if invalid object */
    assert_is_object_valid ( "prepare arrays" );

    /* loop through enabled vertex attributes and check buffers are valid */
    for ( const auto& att: vertex_attribs )
    {
        if ( att.enabled ) 
        {
            /* assert is valid */
            att.buff->assert_is_object_valid ( "prepare arrays" );
            /* assure that buffer is not mapped */
            att.buff->unmap_buffer ();
        }
    }
}

/* prepare_elements
 *
 * prepare for drawing using an ebo
 * will not bind the vao, however
 * will throw if fails to prepare
 */
void glh::core::vao::prepare_elements () const
{
    /* prepare arrays */
    prepare_arrays ();

    /* assert that ebo is valid and that it is not mapped */
    if ( !bound_ebo ) throw exception::buffer_exception { "attempted to perform draw elements operation, however no ebo has been bound to the vao" };
    bound_ebo->assert_is_object_valid ( "draw elements" );
    bound_ebo->unmap_buffer ();
}