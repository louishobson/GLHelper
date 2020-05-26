/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * include/glhelper/glhelper_core.hpp
 * 
 * core header for glhelper library
 * sets up OpenGL headers and defines core base classes
 * notable constructs include:
 * 
 * 
 * 
 * CLASS GLH::CORE::OBJECT
 * 
 * abstract base class to derive all OpenGL object classes from
 * it provides the basis for storing the object id as well as defining several operator overloads
 * 
 */



/* HEADER GUARD */
#ifndef GLHELPER_CORE_HPP_INCLUDED
#define GLHELPER_CORE_HPP_INCLUDED



/* INCLUDES */

/* include core headers */
#include <array>
#include <iostream>
#include <vector>

/* include GLAD followed by GLLFW
 * this order is necesarry, as GLAD defines macros, which GLFW requires even to be included
 */
#include <glad/glad.h>
#include <GLFW/glfw3.h>

/* include glhelper_exception.hpp */
#include <glhelper/glhelper_exception.hpp>



/* MACROS */

/* glh_if_constexpr
 *
 * either "if" or "if constexpr" depending on availibility
 */
#ifdef __cpp_if_constexpr
    #define glh_if_constexpr if constexpr
#else
    #define glh_if_constexpr if
#endif



/* NAMESPACE DECLARATIONS */

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



        /* DECLARATIONS FOR THIS HEADER */

        /* class object
         *
         * abstract base class to represent any OpenGL object
         */
        class object;
    }

    namespace exception
    {
        /* class object_exception : exception
         *
         * for exceptions related to object management
         */
        class object_exception;
    }
}



/* GLOBAL DECLARATIONS */

/* comparison operators for objects
 *
 * determines if two objects are equal by comparing ids
 * 
 * return: boolean representing equality
 */
bool operator== ( const glh::core::object& lhs, const glh::core::object& rhs );
bool operator!= ( const glh::core::object& lhs, const glh::core::object& rhs );



/* ENUM DEFINITIONS */

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

    GLH_UNKNOWN_TYPE,

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

    GLH_UNKNOWN_TYPE,

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

    GLH_UNKNOWN_TARGET,

    __COUNT__
};



/* OBJECT DEFINITION */

/* class object
 *
 * abstract base class to represent any OpenGL object
 */
class glh::core::object
{
public:

    /* new object constructor
     *
     * construct a new object of the type supplied
     * 
     * type: the type of the object to generate (minor type)
     */
    explicit object ( const minor_object_type type );

    /* deleted zero-parameter constructor */
    object () = delete;

    /* deleted copy constructor */
    object ( const object& other ) = delete;

    /* move constructor */
    object ( object&& other );

    /* deleted copy assignment operator
     *
     * it makes no sense to assign the object after comstruction
     * what do you want to happen to the old object?
     * what if its of a different object type?
     */
    object& operator= ( const object& other ) = delete;

    /* virtual destructor
     *
     * virtual in preparation for polymorphism
     */
    virtual ~object ();



    /* get_bound_object_pointer
     *
     * produce a pointer to the object bound to a given bind point
     * NULL is returned if no object is bound to the bind point
     */
    static object * get_bound_object_pointer ( const object_bind_target target );



    /* bind/unbind
     *
     * bind/unbind the object
     * unbinding is silently ignored if object is not already bound
     * 
     * returns true if a change in binding occured
     */
    bool bind () const;
    bool unbind () const;

    /* bind/unbind unit version
     *
     * bind/unbind the object using a unit integer
     * the base class method will always throw
     * a derived class may wish to override this method, such that unit bindings become possible
     * 
     * returns true if a change in binding occured
     */
    virtual bool bind ( const unsigned unit ) const;
    virtual bool unbind ( const unsigned unit ) const;

    /* force_unbind
     *
     * force the unbinding of the target of this object
     * 
     * returns true if a change in binding occured
     */
    bool force_unbind () const;

    /* unbind_all
     *
     * the base class definition purely calls unbind
     * however, derived classes may wish to override this method, such that all units are unbound
     * 
     * returns true if a change in binding occured
     */
    bool virtual unbind_all () const { return unbind (); }

