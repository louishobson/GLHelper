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

        /* class buffer_iterator_base 
         * class forward_buffer_iterator : buffer_iterator_base
         * class reverse_buffer_iterator : buffer_iterator_base
         *
         * iterators for a buffer
         */
        template<class T> class buffer_iterator_base;
        template<class T> class forward_buffer_iterator;
        template<class T> class reverse_buffer_iterator;

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

    /* friend of buffer_iterator_base */
    template<class T> friend class buffer_iterator_base;

public:

    /* constructor
     *
     * generates a buffer
     * 
     * _minor_type: the minor type of the buffer
     */
    buffer ( const minor_object_type _minor_type );

    /* construct and immediately buffer data
     *
     * generates a buffer and immediately buffers data
     * 
     * _minor_type: the minor type of the buffer
     * size: size of data in bytes
     * data: pointer to data
     * usage: the storage method for the data (defaults to GL_STATIC_DRAW)
     */
    buffer ( const minor_object_type _minor_type, const GLsizeiptr size, const GLvoid * data = NULL, const GLenum usage = GL_STATIC_DRAW );

    /* construct and immediately buffer data from iterators
     *
     * generates a buffer and immediately buffers data from iterators
     * 
     * _minor_type: the minor type of the buffer
     * first/last: iterators to the beginning and end of the data to buffer
     * usage: the storage method for the data (defaults to GL_STATIC_DRAW)
     */
    template<class It> buffer ( const minor_object_type _minor_type, It first, It last, const GLenum usage = GL_STATIC_DRAW )
        : buffer { _minor_type, std::distance ( first, last ) * sizeof ( typename std::iterator_traits<It>::value_type ), NULL, usage }
    { std::copy ( first, last, begin<typename std::iterator_traits<It>::value_type> () ); }

    /* deleted zero-parameter constructor */
    buffer () = delete;

    /* deleted copy constructor */
    buffer ( const buffer& other ) = delete;

    /* default move constructor */
    buffer ( buffer&& other ) = default;

    /* deleted copy assignment operator */
    buffer& operator= ( const buffer& other ) = delete;

    /* default virtual destructor */
    virtual ~buffer () = default;



    /* iterator using declarations */
    template<class T> using iterator = forward_buffer_iterator<T>;
    template<class T> using const_iterator = forward_buffer_iterator<const T>;
    template<class T> using reverse_iterator = reverse_buffer_iterator<T>;
    template<class T> using const_reverse_iterator = reverse_buffer_iterator<const T>;



    /* buffer_data with pointer
     *
     * size: size of data in bytes
     * data: pointer to data (defaults to NULL)
     * usage: the storage method for the data (defaults to static draw)
     */
    void buffer_data ( const GLsizeiptr size, const GLvoid * data = NULL, const GLenum usage = GL_STATIC_DRAW );

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
    void buffer_sub_data ( const GLintptr offset, const GLsizeiptr size, const GLvoid * data );

    /* buffer_sub_data with iterators
     *
     * first/last: iterators for the data (ie. from begin and end)
     * offset: iterator to the position in buffer to write the elements
     */
    template<class It> void buffer_sub_data ( It first, It last, iterator<typename std::iterator_traits<It>::value_type> offset );

    /* copy_sub_data
     *
     * copy data FROM ANOTHER BUFFER INTO THIS BUFFER
     * a call to buffer_data is required to first resize this buffer to the capacity required
     *
     * read_buff: the buffer to read from
     * read/write_offset: the offsets for reading and writing
     * size: the number of bytes to copy
     */
    void copy_sub_data ( const buffer& read_buff, const GLintptr read_offset, const GLintptr write_offset, const GLsizeiptr size );

    /* clear_data
     *
     * clear the data from the buffer
     */
    void clear_data ();



    /* iterator-getting methods
     *
     * get an iterator to the buffers storage
     * all iterators are invalidated on a call to buffer_data  
     */
    template<class T> iterator<T> begin () { return iterator<T> { 0, map_id, * this }; }
    template<class T> const_iterator<T> begin () const { return iterator<T> { 0, map_id, * this }; }
    template<class T> const_iterator<T> cbegin () const { return iterator<T> { 0, map_id, * this }; }
    template<class T> reverse_iterator<T> rbegin () { return iterator<T> { ( capacity / sizeof ( T ) ) - 1, map_id, * this }; }
    template<class T> const_reverse_iterator<T> rbegin () const { return iterator<T> { ( capacity / sizeof ( T ) ) - 1, map_id, * this }; }
    template<class T> const_reverse_iterator<T> crbegin () const { return iterator<T> { ( capacity / sizeof ( T ) ) - 1, map_id, * this }; }

    template<class T> iterator<T> end () { return iterator<T> { capacity / sizeof ( T ), map_id, * this }; }
    template<class T> const_iterator<T> end () const { return iterator<T> { capacity / sizeof ( T ), map_id, * this }; }
    template<class T> const_iterator<T> cend () const { return iterator<T> { capacity / sizeof ( T ), map_id, * this }; }
    template<class T> reverse_iterator<T> rend () { return iterator<T> { -1, map_id, * this }; }
    template<class T> const_reverse_iterator<T> rend () const { return iterator<T> { -1, map_id, * this }; }
    template<class T> const_reverse_iterator<T> crend () const { return iterator<T> { -1, map_id, * this }; }

    /* at
     *
     * return a reference to a value of type T at offset i
     */
    template<class T> T& at ( const unsigned i );
    template<class T> const T& at ( const unsigned i ) const;



    /* bind_copy_read/write
     *
     * bind the buffer to the copy read/write targets
     * 
     * returns true if a change in binding occured
     */
    bool bind_copy_read () const { return bind ( object_bind_target::GLH_COPY_READ_BUFFER_TARGET ); }
    bool bind_copy_write () const  { return bind ( object_bind_target::GLH_COPY_WRITE_BUFFER_TARGET ); }

    /* unbind_copy_read/write
     *
     * unbind the buffer to the copy read/write targets
     * 
     * returns true if a change in binding occured
     */
    bool unbind_copy_read () const { return unbind ( object_bind_target::GLH_COPY_READ_BUFFER_TARGET ); }
    bool unbind_copy_write () const  { return unbind ( object_bind_target::GLH_COPY_READ_BUFFER_TARGET ); }

    /* unbind_all
     *
     * unbind from all targets
     * this includes copy read/write targets
     * 
     * returns true if a change in binding occured
     */
    bool unbind_all () const { return ( unbind () | unbind_copy_read () | unbind_copy_write () ); }

    /* is_copy_read/write_bound
     *
     * check if the buffer is bound to the copy read/write targets
     */
    bool is_copy_read_bound () const { return is_bound ( object_bind_target::GLH_COPY_READ_BUFFER_TARGET ); }
    bool is_copy_write_bound () const { return is_bound ( object_bind_target::GLH_COPY_READ_BUFFER_TARGET ); }



    /* map_buffer
     *
     * generate a mapping, if not already mapped
     * 
     * return: the map to the buffer, or NULL on failure
     */
    GLvoid * map_buffer () const;

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
    const GLsizeiptr& get_capacity () const { return capacity; }



