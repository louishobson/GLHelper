/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * include/glhelper/glhelper_shader.hpp
 * 
 * constructs for managing and using shaders
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

/* include glhelper_core.hpp */
#include <glhelper/glhelper_core.hpp>

/* include glhelper_exception.hpp */
#include <glhelper/glhelper_exception.hpp>

/* glhelper_manager.hpp */
#include <glhelper/glhelper_manager.hpp>

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

    /* class program : object
     *
     * shader program
     */
    class program;

    /* struct is_uniform
     *
     * is_uniform::value is true if the type is a uniform
     */
    template<class T> struct is_uniform;

    /* class uniform
     *
     * a reference to a uniform in a program
     * can set the uniform without querying its name
     */
    class uniform;

    /* class complex_uniform
     *
     * a base class for a uniform which stores other uniforms
     */
    class complex_uniform;

    /* class struct_uniform : complex_uniform
     *
     * a reference to a structure uniform in a program
     */
    class struct_uniform;

    /* class array_uniform : complex_uniform
     *
     * a reference to an array uniform in a program
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



/* SHADER DEFINITION */

/* class shader : object
 *
 * base class for any type of shader
 */
class glh::shader : public object
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
    void destroy () override { glh::object_manager::destroy_shader ( id ); id = 0; }



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
class glh::vshader : public shader
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
class glh::gshader : public shader
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
class glh::fshader : public shader
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



/* PROGRAM DEFINITION */

/* class program : object
 *
 * shader program
 */
class glh::program : public object
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
    uniform get_uniform ( const std::string& name );
    const uniform get_unfiorm ( const std::string& name ) const;
    struct_uniform get_struct_uniform ( const std::string& name );
    const struct_uniform get_struct_uniform ( const std::string& name ) const;
    template<class T> array_uniform<T> get_array_uniform ( const std::string& name ) { return array_uniform<T> { name, * this }; }
    template<class T> const array_uniform<T> get_array_uniform ( const std::string& name ) const { return array_uniform<T> { name, * this }; }

    /* get_..._array_uniform
     *
     * simplified versions of get_array_uniform
     */
    uniform_array_uniform get_uniform_array_uniform ( const std::string& name );
    const uniform_array_uniform get_uniform_array_uniform ( const std::string& name ) const;
    struct_array_uniform get_struct_array_uniform ( const std::string& name );
    const struct_array_uniform get_struct_array_uniform ( const std::string& name ) const;
    uniform_2d_array_uniform get_uniform_2d_array_uniform ( const std::string& name );
    const uniform_2d_array_uniform get_uniform_2d_array_uniform ( const std::string& name ) const;
    struct_2d_array_uniform get_struct_2d_array_uniform ( const std::string& name );
    const struct_2d_array_uniform get_struct_2d_array_uniform ( const std::string& name ) const;



    /* destroy
     *
     * destroys the shader program, setting id to 0
     */
    void destroy () override { glh::object_manager::destroy_program ( id ); id = 0; }

    /* use
     *
     * use the shader program for the following OpenGL function calls
     * will not call glUseProgram if already in use
     */
    void use () const { glh::object_manager::use_program ( id ); }

    /* is_in_use
     *
     * return: boolean for if the program is in use
     */
    bool is_in_use () const { return glh::object_manager::is_program_in_use ( id ); }



private:

    /* uniform_locations
     *
     * a map to the location of uniforms based on their names
     * saves some time on searching for uniforms every time they are changed
     */
    mutable std::map<std::string, GLint> uniform_locations;

    /* get_uniform_location
     *
     * get the location of a uniform
     * 
     * _name: the name of the uniform
     * 
     * return: location of the uniform
     */
    GLint get_uniform_location ( const std::string& _name ) const;

};



/* IS_UNIFORM DEFINITION */

/* struct is_uniform
 *
 * is_uniform::value is true if the type is a uniform
 */
template<class T> struct glh::is_uniform
{
    static const bool value = false;
    operator bool () { return value; }
};
template<> struct glh::is_uniform<glh::uniform>
{
    static const bool value = true;
    operator bool () { return value; }
};
template<> struct glh::is_uniform<glh::struct_uniform>
{
    static const bool value = true;
    operator bool () { return value; }
};
template<> template<class _T> struct glh::is_uniform<glh::array_uniform<_T>>
{
    static const bool value = is_uniform<_T>::value;
    operator bool () { return value; }
};


/* UNIFORM DEFINITION */

