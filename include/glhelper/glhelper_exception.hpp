/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * include/glhelper/glhelper_exception.hpp
 * 
 * declares the basis for exception handling for the library
 * notable constructs include:
 * 
 * 
 * 
 * CLASS GLH::EXCEPTION::EXCEPTION
 * 
 * base class (inheriting from std::exception) to derive all GLHelper exceptions from
 * means that minimal effort has to be put into defining new exceptions, as this class defines most of the leg work
 * 
 */



/* HEADER GUARD */
#ifndef GLHELPER_EXCEPTION_HPP_INCLUDED
#define GLHELPER_EXCEPTION_HPP_INCLUDED



/* INCLUDES */

/* include core headers */
#include <exception>
#include <iostream>
#include <string>



/* NAMESPACE DECLARATIONS */

namespace glh
{
    namespace exception
    {
        /* class exception : std::exception
         *
         * base class for exceptions in glh
         */
        class exception;
    }
}



/* EXCEPTION DEFINITION */

/* class exception : std::exception
 *
 * base class for exceptions in glh
 */
class glh::exception::exception : public std::exception
{
public:
    
    /* full constructor
     *
     * __what: description of the exception
     */
    explicit exception ( const std::string& __what )
        : _what { __what }
    {}

    /* default zero-parameter constructor
     *
     * creates an exception with no description (by setting _what to NULL)
     */
    exception () = default;

    /* default copy constructor */
    /* default asignment operator */
    
    /* virtual default destructor
     *
     * virtual in preparation for polymorphism
     */
    virtual ~exception () {}

    

    /* virtual what
     *
     * returns the description of the exception
     * 
     * return: _what
     */
    virtual const char * what () const throw () { return _what.c_str (); }


private:

    /* std::string _what
     *
     * string containing description of the exception
     */
    const std::string _what; 

};



/* #ifndef GLHELPER_EXCEPTION_HPP_INCLUDED */
#endif