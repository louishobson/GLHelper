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
 * CLASS GLH::MATH::UNIFORM_ALIGNED_VECTOR/MATRIX
 * 
 * classes which can be implicity converted to and from normal vectors and matrices
 * the storage of these special types is such that they are valid to be written into a ubo as is
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
#include <vector>

/* include glhelper_core.hpp */
#include <glhelper/glhelper_core.hpp>

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



/* NAMESPACE DECLARATIONS */

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

    namespace math
    {
        /* class uniform_aligned_vector
         *
         * a class to represent a vector with data aligned specifically for use with ubos
         * will implicitly convert to and from a normal vector
         */
        template<unsigned M, class T> class uniform_aligned_vector;

        /* class uniform_aligned_matrix
         *
         * a class to represent a matrix with data aligned specifically for use with ubos
         * will implicitly convert to and from a normal matrix
         */
        template<unsigned M, unsigned N, class T> class uniform_aligned_matrix;

        /* default uniform_aligned_vector types */
        template<unsigned M, class T> using ua_vector = uniform_aligned_vector<M, T>;
        template<unsigned M> using ua_fvector = ua_vector<M, float>;
        template<unsigned M> using ua_dvector = ua_vector<M, double>;
        template<unsigned M> using ua_ivector = ua_vector<M, int>;
        using ua_fvec2 = ua_fvector<2>;
        using ua_fvec3 = ua_fvector<3>;
        using ua_fvec4 = ua_fvector<4>;
        using ua_dvec2 = ua_dvector<2>;
        using ua_dvec3 = ua_dvector<3>;
        using ua_dvec4 = ua_dvector<4>;
        using ua_ivec2 = ua_ivector<2>;
        using ua_ivec3 = ua_ivector<3>;
        using ua_ivec4 = ua_ivector<4>;
        using ua_vec2 = ua_dvec2;
        using ua_vec3 = ua_dvec3;
        using ua_vec4 = ua_dvec4;


        /* default uniform_aligned_matrix types */
        template<unsigned M, unsigned N, class T> using ua_matrix = uniform_aligned_matrix<M, N, T>;
        template<unsigned M, unsigned N> using ua_fmatrix = ua_matrix<M, N, GLfloat>;
        template<unsigned M, unsigned N> using ua_dmatrix = ua_matrix<M, N, GLdouble>;
        template<unsigned M, unsigned N> using ua_imatrix = ua_matrix<M, N, GLint>;
        using ua_fmat2 = ua_fmatrix<2, 2>; using ua_fmat2x3 = ua_fmatrix<2, 3>; using ua_fmat2x4 = ua_fmatrix<2, 4>;
        using ua_fmat3 = ua_fmatrix<3, 3>; using ua_fmat3x2 = ua_fmatrix<3, 2>; using ua_fmat3x4 = ua_fmatrix<3, 4>;
        using ua_fmat4 = ua_fmatrix<4, 4>; using ua_fmat4x2 = ua_fmatrix<4, 2>; using ua_fmat4x3 = ua_fmatrix<4, 3>;
        using ua_dmat2 = ua_dmatrix<2, 2>; using ua_dmat2x3 = ua_dmatrix<2, 3>; using ua_dmat2x4 = ua_dmatrix<2, 4>;
        using ua_dmat3 = ua_dmatrix<3, 3>; using ua_dmat3x2 = ua_dmatrix<3, 2>; using ua_dmat3x4 = ua_dmatrix<3, 4>;
        using ua_dmat4 = ua_dmatrix<4, 4>; using ua_dmat4x2 = ua_dmatrix<4, 2>; using ua_dmat4x3 = ua_dmatrix<4, 3>;
        using ua_imat2 = ua_imatrix<2, 2>; using ua_imat2x3 = ua_imatrix<2, 3>; using ua_imat2x4 = ua_imatrix<2, 4>;
        using ua_imat3 = ua_imatrix<3, 3>; using ua_imat3x2 = ua_imatrix<3, 2>; using ua_imat3x4 = ua_imatrix<3, 4>;
        using ua_imat4 = ua_imatrix<4, 4>; using ua_imat4x2 = ua_imatrix<4, 2>; using ua_imat4x3 = ua_imatrix<4, 3>;
        using ua_mat2 = ua_dmat2; using ua_mat2x3 = ua_dmat2x3; using ua_mat2x4 = ua_dmat2x4;
        using ua_mat3 = ua_dmat3; using ua_mat3x2 = ua_dmat3x2; using ua_mat3x4 = ua_dmat3x4;
        using ua_mat4 = ua_dmat4; using ua_mat4x2 = ua_dmat4x2; using ua_mat4x3 = ua_dmat4x3;

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

    /* default virtual destructor */
    virtual ~shader () = default;



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



/* UNIFORM_ALIGNED_VECTOR DEFINITION */

/* class uniform_aligned_vector
 *
 * a class to represent a vector with data aligned specifically for use with ubos
 * will implicitly convert to and from a normal vector
 */
template<unsigned M, class T> class glh::math::uniform_aligned_vector
{

    /* assert that M >= 2 && M <= 4 */
    static_assert ( M >=2 && M <= 4, "a uniform aligned vector can not have a dimension outside of the range 2-4" );

    /* static assert that T is arithmetic */
    static_assert ( std::is_arithmetic<T>::value, "a uniform aligned vector cannot be instantiated from a non-arithmetic type" );

    /* assert that T is 4 bytes */
    static_assert ( sizeof ( T ) == 4, "a uniform aligned vector must have a value of T which is 4 bytes in size" );

public:

    /* deleted zero-parameter constructor
     *
     * can only be constructed from another vector
     */
    uniform_aligned_vector () = delete;

    /* retype constructor
     *
     * construct from a (uniform aligned) vector of the same size but different type
     */
    template<class _T> uniform_aligned_vector ( const uniform_aligned_vector<M, _T>& other );
    template<class _T> uniform_aligned_vector ( const vector<M, _T>& other );

    /* implicitly casts to vector of the same type and size */
    operator vector<M, T> () const;

    /* deleted copy assignment operator */
    uniform_aligned_vector& operator= ( const uniform_aligned_vector& other ) = delete;

    /* default destructor */
    ~uniform_aligned_vector () = default;



    /* the size of the vector */
    static const unsigned size = M;
    
    /* the type of the vector */
    typedef T value_type;



    /* internal_ptr
     *
     * return: pointer to the internal array of data
     */
    T * internal_ptr () { return data.data (); }
    const T * internal_ptr () const { return data.data (); }



private:

    /* data
     *
     * either 2 or 4 T's
     */
    std::conditional_t<M <= 2, std::array<T, 2>, std::array<T, 4>> data;

};



/* UNIFORM_ALIGNED_MATRIX DEFINITION */

/* class uniform_aligned_matrix
 *
 * a class to represent a matrix with data aligned specifically for use with ubos
 * will implicitly convert to and from a normal matrix
 */
template<unsigned M, unsigned N, class T> class glh::math::uniform_aligned_matrix
{

    /* assert that M >= 2 && M <= 4 && N >= 2 && N <= 4 */
    static_assert ( M >=2 && M <= 4 && N >=2 && N <= 4, "a uniform aligned matrix can not have dimensions outside of the range (2-4)x(2-4)" );

    /* static assert that T is arithmetic */
    static_assert ( std::is_arithmetic<T>::value, "a uniform aligned matrix cannot be instantiated from a non-arithmetic type" );

    /* assert that T is 4 bytes */
    static_assert ( sizeof ( T ) == 4, "a uniform aligned matrix must have a value of T which is 4 bytes in size" );

public:

    /* deleted zero-parameter constructor
     *
     * can only be constructed from another vector
     */
    uniform_aligned_matrix () = delete;

    /* retype constructor
     *
     * construct from a (uniform aligned) vector of the same size but different type
     */
    template<class _T> uniform_aligned_matrix ( const uniform_aligned_matrix<M, N, _T>& other );
    template<class _T> uniform_aligned_matrix ( const matrix<M, N, _T>& other );

    /* implicitly casts to matrix of the same type and size */
    operator matrix<M, N, T> () const;

    /* deleted copy assignment operator */
    uniform_aligned_matrix& operator= ( const uniform_aligned_matrix& other ) = delete;

    /* default destructor */
    ~uniform_aligned_matrix () = default;



    /* width/height
     *
     * width and height of the matrix
     */
    static const unsigned height = M;
    static const unsigned width = N;
    
    /* the type of the matrix */
    typedef T value_type;



    /* internal_ptr
     *
     * return: pointer to the internal array of data
     */
    T * internal_ptr () { return data.data (); }
    const T * internal_ptr () const { return data.data (); }



private:

    /* data
     *
     * stored as an array of collumn vectors of dimension 4
     */
    std::array<T, N * 4> data;

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
    void set_int ( const GLint v0 );
    void set_uint ( const GLuint v0 );
    void set_float ( const GLfloat v0 );
    void set_vector ( const math::fvec2& v0 );
    void set_vector ( const math::fvec3& v0 );
    void set_vector ( const math::fvec4& v0 );
    void set_matrix ( const math::fmat2& v0 );
    void set_matrix ( const math::fmat2x3& v0 );
    void set_matrix ( const math::fmat2x4& v0 );
    void set_matrix ( const math::fmat3x2& v0 );
    void set_matrix ( const math::fmat3& v0 );
    void set_matrix ( const math::fmat3x4& v0 );
    void set_matrix ( const math::fmat4x2& v0 );
    void set_matrix ( const math::fmat4x3& v0 );
    void set_matrix ( const math::fmat4& v0 );



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



    /* is_set_valid
     *
     * returnd true if it is safe to set values
     */
    bool is_set_valid () const;



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

    /* default destructor */
    ~program () = default;



    /* get_bound_object_pointer
     *
     * produce a pointer to the program currently bound
     */
    using object::get_bound_object_pointer;
    static object_pointer<program> get_bound_object_pointer () { return get_bound_object_pointer<program> ( object_bind_target::GLH_PROGRAM_TARGET ); }




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

    /* set_uniform_block_binding
     *
     * bind a uniform block to an index
     *
     * block_index/name: the index/name of the uniform block
     * bp_index: the index of the bind point
     * 
     * returns true if a change to the binding occured
     */
    bool set_uniform_block_binding ( const GLuint block_index, const GLuint bp_index ) const;
    bool set_uniform_block_binding ( const std::string& block_name, const GLuint bp_index ) const;

    /* get_uniform_block_binding
     *
     * returns the index of the bind point a uniform block is bound to
     * -1 if not bound
     * 
     * block_index/name: the index/name of the block to get the bind point of
     */
    GLint get_uniform_block_binding ( const GLuint block_index ) const;
    GLint get_uniform_block_binding ( const std::string& block_name ) const;



    /* use
     *
     * use the shader program for the following OpenGL function calls
     * will not call glUseProgram if already in use
     * same as bind method
     */
    bool use () const { return bind (); }

    /* is_in_use
     *
     * same as is_bound method
     * 
     * return: boolean for if the program is in use
     */
    bool is_in_use () const { return is_bound (); }



private:

    /* uniform_locations
     * uniform_indices
     * uniform_block_indices
     *
     * maps to the locations/indices of uniforms and uniform blocks based on their full names
     * saves some time on repeated queries
     */
    mutable std::map<std::string, GLint> uniform_locations;
    mutable std::map<std::string, GLuint> uniform_indices;
    mutable std::map<std::string, GLuint> uniform_block_indices;

    /* uniform_block_bindings
     *
     * vector mapping uniform block indices to their current bind boints
     * -1 if not mapped
     */
    mutable std::vector<GLint> uniform_block_bindings;

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



/* UNIFORM_ALIGNED_VECTOR IMPLEMENTATION */

/* retype constructor
 *
 * construct from a (uniform aligned) vector of the same size but different type
 */
template<unsigned M, class T>
template<class _T> glh::math::uniform_aligned_vector<M, T>::uniform_aligned_vector ( const uniform_aligned_vector<M, _T>& other )
{
    /* copy each value from other to this */
    for ( unsigned i = 0; i < M; ++i ) data.at ( i ) = static_cast<T> ( other.data.at ( i ) );
}
template<unsigned M, class T>
template<class _T> glh::math::uniform_aligned_vector<M, T>::uniform_aligned_vector ( const vector<M, _T>& other )
{
    /* copy each value from other to this */
    for ( unsigned i = 0; i < M; ++i ) data.at ( i ) = static_cast<T> ( other.at ( i ) );
}

/* implicitly casts to vector of the same type and size */
template<unsigned M, class T> glh::math::uniform_aligned_vector<M, T>::operator vector<M, T> () const
{
    /* copy values into vector and return it */
    vector<M, T> other;
    for ( unsigned i = 0; i < M; ++i ) other.at ( i ) = data.at ( i );
    return other;
}



/* UNIFORM_ALIGNED_MATRIX IMPLEMENTATION */

/* retype constructor
 *
 * construct from a (uniform aligned) vector of the same size but different type
 */
template<unsigned M, unsigned N, class T>
template<class _T> glh::math::uniform_aligned_matrix<M, N, T>::uniform_aligned_matrix ( const uniform_aligned_matrix<M, N, _T>& other )
{
    /* iterate over collumns and set the values in them */
    for ( unsigned i = 0; i < N; ++i ) for ( unsigned j = 0; j < M; ++j ) data.at ( ( i * 4 ) + j ) = static_cast<T> ( other.data.at ( ( i * 4 ) + j ) );
}
template<unsigned M, unsigned N, class T>
template<class _T> glh::math::uniform_aligned_matrix<M, N, T>::uniform_aligned_matrix ( const matrix<M, N, _T>& other )
{
    /* iterate over collumns and set the values in them */
    for ( unsigned i = 0; i < N; ++i ) for ( unsigned j = 0; j < M; ++j ) data.at ( ( i * 4 ) + j ) = static_cast<T> ( other.at ( j, i ) );
}

/* implicitly casts to matrix of the same type and size */
template<unsigned M, unsigned N, class T> glh::math::uniform_aligned_matrix<M, N, T>::operator matrix<M, N, T> () const
{
    /* copy values into matrix and return it */
    matrix<M, N, T> other;
    for ( unsigned i = 0; i < N; ++i ) for ( unsigned j = 0; j < M; ++j ) other.at ( j, i ) = data.at ( ( i * 4 ) + j );
    return other;
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



/* #ifndef GLHELPER_SHADER_HPP_INCLUDED */
#endif