/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * include/glhelper/glhelper_buffer.hpp
 * 
 * constructs for creating and managing buffer objects
 * notable constructs include:
 * 
 * 
 * 
 * CLASS GLH::CORE::BUFFER
 * 
 * generic buffer and the base class for more specific buffer types
 * it provides core functionality for buffering data and binding the buffer
 * the type of buffer is defined by giving the class the bind target (e.g. GL_ARRAY_BUFFER)
 * 
 * 
 * 
 * CLASS GLH::CORE::FORWARD/BACKWARD_BUFFER_ITERATOR
 * 
 * iterators to allow for access to a buffer C++ style
 * the buffer is mapped and unmapped implicity and as required
 * 
 * 
 * 
 * CLASS GLH::CORE::VBO, GLH::CORE::EBO and GLH::CORE::UBO
 * 
 * derived from buffer base class specifically for vertex and element buffer objects
 * sets defaults for the bind target to that specific type of buffer and specilaised methods
 * for details on a ubo see below
 *
 * 
 * 
 * CLASS GLH::CORE::VAO
 * 
 * a vertex array object (does not inherit from buffer base class as a vao is not a buffer per se)
 * one buffer can be bound to each vertex attribute using the set_vertex_attrib method
 * one ebo can be bound to the vao using the bind_ebo method 
 * 
 */



/* HEADER GUARD */
#ifndef GLHELPER_BUFFER_HPP_INCLUDED
#define GLHELPER_BUFFER_HPP_INCLUDED



/* INCLUDES */

/* include core headers */
#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <type_traits>
#include <vector>

/* include glhelper_core.hpp */
#include <glhelper/glhelper_core.hpp>

/* include glhelper_math.hpp */
#include <glhelper/glhelper_math.hpp>



/* NAMESPACE DECLARATIONS */

namespace glh
{
    namespace core
    {
        /* class buffer : object
         *
         * class for storing a buffer
         */
        class buffer;

        /* class vbo : buffer
         *
         * vertex buffer object
         */
        class vbo;

        /* class ebo : buffer
         *
         * element buffer object
         */
        class ebo;

        /* class ubo : buffer
         *
         * uniform buffer object
         */
        class ubo;

        /* class vao : object
         *
         * vertex array object
         */
        class vao;
    }

    namespace exception
    {
        /* class buffer_exception : exception
         *
         * for exceptions related to buffers
         */
        class buffer_exception;
    }
}



/* buffer DEFINITION */

/* class buffer : object
 *
 * class for storing a buffer
 */
class glh::core::buffer : public object
{
public:

    /* constructor
     *
     * generates a buffer
     */
    buffer ();

    /* deleted copy constructor */
    buffer ( const buffer& other ) = delete;

    /* default move constructor */
    buffer ( buffer&& other ) = default;

    /* deleted copy assignment operator */
    buffer& operator= ( const buffer& other ) = delete;

    /* virtual destructor */
    virtual ~buffer ();



    /* bind_copy_read/write
     *
     * bind the buffer to the copy read/write targets
     * 
     * returns true if a change in binding occured
     */
    bool bind_copy_read () const;
    bool bind_copy_write () const;

    /* unbind_copy_read/write
     *
     * unbind the buffer to the copy read/write targets
     * 
     * returns true if a change in binding occured
     */
    bool unbind_copy_read () const;
    bool unbind_copy_write () const;

    /* unbind_all
     *
     * unbind from all targets
     * this includes copy read/write targets
     * 
     * returns true if a change in binding occured
     */
    virtual bool unbind_all () const { return ( unbind () | unbind_copy_read () | unbind_copy_write () ); }

    /* is_copy_read/write_bound
     *
     * check if the buffer is bound to the copy read/write targets
     */
    bool is_copy_read_bound () const { return bound_copy_read_buffer == this; }
    bool is_copy_write_bound () const { return bound_copy_write_buffer == this; }

    /* get_bound_copy_read/write_buffer
     *
     * return a reference to the bound copy read/write buffers
     */
    static const object_pointer<buffer>& get_bound_copy_read_buffer () { return bound_copy_read_buffer; }
    static const object_pointer<buffer>& get_bound_copy_write_buffer () { return bound_copy_write_buffer; }



