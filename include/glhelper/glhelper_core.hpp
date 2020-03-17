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



/* NAMESPACE FORWARD DECLARATIONS */

namespace glh
{
    /* class object
     *
     * abstract base class to represent any OpenGL object
     */
    class object;
}



/* FULL DECLARATIONS */

/* class glh_object
 *
 * abstract base class to represent any OpenGL object
 */
class glh::object
{
public:

    /* full constructor
     *
     * _id: the id of the object
     */
    explicit object ( const int _id )
        : id { _id }
    {}

    /* zero-parameter constructor
     *
     * in this case, id is initialised to -1
     */
    explicit object ()
        : id { -1 }
    {}

    /* deleted copy constructor
     *
     * it makes no sense to copy an object
     */
    object ( const object& other ) = delete;

    /* default move constructor */
    object ( object&& other ) = default;

    /* deleted copy assignment operator
     *
     * it makes no sense to assign the object after comstruction
     */
    object& operator= ( const object& other ) = delete;

    /* virtual destructor
     *
     * virtual in preparation for polymorphism
     */
    virtual ~object () {}



    /* internal_id
     *
     * returns the internal id of the object
     */
    const int& internal_id () const { return id; }

    /* virtual is_valid
     *
     * determines if the object is valid (id >= 0)
     * may be overloaded when derived to add more parameters to validity
     * 
     * return: boolean representing validity
     */
    virtual bool is_valid () const { return ( id >= 0 ); }

    /* virtual not operator
     *
     * determines if the object is invalid
     * synonymous to !object.is_valid ()
     * 
     * return: boolean representing invalidity
     */
    virtual bool operator! () const { return !is_valid (); }

    /* virtual comparison operators
     *
     * determines if two objects are equal by comparing ids
     * may be overloaded to add more parameters to equality
     * 
     * return: boolean representing equality
     */
    virtual bool operator== ( const object& other ) const { return ( id == other.id ); }
    virtual bool operator!= ( const object& other ) const { return ( id != other.id ); }

    /* pure virtual destroy
     *
     * destroys the object, at least setting id to -1
     * although multiple calls to this function are valid, only the first should have effect
     */
    virtual void destroy () = 0;



protected:

    /* int id
     *
     * the OpenGL id of the class
     */
    int id;

};



/* #ifndef GLHELPER_CORE_HPP_INCLUDED */
#endif