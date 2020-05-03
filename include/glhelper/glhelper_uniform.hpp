/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * include/glhelper/glhelper_uniform.hpp
 * 
 * constructs for managing and using uniforms
 * mostly base classes to be derived for specific locations of uniform
 * notable constructs include:
 * 
 * 
 * 
 * CLASS GLH::CORE::UNIFORM
 * 
 * base class for an endpoint uniform
 * a value is given to the uniform via the set_... methods
 * these methods are virtual, and the specifics modification is specified by derived classes
 * 
 * 
 * 
 * CLASS GLH::CORE::STRUCT_UNIFORM
 * 
 * base class for structure uniforms
 * further members the struct extracted via the get_..._uniform methods
 * these methods are virtual, and the specifics of gettibg a member is specified by derived classes
 * 
 * 
 * 
 * CLASS GLH::CORE::ARRAY_UNIFORM
 * 
 * a template base class to represent an array of uniforms
 * the template parameter must be a derivation of the uniform, struct_uniform or array_uniform classes
 * this will be the type of uniform the array contains
 * the at method will then extract one of these uniforms at a specific index
 * this method is virtual, and the specifics of getting an element is specified by derived classes
 * 
 */



/* HEADER GUARD */
#ifndef GLHELPER_UNIFORM_HPP_INCLUDED
#define GLHELPER_UNIFORM_HPP_INCLUDED



/* INCLUDES */

/* include core headers */
#include <iostream>
#include <string>
#include <type_traits>
#include <map>

/* include glhelper_core.hpp */
#include <glhelper/glhelper_core.hpp>

/* include glhelper_exception.hpp */
#include <glhelper/glhelper_exception.hpp>

/* include glhelper_math.hpp */
#include <glhelper/glhelper_math.hpp>



/* NAMESPACE FORWARD DECLARATIONS */

namespace glh
{
    namespace core
    {
        /* class uniform
         *
         * base class for an endpoint uniform
         */
        class uniform;

        /* class struct_uniform
         *
         * base class for structure uniforms
         */
        class struct_uniform;

        /* class array_uniform
         *
         * a template base class to represent an array of uniforms
         */
        template<class T> class array_uniform;

        /* using declaration for array uniform types
         *
         * simplify common types of array uniforms a bit
         */
        using uniform_array_uniform = array_uniform<uniform>;
        using struct_array_uniform = array_uniform<struct_uniform>;
        using uniform_2d_array_uniform = array_uniform<array_uniform<uniform>>;
        using struct_2d_array_uniform = array_uniform<array_uniform<struct_uniform>>;
    }

    namespace meta
    {
        /* struct aquire_uniform_base
         *
         * aquire_uniform_base::type is typedef'ed to the base of the uniform type supplied
         */
        template<class T, class = void> struct aquire_uniform_base;
        
        /* struct is_uniform
         *
         * is_uniform::value is true if the type is a uniform
         */
        template<class T, class = void> struct is_uniform;

        /* struct is_uniform_base
         *
         * is_uniform_base::value is true if the type is a uniform base class (one of the classes defined in this header)
         */
        template<class T> struct is_uniform_base;

        /* struct is_uniform_derived
         *
         * is_uniform_derived::value is true if the type is a non-abstract uniform type
         */
        template<class T, class = void> struct is_uniform_derived;
    }

    namespace exception
    {
        /* class uniform_exception : exception
         *
         * for exceptions related to uniforms
         */
        class uniform_exception;
    }    
}



/* AQUIRE_UNIFORM_BASE DEFINITION */

/* struct aquire_uniform_base
*
* aquire_uniform_base::type is typedef'ed to the base of the uniform type supplied
*/
template<class T, class> struct glh::meta::aquire_uniform_base {};
template<class T> struct glh::meta::aquire_uniform_base<T, typename std::enable_if<std::is_base_of<glh::core::uniform, T>::value>::type>
{
    typedef core::uniform type;
};
template<class T> struct glh::meta::aquire_uniform_base<T, typename std::enable_if<std::is_base_of<glh::core::struct_uniform, T>::value>::type>
{
    typedef core::struct_uniform type;
};
template<class T> struct glh::meta::aquire_uniform_base<T, typename std::enable_if<std::is_base_of<glh::core::array_uniform<typename glh::meta::aquire_uniform_base<typename T::type>::type>, T>::value>::type>
{
    typedef core::array_uniform<typename aquire_uniform_base<typename T::type>::type> type;
};



/* IS_UNIFORM DEFINITION */

/* struct is_uniform
 *
 * is_uniform::value is true if the type is a uniform
 */
