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



/* NAMESPACE FORWARD DECLARATIONS */

namespace glh
{
    namespace core
    {
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
    explicit object ( const GLuint _id )
        : id { _id }
    {}

    /* zero-parameter constructor
     *
     * in this case, id is initialised to 0
     */
    object ()
        : id { 0 }
    {}

    /* deleted copy constructor
     *
     * it makes no sense to copy an object
     * do you want to duplicate it, or create a reference to the same object?
     */
    object ( const object& other ) = delete;

    /* default move constructor */
    object ( object&& other )
        : id { other.id }
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
    virtual ~object () { id = 0; }



    /* internal_id
     *
     * returns the internal id of the object
     */
    const GLuint& internal_id () const { return id; }

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
    void assert_is_object_valid ( const std::string& operation = "" ) const;

    /* virtual not operator
     *
     * determines if the object is invalid
     * synonymous to !object.is_object_valid ()
     * 
     * return: boolean representing invalidity
     */
    virtual bool operator! () const { return !is_object_valid (); }

    /* pure virtual destroy
     *
     * destroys the object, at least setting id to 0
     * although multiple calls to this function are valid, only the first should have effect
     */
    virtual void destroy () = 0;



protected:

    /* GLint id
     *
     * the OpenGL id of the object
     */
    GLuint id;

};

/* comparison operators
 *
 * determines if two objects are equal by comparing ids
 * 
 * return: boolean representing equality
 */
bool inline operator== ( const glh::core::object& lhs, const glh::core::object& rhs ) { return ( lhs.internal_id () == rhs.internal_id () ); }
bool inline operator!= ( const glh::core::object& lhs, const glh::core::object& rhs ) { return ( lhs.internal_id () != rhs.internal_id () ); }



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
void inline glh::core::object::assert_is_object_valid ( const std::string& operation ) const
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