/* class uniform
 *
 * a reference to a uniform in a program
 * can set the uniform without querying its name
 */
class glh::uniform
{
public:

    /* constructor
     *
     * construct from location and containing program
     *
     * _location: the location of the uniform
     * _prog: the program associated with the uniform
     */
    uniform ( const GLint _location, program& _prog )
        : location { _location }
        , prog { _prog }
    {}

    /* deleted zero-parameter constructor */
    uniform () = delete;

    /* default copy constructor */
    uniform ( const uniform& other ) = default;

    /* deleted copy assignment operator */
    uniform& operator= ( const uniform& other ) = delete;

    /* default destructor */
    ~uniform () = default;



    /* set_float
     *
     * set uniform based on float value(s)
     * program must be already in use
     * 
     * v0...: the value(s) to set the uniform to
     */
    void set_float ( const GLfloat v0 ) 
        { check_is_program_in_use (); glUniform1f ( location, v0 ); }
    void set_float ( const GLfloat v0, const GLfloat v1 ) 
        { check_is_program_in_use (); glUniform2f ( location, v0, v1 ); }
    void set_float ( const GLfloat v0, const GLfloat v1, const GLfloat v2 ) 
        { check_is_program_in_use (); glUniform3f ( location, v0, v1, v2 ); }
    void set_float ( const GLfloat v0, const GLfloat v1, const GLfloat v2, const GLfloat v3 )
        { check_is_program_in_use (); glUniform4f ( location, v0, v1, v2, v3 ); }

    /* set_int
     *
     * set uniform based on integer value(s)
     * program must be already in use
     * 
     * v0...: the value(s) to set the uniform to
     */
    void set_int ( const GLint v0 ) 
        { check_is_program_in_use (); glUniform1i ( location, v0 ); }
    void set_int ( const GLint v0, const GLint v1 ) 
        { check_is_program_in_use (); glUniform2i ( location, v0, v1 ); }
    void set_int ( const GLint v0, const GLint v1, const GLint v2 ) 
        { check_is_program_in_use (); glUniform3i ( location, v0, v1, v2 ); }
    void set_int ( const GLint v0, const GLint v1, const GLint v2, const GLint v3 )
        { check_is_program_in_use (); glUniform4i ( location, v0, v1, v2, v3 ); }

    /* set_uint
     *
     * set uniform based on unsigned integer value(s)
     * program must be already in use
     * 
     * v0...: the value(s) to set the uniform to
     */
    void set_uint ( const GLuint v0 ) 
        { check_is_program_in_use (); glUniform1ui ( location, v0 ); }
    void set_uint ( const GLuint v0, const GLuint v1 ) 
        { check_is_program_in_use (); glUniform2ui ( location, v0, v1 ); }
    void set_uint ( const GLuint v0, const GLuint v1, const GLuint v2 ) 
        { check_is_program_in_use (); glUniform3ui ( location, v0, v1, v2 ); }
    void set_uint ( const GLuint v0, const GLuint v1, const GLuint v2, const GLuint v3 )
        { check_is_program_in_use (); glUniform4ui ( location, v0, v1, v2, v3 ); }

    /* set_matrix
     *
     * set uniform based on a matrix
     * program must be already in use
     * 
     * name: the name of the uniform
     * v0: the matrix to set the uniform to
     */
    void set_matrix ( const glh::math::mat2& v0 ) 
        { check_is_program_in_use (); glUniformMatrix2fv ( location, 1, GL_FALSE, v0.export_data ().data () ); }
    void set_matrix ( const glh::math::mat3& v0 ) 
        { check_is_program_in_use (); glUniformMatrix3fv ( location, 1, GL_FALSE, v0.export_data ().data () ); }
    void set_matrix ( const glh::math::mat4& v0 )
        { check_is_program_in_use (); glUniformMatrix4fv ( location, 1, GL_FALSE, v0.export_data ().data () ); }

    /* set_vector
     *
     * set uniform based on a vector
     * the same as the set_float functions, except the parameters are packed into a vector
     * program must be already in use
     * 
     * v0: the vector to set the uniform to
     */
    void set_vector ( const glh::math::vec1& v0 ) 
        { check_is_program_in_use (); glUniform1f ( location, v0.at ( 0 ) ); }
    void set_vector ( const glh::math::vec2& v0 ) 
        { check_is_program_in_use (); glUniform2f ( location, v0.at ( 0 ), v0.at ( 1 ) ); }
    void set_vector ( const glh::math::vec3& v0 ) 
        { check_is_program_in_use (); glUniform3f ( location, v0.at ( 0 ), v0.at ( 1 ), v0.at ( 2 ) ); }
    void set_vector ( const glh::math::vec4& v0 )
        { check_is_program_in_use (); glUniform4f ( location, v0.at ( 0 ), v0.at ( 1 ), v0.at ( 2 ), v0.at ( 3 ) ); }


    
    /* use_program
     *
     * use the associated program
     */
    void use_program () const { prog.use (); }

