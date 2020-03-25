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
#include <string>

/* include glhelper_core.hpp */
#include <glhelper/glhelper_core.hpp>

/* include glhelper_exception.hpp */
#include <glhelper/glhelper_exception.hpp>

/* include glhelper_glad.hpp */
#include <glhelper/glhelper_glad.hpp>

/* include glhelper_matrix.hpp */
#include <glhelper/glhelper_matrix.hpp>

/* include glhelper_vector.hpp */
#include <glhelper/glhelper_vector.hpp>



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

    /* class v/g/fshader
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
    
    /* class shader_exception : exception
     *
     * for exceptions related to shaders
     */
    class shader_exception;
}



/* FULL DECLARATIONS */

/* class shader : object
 *
 * base class for any type of shader
 */
class glh::shader : public object
{
public:

    /* constructor */
    explicit shader ( const GLenum _target, const std::string& _path );

    /* deleted zero-parameter constructor */
    explicit shader () = delete;

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
    void destroy () override;



protected:

    /* shader target */
    const GLenum target;

    /* path to the shader */
    const std::string path;

    /* shader source */
    std::string source;

};



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
    explicit vshader () = delete;

    /* deleted copy constructor */
    vshader ( const vshader& other ) = delete;

    /* default move constructor */
    vshader ( vshader&& other ) = default;

    /* deleted copy asignment operator */
    vshader& operator= ( const vshader& other ) = delete;

    /* default destructor */
    ~vshader () = default;

};



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
    explicit gshader () = delete;

    /* deleted copy constructor */
    gshader ( const gshader& other ) = delete;

    /* default move constructor */
    gshader ( gshader&& other ) = default;

    /* deleted copy asignment operator */
    gshader& operator= ( const gshader& other ) = delete;

    /* default destructor */
    ~gshader () = default;

};



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
    explicit fshader () = delete;

    /* deleted copy constructor */
    fshader ( const fshader& other ) = delete;

    /* default move constructor */
    fshader ( fshader&& other ) = default;

    /* deleted copy asignment operator */
    fshader& operator= ( const fshader& other ) = delete;

    /* default destructor */
    ~fshader () = default;

};



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
    explicit program ( const vshader& vs, const gshader& gs, const fshader& fs );

    /* two-shader constructor
     *
     * link vertex and fragment shaders into a program
     * uses the default geometry shader
     * NOTE: the shader program remains valid even when linked shaders are destroyed
     */
    explicit program ( const vshader& vs, const fshader& fs );

    /* deleted zero-parameter constructor */
    explicit program () = delete;

    /* deleted copy constructor */
    program ( const program& other ) = delete;

    /* default move constructor */
    program ( program&& other ) = default;

    /* deleted copy assignment operator */
    program& operator= ( const program& other ) = delete;

    /* destructor */
    ~program () { destroy (); }



    /* set_uniform_float
     *
     * set uniform based on float value(s)
     */
    void set_uniform_float ( const std::string& name, const GLfloat v0 ) 
        { glUniform1f ( get_uniform_location ( name ), v0 ); }
    void set_uniform_float ( const std::string& name, const GLfloat v0, const GLfloat v1 ) 
        { glUniform2f ( get_uniform_location ( name ), v0, v1 ); }
    void set_uniform_float ( const std::string& name, const GLfloat v0, const GLfloat v1, const GLfloat v2 ) 
        { glUniform3f ( get_uniform_location ( name ), v0, v1, v2 ); }
    void set_uniform_float ( const std::string& name, const GLfloat v0, const GLfloat v1, const GLfloat v2, const GLfloat v3 )
        { glUniform4f ( get_uniform_location ( name ), v0, v1, v2, v3 ); }

    /* set_uniform_int
     *
     * set uniform based on integer value(s)
     */
    void set_uniform_int ( const std::string& name, const GLint v0 ) 
        { glUniform1i ( get_uniform_location ( name ), v0 ); }
    void set_uniform_int ( const std::string& name, const GLint v0, const GLint v1 ) 
        { glUniform2i ( get_uniform_location ( name ), v0, v1 ); }
    void set_uniform_int ( const std::string& name, const GLint v0, const GLint v1, const GLint v2 ) 
        { glUniform3i ( get_uniform_location ( name ), v0, v1, v2 ); }
    void set_uniform_int ( const std::string& name, const GLint v0, const GLint v1, const GLint v2, const GLint v3 )
        { glUniform4i ( get_uniform_location ( name ), v0, v1, v2, v3 ); }

    /* set_uniform_uint
     *
     * set uniform based on unsigned integer value(s)
     */
    void set_uniform_uint ( const std::string& name, const GLuint v0 ) 
        { glUniform1ui ( get_uniform_location ( name ), v0 ); }
    void set_uniform_uint ( const std::string& name, const GLuint v0, const GLuint v1 ) 
        { glUniform2ui ( get_uniform_location ( name ), v0, v1 ); }
    void set_uniform_uint ( const std::string& name, const GLuint v0, const GLuint v1, const GLuint v2 ) 
        { glUniform3ui ( get_uniform_location ( name ), v0, v1, v2 ); }
    void set_uniform_uint ( const std::string& name, const GLuint v0, const GLuint v1, const GLuint v2, const GLuint v3 )
        { glUniform4ui ( get_uniform_location ( name ), v0, v1, v2, v3 ); }

    /* set_uniform_matrix
     *
     * set uniform based on a matrix
     */
    void set_uniform_matrix ( const std::string& name, const glh::math::mat2& v0 ) 
        { glUniformMatrix2fv ( get_uniform_location ( name ), 1, GL_FALSE, v0.float_data ().data () ); }
    void set_uniform_matrix ( const std::string& name, const glh::math::mat3& v0 ) 
        { glUniformMatrix3fv ( get_uniform_location ( name ), 1, GL_FALSE, v0.float_data ().data () ); }
    void set_uniform_matrix ( const std::string& name, const glh::math::mat4& v0 )
        { glUniformMatrix4fv ( get_uniform_location ( name ), 1, GL_FALSE, v0.float_data ().data () ); }

    /* set_uniform_vector
     *
     * set uniform based on a vector
     * the same as the set_uniform_float functions, the parameters are packed into a vector
     */
    void set_uniform_vector ( const std::string& name, const glh::math::vec1& v0 ) 
        { glUniform1fv ( get_uniform_location ( name ), 1, v0.float_data ().data () ); }
    void set_uniform_vector ( const std::string& name, const glh::math::vec2& v0 ) 
        { glUniform2fv ( get_uniform_location ( name ), 1, v0.float_data ().data () ); }
    void set_uniform_vector ( const std::string& name, const glh::math::vec3& v0 ) 
        { glUniform3fv ( get_uniform_location ( name ), 1, v0.float_data ().data () ); }
    void set_uniform_vector ( const std::string& name, const glh::math::vec4& v0 )
        { glUniform4fv ( get_uniform_location ( name ), 1, v0.float_data ().data () ); }



    /* destroy
     *
     * destroys the shader program, setting id to 0
     */
    void destroy () override;

    /* use
     *
     * use the shader program for the following OpenGL function calls
     */
    void use () const;



private:

    /* get_uniform_location
     *
     * get the location of a uniform
     * 
     * return: location of the uniform
     */
    GLint get_uniform_location ( const std::string& name );

};



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
    explicit shader_exception () = default;

    /* default everything else and inherits what () function */

};




/* #ifndef GLHELPER_SHADER_HPP_INCLUDED */
#endif