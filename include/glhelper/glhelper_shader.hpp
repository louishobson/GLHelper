/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * include/glhelper/glhelper_shader.hpp
 * 
 * constructs for managing and using shaders
 * notable constructs include:
 * 
 * 
 * 
 * CLASS GLH::CORE::SHADER
 * 
 * base class for any type of shader
 * the type of shader and source code path is passed in the constructor and the shader is compiled there and then
 * if any error orrcurs, a shader_exception will be thrown with a description of the error
 * this shader can then be used within a shader program
 * 
 * 
 * 
 * CLASS GLH::CORE::V/G/FSHADER
 * 
 * derivations of the shader base class to set defaults for the shader type
 * no more complicated than that
 * 
 * 
 * 
 * CLASS GLH::CORE::UNIFORM
 * 
 * class for an endpoint uniform
 * a value is given to the uniform via the set_... methods
 * if the uniform is in the default block, glUniform... functions will be used to set the value(s)
 * if the unfirom is not in the default block, the ubo responsible for its data storage is located, and modified accordingly
 * 
 * 
 * 
 * CLASS GLH::CORE::STRUCT_UNIFORM
 * 
 * class for structure uniforms
 * further members the struct extracted via the get_..._uniform methods
 * 
 * 
 * 
 * CLASS GLH::CORE::ARRAY_UNIFORM
 * 
 * a template class to represent an array of uniforms
 * the template parameter must be one of the uniform, struct_uniform or array_uniform classes
 * this will be the type of uniform the array contains
 * the at method will then extract one of these uniforms at a specific index
 * 
 * 
 * 
 * CLASS GLH::CORE::PROGRAM
 * 
 * class for a shader program
 * vertex and fragment shaders are mandatory for consruction, however a geometry shader can be used as well
 * uniforms in the program are extracted using the member functions get_..._uniform
 * the program class remembers loactions of uniforms, although it would still be better to not keep running get_..._uniform
 * hence many constructs throught GLHelper use uniform caching to avoid this
 * 
 * 
 * 
 * CLASS GLH::EXCEPTION::SHADER_EXCEPTION
 * 
 * thrown when an error occurs in one of the shader/program methods (e.g. if shader compilation fails)
 * 
 * 
 * 
 * CLASS GLH::EXCEPTION::UNIFORM_EXCEPTION
 * 
 * thrown when an error occurs related to uniforms (e.g. if a uniform with a given name cannot be found)
 * this is exception is thrown by many different classes throughout GLGelper
 * 
 */



/* HEADER GUARD */
#ifndef GLHELPER_SHADER_HPP_INCLUDED
#define GLHELPER_SHADER_HPP_INCLUDED



/* INCLUDES */

/* include core headers */
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <type_traits>

/* include glhelper_core.hpp */
#include <glhelper/glhelper_core.hpp>

/* include glhelper_exception.hpp */
#include <glhelper/glhelper_exception.hpp>

/* glhelper_manager.hpp */
#include <glhelper/glhelper_manager.hpp>

/* include glhelper_math.hpp */
#include <glhelper/glhelper_math.hpp>

/* include glhelper_buffer.hpp */
#include <glhelper/glhelper_buffer.hpp>



/* MACROS */

/* GLH_SHADER_LOG_SIZE
 *
 * integer defining the maximum log size for shader compilation or linking errors
 */
#define GLH_SHADER_LOG_SIZE 512

/* glh_if_constexpr
 *
 * either "if" or "if constexpr" depending on availibility
 */
#ifdef __cpp_if_constexpr
    #define glh_if_constexpr if constexpr
#else
    #define glh_if_constexpr if
#endif



/* NAMESPACE FORWARD DECLARATIONS */

namespace glh
{
    namespace core
    {
        /* class shader : object
         *
         * base class for any type of shader
         */
        class shader;

        /* class v/g/fshader : shader
         *
         * derived classes for specific shader types
         */
        class vshader;
        class gshader;
        class fshader;



        /* class uniform_storage : uniform_storage
         *
         * stores hard copies of derived uniform types
         */
        template<class T> class uniform_storage;

        /* class uniform
         *
         * class for an endpoint uniform
         */
        class uniform;

        /* class array_uniform
         *
         * a template class to represent an array of uniforms
         */
        template<class T> class array_uniform;

        /* class struct_uniform
         *
         * class for structure uniforms
         */
        class struct_uniform;

