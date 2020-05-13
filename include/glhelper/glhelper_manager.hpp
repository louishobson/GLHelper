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
 * SCOPED ENUM GLH::CORE::MINOR_OBJECT_TYPE
 * SCOPED ENUM GLH::CORE::MAJOR_OBJECT_TYPE
 * SCOPED ENUM GLH::CORE::OBJECT_BIND_TARGET
 * 
 * these enums allow for the representation of different types of object and bind points, used by the object manager class
 * the object manager class knows nothing about the actual derived class of an object, hence these enum values are used to identify objects
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
#include <array>
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
        /* enum major_object_type
         *
         * enum for general types of object (e.g. GLH_BUFFER_TYPE rather than GLH_VBO_TYPE)
         */
        enum struct major_object_type : unsigned;

        /* enum minor_object_type
         *
         * enum for more specific types of object (e.g. GLH_VBO_TYPE rather than GLH_BUFFER_TYPE)
         */
        enum struct minor_object_type : unsigned;

        /* enum object_bind_target
         *
         * enum for bind targets of objects
         */
        enum struct object_bind_target : unsigned;



        /* class object_manager
         *
         * responsible for generating and destroying OpenGL objects
         * also tracks bindings and avoids rebinds
         */
        class object_manager;

        /* using expression for object_manager class */
        using om = object_manager;
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

/* operators+- for bind target enum
 *
 * allows one to step through indexed bind points
 */
glh::core::object_bind_target operator+ ( const glh::core::object_bind_target target, const int scalar );
glh::core::object_bind_target operator- ( const glh::core::object_bind_target target, const int scalar );
int operator- ( const glh::core::object_bind_target target0, const glh::core::object_bind_target target1 );



/* ENUM DEFINITIONS DEFINITION */

/* enum major_object_type
 *
 * enum for general types of object (e.g. GLH_BUFFER_TYPE rather than GLH_VBO_TYPE)
 */
enum struct glh::core::major_object_type : unsigned
{
    GLH_BUFFER_TYPE,
    GLH_VAO_TYPE,
    
    GLH_RBO_TYPE,
    GLH_FBO_TYPE,

    GLH_SHADER_TYPE,
    GLH_PROGRAM_TYPE,

    GLH_TEXTURE_TYPE,

    __COUNT__
};

/* enum minor_object_type
 *
 * enum for more specific types of object (e.g. GLH_VBO_TYPE rather than GLH_BUFFER_TYPE)
 */
enum struct glh::core::minor_object_type : unsigned
{
    GLH_VBO_TYPE,
    GLH_EBO_TYPE,
    GLH_UBO_TYPE,
    GLH_VAO_TYPE,
    
    GLH_RBO_TYPE,
    GLH_FBO_TYPE,

    GLH_VSHADER_TYPE,
    GLH_GSHADER_TYPE,
    GLH_FSHADER_TYPE,
    GLH_PROGRAM_TYPE,

    GLH_TEXTURE2D_TYPE,
    GLH_CUBEMAP_TYPE,

    __COUNT__
};

/* enum object_bind_target
 *
 * enum for bind targets of objects
 */
enum struct glh::core::object_bind_target : unsigned
{
    GLH_VBO_TARGET,
    GLH_EBO_TARGET,
    GLH_UBO_TARGET,
    GLH_COPY_READ_BUFFER_TARGET,
    GLH_COPY_WRITE_BUFFER_TARGET,
    GLH_VAO_TARGET,
    
    GLH_RBO_TARGET,
    GLH_FBO_TARGET,

    GLH_PROGRAM_TARGET,

    __TEXTURE2D_START__,
    GLH_TEXTURE2D_0_TARGET,  GLH_TEXTURE2D_1_TARGET,  GLH_TEXTURE2D_2_TARGET,  GLH_TEXTURE2D_3_TARGET,  GLH_TEXTURE2D_4_TARGET,  GLH_TEXTURE2D_5_TARGET,  GLH_TEXTURE2D_6_TARGET,  GLH_TEXTURE2D_7_TARGET, 
    GLH_TEXTURE2D_8_TARGET,  GLH_TEXTURE2D_9_TARGET,  GLH_TEXTURE2D_10_TARGET, GLH_TEXTURE2D_11_TARGET, GLH_TEXTURE2D_12_TARGET, GLH_TEXTURE2D_13_TARGET, GLH_TEXTURE2D_14_TARGET, GLH_TEXTURE2D_15_TARGET, 
    GLH_TEXTURE2D_16_TARGET, GLH_TEXTURE2D_17_TARGET, GLH_TEXTURE2D_18_TARGET, GLH_TEXTURE2D_19_TARGET, GLH_TEXTURE2D_20_TARGET, GLH_TEXTURE2D_21_TARGET, GLH_TEXTURE2D_22_TARGET, GLH_TEXTURE2D_23_TARGET, 
    GLH_TEXTURE2D_24_TARGET, GLH_TEXTURE2D_25_TARGET, GLH_TEXTURE2D_26_TARGET, GLH_TEXTURE2D_27_TARGET, GLH_TEXTURE2D_28_TARGET, GLH_TEXTURE2D_29_TARGET, GLH_TEXTURE2D_30_TARGET, GLH_TEXTURE2D_31_TARGET, 
    __TEXTURE2D_END__,