    /* is_program_in_use
     *
     * return a boolean for if the associated program is in use
     */
    bool is_program_in_use () const { return prog.is_in_use (); } 

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

    /* location
     *
     * the location of the uniform
     */
    const GLint location;

    /* prog
     *
     * the program the uniform is associated with
     */
    program& prog;

    /* check_is_program_in_use
     *
     * will throw if the associated program is not in use
     */
    void check_is_program_in_use () const;

};

/* comparison operators */
inline bool operator== ( const glh::uniform& lhs, const glh::uniform& rhs ) { return ( lhs.get_location () == rhs.get_location () && lhs.get_program () == rhs.get_program () ); }
inline bool operator!= ( const glh::uniform& lhs, const glh::uniform& rhs ) { return ( lhs.get_location () != rhs.get_location () || lhs.get_program () != rhs.get_program () ); }



/* class complex_uniform
 *
 * a base class for a uniform which stores other uniforms
 */
class glh::complex_uniform
{
public:

    /* constructor
     *
     * construct from a name and containing program
     *
     * _name: the name of the uniform
     * _prog: the program associated with the uniform
     */
    complex_uniform ( const std::string& _name, program& _prog )
        : name { _name }
        , prog { _prog }
    {}

    /* deleted zero-parameter constructor */
    complex_uniform () = default;

    /* default copy constructor */
    complex_uniform ( const complex_uniform& other ) = default;

    /* deleted copy assignment operator */
    complex_uniform& operator= ( const complex_uniform& other ) = delete;

    /* default pure virtual destructor */
    virtual ~complex_uniform () = 0;



    /* use_program
     *
     * use the associated program
     */
    void use_program () const { prog.use (); }

    /* is_program_in_use
     *
     * return a boolean for if the associated program is in use
     */
    bool is_program_in_use () const { return prog.is_in_use (); }

    /* get_name
     *
     * get the name of the uniform
     */
    const std::string& get_name () const { return name; }

    /* get_program
     *
     * get the program associated with the uniform
     */
    program& get_program () { return prog; }
    const program& get_program () const { return prog; }



protected:

    /* name
     *
     * the name of the structure uniform
     */
    const std::string name;

    /* prog
     *
     * the program the uniform is associated with
     */
    program& prog;

};

/* make destructor default */
inline glh::complex_uniform::~complex_uniform () = default;

/* comparison operators */
inline bool operator== ( const glh::complex_uniform& lhs, const glh::complex_uniform& rhs ) { return ( lhs.get_name () == rhs.get_name () && lhs.get_program () == rhs.get_program () ); }
inline bool operator!= ( const glh::complex_uniform& lhs, const glh::complex_uniform& rhs ) { return ( lhs.get_name () != rhs.get_name () || lhs.get_program () != rhs.get_program () ); }



/* class struct_uniform : complex_uniform
 *
 * a reference to a structure uniform in a program
 */
class glh::struct_uniform : public complex_uniform
{
public:

    /* constructor
     *
     * construct from a name and containing program
     *
     * _name: the name of the uniform
     * _prog: the program associated with the uniform
     */
    struct_uniform ( const std::string& _name, program& _prog )
        : complex_uniform { _name, _prog }
    {}

    /* deleted zero-parameter constructor */
    struct_uniform () = delete;

    /* default copy constructor */
    struct_uniform ( const struct_uniform& other ) = default;

    /* deleted copy assignment operator */
    struct_uniform& operator= ( const struct_uniform& other ) = delete;

    /* default destructor */
    ~struct_uniform () = default;



    /* get_(struct_/array_)uniform
     *
     * get a member of the struct
     */
    uniform get_uniform ( const std::string& member );
    const uniform get_uniform ( const std::string& member ) const;
    struct_uniform get_struct_uniform ( const std::string& member );
    const struct_uniform get_struct_uniform ( const std::string& member ) const;
    template<class T> array_uniform<T> get_array_uniform ( const std::string& member ) { return array_uniform<T> { name + "." + member, prog }; }
    template<class T> const array_uniform<T> get_array_uniform ( const std::string& member ) const { return array_uniform<T> { name + "." + member, prog }; }

