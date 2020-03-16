/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * src/glhelper_exception.hpp
 * 
 * declares the basis for exception handling for the library
 * 
 */



/* HEADER GUARD */
#ifndef GLHELPER_EXCEPTION_HPP_INCLUDED
#define GLHELPER_EXCEPTION_HPP_INCLUDED



/* INCLUDES */

/* include core headers */
#include <iostream>

/* include exception */
#include <exception>

/* include glhelper_core.hpp */
#include <glhelper/glhelper_core.hpp>



/* NAMESPACE FORWARD DECLARATIONS */

namespace glh
{
    /* class glh_exception : std::exception
     *
     * base class for exceptions in glh
     */
    class glh_exception;
}



/* FULL DECLARATIONS */

/* class glh_exception : std::exception
 *
 * base class for exceptions in glh
 */
class glh::glh_exception : public std::exception
{
    /* _what
     *
     * string containing description of the exception
     */
    const char * _what; 


    
    /* full constructor
     *
     * __what: description of the exception
     */
    glh_exception ( const char * __what )
        : _what { __what }
    {}

    /* zero-parameter constructor
     *
     * creates an exception with no description (by setting _what to NULL)
     */
    glh_exception ()
        : _what { NULL }
    {}

    /* default copy constructor */
    /* default asignment operator */
    
    /* virtual default destructor
     *
     * virtual in preparation for polymorphism
     */
    virtual ~glh_exception () {}

    

    /* virtual what
     *
     * returns the description of the exception
     * 
     * return: _what
     */
    virtual const char * what () const throw () { return _what; }

};



/* #ifndef GLHELPER_EXCEPTION_HPP_INCLUDED */
#endif