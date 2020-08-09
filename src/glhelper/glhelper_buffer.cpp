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
 */
glh::core::buffer::buffer ()
    : capacity { 0 }
    , map_ptr { NULL }
    , is_immutable { false }
{ 
    /* generate object */
    glGenBuffers ( 1, &id );
}

/* virtual destructor */
glh::core::buffer::~buffer ()
{
    /* destroy object */
    if ( id != 0 ) glDeleteBuffers ( 1, &id );
}



/* bind_copy_read/write
 *
 * bind the buffer to the copy read/write targets
 * 
 * returns true if a change in binding occured
 */
bool glh::core::buffer::bind_copy_read () const
{
    /* if bound, return false, otherwise bind and return true */
    if ( bound_copy_read_buffer == this ) return false;
    glBindBuffer ( GL_COPY_READ_BUFFER, id );
    bound_copy_read_buffer = const_cast<buffer *> ( this );
    return true;
}
bool glh::core::buffer::bind_copy_write () const
{
    /* if bound, return false, otherwise bind and return true */
    if ( bound_copy_write_buffer == this ) return false;
    glBindBuffer ( GL_COPY_WRITE_BUFFER, id );
    bound_copy_write_buffer = const_cast<buffer *> ( this );
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
    /* if not bound, return false, otherwise unbind and return true */
    if ( bound_copy_read_buffer != this ) return false;
    glBindBuffer ( GL_COPY_READ_BUFFER, 0 );
    bound_copy_read_buffer = NULL;
    return true;
}
bool glh::core::buffer::unbind_copy_write () const
{
    /* if not bound, return false, otherwise unbind and return true */
    if ( bound_copy_write_buffer != this ) return false;
    glBindBuffer ( GL_COPY_WRITE_BUFFER, 0 );
    bound_copy_write_buffer = NULL;
    return true;
}



/* buffer_storage with pointer
 *
 * size: the size of the data in bytes
 * data: pointer to the data (defaults to NULL)
 * flags: special storage flags (defaults to GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT)
 */
void glh::core::buffer::buffer_storage ( const unsigned size, const void * data, const GLbitfield flags )
{
    /* throw if immutable */
    if ( is_immutable ) throw exception::buffer_exception { "attempted to modify an immutable buffer" };

    /* set immutable flags */
    is_immutable = true; immutable_flags = flags;

    /* set data and change capacity */
    glNamedBufferStorage ( id, size, data, immutable_flags );
    capacity = size;
}



/* buffer_data
 *
 * buffer data into the buffer
 * 
 * size: size of data in bytes
 * data: pointer to data
 * usage: the storage method for the data (defaults to static draw)
 */
void glh::core::buffer::buffer_data ( const unsigned size, const void * data, const GLenum usage ) 
{
    /* throw if immutable */
    if ( is_immutable ) throw exception::buffer_exception { "attempted to modify an immutable buffer" };

    /* unmap */
    unmap_buffer ();

    /* buffer data and change capacity */
    glNamedBufferData ( id, size, data, usage );
    capacity = size;
}



/* buffer_sub_data
 *
 * offset: offset in bytes of where to start writing
 * size: size of the data in bytes
 * data: pointer to data
 */
void glh::core::buffer::buffer_sub_data ( const unsigned offset, const unsigned size, const void * data = NULL )
{
    /* throw if immutable */
    if ( is_immutable && ~immutable_flags & GL_DYNAMIC_STORAGE_BIT ) throw exception::buffer_exception { "attempted to modify an immutable buffer" };

    /* unmap */
    unmap_buffer ();

    /* check offsets */
    if ( offset + size > capacity )
    throw exception::buffer_exception { "attempted to perform buffer sub data operation with incompatible paramaters for buffer capacity" };

    /* buffer data */
    glNamedBufferSubData ( id, offset, size, data );
}



/* copy_sub_data
 *
 * copy data FROM ANOTHER BUFFER INTO THIS BUFFER
 * a call to buffer_data is required to first resize this buffer to the capacity required
 *
 * read_buff: the buffer to read from
 * size: the number of bytes to copy
 * read/write_offset: the offsets for reading and writing
 */
