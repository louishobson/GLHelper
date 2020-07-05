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
 * ENUM STRUCT GLH::CORE::MINOR_OBJECT_TYPE
 * ENUM STRUCT GLH::CORE::OBJECT_BIND_TARGET
 * ENUM STRUCT GLH::CORE::MAJOR_OBJECT_TYPE
 * 
 * different types of object and bind points
 * a minor object type can be converted to a more general major type
 * a minor object type can be converted to its default bind point
 * there may also be more bind points available to that object type, however
 * this is up to derivations to implement access to those
 * 
 * 
 * 
 * CLASS GLH::CORE::OBJECT
 * 
 * abstract base class to derive all OpenGL object classes from
 * it provides the basis for storing the object id as well as defining several operator overloads
 * 
 * 
 * 
 * CLASS GLH::CORE::OBJECT_POINTER
 * 
 * acts as a pointer to an object that may at some point be destroyed
 * one can get an actual pointer to the object through a static method of the object class or through the operators in this class
 * 
 */



/* HEADER GUARD */
#ifndef GLHELPER_CORE_HPP_INCLUDED
#define GLHELPER_CORE_HPP_INCLUDED



/* INCLUDES */

/* include core headers */
#include <iostream>
#include <map>
#include <type_traits>
#include <utility>

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
        /* class object
         *
         * abstract base class to represent any OpenGL object
         */
        class object;

        /* class object_pointer
         *
         * safely references an object
         */
        template<class T> class object_pointer;
        template<class T> using const_object_pointer = object_pointer<const T>;

    }

    namespace meta
    {
        /* struct is_object
         *
         * if T is an object, value is true, else is false
         */
        template<class T> struct is_object;

        /* stuct promote_arithmetic_type
         *
         * if A preferred type can be determined, the class has a member type of that type
         */
        template<class T0, class T1, class = void> struct promote_arithmetic_type;
        template<class T0, class T1> using promote_arithmetic_type_t = typename promote_arithmetic_type<T0, T1>::type;
        template<class T0, class T1> using pat = promote_arithmetic_type<T0, T1>;
        template<class T0, class T1> using pat_t = typename promote_arithmetic_type<T0, T1>::type;
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



/* IS_OBJECT DEFINITION */

/* struct is_object
 *
 * if T is an object, value is true, else is false
 */
template<class T> struct glh::meta::is_object : public std::is_base_of<glh::core::object, T> {};



/* PROMOTE_ARITHMETIC_TYPE DEFINITION */

/* stuct promote_arithmetic_type
 *
 * if A preferred type can be determined, the class has a member type of that type
 */
/* default empty struct */
template<class T0, class T1, class> struct glh::meta::promote_arithmetic_type {};
/* if one is floating, and the other is not, choose the floating point */
template<class T0, class T1> struct glh::meta::promote_arithmetic_type<T0, T1, std::enable_if_t
<
    std::is_arithmetic<T0>::value && std::is_arithmetic<T1>::value &&
    ( ( std::is_floating_point<T0>::value && !std::is_floating_point<T1>::value ) || ( std::is_floating_point<T1>::value && !std::is_floating_point<T0>::value ) )
>> { typedef std::conditional_t<std::is_floating_point<T0>::value, T0, T1> type; };
/* if both are floating, choose the larger floating point */
template<class T0, class T1> struct glh::meta::promote_arithmetic_type<T0, T1, std::enable_if_t
<
    std::is_arithmetic<T0>::value && std::is_arithmetic<T1>::value &&
    std::is_floating_point<T0>::value && std::is_floating_point<T1>::value
>> { typedef std::conditional_t<sizeof ( T0 ) >= sizeof ( T1 ), T0, T1> type; };
/* if both are signed ints or both are unsigned ints, choose the larger int */
template<class T0, class T1> struct glh::meta::promote_arithmetic_type<T0, T1, std::enable_if_t
<
    std::is_arithmetic<T0>::value && std::is_arithmetic<T1>::value &&
    std::is_integral<T0>::value && std::is_integral<T1>::value &&
    ( ( std::is_signed<T0>::value && std::is_signed<T1>::value ) || ( std::is_unsigned<T0>::value && std::is_unsigned<T1>::value ) )
>> { typedef std::conditional_t<sizeof ( T0 ) >= sizeof ( T1 ), T0, T1> type; };
/* if one is signed, and the other is unsigned, choose the signed int */
template<class T0, class T1> struct glh::meta::promote_arithmetic_type<T0, T1, std::enable_if_t
<
    std::is_arithmetic<T0>::value && std::is_arithmetic<T1>::value &&
    std::is_integral<T0>::value && std::is_integral<T1>::value &&
    ( ( std::is_signed<T0>::value && std::is_unsigned<T1>::value ) || ( std::is_signed<T1>::value && std::is_unsigned<T0>::value ) )
>> { typedef std::conditional_t<std::is_signed<T0>::value, T0, T1> type; };



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



/* OBJECT DEFINITION */

/* class object
 *
 * abstract base class to represent any OpenGL object
 */
class glh::core::object
{

    /* friend of object_pointer */
    template<class T> friend class object_pointer;

public:

    /* new object constructor
     *
     * _id: the id of the new object (defaults to 0)
     */
    explicit object ( const unsigned _id = 0 );

    /* deleted copy constructor */
    object ( const object& other ) = delete;

    /* move constructor */
    object ( object&& other );

    /* deleted copy assignment operator */
    object& operator= ( const object& other ) = delete;

    /* virtual destructor */
    virtual ~object ();



    /* virtual bind/unbind
     *
     * implemented by derivations to bind the object
     * if the object is not bound on a call to unbind, the function will do nothing
     * the base class method will always throw
     * 
     * returns true if a change in binding occured
     */
    virtual bool bind () const
        { throw exception::object_exception { "not a bindable object" }; }
    virtual bool unbind () const
        { throw exception::object_exception { "not an unbindable object" }; }
    virtual bool bind ( const unsigned index ) const
        { throw exception::object_exception { "not an index bindable object" }; }
    virtual bool unbind ( const unsigned index ) const
        { throw exception::object_exception { "not an index unbindable object" }; }



    /* unbind_all
     *
     * cam be implemented by derivations to unbind the object from all bind points
     * the base class just calls unbind
     * 
     * returns true if a change in binding occured
     */
    bool virtual unbind_all () const { return unbind (); }


    
    /* virtual is_bound
     *
     * returns true if the object is bound
     * base class method always returns false
     */
    virtual bool is_bound () const { return false; }



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



    /* comparison operator */
    bool operator== ( const object& other ) const { return unique_id == other.unique_id; }
    bool operator!= ( const object& other ) const { return unique_id != other.unique_id; }



    /* internal_id
     *
     * returns the internal id of the object
     */
    const unsigned& internal_id () const { return id; }

    /* internal_unique_id
     *
     * returns the unique id of the object
     */
    const unsigned& internal_unique_id () const { return unique_id; }



protected:

    /* NON-STATIC MEMBERS */

    /* id
     *
     * the OpenGL id of the object
     */
    unsigned id;

    /* unique_id
     *
     * the unique id of the object
     * will be different for EVERY object that ever exists
     * will not be changed for move-constructed objects, however
     */
    const unsigned unique_id;



    /* STATIC MEMBERS */

    /* next_unique_id
     *
     * the next unique id to be used
     * incremented for each new object
     */
    static unsigned next_unique_id;

    /* object_pointers
     *
     * map between unique ids and pointers to their objects
     */
    static std::map<unsigned, object *> object_pointers;


};




/* OBJECT_POINTER DEFINITION */

/* class object_pointer
 *
 * safely points to an object 
 */
template<class T> class glh::core::object_pointer
{

    /* static assert that T is an object type */
    static_assert ( meta::is_object<T>::value, "object_pointer cannot be instantiated from non-object-derived type" );

public:

    /* construct from an object */
    object_pointer ( T& obj )
        : id { obj.internal_id () }
        , unique_id { obj.internal_unique_id () }
    {}

    /* pointer constructor */
    object_pointer ( T * obj )
        : id ( obj ? obj->internal_id () : 0 )
        , unique_id { obj ? obj->internal_unique_id () : 0 }
    {}

    /* zero-parameter constructor */
    object_pointer ()
        : id { 0 }
        , unique_id { 0 }
    {}

    /* default copy constructor */
    object_pointer ( const object_pointer<T>& other ) = default;

    /* assignment from object */
    object_pointer& operator= ( T& obj );
    object_pointer& operator= ( T * obj );

    /* default copy assignment operator */
    object_pointer& operator= ( const object_pointer& other ) = default;

    /* default destructor */
    ~object_pointer () = default;



    /* pointer operators */
    T * operator* () const { return get (); }
    T * operator-> () const { return get (); }

    /* get
     *
     * get pointer to object
     * returns NULL if invalid
     */
    T * get () const;

    /* implicitly cast into pointer */
    operator T * () const { return get (); }



    /* is_valid
     *
     * returns true if the pointer is valid
     */
    bool is_pointer_valid () const { return get () != NULL; }

    /* operator bool
     *
     * returns true if the pointer is valid
     */
    operator bool () const { return is_pointer_valid (); }

    /* operator!
     *
     * returns true if the pointer is invalid
     */
    bool operator! () const { return !is_pointer_valid (); }



    /* comparison operators */
    bool operator== ( const object_pointer& other ) const { return unique_id == other.unique_id; }
    bool operator== ( const T& obj ) const { return unique_id == obj->internal_unique_id (); }
    bool operator== ( const T * obj ) const { return ( obj ? unique_id == obj->internal_unique_id () : unique_id == 0 ); }
    bool operator!= ( const object_pointer& other ) const { return unique_id == other.unique_id; }
    bool operator!= ( const T& obj ) const { return unique_id != obj->internal_unique_id (); }
    bool operator!= ( const T * obj ) const { return ( obj ? unique_id != obj->internal_unique_id () : unique_id != 0 ); }



    /* internal_id
     * internal_unique_id
     *  
     * get the id and unique id
     */
    const unsigned& internal_id () const { return id; }
    const unsigned& internal_unique_id () const { return unique_id; }



private:

    /* id and unique id */
    unsigned id;
    unsigned unique_id;

};




/* OBJECT_POINTER IMPLEMENTATION */

/* assignment from object */
template<class T>
glh::core::object_pointer<T>& glh::core::object_pointer<T>::operator= ( T& obj )
{
    /* set id and unique id */
    id = obj.internal_id ();
    unique_id = obj.internal_unique_id ();

    /* return * this */
    return * this;
}
template<class T>
glh::core::object_pointer<T>& glh::core::object_pointer<T>::operator= ( T * obj )
{
    /* set id and unique id */
    if ( obj )
    {
        id = obj->internal_id ();
        unique_id = obj->internal_unique_id ();
    } else
    {
        id = 0;
        unique_id = 0;
    }
    
    /* return * this */
    return * this;
}

/* get
 *
 * get pointer to object
 * returns NULL if invalid
 */
template<class T> inline T * glh::core::object_pointer<T>::get () const
{
    /* if unique_id == 0 return NULL */
    if ( unique_id == 0 ) return NULL;

    /* find the pointer */
    auto ptr = object::object_pointers.find ( unique_id );

    /* if not found, return NULL, else cast the pointer and return it */
    return dynamic_cast<T *> ( ptr == object::object_pointers.end () ? NULL : ptr->second );
}




/* #ifndef GLHELPER_CORE_HPP_INCLUDED */
#endif