    /* buffer_storage with pointer
     *
     * size: the size of the data in bytes
     * data: pointer to the data (defaults to NULL)
     * flags: special storage flags (defaults to GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT)
     */
    void buffer_storage ( const unsigned size, const void * data = NULL, const GLbitfield flags = GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT );

    /* buffer_storage with iterators
     *
     * first/last: iterators for the data (ie. from begin and end)
     * flags: special storage flags (defaults to GL_DYNAMIC_STORAGE_BIT), GL_MAP_READ_BIT and GL_MAP_WRITE_BIT are forced to be present
     */
    template<class It> void buffer_storage ( It first, It last, const GLbitfield flags = GL_DYNAMIC_STORAGE_BIT );



    /* buffer_data with pointer
     *
     * size: size of data in bytes
     * data: pointer to data (defaults to NULL)
     * usage: the storage method for the data (defaults to static draw)
     */
    void buffer_data ( const unsigned size, const void * data = NULL, const GLenum usage = GL_STATIC_DRAW );

    /* buffer_data with iterators
     *
     * first/last: iterators for the data (ie. from begin and end)
     * usage: the storage method for the data (defaults to static draw)
     */
    template<class It> void buffer_data ( It first, It last, const GLenum usage = GL_STATIC_DRAW );

    /* buffer_sub_data with pointer
     *
     * offset: offset in bytes of where to start writing
     * size: size of the data in bytes
     * data: pointer to data
     */
    void buffer_sub_data ( const unsigned offset, const unsigned size, const void * data );

    /* buffer_sub_data with iterators
     *
     * first/last: iterators for the data (ie. from begin and end)
     * offset: index of the buffer to write the elements
     */
    template<class It> void buffer_sub_data ( It first, It last, const unsigned offset );

    /* copy_sub_data
     *
     * copy data FROM ANOTHER BUFFER INTO THIS BUFFER
     * a call to buffer_data is required to first resize this buffer to the capacity required
     *
     * read_buff: the buffer to read from
     * read/write_offset: the offsets for reading and writing
     * size: the number of bytes to copy
     */
    void copy_sub_data ( const buffer& read_buff, const unsigned read_offset, const unsigned write_offset, const unsigned size );

    /* clear_data
     *
     * clear the data from the buffer
     */
    void clear_data ();




    /* at
     *
     * return a reference to a value of type T at offset i
     */
    template<class T> T& at ( const unsigned i );
    template<class T> const T& at ( const unsigned i ) const;



    /* map_buffer
     *
     * generate a mapping, if not already mapped
     * 
     * return: the map to the buffer, or NULL on failure
     */
    void * map_buffer () const;

    /* unmap_buffer
     *
     * unmaps the buffer, although the buffer will be remapped when an iterator is dereferenced
     */
    void unmap_buffer () const;



    /* is_buffer_mapped
     *
     * returns true if the buffer is mapped
     */
    bool is_buffer_mapped () const { return map_ptr; }

    /* assert_not_is_buffer_mapped
     *
     * throws if the buffer is mapped
     * 
     * operationL the operation being performed
     */
    void assert_not_is_buffer_mapped ( const std::string& operation ) const;



    /* get_capacity
     *
     * return the capacity of the buffer in bytes
     */
    const unsigned& get_capacity () const { return capacity; }



private:

    /* NON-STATIC MEMBERS */

    /* unsigned capacity
     *
     * the number of bytes allocated to the buffer
     */
    unsigned capacity;

    /* void * map_ptr
     *
     * pointer to the current map to the buffer (NULL for no map)
     */
    mutable void * map_ptr;

    /* unsigned map_id
     *
     * the map id, which is incremented every time the buffer_data is called
     * this ensures that outdated iterators know when they are outdated
     */
    mutable unsigned map_id;

    /* is_immutable
     *
     * true if the buffer has been set by buffer_storage, and is now immutable
     */
    bool is_immutable;

    /* immutable_flags
     *
     * flags set when the buffer was set up as immutable
     */
    GLbitfield immutable_flags;



    /* STATIC MEMBERS */