private:

    /* GLsizeiptr capacity
     *
     * the number of bytes allocated to the buffer
     */
    GLsizeiptr capacity;

    /* GLvoid * map_ptr
     *
     * pointer to the current map to the buffer (NULL for no map)
     */
    mutable GLvoid * map_ptr;

    /* unsigned map_id
     *
     * the map id, which is incremented every time the buffer_data is called
     * this ensures that outdated iterators know when they are outdated
     */
    unsigned map_id;

};



/* BUFFER ITERATOR DEFINITIONS */

/* class buffer_iterator_base 
 * class forward_buffer_iterator : buffer_iterator_base
 * class reverse_buffer_iterator : buffer_iterator_base
 *
 * iterators for a buffer
 */
template<class T> class glh::core::buffer_iterator_base
{
public:

    /* full constructor
     *
     * construct iterator from an offset, map id and a reference to the buffer object
     *
     * _offset: offset in multiples of T from the start of the buffer
     * _map_id: the id of this map (supplied by buffer class)
     * _buff: the buffer object the map is for
     */
    buffer_iterator_base ( const int _offset, const unsigned _map_id, std::conditional_t<std::is_const<T>::value, const buffer&, buffer&> _buff )
        : map_ptr { reinterpret_cast<T *> ( NULL ) }
        , offset_map_ptr { reinterpret_cast<T *> ( _offset * sizeof ( T ) ) }
        , map_id { _map_id }
        , buff { _buff }
    {}

    /* deleted zero-parameter constructor */
    buffer_iterator_base () = delete;

    /* construct from non-cv version */
    buffer_iterator_base ( const buffer_iterator_base<std::remove_const_t<T>>& other )
        : map_ptr { other.map_ptr }
        , offset_map_ptr { other.offset_map_ptr }
        , map_id { other.map_id }
        , buff { other.buff }
    {}


    /* default copy assignment operator */
    buffer_iterator_base& operator= ( const buffer_iterator_base& other ) = default;

    /* default destructor */
    ~buffer_iterator_base () = default;



    /* iterator typedefs */
    typedef T value_type;
    typedef std::ptrdiff_t difference_type;
    typedef T * pointer;
    typedef T& reference;
    typedef std::random_access_iterator_tag iterator_category;



    /* dereferencing methods */
    T& operator* () const { return * get_map_pointer (); }
    T& operator-> () const { return * get_map_pointer (); }



    /* iterator comparison operators */
    bool operator== ( const buffer_iterator_base& other ) const { return ( this->offset_map_ptr - this->map_ptr == other.offset_map_ptr - other.map_ptr ); }
    bool operator!= ( const buffer_iterator_base& other ) const { return ( this->offset_map_ptr - this->map_ptr != other.offset_map_ptr - other.map_ptr ); }



protected:

    /* pointer to the map and the current index of the map */
    mutable T * map_ptr;
    mutable T * offset_map_ptr;

    /* id for the map */
    const unsigned map_id;

    /* reference to the buffer that is mapped */
    std::conditional_t<std::is_const<T>::value, const buffer&, buffer&> buff;



    /* get_map_pointer
     *
     * get a pointer to the position in the map after applying the offset
     * will throw if the iterator has been invalidated or offset causes an out of range error
     */
    T * get_map_pointer () const;

};
template<class T> class glh::core::forward_buffer_iterator : public buffer_iterator_base<T>
{
public:

    /* full constructor
     *
     * construct iterator from an offset, map id and a reference to the buffer object
     *
     * _offset: offset in the map in multiples of T
     * _map_id: the id of this map (supplied by buffer class)
     * _buff: the buffer object the map is for
     */
    forward_buffer_iterator ( const int _offset, const unsigned _map_id, std::conditional_t<std::is_const<T>::value, const buffer&, buffer&> _buff )
        : buffer_iterator_base<T> { _offset, _map_id, _buff }
    {}

    /* deleted zero-parameter constructor */
    forward_buffer_iterator () = delete;

    /* construct from non-cv version */
    forward_buffer_iterator ( const forward_buffer_iterator<std::remove_const_t<T>>& other )
        : buffer_iterator_base<T> { other }
    {}

    /* default destructor */
    ~forward_buffer_iterator () = default;



    /* iterator-modifying operators */
    forward_buffer_iterator& operator++ () { ++this->offset_map_ptr; return * this; }
    forward_buffer_iterator& operator-- () { --this->offset_map_ptr; return * this; }
    forward_buffer_iterator operator++ ( int ) { auto tmp = * this; ++this->offset_map_ptr; return tmp; }
    forward_buffer_iterator operator-- ( int ) { auto tmp = * this; --this->offset_map_ptr; return tmp; }
    forward_buffer_iterator& operator+= ( const int scalar ) { this->offset_map_ptr += scalar; return * this; }
    forward_buffer_iterator& operator-= ( const int scalar ) { this->offset_map_ptr -= scalar; return * this; }
    forward_buffer_iterator operator+ ( const int scalar ) const { auto tmp = * this; tmp += scalar; return tmp; }
    forward_buffer_iterator operator- ( const int scalar ) const { auto tmp = * this; tmp -= scalar; return tmp; }



    /* difference operator */
    std::ptrdiff_t operator- ( const forward_buffer_iterator& other ) const { return ( this->offset_map_ptr - this->map_ptr ) - ( other.offset_map_ptr - other.map_ptr ); }

};
template<class T> class glh::core::reverse_buffer_iterator : public buffer_iterator_base<T>
{
public:

    /* full constructor
     *
     * construct iterator from an offset, map id and a reference to the buffer object
     *
     * _offset: offset in the map in multiples of T
     * _map_id: the id of this map (supplied by buffer class)
     * _buff: the buffer object the map is for
     */
    reverse_buffer_iterator ( const int _offset, const unsigned _map_id, std::conditional_t<std::is_const<T>::value, const buffer&, buffer&> _buff )
        : buffer_iterator_base<T> { _offset, _map_id, _buff }
    {}

    /* deleted zero-parameter constructor */
    reverse_buffer_iterator () = delete;

    /* construct from non-cv version */
    reverse_buffer_iterator ( const reverse_buffer_iterator<std::remove_const_t<T>>& other )
        : buffer_iterator_base<T> { other }
    {}

    /* default destructor */
    ~reverse_buffer_iterator () = default;



    /* iterator-modifying operators */
    reverse_buffer_iterator& operator++ () { --this->offset_map_ptr; return * this; }
    reverse_buffer_iterator& operator-- () { ++this->offset_map_ptr; return * this; }
    reverse_buffer_iterator operator++ ( int ) { auto tmp = * this; --this->offset_map_ptr; return tmp; }
    reverse_buffer_iterator operator-- ( int ) { auto tmp = * this; ++this->offset_map_ptr; return tmp; }
    reverse_buffer_iterator& operator+= ( const int scalar ) { this->offset_map_ptr -= scalar; return * this; }
    reverse_buffer_iterator& operator-= ( const int scalar ) { this->offset_map_ptr += scalar; return * this; }
    reverse_buffer_iterator operator+ ( const int scalar ) const { auto tmp = * this; tmp += scalar; return tmp; }
    reverse_buffer_iterator operator- ( const int scalar ) const { auto tmp = * this; tmp -= scalar; return tmp; }



    /* difference operator */
    std::ptrdiff_t operator- ( const reverse_buffer_iterator& other ) const { return ( other.offset_map_ptr - other.map_ptr ) - ( this->offset_map_ptr - this->map_ptr ); }

};



