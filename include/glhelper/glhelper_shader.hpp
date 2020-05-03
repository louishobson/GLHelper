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
 * CLASS GLH::CORE::PURE_UNIFORM
 * 
 * derivation of uniform class to represent a normal uniform in a shader
 * this uniform directly references a variable in the shader which can be written to using the set_... methods
 * NOTE: the parent program must be in use to set the uniform
 * 
 * 
 * 
 * CLASS GLH::CORE::PURE_ARRAY_UNIFORM
 * 
 * derivation of array_uniform to represent a normal array uniform in a shader
 * the at method allows for elements of the array to be accessed
 * the type of uniform returned is based on the template parameter of the pure_array_uniform class
 * 
 * 
 * 
 * CLASS GLH::CORE::PURE_STRUCT_UNIFORM
 * 
 * derivation of struct_uniform to represent a normal struct uniform in a shader
 * members can be accessed using the get_..._uniform methods, with a member name
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

/* include glhelper_uniform.hpp */
#include <glhelper/glhelper_uniform.hpp>

/* include glhelper_math.hpp */
#include <glhelper/glhelper_math.hpp>



/* MACROS */

/* GLH_SHADER_LOG_SIZE
 *
 * integer defining the maximum log size for shader compilation or linking errors
 */
#define GLH_SHADER_LOG_SIZE 512



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

        /* class pure_uniform_storage : uniform_storage
         *
         * stores hard copies of pure uniforms so that pure uniform types can return references to these objects
         */
        template<class T> class pure_uniform_storage;

        /* class pure_uniform : uniform
         *
         * derivation of uniform class for normal uniforms in shaders
         */
        class pure_uniform;

        /* class pure_array_uniform : array_uniform
         *
         * derivation of array_uniform class for normal array uniforms in shaders
         * declared before pure_struct_uniform because pure_struct_uniform depends on this class
         */
        template<class T> class pure_array_uniform;

        /* class pure_struct_uniform : struct_uniform
         *
         * derivation of struct_uniform class for normal struct uniforms in shaders
         */
        class pure_struct_uniform;

        /* using declaration for pure array uniform types
         *
         * simplify common types of array uniforms a bit
         */
        using pure_uniform_array_uniform = pure_array_uniform<pure_uniform>;
        using pure_struct_array_uniform = pure_array_uniform<pure_struct_uniform>;
        using pure_uniform_2d_array_uniform = pure_array_uniform<pure_array_uniform<pure_uniform>>;
        using pure_struct_2d_array_uniform = pure_array_uniform<pure_array_uniform<pure_struct_uniform>>;

        /* class program : object
         *
         * shader program
         */
        class program;

    }

    namespace meta
    {
        /* struct is_uniform_pure
         *
         * is_uniform_pure::value is true if the type is a pure uniform
         */
        template<class T> struct is_uniform_pure;
    }

    namespace exception
    {
        /* class shader_exception : exception
         *
         * for exceptions related to shaders
         */
        class shader_exception;
    }    
}



/* SHADER DEFINITION */

/* class shader : object
 *
 * base class for any type of shader
 */
class glh::core::shader : public object
{
public:

    /* constructor */
    shader ( const GLenum _target, const std::string& _path );

    /* deleted zero-parameter constructor */
    shader () = delete;

    /* deleted copy constructor */
    shader ( const shader& other ) = delete;

    /* default move constructor */
    shader ( shader&& other ) = default;

    /* deleted copy asignment operator */
    shader& operator= ( const shader& other ) = delete;

    /* virtual destructor */
    virtual ~shader () { destroy (); }



    /* destroy
     *
     * destroys the shader, setting its id to 0
     * any program using this shader will still function
     */
    void destroy () { object_manager::destroy_shader ( id ); id = 0; }



    /* get_target
     *
     * get the target of the shader
     */
    const GLenum& get_target () const { return target; }

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