template<class T, class> struct glh::meta::is_uniform : std::false_type {};
template<class T> struct glh::meta::is_uniform<T, typename std::enable_if<( sizeof ( typename glh::meta::aquire_uniform_base<T>::type ) > 0 )>::type> : std::true_type {};



/* IS_UNIFORM_BASE DEFINITION */

/* struct is_uniform_base
 *
 * is_uniform_base::value is true if the type is a uniform base class (one of the classes defined in this header)
 */
template<class T> struct glh::meta::is_uniform_base : std::false_type {};
template<> struct glh::meta::is_uniform_base<glh::core::uniform> : std::true_type {};
template<> struct glh::meta::is_uniform_base<glh::core::struct_uniform> : std::true_type {};
template<> template<class _T> struct glh::meta::is_uniform_base<glh::core::array_uniform<_T>> : is_uniform_base<_T> {};



/* IS_UNIFORM_DERIVED DEFINITION */

/* struct is_uniform_derived
 *
 * is_uniform_derived::value is true if the type is a non-abstract uniform type
 */
template<class T, class> struct glh::meta::is_uniform_derived : std::false_type {};
template<class T> struct glh::meta::is_uniform_derived<T, typename std::enable_if<glh::meta::is_uniform<T>::value && !std::is_abstract<T>::value>::type> : std::true_type {};



/* UNIFORM DEFINITION */

/* class uniform
 *
 * base class for an endpoint uniform
 */
class glh::core::uniform
{
public:

    /* full constructor
     *
     * _name: the name of the uniform
     */
    uniform ( const std::string& _name )
        : name { _name }
    {}

    /* deleted zero-parameter constructor */
    uniform () = delete;

    /* default copy constructor */
    uniform ( const uniform& other ) = default;

    /* deleted copy assignment operator */
    uniform& operator= ( const uniform& other ) = delete;

    /* virtual default destructor */
    virtual ~uniform () = default;



    /* pure virtual set_float
     *
     * set uniform based on float value(s)
     * 
     * v0...: the value(s) to set the uniform to
     */
    virtual void set_float ( const GLfloat v0 ) const = 0;
    virtual void set_float ( const GLfloat v0, const GLfloat v1 ) const = 0;
    virtual void set_float ( const GLfloat v0, const GLfloat v1, const GLfloat v2 ) const = 0;
    virtual void set_float ( const GLfloat v0, const GLfloat v1, const GLfloat v2, const GLfloat v3 ) const = 0;

    /* pure virtual set_int
     *
     * set uniform based on integer value(s)
     * 
     * v0...: the value(s) to set the uniform to
     */
    virtual void set_int ( const GLint v0 ) const = 0;
    virtual void set_int ( const GLint v0, const GLint v1 ) const = 0;
    virtual void set_int ( const GLint v0, const GLint v1, const GLint v2 ) const = 0;
    virtual void set_int ( const GLint v0, const GLint v1, const GLint v2, const GLint v3 ) const = 0;

    /* pure virtual set_uint
     *
     * set uniform based on unsigned integer value(s)
     * 
     * v0...: the value(s) to set the uniform to
     */
    virtual void set_uint ( const GLuint v0 ) const = 0;
    virtual void set_uint ( const GLuint v0, const GLuint v1 ) const = 0;
    virtual void set_uint ( const GLuint v0, const GLuint v1, const GLuint v2 ) const = 0;
    virtual void set_uint ( const GLuint v0, const GLuint v1, const GLuint v2, const GLuint v3 ) const = 0;

    /* pure virtual set_matrix
     *
     * set uniform based on a matrix
     * 
     * name: the name of the uniform
     * v0: the matrix to set the uniform to
     */
    virtual void set_matrix ( const glh::math::mat2& v0 ) const = 0;
    virtual void set_matrix ( const glh::math::mat3& v0 ) const = 0;
    virtual void set_matrix ( const glh::math::mat4& v0 ) const = 0;
    
    /* pure virtual set_vector
     *
     * set uniform based on a vector
     * 
     * v0: the vector to set the uniform to
     */
    virtual void set_vector ( const glh::math::vec1& v0 ) const = 0;
    virtual void set_vector ( const glh::math::vec2& v0 ) const = 0;
    virtual void set_vector ( const glh::math::vec3& v0 ) const = 0;
    virtual void set_vector ( const glh::math::vec4& v0 ) const = 0;



    /* is_set_valid
     *
     * returnd true if it is save to set values
     */
    virtual bool is_set_valid () const = 0;