        /* using declaration for array uniform types
         *
         * simplify common types of array uniforms a bit
         */
        using uniform_array_uniform = array_uniform<uniform>;
        using struct_array_uniform = array_uniform<struct_uniform>;
        using uniform_2d_array_uniform = array_uniform<array_uniform<uniform>>;
        using struct_2d_array_uniform = array_uniform<array_uniform<struct_uniform>>;



        /* class program : object
         *
         * shader program
         */
        class program;

    }

    namespace meta
    {
        /* struct is_uniform
         *
         * is_uniform::value is true if the type is a uniform
         */
        template<class T> struct is_uniform;

        /* struct is_pack_same
         *
         * is_pack_same::value is true if the parameter pack supplied are all of type T
         */
        template<class T, class T0, class ...Ts> struct is_pack_same;
    }

    namespace exception
    {
        /* class shader_exception : exception
         *
         * for exceptions related to shaders
         */
        class shader_exception;

        /* class uniform_exception : exception
         *
         * for exceptions related to uniforms
         */
        class uniform_exception;
    }    
}

/* comparison operators for uniform types
 *
 * returns true if the objects refer to the same uniform
 */
bool operator== ( const glh::core::uniform& lhs, const glh::core::uniform& rhs );
bool operator!= ( const glh::core::uniform& lhs, const glh::core::uniform& rhs );
bool operator== ( const glh::core::struct_uniform& lhs, const glh::core::struct_uniform& rhs );
bool operator!= ( const glh::core::struct_uniform& lhs, const glh::core::struct_uniform& rhs );
template<class T> bool operator== ( const glh::core::array_uniform<T>& lhs, const glh::core::array_uniform<T>& rhs );
template<class T> bool operator!= ( const glh::core::array_uniform<T>& lhs, const glh::core::array_uniform<T>& rhs );



/* SHADER DEFINITION */

/* class shader : object
 *
 * base class for any type of shader
 */
class glh::core::shader : public object
{
public:

    /* constructor */
    shader ( const minor_object_type _type, const std::string& _path );

    /* deleted zero-parameter constructor */
    shader () = delete;

    /* deleted copy constructor */
    shader ( const shader& other ) = delete;

    /* default move constructor */
    shader ( shader&& other ) = default;

    /* deleted copy asignment operator */
    shader& operator= ( const shader& other ) = delete;

    /* destructor */
    virtual ~shader () { destroy (); }



    /* get_path
     *
     * get the path of the shader
     */
    const std::string& get_path () const { return path; }

    /* get_source
     *
     * get the source of the shader
     */
    const std::string& get_source () const { return source; }



private:

    /* path to the shader */
    const std::string path;

    /* shader source */
    std::string source;

};



/* VSHADER DEFINITION */

/* class vshader
 *
 * derived class for a vertex shader
 */
class glh::core::vshader : public shader
{
public:

    /* constructor */
    explicit vshader ( const std::string& _path )
        : shader { minor_object_type::GLH_VSHADER_TYPE, _path }
    {}

    /* deleted zero-parameter constructor */
    vshader () = delete;

    /* deleted copy constructor */
    vshader ( const vshader& other ) = delete;

    /* default move constructor */
    vshader ( vshader&& other ) = default;

    /* deleted copy asignment operator */
    vshader& operator= ( const vshader& other ) = delete;

    /* default destructor */
    ~vshader () = default;

};



/* GSHADER DEFINITION */

/* class gshader
 *
 * derived class for a geometry shader
 */
class glh::core::gshader : public shader
{
public:

    /* constructor */
    explicit gshader ( const std::string& _path )
        : shader { minor_object_type::GLH_GSHADER_TYPE, _path }
    {}

    /* deleted zero-parameter constructor */
    gshader () = delete;

    /* deleted copy constructor */
    gshader ( const gshader& other ) = delete;

    /* default move constructor */
    gshader ( gshader&& other ) = default;

    /* deleted copy asignment operator */
    gshader& operator= ( const gshader& other ) = delete;

    /* default destructor */
    ~gshader () = default;

};



/* FSHADER DEFINITION */

/* class fshader
 *
 * derived class for a fragment shader
 */
class glh::core::fshader : public shader
{
public:

    /* constructor */
    explicit fshader ( const std::string& _path )
        : shader { minor_object_type::GLH_FSHADER_TYPE, _path }
    {}

    /* deleted zero-parameter constructor */
    fshader () = delete;