void glh::core::buffer::copy_sub_data ( const buffer& read_buff, const unsigned size, const unsigned read_offset, const unsigned write_offset )
{
    /* unmap */
    unmap_buffer ();
    read_buff.unmap_buffer ();

    /* check offsets */
    if ( read_offset + size > read_buff.get_size () || write_offset + size > capacity )
    throw exception::buffer_exception { "attempted to perform copy buffer sub data operation with incompatible paramaters for buffer capacities" };

    /* copy sub data */
    glCopyNamedBufferSubData ( read_buff.internal_id (), id, read_offset, write_offset, size );
}



/* clear_data
 *
 * clear the data from the buffer with a certain value
 * 
 * internal_format: the format of the data within the buffer
 * format: the format of the data being put into the buffer
 * type: the type of the data being put into the buffer
 * data: the data to be copied into the buffer
 */
void glh::core::buffer::clear_data ( const GLenum internal_format, const GLenum format, const GLenum type, const void * data )
{
    /* unmap the buffer */
    unmap_buffer ();

    /* clear data */
    glClearNamedBufferData ( id, internal_format, format, type, data );
}



/* map_buffer
 *
 * generate a mapping, if not already mapped
 * 
 * return: the map to the buffer, or NULL on failure
 */
void * glh::core::buffer::map_buffer () const
{
    /* if already mapped, return that */
    if ( map_ptr ) return map_ptr;

    /* else throw if immutable storage and correct flags are not set */
    if ( is_immutable && ( ~immutable_flags & GL_DYNAMIC_STORAGE_BIT || ~immutable_flags & GL_MAP_READ_BIT || ~immutable_flags & GL_MAP_WRITE_BIT ) )
        throw exception::buffer_exception { "cannot map immutable buffer without GL_DYNAMIC_STORAGE_BIT, GL_MAP_READ_BIT and GL_MAP_WRITE_BIT flags set" };

    /* else generate the map */
    map_ptr = glMapNamedBuffer ( id, GL_READ_WRITE );

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
    glUnmapNamedBuffer ( id );

    /* set to NULL */
    map_ptr = NULL;
}



/* bound_copy_read/write_buffer
 *
 * object pointers to the currently bound read and write buffers
 */
glh::core::object_pointer<glh::core::buffer> glh::core::buffer::bound_copy_read_buffer {};
glh::core::object_pointer<glh::core::buffer> glh::core::buffer::bound_copy_write_buffer {};



/* VBO IMPLEMENTATION */

/* default bind/unbind the vbo */
bool glh::core::vbo::bind () const
{
    /* if already bound, return false, else bind and return true */
    if ( bound_vbo == this ) return false;
    glBindBuffer ( GL_ARRAY_BUFFER, id );
    bound_vbo = const_cast<vbo *> ( this );
    return true;
}
bool glh::core::vbo::unbind () const
{
    /* if not bound, return false, else unbind and return true */
    if ( bound_vbo != this ) return false;
    glBindBuffer ( GL_ARRAY_BUFFER, 0 );
    bound_vbo = NULL;
    return true;
}



/* the currently bound vbo */
glh::core::object_pointer<glh::core::vbo> glh::core::vbo::bound_vbo {};



/* EBO IMPLEMENTATION */

/* default bind/unbind the ebo */
bool glh::core::ebo::bind () const
{
    /* if already bound, return false, else bind and return true */
    if ( bound_ebo == this ) return false;
    glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, id );
    bound_ebo = const_cast<ebo *> ( this );
    return true;
}
bool glh::core::ebo::unbind () const
{
    /* if not bound, return false, else unbind and return true */
    if ( bound_ebo != this ) return false;
    glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, 0 );
    bound_ebo = NULL;
    return true;
}



/* the currently bound ebo */
glh::core::object_pointer<glh::core::ebo> glh::core::ebo::bound_ebo {};