    /* bound_copy_read/write_buffer
     *
     * object pointers to the currently bound read and write buffers
     */
    static object_pointer<buffer> bound_copy_read_buffer;
    static object_pointer<buffer> bound_copy_write_buffer;

};



/* VBO DEFINITION */

/* class vbo : buffer
 *
 * vertex buffer object
 */
class glh::core::vbo : public buffer 
{
public:

    /* zero-parameter constructor */
    vbo () { bind (); }

    /* construct and immediately buffer data with pointer
     *
     * generates a buffer and immediately buffers data
     * 
     * size: size of data in bytes
     * data: pointer to data
     * usage: the storage method for the data
     */
    vbo ( const unsigned size, const void * data = NULL, const GLenum usage = GL_STATIC_DRAW )
        { bind (); buffer_data ( size, data, usage ); }

    /* construct and immediately buffer data with iterators
     *
     * generates a buffer and immediately buffers data
     * 
     * first/last: iterators for the data (ie. from begin and end)
     * usage: the storage method for the data
     */
    template<class It> vbo ( It first, It last, const GLenum usage = GL_STATIC_DRAW )
        { bind (); buffer_data ( first, last, usage ); }

    /* deleted copy constructor */
    vbo ( const vbo& other ) = delete;

    /* default move constructor */
    vbo ( vbo&& other ) = default;

    /* deleted copy assignment operator */
    vbo& operator= ( const vbo& other ) = delete;

    /* default destructor */
    ~vbo () = default;



    /* default bind/unbind the vbo */
    bool bind () const;
    bool unbind () const;
    bool is_bound () const { return bound_vbo == this; }

    /* get the currently bound vbo */
    static const object_pointer<vbo>& get_bound_vbo () { return bound_vbo; }



private:

    /* the currently bound vbo */
    static object_pointer<vbo> bound_vbo;

};



/* EBO DEFINITION */

/* class ebo : buffer
 *
 * element buffer object
 */
class glh::core::ebo : public buffer 
{
public:

    /* zero-parameter constructor */
    ebo () { bind (); }

    /* construct and immediately buffer data with pointer
     *
     * generates a buffer and immediately buffers data
     * 
     * size: size of data in bytes
     * data: pointer to data
     * usage: the storage method for the data
     */
    ebo ( const unsigned size, const void * data = NULL, const GLenum usage = GL_STATIC_DRAW )
        { bind (); buffer_data ( size, data, usage ); }

    /* construct and immediately buffer data with iterators
     *
     * generates a buffer and immediately buffers data
     * 
     * first/last: iterators for the data (ie. from begin and end)
     * usage: the storage method for the data
     */
    template<class It> ebo ( It first, It last, const GLenum usage = GL_STATIC_DRAW )
        { bind (); buffer_data ( first, last, usage ); }

    /* deleted copy constructor */
    ebo ( const ebo& other ) = delete;

    /* default move constructor */
    ebo ( ebo&& other ) = default;

    /* deleted copy assignment operator */
    ebo& operator= ( const ebo& other ) = delete;

    /* default destructor */
    ~ebo () = default;



    /* default bind/unbind the ebo */
    bool bind () const;
    bool unbind () const;
    bool is_bound () const { return bound_ebo == this; }

    /* get the currently bound ebo */
    static const object_pointer<ebo>& get_bound_ebo () { return bound_ebo; }



private:

    /* the currently bound ebo */
    static object_pointer<ebo> bound_ebo;

};



/* UBO DEFINITION */

/* class ubo : buffer
 *
 * uniform buffer object
 */
class glh::core::ubo : public buffer
{
public:

    /* zero-parameter constructor */
    ubo ();

    /* construct and immediately buffer data with pointer
     *
     * generates a buffer and immediately buffers data
     * 
     * size: size of data in bytes
     * data: pointer to data
     * usage: the storage method for the data
     */
    ubo ( const unsigned size, const void * data = NULL, const GLenum usage = GL_STATIC_DRAW );

    /* construct and immediately buffer data with iterators
     *
     * generates a buffer and immediately buffers data
     * 
     * first/last: iterators for the data (ie. from begin and end)
     * usage: the storage method for the data
     */
    template<class It> ubo ( It first, It last, const GLenum usage = GL_STATIC_DRAW );