    /* deleted copy constructor */
    fshader ( const fshader& other ) = delete;

    /* default move constructor */
    fshader ( fshader&& other ) = default;

    /* deleted copy asignment operator */
    fshader& operator= ( const fshader& other ) = delete;

    /* default destructor */
    ~fshader () = default;

};



/* IS_UNIFORM DEFINITION */

/* struct is_uniform
 *
 * is_uniform::value is true if the type is a uniform
 */
template<class T> struct glh::meta::is_uniform : std::false_type {};
template<> struct glh::meta::is_uniform<glh::core::uniform> : std::true_type {};
template<> struct glh::meta::is_uniform<glh::core::struct_uniform> : std::true_type {};
template<> template<class _T> struct glh::meta::is_uniform<glh::core::array_uniform<_T>> : is_uniform<_T> {};



/* IS_PACK_SAME DEFINITION */

/* struct is_pack_same
 *
 * is_pack_same::value is true if the parameter pack supplied are all of type T
 */
template<class T, class T0, class ...Ts> struct glh::meta::is_pack_same 
    : std::integral_constant<bool, std::is_same<T, T0>::value && glh::meta::is_pack_same<T, Ts...>::value> {};




/* UNIFORM_STORAGE DEFINITION */

/* class uniform_storage : uniform_storage
 *
 * stores hard copies of uniform types
 */
template<class T> class glh::core::uniform_storage
{

    /* static_assert that T is a uniform type */
    static_assert ( meta::is_uniform<T>::value, "class uniform_storage must have a template parameter of a uniform type" );

public:

    /* full constructor 
     *
     * _prefix: the prefix for any member uniforms
     * _prog: the program the uniform storage is associated with
     */
    uniform_storage ( const std::string& _prefix, program& _prog )
        : prefix { _prefix }
        , prog { _prog }
    {}

    /* deleted zero-parameter constructor */
    uniform_storage () = delete;

    /* default copy constructor */
    uniform_storage ( const uniform_storage& other ) = default;

    /* deleted copy assignment operator */
    uniform_storage& operator= ( const uniform_storage& other ) = delete;

    /* default destructor */
    ~uniform_storage () = default;



    /* get
     *
     * get a member out of the uniform storage
     * if it does not exist, it will be created
     * 
     * postfix: the postfix for the member uniform
     */
    T& get ( const std::string& postfix ) { return __get ( postfix ); }
    const T& get ( const std::string& postfix ) const { return __get ( postfix ); }



    /* get_prefix
     *
     * get the prefix of the uniform storage
     */
    const std::string& get_prefix () const { return prefix; }

    /* get_program
     *
     * return the program associated with the uniform
     */
    program& get_program () { return prog; }
    const program& get_program () const { return prog; }



private:

    /* the prefix for any member uniforms */
    const std::string prefix;

    /* the program the uniform storage is associated with */
    program& prog;

    /* map of uniform names to their instances
     *
     * the key here is appended to the prefix member
     * so if prefix = "test_uniform." then a key could be "test" to make "test_uniform.test"
     */
    mutable std::map<std::string, T> uniforms;

    /* __get
     *
     * add a uniform to the map if not already present, then return a reference
     *
     * postfix: the postfix for the member uniform
     */
    T& __get ( const std::string& postfix ) const;

};



/* UNIFORM DEFINITION */

/* class uniform
 *
 * class for an endpoint uniform
 */
class glh::core::uniform
{
public:

    /* full constructor
     *
     * _name: the name of the uniform
     * _prog: the program the uniform is associated with
     */
    uniform ( const std::string& _name, program& _prog );

    /* deleted zero-parameter constructor */
    uniform () = delete;

    /* default copy constructor */
    uniform ( const uniform& other ) = default;

    /* deleted copy assignment operator */
    uniform& operator= ( const uniform& other ) = delete;

    /* default destructor */
    ~uniform () = default;



