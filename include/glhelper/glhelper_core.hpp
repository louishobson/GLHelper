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
#include <array>
#include <iostream>
#include <type_traits>
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
    GLH_TEXTURE2D_ARRAY_TYPE,
    GLH_TEXTURE2D_MULTISAMPLE_TYPE,
    GLH_CUBEMAP_TYPE,
    GLH_CUBEMAP_ARRAY_TYPE,

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
    GLH_READ_FBO_TARGET,
    GLH_DRAW_FBO_TARGET,

    GLH_PROGRAM_TARGET,

    __TEXTURE2D_START__,
    GLH_TEXTURE2D_0_TARGET,  GLH_TEXTURE2D_1_TARGET,  GLH_TEXTURE2D_2_TARGET,  GLH_TEXTURE2D_3_TARGET,  GLH_TEXTURE2D_4_TARGET,  GLH_TEXTURE2D_5_TARGET,  GLH_TEXTURE2D_6_TARGET,  GLH_TEXTURE2D_7_TARGET, 
    GLH_TEXTURE2D_8_TARGET,  GLH_TEXTURE2D_9_TARGET,  GLH_TEXTURE2D_10_TARGET, GLH_TEXTURE2D_11_TARGET, GLH_TEXTURE2D_12_TARGET, GLH_TEXTURE2D_13_TARGET, GLH_TEXTURE2D_14_TARGET, GLH_TEXTURE2D_15_TARGET, 
    GLH_TEXTURE2D_16_TARGET, GLH_TEXTURE2D_17_TARGET, GLH_TEXTURE2D_18_TARGET, GLH_TEXTURE2D_19_TARGET, GLH_TEXTURE2D_20_TARGET, GLH_TEXTURE2D_21_TARGET, GLH_TEXTURE2D_22_TARGET, GLH_TEXTURE2D_23_TARGET, 
    GLH_TEXTURE2D_24_TARGET, GLH_TEXTURE2D_25_TARGET, GLH_TEXTURE2D_26_TARGET, GLH_TEXTURE2D_27_TARGET, GLH_TEXTURE2D_28_TARGET, GLH_TEXTURE2D_29_TARGET, GLH_TEXTURE2D_30_TARGET, GLH_TEXTURE2D_31_TARGET, 
    __TEXTURE2D_END__,

    __TEXTURE2D_ARRAY_START__,
    GLH_TEXTURE2D_ARRAY_0_TARGET,  GLH_TEXTURE2D_ARRAY_1_TARGET,  GLH_TEXTURE2D_ARRAY_2_TARGET,  GLH_TEXTURE2D_ARRAY_3_TARGET,  GLH_TEXTURE2D_ARRAY_4_TARGET,  GLH_TEXTURE2D_ARRAY_5_TARGET,  GLH_TEXTURE2D_ARRAY_6_TARGET,  GLH_TEXTURE2D_ARRAY_7_TARGET, 
    GLH_TEXTURE2D_ARRAY_8_TARGET,  GLH_TEXTURE2D_ARRAY_9_TARGET,  GLH_TEXTURE2D_ARRAY_10_TARGET, GLH_TEXTURE2D_ARRAY_11_TARGET, GLH_TEXTURE2D_ARRAY_12_TARGET, GLH_TEXTURE2D_ARRAY_13_TARGET, GLH_TEXTURE2D_ARRAY_14_TARGET, GLH_TEXTURE2D_ARRAY_15_TARGET, 
    GLH_TEXTURE2D_ARRAY_16_TARGET, GLH_TEXTURE2D_ARRAY_17_TARGET, GLH_TEXTURE2D_ARRAY_18_TARGET, GLH_TEXTURE2D_ARRAY_19_TARGET, GLH_TEXTURE2D_ARRAY_20_TARGET, GLH_TEXTURE2D_ARRAY_21_TARGET, GLH_TEXTURE2D_ARRAY_22_TARGET, GLH_TEXTURE2D_ARRAY_23_TARGET, 
    GLH_TEXTURE2D_ARRAY_24_TARGET, GLH_TEXTURE2D_ARRAY_25_TARGET, GLH_TEXTURE2D_ARRAY_26_TARGET, GLH_TEXTURE2D_ARRAY_27_TARGET, GLH_TEXTURE2D_ARRAY_28_TARGET, GLH_TEXTURE2D_ARRAY_29_TARGET, GLH_TEXTURE2D_ARRAY_30_TARGET, GLH_TEXTURE2D_ARRAY_31_TARGET, 
    __TEXTURE2D_ARRAY_END__,

    __TEXTURE2D_MULTISAMPLE_START__,
    GLH_TEXTURE2D_MULTISAMPLE_0_TARGET,  GLH_TEXTURE2D_MULTISAMPLE_1_TARGET,  GLH_TEXTURE2D_MULTISAMPLE_2_TARGET,  GLH_TEXTURE2D_MULTISAMPLE_3_TARGET,  GLH_TEXTURE2D_MULTISAMPLE_4_TARGET,  GLH_TEXTURE2D_MULTISAMPLE_5_TARGET,  GLH_TEXTURE2D_MULTISAMPLE_6_TARGET,  GLH_TEXTURE2D_MULTISAMPLE_7_TARGET, 
    GLH_TEXTURE2D_MULTISAMPLE_8_TARGET,  GLH_TEXTURE2D_MULTISAMPLE_9_TARGET,  GLH_TEXTURE2D_MULTISAMPLE_10_TARGET, GLH_TEXTURE2D_MULTISAMPLE_11_TARGET, GLH_TEXTURE2D_MULTISAMPLE_12_TARGET, GLH_TEXTURE2D_MULTISAMPLE_13_TARGET, GLH_TEXTURE2D_MULTISAMPLE_14_TARGET, GLH_TEXTURE2D_MULTISAMPLE_15_TARGET, 
    GLH_TEXTURE2D_MULTISAMPLE_16_TARGET, GLH_TEXTURE2D_MULTISAMPLE_17_TARGET, GLH_TEXTURE2D_MULTISAMPLE_18_TARGET, GLH_TEXTURE2D_MULTISAMPLE_19_TARGET, GLH_TEXTURE2D_MULTISAMPLE_20_TARGET, GLH_TEXTURE2D_MULTISAMPLE_21_TARGET, GLH_TEXTURE2D_MULTISAMPLE_22_TARGET, GLH_TEXTURE2D_MULTISAMPLE_23_TARGET, 
    GLH_TEXTURE2D_MULTISAMPLE_24_TARGET, GLH_TEXTURE2D_MULTISAMPLE_25_TARGET, GLH_TEXTURE2D_MULTISAMPLE_26_TARGET, GLH_TEXTURE2D_MULTISAMPLE_27_TARGET, GLH_TEXTURE2D_MULTISAMPLE_28_TARGET, GLH_TEXTURE2D_MULTISAMPLE_29_TARGET, GLH_TEXTURE2D_MULTISAMPLE_30_TARGET, GLH_TEXTURE2D_MULTISAMPLE_31_TARGET, 
    __TEXTURE2D_MULTISAMPLE_END__,

    __CUBEMAP_START__,
    GLH_CUBEMAP_0_TARGET,  GLH_CUBEMAP_1_TARGET,  GLH_CUBEMAP_2_TARGET,  GLH_CUBEMAP_3_TARGET,  GLH_CUBEMAP_4_TARGET,  GLH_CUBEMAP_5_TARGET,  GLH_CUBEMAP_6_TARGET,  GLH_CUBEMAP_7_TARGET, 
    GLH_CUBEMAP_8_TARGET,  GLH_CUBEMAP_9_TARGET,  GLH_CUBEMAP_10_TARGET, GLH_CUBEMAP_11_TARGET, GLH_CUBEMAP_12_TARGET, GLH_CUBEMAP_13_TARGET, GLH_CUBEMAP_14_TARGET, GLH_CUBEMAP_15_TARGET, 
    GLH_CUBEMAP_16_TARGET, GLH_CUBEMAP_17_TARGET, GLH_CUBEMAP_18_TARGET, GLH_CUBEMAP_19_TARGET, GLH_CUBEMAP_20_TARGET, GLH_CUBEMAP_21_TARGET, GLH_CUBEMAP_22_TARGET, GLH_CUBEMAP_23_TARGET, 
    GLH_CUBEMAP_24_TARGET, GLH_CUBEMAP_25_TARGET, GLH_CUBEMAP_26_TARGET, GLH_CUBEMAP_27_TARGET, GLH_CUBEMAP_28_TARGET, GLH_CUBEMAP_29_TARGET, GLH_CUBEMAP_30_TARGET, GLH_CUBEMAP_31_TARGET,
    __CUBEMAP_END__,

    __CUBEMAP_ARRAY_START__,
    GLH_CUBEMAP_ARRAY_0_TARGET,  GLH_CUBEMAP_ARRAY_1_TARGET,  GLH_CUBEMAP_ARRAY_2_TARGET,  GLH_CUBEMAP_ARRAY_3_TARGET,  GLH_CUBEMAP_ARRAY_4_TARGET,  GLH_CUBEMAP_ARRAY_5_TARGET,  GLH_CUBEMAP_ARRAY_6_TARGET,  GLH_CUBEMAP_ARRAY_7_TARGET, 
    GLH_CUBEMAP_ARRAY_8_TARGET,  GLH_CUBEMAP_ARRAY_9_TARGET,  GLH_CUBEMAP_ARRAY_10_TARGET, GLH_CUBEMAP_ARRAY_11_TARGET, GLH_CUBEMAP_ARRAY_12_TARGET, GLH_CUBEMAP_ARRAY_13_TARGET, GLH_CUBEMAP_ARRAY_14_TARGET, GLH_CUBEMAP_ARRAY_15_TARGET, 
    GLH_CUBEMAP_ARRAY_16_TARGET, GLH_CUBEMAP_ARRAY_17_TARGET, GLH_CUBEMAP_ARRAY_18_TARGET, GLH_CUBEMAP_ARRAY_19_TARGET, GLH_CUBEMAP_ARRAY_20_TARGET, GLH_CUBEMAP_ARRAY_21_TARGET, GLH_CUBEMAP_ARRAY_22_TARGET, GLH_CUBEMAP_ARRAY_23_TARGET, 
    GLH_CUBEMAP_ARRAY_24_TARGET, GLH_CUBEMAP_ARRAY_25_TARGET, GLH_CUBEMAP_ARRAY_26_TARGET, GLH_CUBEMAP_ARRAY_27_TARGET, GLH_CUBEMAP_ARRAY_28_TARGET, GLH_CUBEMAP_ARRAY_29_TARGET, GLH_CUBEMAP_ARRAY_30_TARGET, GLH_CUBEMAP_ARRAY_31_TARGET,
    __CUBEMAP_ARRAY_END__,

    GLH_UNKNOWN_TARGET,

    __COUNT__
};



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
     *
     * target: the bind target to get the object from
     */
    template<class T = object> 
    static object_pointer<T> get_bound_object_pointer ( const object_bind_target target );



    /* bind/unbind to a target
     *
     * bind/unbind to a given target
     * binding/unbinding is silently ignored if object is already bound/not bound
     * 
     * returns true if a change in binding occured
     */
    bool bind ( const object_bind_target& target ) const;
    bool unbind ( const object_bind_target& target ) const;

    /* bind/unbind to default target
     *
     * bind/unbind the object from its default bind target
     * 
     * returns true if a change in binding occured
     */
    bool bind () const { return bind ( bind_target ); }
    bool unbind () const { return unbind ( bind_target ); }

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

    /* unbind_all
     *
     * the base class definition purely calls unbind
     * however, derived classes may wish to override this method, such that all units are unbound
     * 
     * returns true if a change in binding occured
     */
    bool virtual unbind_all () const { return unbind (); }

    /* is_bound to a target
     *
     * returns true if the object is bound to the target supplied
     */
    bool is_bound ( const object_bind_target& target ) const;

    /* is bound to default target
     *
     * returns true if the object is bound to the default target
     */
    bool is_bound () const { return is_bound ( bind_target ); }



    /* force_unbind
     *
     * force the unbinding of any object bound to a bind point
     *
     * returns true if an object was unbound
     */
    static bool force_unbind ( const object_bind_target& target );



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



    /* internal_id
     *
     * returns the internal id of the object
     */
    const GLuint& internal_id () const { return id; }

    /* internal_unique_id
     *
     * returns the unique id of the object
     */
    const GLuint& internal_unique_id () const { return id; }

    /* get_minor/major_object_type
     * get_object_bind_target
     * get_opengl_bind_taregt
     *
     * get the different types/targets associated with the object
     */
    const minor_object_type& get_minor_object_type () const { return minor_type; }
    const major_object_type& get_major_object_type () const { return major_type; }
    const object_bind_target& get_object_bind_target () const { return bind_target; }
    const GLenum& get_opengl_bind_taregt () const { return opengl_bind_target; }



