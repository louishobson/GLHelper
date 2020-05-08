/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * include/glhelper/glhelper_manager.hpp
 * 
 * constructs to handle OpenGL objects
 * notable constructs include:
 * 
 * 
 * 
 * CLASS GLH::CORE::OBJECT_MANAGER
 * 
 * a class with static methods to create, destroy and bind many types of OpenGL objects
 * this class is designed to be used internally, rather than by an external programmer
 * the binding of OpenGL objects is tracked so that duplicate bind operations are avoided
 * 
 * 
 * 
 * CLASS GLH::EXCEPTION::OBJECT_MANAGEMENT_EXCEPTION
 * 
 * thrown when an error occurs in one of the object_manager methods (e.g. bind operation on invalid OpenGL object)
 * 
 */



/* HEADER GUARD */
#ifndef GLHELPER_MANAGER_HPP_INCLUDED
#define GLHELPER_MANAGER_HPP_INCLUDED



/* INCLUDES */

/* include core headers */
#include <iostream>
#include <vector>

/* include glhelper_core.hpp */
#include <glhelper/glhelper_core.hpp>

/* include glhelper_exception.hpp */
#include <glhelper/glhelper_exception.hpp>

/* include glhelper_glad.hpp */
#include <glhelper/glhelper_glad.hpp>



/* NAMESPACE FORWARD DECLARATIONS */

namespace glh
{
    namespace core
    {
        /* class object_manager
         *
         * responsible for generating and destroying OpenGL objects
         * also tracks bindings and avoids rebinds
         */
        class object_manager;
    }

    namespace exception
    {
        /* class object_management_exception : exception
         *
         * exception relating to object management
         */
        class object_management_exception;
    }
}



/* OBJECT_MANAGER DEFINITION */

/* class object_manager
 *
 * responsible for generating and destroying OpenGL objects
 * also tracks bindings and avoids rebinds
 */
class glh::core::object_manager
{
public:

    /* default constructor */
    object_manager () = default;

    /* default copy constructor */
    object_manager ( const object_manager& other ) = default;

    /* default copy assignment operator */
    object_manager& operator= ( const object_manager& other ) = default;

    /* default destructor */
    ~object_manager () = default;



    /* BUFFER OBJECTS */

    /* generate_buffer
     *
     * generate a buffer object
     */
    static GLuint generate_buffer ();

    /* destroy_buffer
     *
     * destroy a buffer object, unbinding it from any bindings
     */
    static void destroy_buffer ( const GLuint id );

    /* bind_vbo
     *
     * bind a buffer object as a vbo
     */
    static void bind_vbo ( const GLuint id );

    /* unbind_vbo
     *
     * unbind the vbo, only if it is already bound
     */
    static void unbind_vbo ( const GLuint id );

    /* get_bound_vbo
     *
     * return the id of the bound vbo
     * returns 0 if no vbo is bound
     */
    static GLuint get_bound_vbo () { return bound_vbo; }

    /* is_vbo_bound
     *
     * returns true if the vbo is bound
     */
    static bool is_vbo_bound ( const GLuint id ) { return ( id != 0 && id == bound_vbo ); }

    /* bind_ebo
     *
     * bind a buffer object as an ebo
     */
    static void bind_ebo ( const GLuint id );

    /* unbind_ebo
     *
     * unbind the ebo, only if it is already bound
     */
    static void unbind_ebo ( const GLuint id );

    /* get_bound_ebo
     *
     * return the id of the bound ebo
     * returns 0 if no ebo is bound
     */
    static GLuint get_bound_ebo () { return bound_ebo; }

    /* is_ebo_bound
     *
     * returns true if the ebo is bound
     */
    static bool is_ebo_bound ( const GLuint id ) { return ( id != 0 && id == bound_ebo ); }

    /* bind_ubo
     *
     * bind a buffer object as a ubo
     */
    static void bind_ubo ( const GLuint id );

    /* unbind_ubo
     *
     * unbind the ubo, only if it is already bound
     */
    static void unbind_ubo ( const GLuint id );

    /* get_bound_ubo
     *
     * return the id of the bound ubo
     * returns 0 if no ubo is bound
     */
    static GLuint get_bound_ubo () { return bound_ubo; }

    /* is_ubo_bound
     *
     * returns true if the ubo is bound
     */
    static bool is_ubo_bound ( const GLuint id ) { return ( id != 0 && id == bound_ubo ); }

    /* bind_ubo_index
     *
     * bind a ubo to an index
     * does NOT bind to the default ubo target
     *
     * index: the index to bind the ubo to
     */
    static void bind_ubo_index ( const GLuint id, const unsigned index );

    /* unbind_ubo_index
     *
     * unbind a ubo from an index
     * does NOT unbind from the default ubo target
     *
     * index: the index to unbind the ubo from
     */
    static void unbind_ubo_index ( const GLuint id, const unsigned index );

    /* get_bound_ubo_index
     *
     * get the ubo bound to an index
     * returns 0 if no ubo is bound to the index
     */
    static GLuint get_bound_ubo_index ( const unsigned index );