    /* set_...
     *
     * set the uniform to varying values
     */
    void set_int ( const GLint v0 ) const;
    void set_int ( const GLint v0, const GLint v1 ) const;
    void set_int ( const GLint v0, const GLint v1, const GLint v2 ) const;
    void set_int ( const GLint v0, const GLint v1, const GLint v2, const GLint v3 ) const;
    void set_uint ( const GLuint v0 ) const;
    void set_uint ( const GLuint v0, const GLuint v1 ) const;
    void set_uint ( const GLuint v0, const GLuint v1, const GLuint v2 ) const;
    void set_uint ( const GLuint v0, const GLuint v1, const GLuint v2, const GLuint v3 ) const;
    void set_float ( const GLfloat v0 ) const;
    void set_float ( const GLfloat v0, const GLfloat v1 ) const;
    void set_float ( const GLfloat v0, const GLfloat v1, const GLfloat v2 ) const;
    void set_float ( const GLfloat v0, const GLfloat v1, const GLfloat v2, const GLfloat v3 ) const;
    void set_vector ( const math::vec2& v0 ) const;
    void set_vector ( const math::vec3& v0 ) const;
    void set_vector ( const math::vec4& v0 ) const;
    void set_matrix ( const math::matrix<2, 2>& v0 ) const;
    void set_matrix ( const math::matrix<2, 3>& v0 ) const;
    void set_matrix ( const math::matrix<2, 4>& v0 ) const;
    void set_matrix ( const math::matrix<3, 2>& v0 ) const;
    void set_matrix ( const math::matrix<3, 3>& v0 ) const;
    void set_matrix ( const math::matrix<3, 4>& v0 ) const;
    void set_matrix ( const math::matrix<4, 2>& v0 ) const;
    void set_matrix ( const math::matrix<4, 3>& v0 ) const;
    void set_matrix ( const math::matrix<4, 4>& v0 ) const;


    /* use_program
     *
     * use the associated program
     */
    void use_program () const;

    /* is_program_in_use
     *
     * return a boolean for if the associated program is in use
     */
    bool is_program_in_use () const;

    /* assert_is_program_in_use
     *
     * will throw if the associated program is not in use
     * 
     * operation: description of the operation being performed
     */
    void assert_is_program_in_use ( const std::string& operation = "" ) const;



    /* is_set_valid
     *
     * returnd true if it is safe to set values
     */
    bool is_set_valid () const;

    /* assert_is_set_valid
     *
     * throws if the uniform is not set valid
     * 
     * operation: description of the operation
     */
    void assert_is_set_valid ( const std::string& operation = "" ) const;



    /* get_name
     *
     * return the name of the uniform
     */
    const std::string& get_name () const { return name; }

    /* get_program
     *
     * return the program associated with the uniform
     */
    program& get_program () { return prog; }
    const program& get_program () const { return prog; }

    /* get_location
     * 
     * gets the location of the uniform
     */
    const GLint& get_location () const { return location; }

    /* get_index
     *
     * gets the index of the uniform
     */
    const GLuint& get_index () const { return index; }

    /* get_block_index
     *
     * get the index of the block the uniform is part of
     */
    const GLint& get_block_index () const { return block_index; }



protected:

    /* store the name of the uniform */
    const std::string name;

    /* the program the uniform is associated with */
    program& prog;

    /* the location and index of the uniform */
    const GLint location;
    const GLuint index;

    /* size and offset of the uniform */
    const GLint size;
    const GLint offset;

    /* the index of the uniform block associated with this uniform */
    const GLint block_index;

    /* the type of the uniform in the shader */
    const GLint uniform_type;



    /* default_set
     *
     * set a uniform given a functor to a glUniform function and its parameters, excluding the location parameter
     */
    template<class F, class ...Ts> void default_set ( const F func, const Ts&... params ) const;

    /* ubo_set
     *
     * set a uniform to the ubo bound to its block index
     * the template parameter T is the type that will be written to the uniform
     * all of Ts... must be convertible to type T
     */
    template<class T, class ...Ts> void ubo_set ( const Ts&... vs ) const;

    /* ubo_set_array
     *
     * set a uniform to the ubo bound to its block index
     * takes a pointer to data rather than individual values
     */
    template<class T> void ubo_set_array ( const T * vs, const unsigned size ) const;

    /* __ubo_set
     *
     * for internal use by ubo_set
     */
    template<class T,  class ...Ts> void __ubo_set ( buffer_map<T>& map, const unsigned index, const T& v0, const Ts&... vs ) const;

};



/* ARRAY_UNIFORM DEFINITION */

/* class array_uniform
 *
 * a template class to represent an array of uniforms
 */