    /* get_..._array_uniform
     *
     * simplified versions of get_array_uniform
     */
    uniform_array_uniform get_uniform_array_uniform ( const std::string& member );
    const uniform_array_uniform get_uniform_array_uniform ( const std::string& member ) const;
    struct_array_uniform get_struct_array_uniform ( const std::string& member );
    const struct_array_uniform get_struct_array_uniform ( const std::string& member ) const;
    uniform_2d_array_uniform get_uniform_2d_array_uniform ( const std::string& member );
    const uniform_2d_array_uniform get_uniform_2d_array_uniform ( const std::string& member ) const;
    struct_2d_array_uniform get_struct_2d_array_uniform ( const std::string& member );
    const struct_2d_array_uniform get_struct_2d_array_uniform ( const std::string& member ) const;


};



/* class array_uniform : complex_uniform
 *
 * a reference to an array uniform in a program
 */
template<class T = glh::uniform> class glh::array_uniform : public complex_uniform
{
    /* static assert that T is a uniform */
    static_assert ( is_uniform<T>::value, "cannot create array_uniform object containing non-uniform type" );

public:

    /* constructor
     *
     * construct from a name and containing program
     *
     * _name: the name of the uniform
     * _prog: the program associated with the uniform
     */
    array_uniform ( const std::string& _name, program& _prog )
        : complex_uniform { _name, _prog }
    {}

    /* deleted zero-parameter constructor */
    array_uniform () = delete;

    /* default copy constructor */
    array_uniform ( const array_uniform& other ) = default;

    /* deleted copy assignment operator */
    array_uniform& operator= ( const array_uniform& other ) = delete;

    /* default destructor */
    ~array_uniform () = default;



    /* at/operator[]
     *
     * return the uniform at an index
     */
    T at ( const unsigned i ) { return T { name + "[" + std::to_string ( i ) + "]", prog }; }
    const T at ( const unsigned i ) const { return T { name + "[" + std::to_string ( i ) + "]", prog }; }
    T operator[] ( const unsigned i ) { return at ( i ); }
    const T operator[] ( const unsigned i ) const { return at ( i ); }

};
template<> class glh::array_uniform<glh::uniform> : public complex_uniform
{
    /* static assert that T is a uniform */
    static_assert ( is_uniform<glh::uniform>::value, "cannot create array_uniform object containing non-uniform type" );

public:

    /* constructor
     *
     * construct from a name and containing program
     *
     * _name: the name of the uniform
     * _prog: the program associated with the uniform
     */
    array_uniform ( const std::string& _name, program& _prog )
        : complex_uniform { _name, _prog }
    {}

    /* deleted zero-parameter constructor */
    array_uniform () = delete;

    /* default copy constructor */
    array_uniform ( const array_uniform& other ) = default;

    /* deleted copy assignment operator */
    array_uniform& operator= ( const array_uniform& other ) = delete;

    /* default destructor */
    ~array_uniform () = default;



    /* at/operator[]
     *
     * return the uniform at an index
     */
    uniform at ( const unsigned i ) { return prog.get_uniform ( name + "[" + std::to_string ( i ) + "]" ); }
    const uniform at ( const unsigned i ) const { return prog.get_uniform ( name + "[" + std::to_string ( i ) + "]" ); }
    uniform operator[] ( const unsigned i ) { return at ( i ); }
    const uniform operator[] ( const unsigned i ) const { return at ( i ); }

};



/* SHADER_EXCEPTION DEFINITION */

/* class shader_exception : exception
 *
 * for exceptions related to shaders
 */
class glh::shader_exception : public exception
{
public:

    /* full constructor
     *
     * __what: description of the exception
     */
    explicit shader_exception ( const std::string& __what )
        : exception ( __what )
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
class glh::uniform_exception : public exception
{
public:

    /* full constructor
     *
     * __what: description of the exception
     */
    explicit uniform_exception ( const std::string& __what )
        : exception ( __what )
    {}

    /* default zero-parameter constructor
     *
     * construct uniform_exception with no descrption
     */
    uniform_exception () = default;

    /* default everything else and inherits what () function */

};




/* #ifndef GLHELPER_SHADER_HPP_INCLUDED */
#endif