    /* is_ubo_index_bound
     *
     * returns true if the ubo is bound to an index
     */
    static bool is_ubo_bound_index ( const GLuint id, const unsigned index );

    /* bind_copy_read_buffer
     *
     * bind a buffer object to GL_COPY_READ_BUFFER
     */
    static void bind_copy_read_buffer ( const GLuint id );
    
    /* unbind_copy_read_buffer
     *
     * unbind a buffer object from GL_COPY_READ_BUFFER
     */
    static void unbind_copy_read_buffer ( const GLuint id );

    /* get_bound_copy_read_buffer
     *
     * return the id of the bound copy read buffer
     * returns 0 if no copy read buffer is bound
     */
    static GLuint get_bound_copy_read_buffer () { return bound_copy_read_buffer; }

    /* is_copy_read_buffer_bound
     *
     * returns true if the buffer is bound to GL_COPY_READ_BUFFER
     */
    static bool is_copy_read_buffer_bound ( const GLuint id ) { return ( id != 0 && id == bound_copy_read_buffer ); }

    /* bind_copy_write_buffer
     *
     * bind a buffer object to GL_COPY_WRITE_BUFFER
     */
    static void bind_copy_write_buffer ( const GLuint id );

    /* unbind_copy_write_buffer
     *
     * unbind a buffer object to GL_COPY_WRITE_BUFFER
     */
    static void unbind_copy_write_buffer ( const GLuint id );

    /* get_bound_copy_write_buffer
     *
     * return the id of the bound copy write buffer
     * returns 0 if no copy write buffer is bound
     */
    static GLuint get_bound_copy_write_buffer () { return bound_copy_write_buffer; }

    /* is_copy_write_buffer_bound
     *
     * returns true if the buffer is bound to GL_COPY_WRITE_BUFFER
     */
    static bool is_copy_write_buffer_bound ( const GLuint id ) { return ( id != 0 && id == bound_copy_write_buffer ); }



    /* VERTEX ARRAY OBJECTS */

    /* generate_vao
     *
     * generate a vertex array object
     */
    static GLuint generate_vao ();

    /* destroy_vao
     *
     * destroy a vao, unbinding it if is bound
     */
    static void destroy_vao ( const GLuint id );

    /* bind_vao
     *
     * bind a vao
     */
    static void bind_vao ( const GLuint id );

    /* unbind_vao
     *
     * unbind the vao, only if it is already bound
     */
    static void unbind_vao ( const GLuint id );

    /* get_bound_vao
     *
     * return the id of the bound vao
     * returns 0 if no vao is bound
     */
    static GLuint get_bound_vao () { return bound_vao; }

    /* is_vao_bound
     *
     * returns true if the vao is bound
     */
    static bool is_vao_bound ( const GLuint id ) { return ( id != 0 && id == bound_vao ); }



    /* SHADER/PROGRAM OBJECTS */

    /* generate_shader
     *
     * generate a shader object
     * 
     * type: the type of the shader
     */
    static GLuint generate_shader ( const GLenum type );

    /* destroy_shader
     *
     * destroy a shader object
     */
    static void destroy_shader ( const GLuint id );

    /* generate_program
     *
     * generate a program object
     */
    static GLuint generate_program ();

    /* destroy_program
     * 
     * destroy a program object, making it not in use in the progrss
     */
    static void destroy_program ( const GLuint id );

    /* use_program
     *
     * make a program currently in use
     */
    static void use_program ( const GLuint id );

    /* unuse_program
     *
     * unuse the program, only if it is already in use
     */
    static void unuse_program ( const GLuint id );

    /* get_in_use_program
     *
     * returns the id of the program currently in use
     * returns 0 if no program is in use
     */
    static GLuint get_in_use_program () { return in_use_program; }

    /* is_program_in_use
     *
     * returns true if the program is in use
     */
    static bool is_program_in_use ( const GLuint id ) { return ( id != 0 && id == in_use_program ); }



    /* TEXTURE OBJECTS */

    /* generate_texture
     * 
     * generate a texture object
     */
    static GLuint generate_texture ();

    /* destroy_texture
     *
     * destroy a texture object, unbinding it from any texture units
     */
    static void destroy_texture ( const GLuint id );

    /* bind_texture2d
     *
     * bind a texture2d to a texture unit
     * 
     * unit: the texture unit to bind it to
     */
    static void bind_texture2d ( const GLuint id, const unsigned unit );

    /* unbind_texture2d
     *
     * unbind a texture2d from a unit, if it is already bound
     * 
     * unit: the texture unit to unbind it from
     */
    static void unbind_texture2d ( const GLuint id, const unsigned unit );

    /* get_bound_texture2d
     *
     * return the id of the texture currently bound to the texture unit supplied
     * returns 0 if no texture is bound
     * 
     * unit: the texture unit to test
     */
    static GLuint get_bound_texture2d ( const unsigned unit );

    /* is_texture2d_bound
     *
     * returns true if a texture2d is bound to the provided texture unit
     *
     * unit: the texture unit to check
     */
    static bool is_texture2d_bound ( const GLuint id, const unsigned unit );