template<class T> class glh::core::array_uniform
{

    /* static assert that T is a uniform */
    static_assert ( meta::is_uniform<T>::value, "class array_uniform must have a template paramater of a uniform type" );

public:

    /* full constructor
     *
     * _name: the name of the uniform
     * _prog: the program ths uniform is associated with 
     */
    array_uniform ( const std::string& _name, program& _prog )
        : name { _name }
        , prog { _prog }
        , uniforms { _name, _prog }
    {}

    /* deleted zero-parameter constructor */
    array_uniform () = delete;

    /* default copy constructor */
    array_uniform ( const array_uniform& other ) = default;

    /* deleted copy assignment operator */
    array_uniform& operator= ( const array_uniform& other ) = delete;

    /* default destructor */
    ~array_uniform () = default;



    /* typedef of T */
    typedef T type;



    /* pure at
     *
     * return the uniform at an index
     */
    T& at ( const unsigned i ) { return uniforms.get ( "[" + std::to_string ( i ) + "]" ); }
    const T& at ( const unsigned i ) const { return uniforms.get ( "[" + std::to_string ( i ) + "]" ); }

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

    /* get_program
     *
     * return the program associated with the uniform
     */
    program& get_program () { return prog; }
    const program& get_program () const { return prog; }



protected:

    /* store the name of the uniform */
    const std::string name;

    /* the program the uniform is associated with */
    program& prog;

    /* storage of uniforms in the array */
    uniform_storage<T> uniforms;

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
     * _prog: the program ths uniform is associated with
     */
    struct_uniform ( const std::string& _name, program& _prog )
        : name { _name }
        , prog { _prog }
        , uniforms { _name + ".", _prog }, struct_uniforms { _name + ".", _prog }
        , uniform_array_uniforms { _name + ".", _prog }, struct_array_uniforms { _name + ".", _prog }
        , uniform_2d_array_uniforms { _name + ".", _prog }, struct_2d_array_uniforms { _name + ".", _prog }
    {}

    /* deleted zero-parameter constructor */
    struct_uniform () = delete;

    /* default copy constructor */
    struct_uniform ( const struct_uniform& other ) = default;

    /* deleted copy assignment operator */
    struct_uniform& operator= ( const struct_uniform& other ) = delete;

    /* default destructor */
    ~struct_uniform () = default;



    /* pure get_(struct_)uniform
     *
     * get a member of the struct
     */
    uniform& get_uniform ( const std::string& member ) { return uniforms.get ( member ); }
    const uniform& get_uniform ( const std::string& member ) const { return uniforms.get ( member ); }
    struct_uniform& get_struct_uniform ( const std::string& member ) { return struct_uniforms.get ( member ); }
    const struct_uniform& get_struct_uniform ( const std::string& member ) const { return struct_uniforms.get ( member ); }
    
    /* pure get_..._array_uniform
     *
     * get array uniforms out of the struct
     */
    uniform_array_uniform& get_uniform_array_uniform ( const std::string& member ) { return uniform_array_uniforms.get ( member ); }
    const uniform_array_uniform& get_uniform_array_uniform ( const std::string& member ) const { return uniform_array_uniforms.get ( member ); }
    struct_array_uniform& get_struct_array_uniform ( const std::string& member ) { return struct_array_uniforms.get ( member ); }
    const struct_array_uniform& get_struct_array_uniform ( const std::string& member ) const { return struct_array_uniforms.get ( member ); }
    uniform_2d_array_uniform& get_uniform_2d_array_uniform ( const std::string& member ) { return uniform_2d_array_uniforms.get ( member ); }
    const uniform_2d_array_uniform& get_uniform_2d_array_uniform ( const std::string& member ) const { return uniform_2d_array_uniforms.get ( member ); }
    struct_2d_array_uniform& get_struct_2d_array_uniform ( const std::string& member ) { return struct_2d_array_uniforms.get ( member ); }
    const struct_2d_array_uniform& get_struct_2d_array_uniform ( const std::string& member ) const { return struct_2d_array_uniforms.get ( member ); }



    /* get_name
     *
     * return the name of the uniform
     */
    const std::string& get_name () const { return name; }

    /* get_program
     *
     * return the program associated with the uniform
     */
    program& get_program () { return prog; }
    const program& get_program () const { return prog; }



protected:

    /* store the name of the uniform */
    const std::string name;

    /* the program the uniform is associated with */
    program& prog;

    /* (..._)uniforms
     *
     * storage of uniforms in the struct
     */
    uniform_storage<uniform> uniforms;
    uniform_storage<struct_uniform> struct_uniforms;
    uniform_storage<uniform_array_uniform> uniform_array_uniforms;
    uniform_storage<struct_array_uniform> struct_array_uniforms;
    uniform_storage<uniform_2d_array_uniform> uniform_2d_array_uniforms;
    uniform_storage<struct_2d_array_uniform> struct_2d_array_uniforms;

};