    __CUBEMAP_START__,
    GLH_CUBEMAP_0_TARGET,  GLH_CUBEMAP_1_TARGET,  GLH_CUBEMAP_2_TARGET,  GLH_CUBEMAP_3_TARGET,  GLH_CUBEMAP_4_TARGET,  GLH_CUBEMAP_5_TARGET,  GLH_CUBEMAP_6_TARGET,  GLH_CUBEMAP_7_TARGET, 
    GLH_CUBEMAP_8_TARGET,  GLH_CUBEMAP_9_TARGET,  GLH_CUBEMAP_10_TARGET, GLH_CUBEMAP_11_TARGET, GLH_CUBEMAP_12_TARGET, GLH_CUBEMAP_13_TARGET, GLH_CUBEMAP_14_TARGET, GLH_CUBEMAP_15_TARGET, 
    GLH_CUBEMAP_16_TARGET, GLH_CUBEMAP_17_TARGET, GLH_CUBEMAP_18_TARGET, GLH_CUBEMAP_19_TARGET, GLH_CUBEMAP_20_TARGET, GLH_CUBEMAP_21_TARGET, GLH_CUBEMAP_22_TARGET, GLH_CUBEMAP_23_TARGET, 
    GLH_CUBEMAP_24_TARGET, GLH_CUBEMAP_25_TARGET, GLH_CUBEMAP_26_TARGET, GLH_CUBEMAP_27_TARGET, GLH_CUBEMAP_28_TARGET, GLH_CUBEMAP_29_TARGET, GLH_CUBEMAP_30_TARGET, GLH_CUBEMAP_31_TARGET,
    __CUBEMAP_END__,

    GLH_NO_TARGET,

    __COUNT__
};



/* OBJECT_MANAGER DEFINITION */

/* class object_manager
 *
 * responsible for generating and destroying OpenGL objects
 * also tracks bindings and avoids rebinds
 */
class glh::core::object_manager
{
public:

    /* deleted constructor */
    object_manager () = delete;

    /* deleted copy constructor */
    object_manager ( const object_manager& other ) = delete;

    /* deleted copy assignment operator */
    object_manager& operator= ( const object_manager& other ) = delete;

    /* deleted destructor */
    ~object_manager () = delete;



    /* generate_object
     *
     * generate an object of a given type
     * 
     * type: the type of the object (minor type)
     * 
     * return: the id of the new object
     */
    static GLuint generate_object ( const minor_object_type type );

    /* destroy_object
     *
     * destroy an object of a given type
     * 
     * id: the id of the object to destroy
     * type: the type of the object (minor type)
     */
    static void destroy_object ( const GLuint id, const minor_object_type type );

    /* unbind_object_all
     *
     * unbinds an object from all possible bind points
     * includes all indexed bindings, where applicable
     * 
     * id: the id of the object to unbind
     * type: the type of the object (minor type)
     */
    static void unbind_object_all ( const GLuint id, const minor_object_type type );

    /* bind_object
     *
     * bind an object to a target
     * if already bound, it will not be bound twice
     * 
     * id: the id of the object
     * type: the bind target of the object (bind target)
     */
    static void bind_object ( const GLuint id, object_bind_target target );

    /* unbind_object
     *
     * unbind an object from a target
     * if not bound, the function returns having done nothing
     * 
     * id: the id of the object
     * target: the bind target of the object (bind target)
     */
    static void unbind_object ( const GLuint id, const object_bind_target target );

    /* get_bound_object
     *
     * get the object thats bound to a target
     *
     * target: the bind target of the object (bind target)
     *
     * return: the id of the bound object
     */
    static GLuint get_bound_object ( const object_bind_target target );

    /* is_object_bound
     *
     * return true if an object is bound to its target
     * 
     * id: the id of the object to check
     * target: the bind target of the object (bind target)
     *
     * return: true if bound, false if not
     */
    static bool is_object_bound ( const GLuint id, const object_bind_target target );



    /* to_major_object_type
     * to_object_bind_target
     * to_opengl_bind_target
     *
     * convert between enum types
     */
    static major_object_type to_major_object_type ( const minor_object_type type );
    static object_bind_target to_object_bind_target ( const minor_object_type type );
    static GLenum to_opengl_bind_target ( const object_bind_target target );

    /* is_texture2d_object_bind_target
     * is_cubemap_object_bind_target
     * 
     * returns true if the target supplied is a texture/cubemap bind target
     */
    static bool is_texture2d_object_bind_target ( const object_bind_target target );
    static bool is_cubemap_object_bind_target ( const object_bind_target target );



    /* assert_is_object_valid
     *
     * throws if an object has an id of 0
     *
     * id: the id of the object to test
     * operation: a description of the operation
     */
    static void assert_is_object_valid ( const GLuint id, const std::string& operation = "" );



private:

    /* object bindings */
    static std::array<GLuint, static_cast<unsigned> ( object_bind_target::__COUNT__ )> object_bindings;

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