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
 * CLASS GLH::CORE::BUFFER_MAP
 * 
 * returned by map method of buffer class so that a buffer can be accessed like an array
 * after mapping, the buffer must be unmapped before any other modification or draw calls are made to it
 * 
 * 
 * 
 * CLASS GLH::CORE::VBO and GLH::CORE::EBO
 * 
 * derived from buffer base class specifically for vertex and element buffer objects
 * purely sets defaaults for the bind target to that specific type of buffer
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
#include <iostream>
#include <string>
#include <vector>

/* include glhelper_core.hpp */
#include <glhelper/glhelper_core.hpp>

/* include glhelper_exception.hpp */
#include <glhelper/glhelper_exception.hpp>

/* include glhelper_manager.hpp */
#include <glhelper/glhelper_manager.hpp>



/* NAMESPACE FORWARD DECLARATIONS */

namespace glh
{
    namespace core
    {
        /* class buffer : object
         *
         * class for storing a buffer
         */
        class buffer;

        /* class buffer_map
         *
         * object returned by the map_buffer methods of buffer
         * has operator overloads to act like an array to modify the buffer
         */
        template<class T> class buffer_map;

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
     * 
     * _target: the target for the buffer
     */
    buffer ( const GLenum _target )
        : object { object_manager::generate_buffer () }
        , target { _target }
        , capacity { 0 }
        , map_ptr { NULL }
        , map_id { 0 }
    {}

    /* construct and immediately buffer data
     *
     * generates a buffer and immediately buffers data
     * 
     * _target: the target for the buffer (e.g. GL_ARRAY_BUFFER)
     * size: size of data in bytes
     * data: pointer to data
     * usage: the storage method for the data
     */
    buffer ( const GLenum _target, const GLsizeiptr size, const GLvoid * data = NULL, const GLenum usage = GL_STATIC_DRAW )
        : object { object_manager::generate_buffer () }
        , target { _target }
        , capacity { 0 }
        , map_ptr { NULL }
        , map_id { 0 }
    { buffer_data ( size, data, usage ); }

    /* deleted zero-parameter constructor */
    buffer () = delete;

    /* deleted copy constructor */
    buffer ( const buffer& other ) = delete;

    /* default move constructor */
    buffer ( buffer&& other ) = default;

    /* deleted copy assignment operator */
    buffer& operator= ( const buffer& other ) = delete;

    /* virtual destructor */
    virtual ~buffer () { destroy (); }



    /* buffer_data
     * 
     * size: size of data in bytes
     * data: pointer to data (defaults to NULL)
     * usage: the storage method for the data (defaults to static draw)
     */
    void buffer_data ( const GLsizeiptr size, const GLvoid * data = NULL, const GLenum usage = GL_STATIC_DRAW );

    /* buffer_sub_data
     *
     * offset: offset in bytes of where to start writing
     * size: size of the data in bytes
     * data: pointer to data
     */
    void buffer_sub_data ( const GLintptr offset, const GLsizeiptr size, const GLvoid * data );

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



    /* map(_ro)
     *
     * maps the buffer to a buffer_map, if not already mapped
     * 
     * return: the mapped buffer
     */
    template<class T> buffer_map<T> map ();
    template<class T> buffer_map<const T> map_ro ();

    /* unmap
     *
     * unmaps the buffer, making all existing maps invalid
     */
    void unmap ();

    /* is_mapped
     *
     * with no parameters, returns true if the buffer is currently in a mapped state
     * with a map parameter, returns true if the map supplied is currently valid
     */
    bool is_mapped () const { return ( is_object_valid () && map_ptr ); }
    template<class T> bool is_mapped ( const buffer_map<T>& map ) const;

    /* assert_is_not_mapped
     *
     * throws if the buffer is mapped
     * 
     * operation: description of the operation
     */
    void assert_is_not_mapped ( const std::string& operation = "" ) const;



    /* destroy
     *
     * destroys the object, setting id to 0
     */
    void destroy () { object_manager::destroy_buffer ( id ); id = 0; }

    /* bind
     *
     * bind the buffer
     */
    void bind () const;

    /* bind_copy_read/write
     *
     * bind the buffer to the copy read/write targets
     */
    void bind_copy_read () const { object_manager::bind_copy_read_buffer ( id ); }
    void bind_copy_write () const { object_manager::bind_copy_write_buffer ( id ); }

    /* unbind_copy_read/write
     *
     * unbind the buffer to the copy read/write targets
     */
    void unbind_copy_read () const { object_manager::unbind_copy_read_buffer ( id ); }
    void unbind_copy_write () const { object_manager::unbind_copy_write_buffer ( id ); }

    /* is_copy_read/write_bound
     *
     * check if the buffer is bound to the copy read/write targets
     */
    bool is_copy_read_bound () const { return object_manager::is_copy_read_buffer_bound ( id ); }
    bool is_copy_write_bound () const { return object_manager::is_copy_write_buffer_bound ( id ); }