/* PROGRAM DEFINITION */

/* class program : object
 *
 * shader program
 */
class glh::core::program : public object
{
public:

    /* three-shader constructor
     *
     * link all three shaders into a program
     * NOTE: the shader program remains valid even when linked shaders are destroyed
     */
    program ( const vshader& vs, const gshader& gs, const fshader& fs );

    /* two-shader constructor
     *
     * link vertex and fragment shaders into a program
     * uses the default geometry shader
     * NOTE: the shader program remains valid even when linked shaders are destroyed
     */
    program ( const vshader& vs, const fshader& fs );

    /* deleted zero-parameter constructor */
    program () = delete;

    /* deleted copy constructor */
    program ( const program& other ) = delete;

    /* default move constructor */
    program ( program&& other ) = default;

    /* deleted copy assignment operator */
    program& operator= ( const program& other ) = delete;

    /* destructor */
    ~program () { destroy (); }



    /* get_(struct_)uniform
     *
     * return a uniform object based on a name
     * 
     * name: the name of the uniform
     */
    uniform& get_uniform ( const std::string& name ) { return uniforms.get ( name ); }
    const uniform& get_uniform ( const std::string& name ) const { return uniforms.get ( name ); }
    struct_uniform& get_struct_uniform ( const std::string& name ) { return struct_uniforms.get ( name ); }
    const struct_uniform& get_struct_uniform ( const std::string& name ) const { return struct_uniforms.get ( name ); }
    
    /* get_..._array_uniform
     *
     * return a uniform array object based on a name
     * 
     * name: the name of the uniform
     */
    uniform_array_uniform& get_uniform_array_uniform ( const std::string& name ) { return uniform_array_uniforms.get ( name ); }
    const uniform_array_uniform& get_uniform_array_uniform ( const std::string& name ) const { return uniform_array_uniforms.get ( name ); }
    struct_array_uniform& get_struct_array_uniform ( const std::string& name ) { return struct_array_uniforms.get ( name ); }
    const struct_array_uniform& get_struct_array_uniform ( const std::string& name ) const { return struct_array_uniforms.get ( name ); }
    uniform_2d_array_uniform& get_uniform_2d_array_uniform ( const std::string& name ) { return uniform_2d_array_uniforms.get ( name ); }
    const uniform_2d_array_uniform& get_uniform_2d_array_uniform ( const std::string& name ) const { return uniform_2d_array_uniforms.get ( name ); }
    struct_2d_array_uniform& get_struct_2d_array_uniform ( const std::string& name ) { return struct_2d_array_uniforms.get ( name ); }
    const struct_2d_array_uniform& get_struct_2d_array_uniform ( const std::string& name ) const { return struct_2d_array_uniforms.get ( name ); }




    /* get_uniform_location
     *
     * get the location of a uniform
     * 
     * name: the name of the uniform
     */
    GLint get_uniform_location ( const std::string& name ) const;

    /* get_uniform_index
     *
     * get the index of a uniform
     * 
     * name: the name of the uniform
     */
    GLuint get_uniform_index ( const std::string& name ) const;

    /* get_uniform_block_index
     *
     * get the index of a uniform block from the program
     * 
     * name: the name of the uniform block
     */
    GLuint get_uniform_block_index ( const std::string& name ) const;



    /* get_active_uniform_iv
     *
     * get integer information about a uniform
     * 
     * index/name: the index/name of the uniform
     * target: the target piece of information
     */
    GLint get_active_uniform_iv ( const GLuint index, const GLenum target ) const;
    GLint get_active_uniform_iv ( const std::string& name, const GLenum target ) const;

    /* get_active_uniform_block_iv
     *
     * get integer information about a uniform block
     * 
     * index/name: the index/name of the uniform block
     * target: the target piece of information
     */
    GLint get_active_uniform_block_iv ( const GLuint index, const GLenum target ) const;
    GLint get_active_uniform_block_iv ( const std::string& name, const GLenum target ) const;



    /* use
     *
     * use the shader program for the following OpenGL function calls
     * will not call glUseProgram if already in use
     * same as bind method
     */
    void use () const { bind (); }