/* UBO IMPLEMENTATION */

/* zero-parameter constructor */
glh::core::ubo::ubo () 
{
    /* bind to set buffer type */
    bind (); unbind ();
    
    /* get buffer offset alignment */
    glGetIntegerv ( GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &uniform_buffer_offset_alignment ); 
}

/* construct and immediately buffer data
 *
 * generates a buffer and immediately buffers data
 * 
 * size: size of data in bytes
 * data: pointer to data
 * usage: the storage method for the data
 */
glh::core::ubo::ubo ( const unsigned size, const void * data, const GLenum usage )
{
    /* bind to set buffer type */
    bind (); unbind ();
    
    /* get buffer offset alignment */
    glGetIntegerv ( GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &uniform_buffer_offset_alignment ); 

    /* buffer data */
    buffer_data ( size, data, usage );
}



/* default bind/unbind the ubo */
bool glh::core::ubo::bind () const
{
    /* if already bound, return false, else bind and return true */
    if ( bound_ubo == this ) return false;
    glBindBuffer ( GL_UNIFORM_BUFFER, id );
    bound_ubo = const_cast<ubo *> ( this );
    return true;
}
bool glh::core::ubo::unbind () const
{
    /* if not bound, return false, else unbind and return true */
    if ( bound_ubo != this ) return false;
    glBindBuffer ( GL_UNIFORM_BUFFER, 0 );
    bound_ubo = NULL;
    return true;
}

/* bind the ubo to an index */
bool glh::core::ubo::bind ( const unsigned index ) const
{
    /* if already bound, return false, else bind and return true */
    if ( bound_ubo_indices.size () > index && bound_ubo_indices.at ( index ) == this ) return true;
    if ( bound_ubo_indices.size () <= index ) bound_ubo_indices.resize ( index + 1 );
    glBindBufferBase ( GL_UNIFORM_BUFFER, index, id );
    bound_ubo_indices.at ( index ) = const_cast<ubo *> ( this );
    return true;
}
bool glh::core::ubo::unbind ( const unsigned index ) const
{
    /* if not bound, return false, else unbind and return true */
    if ( bound_ubo_indices.size () <= index || bound_ubo_indices.at ( index ) != this ) return true;
    glBindBufferBase ( GL_UNIFORM_BUFFER, index, id );
    bound_ubo_indices.at ( index ) = NULL;
    return true;
}

/* unbind from all bind points */
bool glh::core::ubo::unbind_all () const
{
    /* record binding change */
    bool binding_change = false;

    /* default unbind, unbind read and write, and then unbind from all indices */
    binding_change |= unbind_copy_read ();
    binding_change |= unbind_copy_write ();
    binding_change |= unbind ();
    for ( unsigned i = 0; i < bound_ubo_indices.size (); ++i ) binding_change |= unbind ( i );

    /* return binding_change */
    return binding_change;
}



/* bound_ubo
 * bound_ubo_indices
 *
 * bound ubo and index-bound objects
 */
glh::core::object_pointer<glh::core::ubo> glh::core::ubo::bound_ubo {};
std::vector<glh::core::object_pointer<glh::core::ubo>> glh::core::ubo::bound_ubo_indices {};



/* SSBO IMPLEMENTATION */

/* default bind/unbind the ssbo */
bool glh::core::ssbo::bind () const
{
    /* if already bound, return false, else bind and return true */
    if ( bound_ssbo == this ) return false;
    glBindBuffer ( GL_SHADER_STORAGE_BUFFER, id );
    bound_ssbo = const_cast<ssbo *> ( this );
    return true;
}
bool glh::core::ssbo::unbind () const
{
    /* if not bound, return false, else unbind and return true */
    if ( bound_ssbo != this ) return false;
    glBindBuffer ( GL_SHADER_STORAGE_BUFFER, 0 );
    bound_ssbo = NULL;
    return true;
}