protected:

    /* NON-STATIC MEMBERS */

    /* id
     *
     * the OpenGL id of the object
     */
    GLuint id;

    /* unique_id
     *
     * the unique id of the object
     * will be different for EVERY object that ever exists
     * will not be changed for move-constructed objects, however
     */
    const GLuint unique_id;

    /* minor_type, major_type, bind_target
     *
     * the minor and major object types, as well as the object bind target
     */
    const minor_object_type minor_type;
    const major_object_type major_type;
    const object_bind_target bind_target;

    /* opengl_bind_target
     *
     * the opengl enum used for binding
     */
    const GLenum opengl_bind_target;



    /* STATIC MEMBERS */

    /* next_unique_id
     *
     * the next unique id to be used
     * incremented for each new object
     */
    static GLuint next_unique_id;

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
     * is_texture2d_array_bind_target
     * is_texture2d_multisample_bind_target
     * is_cubemap_bind_target
     * is_cubemap_array_bind_target
     * 
     * returns true if the target supplied is one of the above bind targets
     */
    static bool is_texture2d_bind_target ( const object_bind_target target );
    static bool is_texture2d_array_bind_target ( const object_bind_target target );
    static bool is_texture2d_multisample_bind_target ( const object_bind_target target );
    static bool is_cubemap_bind_target ( const object_bind_target target );
    static bool is_cubemap_array_bind_target ( const object_bind_target target );

};