    /* is_bound
     *
     * returns true if the object is bound
     */
    bool is_bound () const;



    /* virtual is_object_valid
     *
     * determines if the object is valid (id > 0)
     * may be overloaded when derived to add more parameters to validity
     * 
     * return: boolean representing validity
     */
    virtual bool is_object_valid () const { return ( id > 0 ); }

    /* not operator
     *
     * determines if the object is invalid
     * synonymous to !object.is_object_valid ()
     * 
     * return: boolean representing invalidity
     */
    bool operator! () const { return !is_object_valid (); }

    /* assert_is_object_valid
     *
     * throws if the object is not valid
     * 
     * operation: description of the operation
     */
    void assert_is_object_valid ( const std::string& operation = "" ) const;



    /* internal_id
     *
     * returns the internal id of the object
     */
    const GLuint& internal_id () const { return id; }

    /* get_minor/major_object_type
     * get_object_bind_target
     * get_gl_target
     *
     * get the different types/targets associated with the object
     */
    const minor_object_type& get_minor_object_type () const { return minor_type; }
    const major_object_type& get_major_object_type () const { return major_type; }
    const object_bind_target& get_object_bind_target () const { return bind_target; }
    const GLenum& get_gl_target () const { return gl_target; }



protected:

    /* NON-STATIC MEMBERS */

    /* GLint id
     *
     * the OpenGL id of the object
     */
    GLuint id;

    /* minor_type, major_type, bind_target
     *
     * the minor and major object types, as well as the object bind target
     */
    const minor_object_type minor_type;
    const major_object_type major_type;
    const object_bind_target bind_target;

    /* minor_type_index, major_type_index, bind_target_index
     * 
     * the above members, except cast to unsigned integers
     */
    const unsigned minor_type_index;
    const unsigned major_type_index;
    const unsigned bind_target_index;

    /* gl_target
     *
     * the opengl enum used for binding
     */
    const GLenum gl_target;



    /* STATIC MEMBERS */

    /* pointers to the object of each type and id */
    static std::array<std::vector<object *>, static_cast<unsigned> ( major_object_type::__COUNT__ )> object_pointers;

    /* object bindings per bind target */
    static std::array<GLuint, static_cast<unsigned> ( object_bind_target::__COUNT__ )> object_bindings;



    /* STATIC METHODS */

    /* create_object_pointers
     *
     * create object pointers array
     */
    static std::array<std::vector<object *>, static_cast<unsigned> ( major_object_type::__COUNT__ )> create_object_pointers ();

    /* create_object_bindings
     *
     * create object bindings array
     */
    static std::array<GLuint, static_cast<unsigned> ( object_bind_target::__COUNT__ )> create_object_bindings ();



    /* to_major_object_type
     * to_object_bind_target
     * to_opengl_bind_target
     *
     * convert between enum types
     */
    static major_object_type to_major_object_type ( const minor_object_type type );
    static object_bind_target to_object_bind_target ( const minor_object_type type );
    static GLenum to_opengl_bind_target ( const object_bind_target target );
    static major_object_type to_major_object_type ( const object_bind_target target );

    /* is_texture2d_bind_target
     * is_cubemap_bind_target
     * 
     * returns true if the target supplied is a texture/cubemap bind target
     */
    static bool is_texture2d_bind_target ( const object_bind_target target );
    static bool is_cubemap_bind_target ( const object_bind_target target );

};




/* OBJECT_EXCEPTION DEFINITION */

/* class object_exception : exception
 *
 * for exceptions related to object management
 */
class glh::exception::object_exception : public exception
{
public:

    /* full constructor
     *
     * __what: description of the exception
     */
    explicit object_exception ( const std::string& __what )
        : exception { __what }
    {}

    /* default zero-parameter constructor
     *
     * construct object_exception with no descrption
     */
    object_exception () = default;

    /* default everything else and inherits what () function */

};




/* #ifndef GLHELPER_CORE_HPP_INCLUDED */
#endif