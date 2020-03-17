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
 * errors can be handled more effectively by the __GLH_PARSE_ERROR_EVENT__ macro function
 * this macro takes a handle level, exception type, message and return and based on the handle, will respond appropriately
 * handle levels are defined in each header such like GLH_ERROR_HANDLE_..., with the following possible values:
 *      -> 0: do not treat as error
 *      -> 1: produce warning, but do not treat as error
 *      -> 2: treat as error, however don't create an exception
 *      -> 3: throw an exception
 * defining these on compilation will override the defaults in the source
 * there is also a GLH_ERROR_HANDLE_OVERRIDE in this source, which allows for a complete override of all these handle levels 
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



/* MACROS */

/* GLH_ERROR_HANDLE_OVERRRIDE
 *
 * override all error handling macros with a new handling level
 */
#ifndef GLH_ERROR_HANDLE_OVERRIDE
#endif

/* __GLH_PARSE_ERROR_EVENT__
 *
 * macro function to handle a possible error event
 * 
 * __handle: the error handle (e.g. GLH_ERROR_HANDLE_OVERRIDE)
 * __type: the type of the exception to raise (derived from std::exception)
 * __msg: the message to accompany the error
 * __rt: what to return if an error has occured
 */
#ifdef GLH_ERROR_HANDLE_OVERRIDE
#define __GLH_PARSE_ERROR_EVENT__(__handle, __type, __msg, __rt) \   
    if ( GLH_ERROR_HANDLE_OVERRIDE == 0 ) {} else \
    if ( GLH_ERROR_HANDLE_OVERRIDE == 1 ) std::cerr << __msg << std::endl; else \
    if ( GLH_ERROR_HANDLE_OVERRIDE == 2 ) { std::cerr << __msg << std::endl; return __rt; ) else \
    if ( GLH_ERROR_HANDLE_OVERRIDE == 3 ) throw __type { __msg };
#else
#define __GLH_PARSE_ERROR_EVENT__(__handle, __type, __msg, __rt) \
    if ( __handle == 0 ) {} else \
    if ( __handle == 1 ) std::cerr << "GLH WARN: " << __msg << std::endl; else \
    if ( __handle == 2 ) { std::cerr << "GLH ERROR: " << __msg << std::endl; return __rt; } else \
    if ( __handle == 3 ) throw new __type { __msg };
#endif



/* NAMESPACE FORWARD DECLARATIONS */

namespace glh
{
    /* class exception : std::exception
     *
     * base class for exceptions in glh
     */
    class exception;
}



/* FULL DECLARATIONS */

/* class exception : std::exception
 *
 * base class for exceptions in glh
 */
class glh::exception : public std::exception
{
public:
    
    /* full constructor
     *
     * __what: description of the exception
     */
    explicit exception ( const char * __what )
        : _what { __what }
    {}

    /* zero-parameter constructor
     *
     * creates an exception with no description (by setting _what to NULL)
     */
    explicit exception ()
        : _what { NULL }
    {}

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
    virtual const char * what () const throw () { return _what; }


private:

    /* const char * _what
     *
     * string containing description of the exception
     */
    const char * _what; 

};



/* #ifndef GLHELPER_EXCEPTION_HPP_INCLUDED */
#endif