    /* unbind
     *
     * unbind the buffer's target
     */
    void unbind () const;

    /* is_bound
     *
     * checks if the buffer is bound
     */
    bool is_bound () const;



    /* get_target
     *
     * get the target in which the buffer is bound
     */
    const GLenum& get_target () const { return target; }

    /* get_capacity
     *
     * return the capacity of the buffer in bytes
     */
    const GLsizeiptr& get_capacity () const { return capacity; }



private:

    /* GLenum target
     *
     * the target to bind the buffer to
     */
    const GLenum target;

    /* GLsizeiptr capacity
     *
     * the number of bytes allocated to the buffer
     */
    GLsizeiptr capacity;

    /* GLvoid * map_ptr
     *
     * pointer to the current map to the buffer (NULL for no map)
     */
    GLvoid * map_ptr;

    /* unsigned map_id
     *
     * the map id, which is incremented every time the buffer is unmapped
     * this ensures that outdated maps are always picked up on
     */
    unsigned map_id;



    /* generate_map
     *
     * generate a mapping, if not already mapped
     * 
     * return: the map to the buffer, or NULL on failure
     */
    GLvoid * generate_map ();

};



/* BUFFER_MAP DEFINITION */

/* class buffer_map
 *
 * object returned by the map_buffer methods of buffer
 * has operator overloads to act like an array to modify the buffer
 */
template<class T = GLubyte> class glh::core::buffer_map
{

    /* friend of buffer */
    friend buffer;

public:

    /* full constructor
     *
     * construct a buffer map from a pointer to the map, access specifier, a map id and a reference to the buffer object
     *
     * _ptr: a pointer to the mapped data
     * _map_id: the id of this map (supplied by buffer class)
     * _buff: the buffer object the map is for
     */
    buffer_map ( GLvoid * _ptr, const unsigned _map_id, buffer& _buff )  
        : ptr { reinterpret_cast<T *> ( _ptr ) }
        , map_id { _map_id }
        , buff { _buff }
    {}

    /* deleted zero-parameter constructor */
    buffer_map () = delete;

    /* default copy constructor */
    buffer_map ( const buffer_map& other ) = default;

    /* type cast constructor */
    template<class _T> explicit buffer_map ( const buffer_map<_T>& other )
        : ptr { reinterpret_cast<T *> ( other.ptr ) }
        , map_id { other.map_id }
        , buff { other.buff }
    {}

    /* deleted copy assignment operator */
    buffer_map& operator= ( const buffer_map& other ) = delete;

    /* default destructor */
    ~buffer_map () = default;



    /* at/operator[]
     *
     * get elements out of the buffer
     * will throw if the map is no longer valid
     */
    T& at ( const unsigned i );
    const T& at ( const unsigned i ) const;
    T& operator[] ( const unsigned i ) { return at ( i ); }
    const T& operator[] ( const unsigned i ) const { return at ( i ); }


    
    /* is_map_valid
     *
     * returns true if the buffer map is valid
     */
    bool is_map_valid () const;

    /* assert_is_map_valid
     *
     * throws if the map is not valid 
     * 
     * operation: description of the operation
     */
    void assert_is_map_valid ( const std::string& operation = "" ) const;



    /* internal_ptr
     *
     * return the pointer to the map
     * will throw if the map is no longer valid
     */
    T * internal_ptr ();
    const T * internal_ptr () const;

    /* get_buffer
     *
     * get the buffer that is mapped
     * will throw if the map is no longer valid
     */
    buffer& get_buffer ();
    const buffer& get_buffer () const;

    /* get_capacity
     *
     * get the capacity of the buffer in bytes
     */
    const GLsizeiptr& get_capacity () const { return buff.get_capacity (); }



private:

    /* pointer to the map */
    T * ptr;

    /* id for the map */
    const unsigned map_id;

    /* reference to the buffer that is mapped */
    buffer& buff;

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
        : buffer { GL_ARRAY_BUFFER }
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
        : buffer { GL_ARRAY_BUFFER, size, data, usage }
    {}

    /* deleted copy constructor */
    vbo ( const vbo& other ) = delete;

    /* default move constructor */
    vbo ( vbo&& other ) = default;

    /* deleted copy assignment operator */
    vbo& operator= ( const vbo& other ) = delete;

    /* default destructor */
    ~vbo () = default;

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
        : buffer { GL_ELEMENT_ARRAY_BUFFER }
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
        : buffer { GL_ELEMENT_ARRAY_BUFFER, size, data, usage }
    {}

    /* deleted copy constructor */
    ebo ( const ebo& other ) = delete;

    /* default move constructor */
    ebo ( ebo&& other ) = default;

    /* deleted copy assignment operator */
    ebo& operator= ( const ebo& other ) = delete;

    /* default destructor */
    ~ebo () = default;

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
        : object { object_manager::generate_vao () }
        , vertex_attribs { 8, { 0, GL_NONE, GL_NONE, 0, 0, NULL, false } }
        , bound_ebo { NULL }
    {}

    /* deleted copy constructor */
    vao ( const vao& other ) = delete;

    /* default move constructor */
    vao ( vao&& other ) = default;

    /* deleted copy assignment operator */
    vao& operator= ( const vao& other ) = delete;

    /* destructor */
    ~vao () { destroy (); }



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



    /* is_draw_arrays_valid
     *
     * returns true if it is safe to draw arrays from the vao
     * may be false if attributes are invalid, or a buffer is mapped
     */
    bool is_draw_arrays_valid () const;

    /* is_draw_elements_valid
     * 
     * returns true if it is safe to draw elements from the vao
     * may be false if ebo is not bound
     */
    bool is_draw_elements_valid () const;

    /* assert_is_draw_arrays_valid
     *
     * throws if the vao is not draw arrays valid
     * 
     * operation: description of the operation
     */
    void assert_is_draw_arrays_valid ( const std::string& operation = "" ) const;

    /* assert_is_draw_elements_valid
     *
     * throws if the vao is not draw elements valid
     * 
     * operation: description of the operation
     */
    void assert_is_draw_elements_valid ( const std::string& operation = "" ) const;



    /* destroy
     *
     * destroys the object, setting id to 0
     */
    void destroy () { object_manager::destroy_vao ( id ); id = 0; }

    /* bind
     *
     * bind the vertex array object
     */
    void bind () const { object_manager::bind_vao ( id ); }

    /* unbind
     *
     * unbind the vertex array object
     */
    void unbind () const { object_manager::unbind_vao ( id ); }

    /* is_bound
     *
     * checks if the vao is bound
     */
    bool is_bound () const { return object_manager::is_vao_bound ( id ); }



private:

    /* struct to represent a vertex attribute */
    struct vertex_attrib
    {
        GLint size;
        GLenum type;
        GLenum norm;
        GLsizei stride;
        GLsizeiptr offset;
        const vbo * buff;
        bool enabled;
    };

    /* array of vertex attributes */
    std::vector<vertex_attrib> vertex_attribs;

    /* bound ebo */
    const ebo * bound_ebo;

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



/* buffer TEMPLATE METHODS IMPLEMENTATION */

/* map(_ro)
 *
 * maps the buffer to a buffer_map, if not already mapped
 * 
 * return: the mapped buffer
 */
template<class T> inline glh::core::buffer_map<T> glh::core::buffer::map ()
{
    /* return buffer_map object */
    return buffer_map<T> { generate_map (), map_id, * this };
}
template<class T> inline glh::core::buffer_map<const T> glh::core::buffer::map_ro ()
{
    /* return buffer_map object */
    return buffer_map<const T> { generate_map (), map_id, * this };
}

/* is_mapped
 *
 * with no parameters, returns true if the buffer is currently in a mapped state
 * with a map parameter, returns true if the map supplied is currently valid
 */
template<class T> inline bool glh::core::buffer::is_mapped ( const buffer_map<T>& map ) const
{
    /* return true if is valid and ids match */
    return ( is_mapped () && map.map_id == map_id );
}



/* BUFFER_MAP IMPLEMENTATION */

/* at/operator[]
 *
 * get elements out of the buffer
 */
template<class T> inline T& glh::core::buffer_map<T>::at ( const unsigned i )
{
    /* throw if invalid or out of bounds */
    assert_is_map_valid ();
    if ( ( i + 1 ) * sizeof ( T ) > buff.get_capacity () ) throw exception::buffer_exception { "buffer map indices are out of range" };

    /* otherwise return valud */
    return * ( ptr + i );
}
template<class T> inline const T& glh::core::buffer_map<T>::at ( const unsigned i ) const
{
    /* throw if invalid or out of bounds */
    assert_is_map_valid ();
    if ( ( i + 1 ) * sizeof ( T ) > buff.capacity () ) throw exception::buffer_exception { "buffer map indices are out of range" };

    /* otherwise return valud */
    return * ( ptr + i );
}

/* is_map_valid
 *
 * returns true if the buffer map is valid
 */
template<class T> inline bool glh::core::buffer_map<T>::is_map_valid () const
{
    /* check map_ptr then use the method of buffer */
    return ( ptr && buff.is_mapped ( * this ) );
}

/* assert_is_map_valid
 *
 * throws if the map is not valid 
 * 
 * operation: description of the operation
 */
template<class T> inline void glh::core::buffer_map<T>::assert_is_map_valid ( const std::string& operation ) const
{
    if ( !is_map_valid () ) 
    {
        if ( operation.size () > 0 ) throw exception::buffer_exception { "attempted to perform " + operation + " operation on invalid buffer map" };
        else throw exception::buffer_exception { "attempted to perform operation on invalid buffer map" };
    }
}



/* #ifndef GLHELPER_BUFFER_HPP_INCLUDED */
#endif