    /* is_in_use
     *
     * same as is_bound method
     * 
     * return: boolean for if the program is in use
     */
    bool is_in_use () const { return is_bound (); }



private:

    /* uniform_locations/indices
     * uniform_block_indices
     *
     * maps to the locations/indices of uniforms and uniform blocks based on their full names
     * saves some time on repeated queries
     */
    mutable std::map<std::string, GLint> uniform_locations;
    mutable std::map<std::string, GLuint> uniform_indices;
    mutable std::map<std::string, GLuint> uniform_block_indices;

    /* pure_(..._)uniforms
     *
     * storage of uniforms in the program
     */
    uniform_storage<uniform> uniforms;
    uniform_storage<struct_uniform> struct_uniforms;
    uniform_storage<uniform_array_uniform> uniform_array_uniforms;
    uniform_storage<struct_array_uniform> struct_array_uniforms;
    uniform_storage<uniform_2d_array_uniform> uniform_2d_array_uniforms;
    uniform_storage<struct_2d_array_uniform> struct_2d_array_uniforms;

};



/* SHADER_EXCEPTION DEFINITION */

/* class shader_exception : exception
 *
 * for exceptions related to shaders
 */
class glh::exception::shader_exception : public exception
{
public:

    /* full constructor
     *
     * __what: description of the exception
     */
    explicit shader_exception ( const std::string& __what )
        : exception { __what }
    {}

    /* default zero-parameter constructor
     *
     * construct shader_exception with no descrption
     */
    shader_exception () = default;

    /* default everything else and inherits what () function */

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



/* ARRAY UNIFORM COMPARISON OPERATORS IMPLEMENTATION */

/* comparison operators for array uniform types
 *
 * returns true if the objects refer to the same uniform
 */
template<class T> inline bool operator== ( const glh::core::array_uniform<T>& lhs, const glh::core::array_uniform<T>& rhs )
{
    return ( lhs.get_program () == rhs.get_program () && lhs.get_name () == rhs.get_name () );
}
template<class T> inline bool operator!= ( const glh::core::array_uniform<T>& lhs, const glh::core::array_uniform<T>& rhs )
{
    return !( lhs == rhs );
}



/* UNIFORM_STORAGE IMPLEMENTATION */

/* __get
 *
 * add a uniform to the map if not already present, then return a reference
 *
 * postfix: the postfix for the member uniform
 */
template<class T> inline T& glh::core::uniform_storage<T>::__get ( const std::string& postfix ) const
{
    /* try to find the uniform from the map */
    auto it = uniforms.find ( postfix );
    if ( it != uniforms.end () ) return it->second;

    /* otherwise add uniform to map */
    else
    {
        uniforms.insert ( { postfix, T { this->prefix + postfix, prog } } );
        return uniforms.at ( postfix );
    }
}



/* UNIFORM IMPLEMENTATION */

/* default_set
 *
 * set a uniform given a functor to a glUniform function and its parameters, excluding the location parameter
 */
template<class F, class ...Ts> inline void glh::core::uniform::default_set ( const F func, const Ts&... params ) const
{
    /* assert program is in use */
    assert_is_program_in_use ( "default set uniform" );

    /* invoke the setting function */
    func ( location, params... );
}

/* ubo_set
 *
 * set a uniform to the ubo bound to its block index
 * the template parameter T is the type that will be written to the uniform
 * all of Ts... must be convertible to type T
 */
template<class T, class ...Ts> inline void glh::core::uniform::ubo_set ( const Ts&... vs ) const
{

    /* if in the default block, throw */
    if ( block_index < 0 ) throw exception::uniform_exception { "attempted to perform block set uniform operation on a uniform named " + name + " in the default block" };
}

/* ubo_set_array
 *
 * set a uniform to the ubo bound to its block index
 * takes a pointer to data rather than individual values
 */
template<class T> inline void glh::core::uniform::ubo_set_array ( const T * vs, const unsigned size ) const
{
    /* if in the default block, throw */
    if ( block_index < 0 ) throw exception::uniform_exception { "attempted to perform block set uniform operation on a uniform named " + name + " in the default block" };
}

/* __ubo_set
 *
 * for internal use by ubo_set
 */
template<class T,  class ...Ts> inline void glh::core::uniform::__ubo_set ( buffer_map<T>& map, const unsigned index, const T& v0, const Ts&... vs ) const
{

}



/* #ifndef GLHELPER_SHADER_HPP_INCLUDED */
#endif