/* VBO DEFINITION */

/* class vbo : buffer
 *
 * vertex buffer object
 */
class glh::core::vbo : public buffer 
{
public:

    /* constructor
     *
     * generates the buffer
     */
    vbo ()
        : buffer { minor_object_type::GLH_VBO_TYPE }
    {}

    /* construct and immediately buffer data
     *
     * generates a buffer and immediately buffers data
     * 
     * size: size of data in bytes
     * data: pointer to data
     * usage: the storage method for the data
     */
    vbo ( const GLsizeiptr size, const GLvoid * data = NULL, const GLenum usage = GL_STATIC_DRAW )
        : buffer { minor_object_type::GLH_VBO_TYPE, size, data, usage }
    {}

    /* deleted copy constructor */
    vbo ( const vbo& other ) = delete;

    /* default move constructor */
    vbo ( vbo&& other ) = default;

    /* deleted copy assignment operator */
    vbo& operator= ( const vbo& other ) = delete;

    /* default destructor */
    ~vbo () = default;



    /* get_bound_object_pointer
     *
     * produce a pointer to the vbo currently bound
     */
    using object::get_bound_object_pointer;
    static object_pointer<vbo> get_bound_object_pointer () { return get_bound_object_pointer<vbo> ( object_bind_target::GLH_VBO_TARGET ); }

};