    /* bind_cubemap
     *
     * bind a cubemap texture to GL_TEXTURE_CUBE_MAP, if not already bounc
     * 
     * unit: the texture unit to bind it to
     */
    static void bind_cubemap ( const GLuint id, const unsigned unit );

    /* unbind_cubemap
     * 
     * unbind a cubemap texture from GL_TEXTURE_CUBE_MAP, if already bound
     * 
     * unit: the texture unit to unbind it from
     */
    static void unbind_cubemap ( const GLuint id, const unsigned unit );

    /* get_bound_cubemap
     *
     * return the id of the cubemap currently bound to the texture unit supplied
     * returns 0 if no cubemap is bound
     * 
     * unit: the texture unit to test
     */
    static GLuint get_bound_cubemap ( const unsigned unit );

    /* is_cubemap_bound
     *
     * returns true if a cubemap is bound to GL_TEXTURE_CUBE_MAP under the provided texture unit
     *
     * unit: the texture unit to check
     */
    static bool is_cubemap_bound ( const GLuint id, const unsigned unit );



    /* RENDERBUFFER OBJECTS */

    /* generate_rbo
     *
     * generate a renderbuffer object
     */
    static GLuint generate_rbo ();

    /* destroy_rbo
     *
     * destroy a renderbuffer object, unbindint it if bound
     */
    static void destroy_rbo ( const GLuint id );

    /* bind_rbo
     *
     * bind a renderbuffer object
     */
    static void bind_rbo ( const GLuint id );

    /* unbind_rbo
     *
     * unbind an renderbuffer object, if already bound
     */
    static void unbind_rbo ( const GLuint id );

    /* get_bound_rbo
     *
     * return the id of the bound rbo
     * returns 0 if no rbo is bound
     */
    static GLuint get_bound_rbo () { return bound_rbo; }

    /* is_rbo_bound
     *
     * return true if the rbo is bound
     */
    static bool is_rbo_bound ( const GLuint id ) { return ( id != 0 && id == bound_rbo ); }



    /* FRAMEBUFFER OBJECTS */

    /* generate_fbo
     *
     * generate a framebuffer object
     */
    static GLuint generate_fbo ();

    /* destroy_fbo
     *
     * destroy a framebuffer object, unbinding it if bound
     */
    static void destroy_fbo ( const GLuint id );

    /* bind_fbo
     *
     * bind a framebuffer object
     */
    static void bind_fbo ( const GLuint id );

    /* unbind_fbo
     *
     * if the fbo is bound, bind the default fbo instead
     */
    static void unbind_fbo ( const GLuint id );

    /* bind_default_fbo
     *
     * bind the default framebuffer
     * this replaces the unbind_framebuffer method, as framebuffer id=0 is the default framebuffer
     */
    static void bind_default_fbo ();

    /* get_bound_fbo
     *
     * return the id of the bound fbo
     * returns 0 if no fbo is bound
     */
    static GLuint get_bound_fbo () { return bound_fbo; }

    /* is_fbo_bound
     *
     * return true if the framebuffer is bound
     */
    static bool is_fbo_bound ( const GLuint id ) { return ( id != 0 && id == bound_fbo ); }



    /* assert_is_object_valid
     *
     * throws if an object has an id of 0
     *
     * id: the id of the object to test
     * operation: a description of the operation
     */
    static void assert_is_object_valid ( const GLuint id, const std::string& operation = "" );



private:

    /* currently bound vbo */
    static GLuint bound_vbo;

    /* currently bound ebo */
    static GLuint bound_ebo;

    /* currently bound ubo */
    static GLuint bound_ubo;

    /* currently bound ubo indices */
    static std::vector<GLuint> bound_ubo_indices;

    /* currently bound read copy buffer */
    static GLuint bound_copy_read_buffer;

    /* currently bound copy write buffer */
    static GLuint bound_copy_write_buffer;

    /* currently bound vao */
    static GLuint bound_vao;

    /* currently in-use program */
    static GLuint in_use_program;

    /* array of texture units and their respectively bound textures */
    static std::vector<GLuint> bound_texture2ds;

    /* array of texture units and their respectively bound cubemaps */
    static std::vector<GLuint> bound_cubemaps;

    /* currently bound renderbuffer */
    static GLuint bound_rbo;

    /* currently bound framebuffer */
    static GLuint bound_fbo;

};



/* OBJECT_MANAGEMENT_EXCEPTION DEFINITION */

/* class object_management_exception : exception
 *
 * for exceptions related to textures
 */
class glh::exception::object_management_exception : public exception
{
public:

    /* full constructor
     *
     * __what: description of the exception
     */
    explicit object_management_exception ( const std::string& __what )
        : exception { __what }
    {}

    /* default zero-parameter constructor
     *
     * construct object_management_exception with no descrption
     */
    object_management_exception () = default;

    /* default everything else and inherits what () function */

};



/* #ifndef GLHELPER_MANAGER_HPP_INCLUDED */
#endif