/* bind the ssbo to an index */
bool glh::core::ssbo::bind ( const unsigned index ) const
{
    /* if already bound, return false, else bind and return true */
    if ( bound_ssbo_indices.size () > index && bound_ssbo_indices.at ( index ) == this ) return true;
    if ( bound_ssbo_indices.size () <= index ) bound_ssbo_indices.resize ( index + 1 );
    glBindBufferBase ( GL_SHADER_STORAGE_BUFFER, index, id );
    bound_ssbo_indices.at ( index ) = const_cast<ssbo *> ( this );
    return true;
}
bool glh::core::ssbo::unbind ( const unsigned index ) const
{
    /* if not bound, return false, else unbind and return true */
    if ( bound_ssbo_indices.size () <= index || bound_ssbo_indices.at ( index ) != this ) return true;
    glBindBufferBase ( GL_SHADER_STORAGE_BUFFER, index, id );
    bound_ssbo_indices.at ( index ) = NULL;
    return true;
}

/* unbind from all bind points */
bool glh::core::ssbo::unbind_all () const
{
    /* record binding change */
    bool binding_change = false;

    /* default unbind, unbind read and write, and then unbind from all indices */
    binding_change |= unbind_copy_read ();
    binding_change |= unbind_copy_write ();
    binding_change |= unbind ();
    for ( unsigned i = 0; i < bound_ssbo_indices.size (); ++i ) binding_change |= unbind ( i );

    /* return binding_change */
    return binding_change;
}



/* bound_ssbo
 * bound_ssbo_indices
 *
 * bound ssbo and index-bound objects
 */
glh::core::object_pointer<glh::core::ssbo> glh::core::ssbo::bound_ssbo {};
std::vector<glh::core::object_pointer<glh::core::ssbo>> glh::core::ssbo::bound_ssbo_indices {};



/* VAO IMPLEMENTATION */

/* constructor
 *
 * creates a vertex array object
 */
glh::core::vao::vao ()
{
    /* generate the vao */
    glGenVertexArrays ( 1, &id );
}

/* default bind/unbind the vao */
bool glh::core::vao::bind () const
{
    /* if already bound, return false, else bind and return true */
    if ( bound_vao == this ) return false;
    glBindVertexArray ( id );
    bound_vao = const_cast<vao *> ( this );
    return true;
}
bool glh::core::vao::unbind () const
{
    /* if not bound, return false, else unbind and return true */
    if ( bound_vao != this ) return false;
    glBindVertexArray ( 0 );
    bound_vao = NULL;
    return true;
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
 * norm: boolean as to whether to normalize the vertex data
 * stride: offset between consecutive vertices in bytes
 * offset: the offset from the start of the vertex data in bytes
 */
void glh::core::vao::set_vertex_attrib ( const unsigned attrib, const vbo& buff, const int size, const GLenum type, const bool norm, const unsigned stride, const unsigned offset )
{
    /* bind vao */
    const bool vao_binding_change = bind ();
    /* bind vbo */
    const bool vbo_binding_change = buff.bind ();

    /* set attribute pointer */
    glVertexAttribPointer ( attrib, size, type, norm, stride, reinterpret_cast<void *> ( offset ) );
    /* enable attribute */
    glEnableVertexAttribArray ( attrib );

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
void glh::core::vao::enable_vertex_attrib ( const unsigned attrib )
{
    /* bind vao, enable, unbind */
    const bool binding_change = bind ();
    glEnableVertexAttribArray ( attrib );
    if ( binding_change ) unbind ();
}

/* disable_vertex_attrib
 *
 * disable a vertex attribute
 * 
 * attrib: the attribute to configure (>=0)
 */
void glh::core::vao::disable_vertex_attrib ( const unsigned attrib )
{
    /* bind vao, disable, unbind */
    const bool binding_change = bind ();
    glDisableVertexAttribArray ( attrib );
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

    /* unbind vao, then ebo */
    if ( vao_binding_change ) unbind ();
    if ( ebo_binding_change ) buff.unbind ();
}



/* the currently bound vao */
glh::core::object_pointer<glh::core::vao> glh::core::vao::bound_vao {};