/* OBJECT_REFERENCE DEFINITION */

/* class object_pointer
 *
 * safely references an object 
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
        , minor_type { obj.get_minor_object_type () }
        , major_type { obj.get_major_object_type () }
        , bind_target { obj.get_object_bind_target () }
    {}

    /* pointer constructor */
    object_pointer ( T * obj )
        : id ( obj ? obj->internal_id () : 0 )
        , unique_id { obj ? obj->internal_unique_id () : 0 }
        , minor_type { obj ? obj->get_minor_object_type () : minor_object_type::GLH_UNKNOWN_TYPE }
        , major_type { obj ? obj->get_major_object_type () : major_object_type::GLH_UNKNOWN_TYPE }
        , bind_target { obj ? obj->get_object_bind_target () : object_bind_target::GLH_UNKNOWN_TARGET }
    {}

    /* zero-parameter constructor */
    object_pointer ()
        : id { 0 }
        , unique_id { 0 }
        , minor_type { minor_object_type::GLH_UNKNOWN_TYPE }
        , major_type { major_object_type::GLH_UNKNOWN_TYPE }
        , bind_target { object_bind_target::GLH_UNKNOWN_TARGET } 
    {}

    /* default copy constructor */
    object_pointer ( const object_pointer& other ) = default;

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



    /* is_valid
     *
     * returns true if the pointer is valid
     */
    bool is_pointer_valid () const;

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



