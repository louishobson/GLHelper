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
#include <iostream>

/* include GLAD followed by GLLFW
 * this order is necesarry, as GLAD defines macros, which GLFW requires even to be included
 */
#include <glad/glad.h>
#include <GLFW/glfw3.h>

/* include glhelper_exception.hpp */
#include <glhelper/glhelper_exception.hpp>

/* include glhelper_manager.hpp */
#include <glhelper/glhelper_manager.hpp>



/* NAMESPACE DECLARATIONS */

namespace glh
{
    namespace core
    {
        /* FORWARD DECLARATIONS FROM GLHELPER_MANAGER.HPP HEADER */

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



        /* DECLARATIONS FOR THIS HEADER */

        /* class object
         *
         * abstract base class to represent any OpenGL object
         */
        class object;
    }

    namespace exception
    {
        /* class invalid_object_exception : exception
         *
         * for exceptions related to invalid objects
         */
        class invalid_object_exception;
    }
}

/* comparison operators for objects
 *
 * determines if two objects are equal by comparing ids
 * 
 * return: boolean representing equality
 */
bool operator== ( const glh::core::object& lhs, const glh::core::object& rhs );
bool operator!= ( const glh::core::object& lhs, const glh::core::object& rhs );



/* OBJECT DEFINITION */

/* class object
 *
 * abstract base class to represent any OpenGL object
 */
class glh::core::object
{
public:

    /* full constructor
     *
     * _id: the id of the object
     */
    explicit object ( const minor_object_type _minor_type );

    /* deleted zero-parameter constructor */
    object () = delete;

    /* deleted copy constructor
     *
     * it makes no sense to copy an object
     * do you want to duplicate it, or create a reference to the same object?
     */
    object ( const object& other ) = delete;

    /* default move constructor */
    object ( object&& other )
        : id { other.id }
        , minor_type { other.minor_type }
        , major_type { other.major_type }
        , bind_target { other.bind_target }
        , gl_target { other.gl_target }
    { other.id = 0; }

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
    virtual ~object () { destroy (); }



    /* bind/unbind
     *
     * bind/unbind the object
     */
    virtual void bind () const;
    virtual void unbind () const;

    /* is_bound
     *
     * returns true if the object is bound
     */
    virtual bool is_bound () const;



    /* destroy
     *
     * destroys the object, setting its id to 0
     */
    void destroy ();



    /* virtual is_object_valid
     *
     * determines if the object is valid (id > 0)
     * may be overloaded when derived to add more parameters to validity
     * 
     * return: boolean representing validity
     */
    virtual bool is_object_valid () const { return ( id > 0 ); }

    /* assert_is_object_valid
     *
     * throws if the object is not valid
     * 
     * operation: description of the operation
     */
    virtual void assert_is_object_valid ( const std::string& operation = "" ) const;



    /* not operator
     *
     * determines if the object is invalid
     * synonymous to !object.is_object_valid ()
     * 
     * return: boolean representing invalidity
     */
    bool operator! () const { return !is_object_valid (); }



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

    /* gl_target
     *
     * the opengl enum used for binding
     */
    const GLenum gl_target;
};




/* INVALID_OBJECT_EXCEPTION DEFINITION */

/* class invalid_object_exception : exception
 *
 * for exceptions related to invalid objects
 */
class glh::exception::invalid_object_exception : public exception
{
public:

    /* full constructor
     *
     * __what: description of the exception
     */
    explicit invalid_object_exception ( const std::string& __what )
        : exception { __what }
    {}

    /* default zero-parameter constructor
     *
     * construct invalid_object_exception with no descrption
     */
    invalid_object_exception () = default;

    /* default everything else and inherits what () function */

};




/* OBJECT IMPLEMENTATION */

/* assert_is_object_valid
 *
 * throws if the object is not valid
 * 
 * operation: description of the operation
 */
inline void glh::core::object::assert_is_object_valid ( const std::string& operation ) const
{ 
    /* throw if invalid */
    if ( !is_object_valid () ) 
    {
        if ( operation.size () > 0 ) throw exception::invalid_object_exception { "attempted to perform " + operation + " operation on invalid OpenGL object" }; 
        else throw exception::invalid_object_exception { "attempted to perform operation on invalid OpenGL object" }; 
    }
}



/* #ifndef GLHELPER_CORE_HPP_INCLUDED */
#endif