    /* shader target */
    const GLenum target;

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
        : shader { GL_VERTEX_SHADER, _path }
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
        : shader { GL_GEOMETRY_SHADER, _path }
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
        : shader { GL_FRAGMENT_SHADER, _path }
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



/* IS_UNIFORM_PURE DEFINITION */

/* struct is_uniform_pure
 *
 * is_uniform_pure::value is true if the type is a pure uniform
 */
template<class T> struct glh::meta::is_uniform_pure : std::false_type {};
template<> struct glh::meta::is_uniform_pure<glh::core::pure_uniform> : std::true_type {};
template<> struct glh::meta::is_uniform_pure<glh::core::pure_struct_uniform> : std::true_type {};
template<> template<class _T> struct glh::meta::is_uniform_pure<glh::core::pure_array_uniform<_T>> : glh::meta::is_uniform_pure<_T> {};



/* PURE_UNIFORM_STORAGE DEFINITION */

/* class pure_uniform_storage : uniform_storage
 *
 * stores hard copies of pure uniforms so that pure uniform types can return references to these objects
 */
template<class T> class glh::core::pure_uniform_storage
{

    /* static assert that T is a pure uniform */
    static_assert ( meta::is_uniform_pure<T>::value, "class pure_uniform_storage must have a template paramater of a pure uniform type" );

public:

    /* full constructor 
     *
     * _prefix: the prefix for any member uniforms
     * _prog: the program the uniform storage is associated with
     */
    pure_uniform_storage ( const std::string& _prefix, program& _prog )
        : prefix { _prefix }
        , prog { _prog }
    {}

    /* deleted zero-parameter constructor */
    pure_uniform_storage () = delete;

    /* default copy constructor */
    pure_uniform_storage ( const pure_uniform_storage& other ) = default;

    /* deleted copy assignment operator */
    pure_uniform_storage& operator= ( const pure_uniform_storage& other ) = delete;

    /* default destructor */
    ~pure_uniform_storage () = default;



    /* get
     *
     * get a member out of the uniform storage
     * if it does not exist, it will be created
     * 
     * postfix: the postfix for the member uniform
     */
    T& get ( const std::string& postfix ) { return __get ( postfix ); }
    const T& get ( const std::string& postfix ) const { return __get ( postfix ); }



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



/* PURE_UNIFORM DEFINITION */

/* class pure_uniform : uniform
 *
 * derivation of uniform class for normal uniforms in shaders
 */
class glh::core::pure_uniform : public uniform
{
public:

    /* constructor
     *
     * construct from location and containing program
     *
     * _name: the name of the uniform
     * _location: the location of the uniform
     * _prog: the program associated with the uniform
     */
    pure_uniform ( const std::string& _name, program& _prog );

    /* deleted zero-parameter constructor */
    pure_uniform () = delete;

    /* default copy constructor */
    pure_uniform ( const pure_uniform& other ) = default;

    /* deleted copy assignment operator */
    pure_uniform& operator= ( const pure_uniform& other ) = delete;

    /* default destructor */
    ~pure_uniform () = default;



    /* set_float
     *
     * set uniform based on float value(s)
     * program must be already in use
     * 
     * v0...: the value(s) to set the uniform to
     */
    void set_float ( const GLfloat v0 ) const
        { assert_is_program_in_use (); glUniform1f ( location, v0 ); }
    void set_float ( const GLfloat v0, const GLfloat v1 )  const
        { assert_is_program_in_use (); glUniform2f ( location, v0, v1 ); }
    void set_float ( const GLfloat v0, const GLfloat v1, const GLfloat v2 ) const
        { assert_is_program_in_use (); glUniform3f ( location, v0, v1, v2 ); }
    void set_float ( const GLfloat v0, const GLfloat v1, const GLfloat v2, const GLfloat v3 ) const
        { assert_is_program_in_use (); glUniform4f ( location, v0, v1, v2, v3 ); }

    /* set_int
     *
     * set uniform based on integer value(s)
     * program must be already in use
     * 
     * v0...: the value(s) to set the uniform to
     */
    void set_int ( const GLint v0 ) const
        { assert_is_program_in_use (); glUniform1i ( location, v0 ); }
    void set_int ( const GLint v0, const GLint v1 ) const
        { assert_is_program_in_use (); glUniform2i ( location, v0, v1 ); }
    void set_int ( const GLint v0, const GLint v1, const GLint v2 ) const
        { assert_is_program_in_use (); glUniform3i ( location, v0, v1, v2 ); }
    void set_int ( const GLint v0, const GLint v1, const GLint v2, const GLint v3 ) const
        { assert_is_program_in_use (); glUniform4i ( location, v0, v1, v2, v3 ); }

    /* set_uint
     *
     * set uniform based on unsigned integer value(s)
     * program must be already in use
     * 
     * v0...: the value(s) to set the uniform to
     */
    void set_uint ( const GLuint v0 ) const
        { assert_is_program_in_use (); glUniform1ui ( location, v0 ); }
    void set_uint ( const GLuint v0, const GLuint v1 ) const
        { assert_is_program_in_use (); glUniform2ui ( location, v0, v1 ); }
    void set_uint ( const GLuint v0, const GLuint v1, const GLuint v2 ) const
        { assert_is_program_in_use (); glUniform3ui ( location, v0, v1, v2 ); }
    void set_uint ( const GLuint v0, const GLuint v1, const GLuint v2, const GLuint v3 ) const
        { assert_is_program_in_use (); glUniform4ui ( location, v0, v1, v2, v3 ); }

    /* set_matrix
     *
     * set uniform based on a matrix
     * program must be already in use
     * 
     * name: the name of the uniform
     * v0: the matrix to set the uniform to
     */
    void set_matrix ( const glh::math::mat2& v0 ) const
        { assert_is_program_in_use (); glUniformMatrix2fv ( location, 1, GL_FALSE, v0.export_data ().data () ); }
    void set_matrix ( const glh::math::mat3& v0 ) const
        { assert_is_program_in_use (); glUniformMatrix3fv ( location, 1, GL_FALSE, v0.export_data ().data () ); }
    void set_matrix ( const glh::math::mat4& v0 ) const
        { assert_is_program_in_use (); glUniformMatrix4fv ( location, 1, GL_FALSE, v0.export_data ().data () ); }

    /* set_vector
     *
     * set uniform based on a vector
     * the same as the set_float functions, except the parameters are packed into a vector
     * program must be already in use
     * 
     * v0: the vector to set the uniform to
     */
    void set_vector ( const glh::math::vec1& v0 ) const
        { assert_is_program_in_use (); glUniform1f ( location, v0.at ( 0 ) ); }
    void set_vector ( const glh::math::vec2& v0 ) const
        { assert_is_program_in_use (); glUniform2f ( location, v0.at ( 0 ), v0.at ( 1 ) ); }
    void set_vector ( const glh::math::vec3& v0 ) const
        { assert_is_program_in_use (); glUniform3f ( location, v0.at ( 0 ), v0.at ( 1 ), v0.at ( 2 ) ); }
    void set_vector ( const glh::math::vec4& v0 ) const
        { assert_is_program_in_use (); glUniform4f ( location, v0.at ( 0 ), v0.at ( 1 ), v0.at ( 2 ), v0.at ( 3 ) ); }


    
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
     */
    void assert_is_program_in_use () const;

    

    /* is_set_valid
     *
     * returns true if it is safe to set values
     */
    bool is_set_valid () const { return true; }

    /* assert_is_set_valid
     *
     * throws if the uniform is not set valid
     * 
     * operation: description of the operation
     */
    void assert_is_set_valid ( const std::string& operation ) const {}



    /* get_location
     * 
     * gets the location of the uniform
     */
    const GLint& get_location () const { return location; }

    /* get_program
     *
     * return a reference to the program
     */
    program& get_program () { return prog; }
    const program& get_program () const { return prog; }



private:

    /* prog
     *
     * the program the uniform is associated with
     */
    program& prog;

    /* location
     *
     * the location of the uniform
     */
    GLint location;

};



/* class pure_array_uniform : array_uniform
 *
 * derivation of array_uniform class for normal array uniforms in shaders
 */
template<class T> class glh::core::pure_array_uniform
    : public array_uniform<typename meta::aquire_uniform_base<T>::type>
{

    /* static assert that T is a pure uniform */
    static_assert ( meta::is_uniform_pure<T>::value, "class pure_array_uniform must have a template paramater of a pure uniform type" );

public:

    /* constructor
     *
     * construct from a name and containing program
     *
     * _name: the name of the uniform
     * _prog: the program associated with the uniform
     */
    pure_array_uniform ( const std::string& _name, program& _prog )
        : array_uniform<typename meta::aquire_uniform_base<T>::type> { _name }
        , prog { _prog }
        , uniforms { _name, _prog }
    {}

    /* deleted zero-parameter constructor */
    pure_array_uniform () = delete;

    /* default copy constructor */
    pure_array_uniform ( const pure_array_uniform& other ) = default;

    /* deleted copy assignment operator */
    pure_array_uniform& operator= ( const pure_array_uniform& other ) = delete;

    /* default destructor */
    ~pure_array_uniform () = default;



    /* typedef of T */
    typedef T type;



    /* at/operator[]
     *
     * return the uniform at an index
     */
    typename meta::aquire_uniform_base<T>::type& at ( const unsigned i ) { return uniforms.get ( "[" + std::to_string ( i ) + "]" ); }
    const typename meta::aquire_uniform_base<T>::type& at ( const unsigned i ) const { return uniforms.get ( "[" + std::to_string ( i ) + "]" ); }



private:

    /* the program the uniform is associated with */
    program& prog;

    /* storage of uniforms in the array */
    pure_uniform_storage<T> uniforms;

};



/* class pure_struct_uniform : struct_uniform
 *
 * derivation of struct_uniform class for normal struct uniforms in shaders
 */
class glh::core::pure_struct_uniform : public struct_uniform
{
public:

    /* constructor
     *
     * construct from a name and containing program
     *
     * _name: the name of the uniform
     * _prog: the program associated with the uniform
     */
    pure_struct_uniform ( const std::string& _name, program& _prog )
        : struct_uniform { _name }
        , prog { _prog }
        , uniforms { _name + ".", _prog }, struct_uniforms { _name + ".", _prog }
        , uniform_array_uniforms { _name + ".", _prog }, struct_array_uniforms { _name + ".", _prog }
        , uniform_2d_array_uniforms { _name + ".", _prog }, struct_2d_array_uniforms { _name + ".", _prog }
    {}

    /* deleted zero-parameter constructor */
    pure_struct_uniform () = delete;

    /* default copy constructor */
    pure_struct_uniform ( const pure_struct_uniform& other ) = default;

    /* deleted copy assignment operator */
    pure_struct_uniform& operator= ( const pure_struct_uniform& other ) = delete;

    /* default destructor */
    ~pure_struct_uniform () = default;


    /* get_(struct_)uniform
     *
     * get a normal/struct member of the struct uniform
     */
    uniform& get_uniform ( const std::string& name ) { return uniforms.get ( name ); }
    const uniform& get_uniform ( const std::string& name ) const { return uniforms.get ( name ); }
    struct_uniform& get_struct_uniform ( const std::string& name ) { return struct_uniforms.get ( name ); }
    const struct_uniform& get_struct_uniform ( const std::string& name ) const { return struct_uniforms.get ( name ); }
    
    
    /* get_..._array_uniform
     *
     * get an array member of the struct uniform
     */
    uniform_array_uniform& get_uniform_array_uniform ( const std::string& name ) { return uniform_array_uniforms.get ( name ); }
    const uniform_array_uniform& get_uniform_array_uniform ( const std::string& name ) const { return uniform_array_uniforms.get ( name ); }
    struct_array_uniform& get_struct_array_uniform ( const std::string& name ) { return struct_array_uniforms.get ( name ); }
    const struct_array_uniform& get_struct_array_uniform ( const std::string& name ) const { return struct_array_uniforms.get ( name ); }
    uniform_2d_array_uniform& get_uniform_2d_array_uniform ( const std::string& name ) { return uniform_2d_array_uniforms.get ( name ); }
    const uniform_2d_array_uniform& get_uniform_2d_array_uniform ( const std::string& name ) const { return uniform_2d_array_uniforms.get ( name ); }
    struct_2d_array_uniform& get_struct_2d_array_uniform ( const std::string& name ) { return struct_2d_array_uniforms.get ( name ); }
    const struct_2d_array_uniform& get_struct_2d_array_uniform ( const std::string& name ) const { return struct_2d_array_uniforms.get ( name ); }



private:

    /* the program the uniform is associated with */
    program& prog;

    /* (..._)uniforms
     *
     * storage of uniforms in the program
     */
    pure_uniform_storage<pure_uniform> uniforms;
    pure_uniform_storage<pure_struct_uniform> struct_uniforms;
    pure_uniform_storage<pure_uniform_array_uniform> uniform_array_uniforms;
    pure_uniform_storage<pure_struct_array_uniform> struct_array_uniforms;
    pure_uniform_storage<pure_uniform_2d_array_uniform> uniform_2d_array_uniforms;
    pure_uniform_storage<pure_struct_2d_array_uniform> struct_2d_array_uniforms;

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



    /* get_(struct_/array_)uniform
     *
     * return a uniform object based on a name
     * 
     * name: the name of the uniform
     * 
     * return: unfirom object
     */
    uniform& get_uniform ( const std::string& name ) { return uniforms.get ( name ); }
    const uniform& get_uniform ( const std::string& name ) const { return uniforms.get ( name ); }
    struct_uniform& get_struct_uniform ( const std::string& name ) { return struct_uniforms.get ( name ); }
    const struct_uniform& get_struct_uniform ( const std::string& name ) const { return struct_uniforms.get ( name ); }
    
    
    /* get_..._array_uniform
     *
     * simplified versions of get_array_uniform
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
     * 
     * return: location of the uniform
     */
    GLint get_uniform_location ( const std::string& name ) const;



    /* destroy
     *
     * destroys the shader program, setting id to 0
     */
    void destroy () { object_manager::destroy_program ( id ); id = 0; }

    /* use
     *
     * use the shader program for the following OpenGL function calls
     * will not call glUseProgram if already in use
     */
    void use () const { object_manager::use_program ( id ); }

    /* is_in_use
     *
     * return: boolean for if the program is in use
     */
    bool is_in_use () const { return object_manager::is_program_in_use ( id ); }



private:

    /* uniform_locations
     *
     * a map to the location of uniforms based on their full names
     * saves some time on searching for uniforms every time they are changed
     */
    mutable std::map<std::string, GLint> uniform_locations;

    /* (..._)uniforms
     *
     * storage of uniforms in the program
     */
    pure_uniform_storage<pure_uniform> uniforms;
    pure_uniform_storage<pure_struct_uniform> struct_uniforms;
    pure_uniform_storage<pure_uniform_array_uniform> uniform_array_uniforms;
    pure_uniform_storage<pure_struct_array_uniform> struct_array_uniforms;
    pure_uniform_storage<pure_uniform_2d_array_uniform> uniform_2d_array_uniforms;
    pure_uniform_storage<pure_struct_2d_array_uniform> struct_2d_array_uniforms;

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



/* PURE_UNIFORM_STORAGE IMPLEMENTATION */

/* __get
 *
 * add a uniform to the map if not already present, then return a reference
 *
 * postfix: the postfix for the member uniform
 */
template<class T> T& glh::core::pure_uniform_storage<T>::__get ( const std::string& postfix ) const
{
    /* try to get the uniform from the map */
    try
    {
        return uniforms.at ( postfix );
    }
    /* catch out of range error and add uniform to map */
    catch ( const std::out_of_range& ex )
    {
        uniforms.insert ( { postfix, T { this->prefix + postfix, prog } } );
        return uniforms.at ( postfix );
    }
}



/* #ifndef GLHELPER_SHADER_HPP_INCLUDED */
#endif