    /* deleted copy constructor */
    ubo ( const ubo& other ) = delete;

    /* default move constructor */
    ubo ( ubo&& other ) = default;

    /* deleted copy assignment operator */
    ubo& operator= ( const ubo& other ) = delete;

    /* default destructor */
    ~ubo () = default;



    /* default bind/unbind the ubo */
    bool bind () const;
    bool unbind () const;
    bool is_bound () const { return bound_ubo == this; }

    /* bind the ubo to an index */
    bool bind ( const unsigned index ) const;
    bool unbind ( const unsigned index ) const;
    bool is_bound ( const unsigned index ) const { return bound_ubo_indices.size () > index && bound_ubo_indices.at ( index ) == this; }

    /* unbind from all bind points */
    bool unbind_all () const;

    /* get bound ubo and bound ubo index */
    static const object_pointer<ubo>& get_bound_ubo () { return bound_ubo; }
    static object_pointer<ubo> get_bound_ubo_index ( const unsigned index ) 
        { return ( bound_ubo_indices.size () > index ?bound_ubo_indices.at ( index ) : object_pointer<ubo> { NULL } ); }



private:

    /* bound_ubo
     * bound_ubo_indices
     *
     * bound ubo and index-bound objects
     */
    static object_pointer<ubo> bound_ubo;
    static std::vector<object_pointer<ubo>> bound_ubo_indices;

    /* uniform_buffer_offset_alignment
     *
     * constant defined by the implementation for uniform alignment in ubos
     */
    int uniform_buffer_offset_alignment;

};



/* VAO DEFINITION */

/* class vao : object
 *
 * vertex array object
 */
class glh::core::vao : public object
{
public:

    /* constructor
     *
     * creates a vertex array object without any vbo or ebo bound
     */
    vao ();

    /* deleted copy constructor */
    vao ( const vao& other ) = delete;

    /* default move constructor */
    vao ( vao&& other ) = default;

    /* deleted copy assignment operator */
    vao& operator= ( const vao& other ) = delete;

    /* default destructor */
    ~vao () = default;



    /* default bind/unbind the vao */
    bool bind () const;
    bool unbind () const;
    bool is_bound () const { return bound_vao == this; }

    /* get the currently bound vao */
    static const object_pointer<vao>& get_bound_vao () { return bound_vao; }



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
    void set_vertex_attrib ( const unsigned attrib, const vbo& buff, const int size, const GLenum type, const bool norm, const unsigned stride, const unsigned offset );

    /* enable_vertex_attrib
     *
     * enable a vertex attribute
     * 
     * attrib: the attribute to configure (>=0)
     */
    void enable_vertex_attrib ( const unsigned attrib );

    /* disable_vertex_attrib
     *
     * disable a vertex attribute
     * 
     * attrib: the attribute to configure (>=0)
     */
    void disable_vertex_attrib ( const unsigned attrib );

    /* bind_ebo
     *
     * binds an element buffer object to the vao
     *
     * buff: the ebo to bind
     */
    void bind_ebo ( const ebo& buff );



    /* prepare_arrays
     * 
     * prepare for drawing from vertex arrays, not using an ebo
     * will not bind the vao, however
     * will throw if fails to prepare
     */
    void prepare_arrays () const;

    /* prepare_elements
     *
     * prepare for drawing using an ebo
     * will not bind the vao, however
     * will throw if fails to prepare
     */
    void prepare_elements () const;



private:

    /* the currently bound vao */
    static object_pointer<vao> bound_vao;

    /* struct to represent a vertex attribute */
    struct vertex_attrib
    {
        int size;
        GLenum type;
        GLenum norm;
        unsigned stride;
        unsigned offset;
        const_object_pointer<vbo> buff;
        bool enabled;
    };

    /* array of vertex attributes */
    std::vector<vertex_attrib> vertex_attribs;

    /* bound ebo */
    const_object_pointer<ebo> bound_ebo;

};



/* BUFFER_EXCEPTION DEFINITION */

/* class buffer_exception : exception
 *
 * for exceptions related to buffers
 */
class glh::exception::buffer_exception : public exception
{
public:

    /* full constructor
     *
     * __what: description of the exception
     */
    explicit buffer_exception ( const std::string& __what )
        : exception { __what }
    {}

    /* default zero-parameter constructor
     *
     * construct buffer_exception with no descrption
     */
    buffer_exception () = default;

    /* default everything else and inherits what () function */

};




/* BUFFER IMPLEMENTATION */

/* buffer_storage with iterators
 *
 * first/last: iterators for the data (ie. from begin and end)
 * flags: special storage flags (defaults to GL_DYNAMIC_STORAGE_BIT), GL_MAP_READ_BIT and GL_MAP_WRITE_BIT are forced to be present
 */
template<class It> inline void glh::core::buffer::buffer_storage ( It first, It last, const GLbitfield flags )
{
    /* throw if immutable */
    if ( is_immutable ) throw exception::buffer_exception { "attempted to modify an immutable buffer" };

    /* set immutable flags */
    is_immutable = true; immutable_flags = flags | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT;

    /* set data and change capacity */
    glNamedBufferStorage ( id, std::distance ( first, last ) * sizeof ( typename std::iterator_traits<It>::value_type ), NULL, immutable_flags );
    capacity = std::distance ( first, last ) * sizeof ( typename std::iterator_traits<It>::value_type );

    /* copy data */
    std::copy ( first, last, reinterpret_cast<typename std::iterator_traits<It>::pointer> ( map_buffer () ) );
}

/* buffer_data with iterators
 *
 * first/last: iterators for the data (ie. from begin and end)
 * usage: the storage method for the data (defaults to static draw)
 */
template<class It> inline void glh::core::buffer::buffer_data ( It first, It last, const GLenum usage )
{
    /* resize data */
    buffer_data ( std::distance ( first, last ) * sizeof ( typename std::iterator_traits<It>::value_type ), NULL, usage );

    /* copy data */
    std::copy ( first, last, reinterpret_cast<typename std::iterator_traits<It>::pointer> ( map_buffer () ) );
}

/* buffer_sub_data with iterators
 *
 * first/last: iterators for the data (ie. from begin and end)
 * offset: index of buffer to write the elements
 */
template<class It> inline void glh::core::buffer::buffer_sub_data ( It first, It last, const unsigned offset )
{
    /* check will fit capacity */
    if ( ( ( last - first ) + offset ) * sizeof ( typename std::iterator_traits<It>::value_type ) > capacity )
    throw exception::buffer_exception { "attempted to perform buffer sub data operation with incompatible paramaters for buffer capacity" };

    /* purely copy using iterators */
    std::copy ( first, last, reinterpret_cast<typename std::iterator_traits<It>::pointer> ( map_buffer () ) + offset );    
}

/* at
 *
 * return a reference to a value of type T at offset i
 */
template<class T> inline T& glh::core::buffer::at ( const unsigned i )
{
    /* check i is in range, then return */
    if ( ( i + 1 ) * sizeof ( T ) > capacity ) throw exception::buffer_exception { "attempted to get reference to out of range object in buffer" };
    return * reinterpret_cast<T *> ( map_buffer () ) + i;
}
template<class T> inline const T& glh::core::buffer::at ( const unsigned i ) const
{
    /* check i is in range, then return */
    if ( ( i + 1 ) * sizeof ( T ) > capacity ) throw exception::buffer_exception { "attempted to get reference to out of range object in buffer" };
    return * reinterpret_cast<const T *> ( map_buffer () ) + i;
}



/* UBO IMPLEMENTATION */

/* construct and immediately buffer data with iterators
 * 
 * generates a buffer and immediately buffers data
 * 
 * first/last: iterators for the data (ie. from begin and end)
 * usage: the storage method for the data
 */
template<class It> inline glh::core::ubo::ubo ( It first, It last, const GLenum usage )
{
    /* bind to set buffer type */
    bind (); 
    
    /* get buffer offset alignment */
    glGetIntegerv ( GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &uniform_buffer_offset_alignment ); 

    /* buffer data */
    buffer_data ( first, last, usage );
}



/* #ifndef GLHELPER_BUFFER_HPP_INCLUDED */
#endif