/* EBO DEFINITION */

/* class ebo : buffer
 *
 * element buffer object
 */
class glh::core::ebo : public buffer 
{
public:

    /* constructor
     *
     * generates the buffer
     */
    ebo ()
        : buffer { minor_object_type::GLH_EBO_TYPE }
    {}

    /* construct and immediately buffer data
     *
     * generates a buffer and immediately buffers data
     * 
     * size: size of data in bytes
     * data: pointer to data
     * usage: the storage method for the data
     */
    ebo ( const GLsizeiptr size, const GLvoid * data = NULL, const GLenum usage = GL_STATIC_DRAW )
        : buffer { minor_object_type::GLH_EBO_TYPE, size, data, usage }
    {}

    /* deleted copy constructor */
    ebo ( const ebo& other ) = delete;

    /* default move constructor */
    ebo ( ebo&& other ) = default;

    /* deleted copy assignment operator */
    ebo& operator= ( const ebo& other ) = delete;

    /* default destructor */
    ~ebo () = default;



    /* get_bound_object_pointer
     *
     * produce a pointer to the ebo currently bound
     */
    using object::get_bound_object_pointer;
    static object_pointer<ebo> get_bound_object_pointer () { return get_bound_object_pointer<ebo> ( object_bind_target::GLH_EBO_TARGET ); }

};