private:

    /* id and unique id */
    GLuint id;
    GLuint unique_id;

    /* minor and major type */
    minor_object_type minor_type;
    major_object_type major_type;

    /* target */
    object_bind_target bind_target;

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




/* OBJECT TEMPLATE METHODS IMPLEMENTATIONS */

/* get_bound_object_pointer
 *
 * produce a pointer to the object bound to a given bind point
 *
 * target: the bind target to get the object from
 */
template<class T> inline glh::core::object_pointer<T> glh::core::object::get_bound_object_pointer ( const object_bind_target target )
{
    /* return object pointer */
    return object_pointer<T> 
    { dynamic_cast<T *> ( object_pointers.at ( static_cast<unsigned> ( to_major_object_type ( target ) ) ).at ( object_bindings.at ( static_cast<unsigned> ( target ) ) ) ) };
}




/* OBJECT_POINTER IMPLEMENTATION */

/* get
 *
 * get pointer to object
 * returns NULL if invalid
 */
template<class T> inline T * glh::core::object_pointer<T>::get () const
{
    /* if id == 0, throw */
    if ( id == 0 ) throw exception::object_exception { "attempted to dereference invalidated object pointer" };

    /* get pointer to object
     * we know it will have an entry in the object pointers vector, as its id has previously been created
     */
    T * ptr = dynamic_cast<T *> ( object::object_pointers.at ( static_cast<unsigned> ( major_type ) ).at ( id ) );

    /* throw if pointer is invalid or to wrong object */
    if ( !ptr || ptr->internal_unique_id () != unique_id ) throw exception::object_exception { "attempted to dereference invalidated object pointer" };

    /* return ptr */
    return ptr;
}

/* is_valid
 *
 * returns true if the pointer is valid
 */
template<class T> inline bool glh::core::object_pointer<T>::is_pointer_valid () const
{
    /* if no exception caught, return true, else false */
    try
    {
        return get ();
    } catch ( std::exception& e )
    {
        return false;
    }
}




/* #ifndef GLHELPER_CORE_HPP_INCLUDED */
#endif