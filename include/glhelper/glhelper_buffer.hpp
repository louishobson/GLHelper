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
         * base class for storing a buffer
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

        /* class vao : object
         *
         * vertex array object
         */
        class vao;
    }
}



/* BUFFER DEFINITION */

/* class buffer : object
 *
 * base class for storing a buffer
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
    buffer ( const GLenum _target, const GLenum _reverse_target )
        : object { object_manager::generate_buffer () }
        , target { _target }
        , reverse_target { _reverse_target }
    {}

    /* construct and immediately buffer data
     *
     * generates a buffer and immediately buffers data
     * 
     * _target: the target for the buffer (e.g. GL_ARRAY_BUFFER)
     * _reverse_target: the reverse target for the buffer (e.g. GL_ARRAY_BUFFER_BINDING for a target of GL_ARRAY_BUFFER)
     * size: size of data in bytes
     * data: pointer to data
     * usage: the storage method for the data
     */
    buffer ( const GLenum _target, const GLenum _reverse_target, const GLsizeiptr size, const GLvoid * data, const GLenum usage )
        : object { object_manager::generate_buffer () }
        , target { _target }
        , reverse_target { _reverse_target }
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
     * data: pointer to data
     * usage: the storage method for the data
     */
    void buffer_data ( const GLsizeiptr size, const GLvoid * data, const GLenum usage );

    /* clear_data
     *
     * clear the data from the buffer
     */
    void clear_data ();



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



    /* get_(reverse)target
     *
     * get the (reverse ) target in which the buffer is bound
     */
    const GLenum& get_target () const { return target; }
    const GLenum& get_reverse_target () const { return reverse_target; }



private:

    /* GLenum target
     *
     * the target to bind the buffer to
     */
    const GLenum target;

    /* GLenum reverse_target
     *
     * the target to use when getting which buffer is bound
     */
    const GLenum reverse_target;

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
        : buffer { GL_ARRAY_BUFFER, GL_ARRAY_BUFFER_BINDING }
    {}

    /* construct and immediately buffer data
     *
     * generates a buffer and immediately buffers data
     * 
     * size: size of data in bytes
     * data: pointer to data
     * usage: the storage method for the data
     */
    vbo ( const GLsizeiptr size, const GLvoid * data, const GLenum usage )
        : buffer { GL_ARRAY_BUFFER, GL_ARRAY_BUFFER_BINDING, size, data, usage }
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
        : buffer { GL_ELEMENT_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER_BINDING }
    {}

    /* construct and immediately buffer data
     *
     * generates a buffer and immediately buffers data
     * 
     * size: size of data in bytes
     * data: pointer to data
     * usage: the storage method for the data
     */
    ebo ( const GLsizeiptr size, const GLvoid * data, const GLenum usage )
        : buffer { GL_ELEMENT_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER_BINDING, size, data, usage }
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
    void set_vertex_attrib ( const GLuint attrib, const vbo& buff, const GLint size, const GLenum type, const GLboolean norm, const GLsizei stride, const GLvoid * offset );

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

};



/* #ifndef GLHELPER_BUFFER_HPP_INCLUDED */
#endif