/* UBO DEFINITION */

/* class ubo : buffer
 *
 * uniform buffer object
 */
class glh::core::ubo : public buffer
{
public:

    /* constructor
     *
     * generates the buffer
     */
    ubo ()
        : buffer { minor_object_type::GLH_UBO_TYPE }
    { glGetIntegerv ( GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &uniform_buffer_offset_alignment ); }

    /* construct and immediately buffer data
     *
     * generates a buffer and immediately buffers data
     * 
     * size: size of data in bytes
     * data: pointer to data
     * usage: the storage method for the data
     */
    ubo ( const GLsizeiptr size, const GLvoid * data = NULL, const GLenum usage = GL_STATIC_DRAW )
        : buffer { minor_object_type::GLH_UBO_TYPE, size, data, usage }
    { glGetIntegerv ( GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &uniform_buffer_offset_alignment ); }

    /* deleted copy constructor */
    ubo ( const ubo& other ) = delete;

    /* default move constructor */
    ubo ( ubo&& other ) = default;

    /* deleted copy assignment operator */
    ubo& operator= ( const ubo& other ) = delete;

    /* default destructor */
    ~ubo () = default;



    /* get_bound_object_pointer
     *
     * produce a pointer to the ebo currently bound
     */
    using object::get_bound_object_pointer;
    static object_pointer<ubo> get_bound_object_pointer () { return get_bound_object_pointer<ubo> ( object_bind_target::GLH_UBO_TARGET ); }

    /* get_index_bound_ubo_pointer
     *
     * produce a pointer to the ubo currently bound to an index bind point
     * 
     * index: the index to produce the pointer from
     */
    static object_pointer<ubo> get_index_bound_ubo_pointer ( const unsigned index );



    /* bind/unbuffer_base
     *
     * special indexed bindings for ubos
     * 
     * returns true if a change in binding occured
     */
    bool bind_buffer_base ( const unsigned index );
    bool unbind_buffer_base ( const unsigned index );

    /* is_bound_buffer_base
     *
     * returns true if is bound to the ubo index supplied
     */
    bool is_bound_buffer_base ( const unsigned index );
    


private:

    /* records of ubo indexed bindings */
    static std::vector<GLuint> ubo_indexed_bindings;

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
    vao ()
        : object { minor_object_type::GLH_VAO_TYPE }
        , vertex_attribs { 8, { 0, GL_NONE, GL_NONE, 0, 0, {}, false } }
        , bound_ebo {}
    {}

    /* deleted copy constructor */
    vao ( const vao& other ) = delete;

    /* default move constructor */
    vao ( vao&& other ) = default;

    /* deleted copy assignment operator */
    vao& operator= ( const vao& other ) = delete;

    /* default destructor */
    ~vao () = default;



    /* get_bound_object_pointer
     *
     * produce a pointer to the vao currently bound
     */
    using object::get_bound_object_pointer;
    static object_pointer<vao> get_bound_object_pointer () { return get_bound_object_pointer<vao> ( object_bind_target::GLH_VAO_TARGET ); }



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
    void set_vertex_attrib ( const GLuint attrib, const vbo& buff, const GLint size, const GLenum type, const GLboolean norm, const GLsizei stride, const GLsizeiptr offset );