    /* assert_is_set_valid
     *
     * throws if the uniform is not set valid
     * 
     * operation: description of the operation
     */
    virtual void assert_is_set_valid ( const std::string& operation ) const = 0;



    /* get_name
     *
     * return the name of the uniform
     */
    const std::string& get_name () const { return name; }



protected:

    /* store the name of the uniform */
    const std::string name;

};



/* STRUCT_UNIFORM DEFINTION */



/* class struct_uniform
*
* base class for structure uniforms
*/
class glh::core::struct_uniform
{
public:

    /* full constructor
     *
     * _name: the name of the uniform
     */
    struct_uniform ( const std::string& _name )
        : name { _name }
    {}

    /* deleted zero-parameter constructor */
    struct_uniform () = delete;

    /* default copy constructor */
    struct_uniform ( const struct_uniform& other ) = default;

    /* deleted copy assignment operator */
    struct_uniform& operator= ( const struct_uniform& other ) = delete;

    /* virtual default destructor */
    virtual ~struct_uniform () = default;



    /* pure virtual get_(struct_)uniform
     *
     * get a member of the struct
     */
    virtual uniform& get_uniform ( const std::string& member ) = 0;
    virtual const uniform& get_uniform ( const std::string& member ) const = 0;
    virtual struct_uniform& get_struct_uniform ( const std::string& member ) = 0;
    virtual const struct_uniform& get_struct_uniform ( const std::string& member ) const = 0;
    
    /* pure virtual get_..._array_uniform
     *
     * get array uniforms out of the struct
     */
    virtual uniform_array_uniform& get_uniform_array_uniform ( const std::string& member ) = 0;
    virtual const uniform_array_uniform& get_uniform_array_uniform ( const std::string& member ) const = 0;
    virtual struct_array_uniform& get_struct_array_uniform ( const std::string& member ) = 0;
    virtual const struct_array_uniform& get_struct_array_uniform ( const std::string& member ) const = 0;
    virtual uniform_2d_array_uniform& get_uniform_2d_array_uniform ( const std::string& member ) = 0;
    virtual const uniform_2d_array_uniform& get_uniform_2d_array_uniform ( const std::string& member ) const = 0;
    virtual struct_2d_array_uniform& get_struct_2d_array_uniform ( const std::string& member ) = 0;
    virtual const struct_2d_array_uniform& get_struct_2d_array_uniform ( const std::string& member ) const = 0;



    /* get_name
     *
     * return the name of the uniform
     */
    const std::string& get_name () const { return name; }



protected:

    /* store the name of the uniform */
    const std::string name;

};



/* ARRAY_UNIFORM DEFINITION */

/* class array_uniform
 *
 * a template base class to represent an array of uniforms
 */
template<class T> class glh::core::array_uniform
{

    /* static assert that T is a base uniform */
    static_assert ( meta::is_uniform_base<T>::value, "class array_uniform must have a template paramater of a base uniform type" );

public:

    /* full constructor
     *
     * _name: the name of the uniform
     */
    array_uniform ( const std::string& _name )
        : name { _name }
    {}

    /* deleted zero-parameter constructor */
    array_uniform () = delete;

    /* default copy constructor */
    array_uniform ( const array_uniform& other ) = default;

    /* deleted copy assignment operator */
    array_uniform& operator= ( const array_uniform& other ) = delete;

    /* virtual default destructor */
    virtual ~array_uniform () = default;



    /* typedef of T */
    typedef T type;



    /* pure virtual at
     *
     * return the uniform at an index
     */
    virtual T& at ( const unsigned i ) = 0;
    virtual const T& at ( const unsigned i ) const = 0;

    /* operator[]
     *
     * return value using at method
     */
    T& operator[] ( const unsigned i ) { return at ( i ); }
    const T& operator[] ( const unsigned i ) const { return at ( i ); }



    /* get_name
     *
     * return the name of the uniform
     */
    const std::string& get_name () const { return name; }



protected:

    /* store the name of the uniform */
    const std::string name;

};



/* UNIFORM_EXCEPTION DEFINITION */

/* class uniform_exception : exception
 *
 * for exceptions related to uniforms
 */
class glh::exception::uniform_exception : public exception
{
public:

    /* full constructor
     *
     * __what: description of the exception
     */
    explicit uniform_exception ( const std::string& __what )
        : exception { __what }
    {}

    /* default zero-parameter constructor
     *
     * construct uniform_exception with no descrption
     */
    uniform_exception () = default;

    /* default everything else and inherits what () function */

};



/* #ifndef GLHELPER_UNIFORM_HPP_INCLUDED */
#endif
