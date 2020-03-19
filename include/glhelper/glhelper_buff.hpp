/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * include/glhelper/glhelper_buff.hpp
 * 
 * declares buffer handling constructs
 * 
 */



/* HEADER GUARD */
#ifndef GLHELPER_BUFF_HPP_INCLUDED
#define GLHELPER_BUFF_HPP_INCLUDED



/* INCLUDES */

/* include core headers */
#include <iostream>

/* include glhelper_core.hpp */
#include <glhelper/glhelper_core.hpp>

/* include glhelper_exception.hpp */
#include <glhelper/glhelper_exception.hpp>

/* include glhelper_glad.hpp */
#include <glhelper/glhelper_glad.hpp>



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

    /* class buffer_exception : exception
     *
     * for exceptions related to buffers
     */
    class buffer_exception;
}



/* FULL DECLARATIONS */

/* class buffer : object
 *
 * base class for storing a buffer
 */
class glh::buffer : public object
{

    /* vao if a friend of a buffer object
     *
     * this allows the vao to access the bind functions
     * this in turn allows for the vao to bind to a vbp/ebo
     */
    friend class vao;

public:

    /* constructor
     *
     * generates a buffer
     */
    explicit buffer ();

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
    virtual ~buffer ();



    /* buffer_data
     * 
     * size: size of data in bytes
     * data: pointer to data
     * usage: the storage method for the data
     */
    void buffer_data ( const size_t size, const void * data, const GLenum usage );

    /* clear_data
     *
     * clear the data from the buffer
     */
    void clear_data ();



    /* destroy
     *
     * destroys the object, setting id to -1
     */
    void destroy () override final;



protected:

    /* pure virtual target
     *
     * get the target of the buffer
     */
    virtual GLenum target () const = 0;

    /* bind
     *
     * bind the buffer
     * 
     * return: the target it is bound to
     */
    GLenum bind () const;

    /* unbind
     *
     * unbind the buffer's target
     * 
     * return: the target just unbound
     */
    GLenum unbind () const;

};

/* class vbo : buffer
 *
 * vertex buffer object
 */
class glh::vbo : public buffer 
{
public:

    /* default constructor
     *
     * generates the buffer
     */
    explicit vbo () = default;

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



protected:

    /* target 
     *
     * return: GLenum for the target of the buffer
     */
    GLenum target () const override final { return GL_ARRAY_BUFFER; }

};

/* class ebo : buffer
 *
 * element buffer object
 */
class glh::ebo : public buffer 
{
public:

    /* default constructor
     *
     * generates the buffer
     */
    explicit ebo () = default;

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



protected:

    /* target 
     *
     * return: GLenum for the target of the buffer
     */
    GLenum target () const override final { return GL_ELEMENT_ARRAY_BUFFER; }

};

/* class vao : object
 *
 * vertex array object
 */
class glh::vao : public object
{
public:

    /* default constructor
     *
     * creates a vertex array object without any vbo or ebo bound
     */
    explicit vao () = default;

    /* deleted copy constructor */
    vao ( const vao& other ) = delete;

    /* default move constructor */
    vao ( vao&& other ) = default;

    /* deleted copy assignment operator */
    vao& operator= ( const vao& other ) = delete;

    /* destructor */
    ~vao ();



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

    /* bind_ebo
     *
     * binds an element buffer object to the vao
     *
     * buff: the ebo to bind
     */
    void bind_ebo ( const ebo& buff );



private:

    /* bind
     *
     * bind the vertex array object
     */
    void bind ();

    /* unbind
     *
     * unbind the vertex array object
     */
    void unbind ();

};

/* class buffer_exception : exception
 *
 * for exceptions related to buffers
 */
class glh::buffer_exception : public exception
{
public:

    /* full constructor
     *
     * __what: description of the exception
     */
    explicit buffer_exception ( const char * __what )
        : exception ( __what )
    {}

    /* zero-parameter constructor
     *
     * construct glad_exception with no descrption
     */
    explicit buffer_exception ()
        : exception { NULL }
    {}

    /* default everything else and inherits what () function */

};




/* #ifndef GLHELPER_BUFF_HPP_INCLUDED */
#endif