    /* enable_vertex_attrib
     *
     * enable a vertex attribute
     * 
     * attrib: the attribute to configure (>=0)
     */
    void enable_vertex_attrib ( const GLuint attrib );

    /* disable_vertex_attrib
     *
     * disable a vertex attribute
     * 
     * attrib: the attribute to configure (>=0)
     */
    void disable_vertex_attrib ( const GLuint attrib );

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

    /* struct to represent a vertex attribute */
    struct vertex_attrib
    {
        GLint size;
        GLenum type;
        GLenum norm;
        GLsizei stride;
        GLsizeiptr offset;
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

/* buffer_data with iterators
 *
 * first/last: iterators for the data (ie. from begin and end)
 * usage: the storage method for the data (defaults to static draw)
 */
template<class It> void glh::core::buffer::buffer_data ( It first, It last, const GLenum usage )
{
    /* resize data */
    buffer_data ( std::distance ( first, last ) * sizeof ( typename std::iterator_traits<It>::value_type ), NULL, usage );

    /* copy data */
    std::copy ( first, last, begin<typename std::iterator_traits<It>::value_type> () );
}

/* buffer_sub_data with iterators
 *
 * first/last: iterators for the data (ie. from begin and end)
 * offset: iterator to the position in buffer to write the elements
 */
template<class It> void glh::core::buffer::buffer_sub_data ( It first, It last, iterator<typename std::iterator_traits<It>::value_type> offset )
{
    /* check will fit capacity */
    if ( ( ( last - first ) + ( offset - begin<typename std::iterator_traits<It>::value_type> () ) ) * sizeof ( typename std::iterator_traits<It>::value_type ) > capacity )
    throw exception::buffer_exception { "attempted to perform buffer sub data operation with incompatible paramaters for buffer capacity" };

    /* purely copy using iterators */
    std::copy ( first, last, offset );    
}

/* at
 *
 * return a reference to a value of type T at offset i
 */
template<class T> T& glh::core::buffer::at ( const unsigned i )
{
    /* check i is in range, then return */
    if ( ( i + 1 ) * sizeof ( T ) > capacity ) throw exception::buffer_exception { "attempted to get reference to out of range object in buffer" };
    return * reinterpret_cast<T *> ( map_buffer () ) + i;
}
template<class T> const T& glh::core::buffer::at ( const unsigned i ) const
{
    /* check i is in range, then return */
    if ( ( i + 1 ) * sizeof ( T ) > capacity ) throw exception::buffer_exception { "attempted to get reference to out of range object in buffer" };
    return * reinterpret_cast<const T *> ( map_buffer () ) + i;
}




/* BUFFER_ITERATOR_BASE IMPLEMENTATION */

/* get_map_pointer
 *
 * get a pointer to the position in the map after applying the offset
 * will throw if the iterator has been invalidated or offset causes an out of range error
 */
template<class T> inline T * glh::core::buffer_iterator_base<T>::get_map_pointer () const
{
    /* check id matches */
    if ( this->map_id != this->buff.map_id ) throw exception::buffer_exception { "attempted to dereference invalidated buffer iterator" };

    /* check range */
    if ( this->offset_map_ptr < this->map_ptr || this->offset_map_ptr > this->map_ptr + this->buff.get_capacity () - 1 ) throw exception::buffer_exception { "attempted to dereference out of range buffer iterator"};

    /* if map location has changed, update it */
    if ( this->map_ptr != reinterpret_cast<T *> ( this->buff.map_buffer () ) ) 
    {
        const int offset = this->offset_map_ptr - this->map_ptr;
        this->map_ptr = reinterpret_cast<T *> ( this->buff.map_buffer () );
        this->offset_map_ptr = reinterpret_cast<T *> ( reinterpret_cast<GLbyte *> ( this->map_ptr ) + offset );
    }

    /* return offset map */
    return this->offset_map_ptr;
}



/* #ifndef GLHELPER_BUFFER_HPP_INCLUDED */
#endif