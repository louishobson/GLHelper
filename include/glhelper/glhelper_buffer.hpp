/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * include/glhelper/glhelper_buffer.hpp
 * 
 * declares buffer handling constructs
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



/* BUFFER DEFINITION */

/* class buffer : object
 *
 * base class for storing a buffer
 */
class glh::buffer : public object
{
public:

    /* constructor
     *
     * generates a buffer
     * 
     * _target: the target for the buffer
     */
    buffer ( const GLenum _target, const GLenum _reverse_target )
        : object { glh::object_manager::generate_buffer () }
        , target { _target }
        , reverse_target { _reverse_target }
    {}

    /* construct and immediately buffer data
     *
     * generates a buffer and immediately buffers data
     * 
     * _target: the target for the buffer
     * size: size of data in bytes
     * data: pointer to data
     * usage: the storage method for the data
     */
    buffer ( const GLenum _target, const GLenum _reverse_target, const GLsizeiptr size, const GLvoid * data, const GLenum usage )
        : buffer { _target, _reverse_target }
    { buffer_data ( size, data, usage ); }

    /* deleted zero-parameter constructor */
    buffer () = delete;

    /* deleted copy constructor
     *
     * it makes no sense to copy a buffer
     */
    buffer ( const buffer& other ) = delete;

    /* default move constructor */
    buffer ( buffer&& other ) = default;

    /* deleted copy assignment operator
     *
     * it makes no sense to assign the buffer after comstruction
     */
    buffer& operator= ( const buffer& other ) = delete;

    /* virtual destructor
     *
     * virtual in preparation for polymorphism
     */
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
    void destroy () { glh::object_manager::destroy_buffer ( id ); id = 0; }

    /* bind
     *
     * bind the buffer
     */
    virtual void bind () const = 0;

    /* unbind
     *
     * unbind the buffer's target
     */
    virtual void unbind () const = 0;

    /* is_bound
     *
     * checks if the buffer is bound
     */
    virtual bool is_bound () const = 0;



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
class glh::vbo : public buffer 
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

    /* deleted copy constructor
     *
     * it makes no sense to copy a buffer
     */
    vbo ( const vbo& other ) = delete;

    /* default move constructor */
    vbo ( vbo&& other ) = default;

    /* deleted copy assignment operator
     *
     * it makes no sense to assign the buffer after comstruction
     */
    vbo& operator= ( const vbo& other ) = delete;

    /* default destructor */
    ~vbo () = default;



    /* bind
     *
     * bind the buffer
     */
    void bind () const { glh::object_manager::bind_vbo ( id ); }

    /* unbind
     *
     * unbind the buffer's target
     */
    void unbind () const { glh::object_manager::unbind_vbo ( id ); }

    /* is_bound
     *
     * checks if the buffer is bound
     */
    bool is_bound () const { return glh::object_manager::is_vbo_bound ( id ); }
};



/* EBO DEFINITION */

/* class ebo : buffer
 *
 * element buffer object
 */
class glh::ebo : public buffer 
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

    /* deleted copy constructor
     *
     * it makes no sense to copy a buffer
     */
    ebo ( const ebo& other ) = delete;

    /* default move constructor */
    ebo ( ebo&& other ) = default;

    /* deleted copy assignment operator
     *
     * it makes no sense to assign the buffer after comstruction
     */
    ebo& operator= ( const ebo& other ) = delete;

    /* default destructor */
    ~ebo () = default;



    /* bind
     *
     * bind the buffer
     */
    void bind () const { glh::object_manager::bind_ebo ( id ); }

    /* unbind
     *
     * unbind the buffer's target
     */
    void unbind () const { glh::object_manager::unbind_ebo ( id ); }

    /* is_bound
     *
     * checks if the buffer is bound
     */
    bool is_bound () const { return glh::object_manager::is_ebo_bound ( id ); }
};



/* VAO DEFINITION */

/* class vao : object
 *
 * vertex array object
 */
class glh::vao : public object
{
public:

    /* constructor
     *
     * creates a vertex array object without any vbo or ebo bound
     */
    vao ()
        : object { glh::object_manager::generate_vao () }
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
    void destroy () { glh::object_manager::destroy_vao ( id ); id = 0; }

    /* bind
     *
     * bind the vertex array object
     */
    void bind () const { glh::object_manager::bind_vao ( id ); }

    /* unbind
     *
     * unbind the vertex array object
     */
    void unbind () const { glh::object_manager::unbind_vao ( id ); }

    /* is_bound
     *
     * checks if the vao is bound
     */
    bool is_bound () const { return glh::object_manager::is_vao_bound ( id ); }

};



/* #ifndef GLHELPER_BUFFER_HPP_INCLUDED */
#endif