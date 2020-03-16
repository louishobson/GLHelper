/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * src/glhelper_core.hpp
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

/* namespace glh
 *
 * encapsulates all glhelper structures and functions
 */
namespace glh
{
    /* class glh_object_t
     *
     * abstract base class to represent any OpenGL object
     */
    class glh_object_t;
}



/* FULL DECLARATIONS */

/* class glh_object_t
 *
 * abstract base class to represent any OpenGL object
 */
class glh::glh_object_t
{
public:

    /* int id
     *
     * the OpenGL id of the class
     */
    int id;



    /* full constructor
     *
     * _id: the id of the object
     */
    explicit glh_object_t ( const int _id )
        : id { _id }
    {}

    /* zero-parameter constructor
     *
     * in this case, id is initialised to -1
     */
    explicit glh_object_t ()
        : id { -1 }
    {}

    /* copy constructor
     *
     * no need for a move constructor, as only has to copy id
     */
    glh_object_t ( const glh_object_t& other )
        : id { other.id }
    {}

    /* default copy assignment operator
     *
     * will replace id with the id of the other class
     * the object referenced by this class will NOT be destroyed
     */
    glh_object_t& operator= ( const glh_object_t& other ) = default;

    /* virtual destructor
     *
     * virtual in preparation for polymorphism
     */
    virtual ~glh_object_t ();



    /* virtual is_valid
     *
     * determines if the object is valid (id >= 0)
     * may be overloaded when derived to add more parameters to validity
     * 
     * return: boolean representing validity
     */
    virtual bool is_valid ();

    /* virtual not operator
     *
     * determines if the object is invalid
     * synonymous to !object.is_valid ()
     * 
     * return: boolean representing invalidity
     */
    virtual bool operator! ();

    /* abstract virtual destroy
     *
     * destroys the object, at least setting id to -1
     * although multiple calls to this function are valid, only the first has effect
     */
    virtual void destroy () = 0;

};



/* #ifndef GLHELPER_CORE_HPP_INCLUDED */
#endif