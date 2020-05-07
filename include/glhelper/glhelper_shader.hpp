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
 * CLASS GLH::CORE::BLOCK_UNIFORM
 * 
 * derivation of uniform class to represent a uniform in a uniform block of a shader
 * modifying this uniform modifies the UBO currently associated with that uniform block
 * 
 * 
 * 
 * CLASS GLH::CORE::BLOCK_ARRAY_UNIFORM
 * 
 * derivation of array_uniform to represent an array uniform in a uniform block of a shader
 * the at method allows for elements of the array to be accessed
 * the type of uniform returned is based on the template parameter of the block_array_uniform class
 * 
 * 
 * 
 * CLASS GLH::CORE::BLOCK_STRUCT_UNIFORM
 * 
 * derivation of struct_uniform to represent a struct uniform in a uniform block of a shader
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



        /* class uniform
         *
         * base class for an endpoint uniform
         */
        class uniform;

        /* class array_uniform
         *
         * a template base class to represent an array of uniforms
         */
        template<class T> class array_uniform;

        /* class struct_uniform
         *
         * base class for structure uniforms
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



        /* class uniform_storage : uniform_storage
         *
         * stores hard copies of derived uniform types
         */
        template<class T> class uniform_storage;



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
         * simplify common types of pure array uniforms a bit
         */
        using pure_uniform_array_uniform = pure_array_uniform<pure_uniform>;
        using pure_struct_array_uniform = pure_array_uniform<pure_struct_uniform>;
        using pure_uniform_2d_array_uniform = pure_array_uniform<pure_array_uniform<pure_uniform>>;
        using pure_struct_2d_array_uniform = pure_array_uniform<pure_array_uniform<pure_struct_uniform>>;



        /* class block_uniform : uniform
         *
         * derivation of uniform class for block uniforms in shaders
         */
        class block_uniform;

        /* class block_array_uniform : array_uniform
         *
         * derivation of array_uniform class for block array uniforms in shaders
         */
        template<class T> class block_array_uniform;

        /* class block_struct_uniform : struct_uniform
         *
         * derivation of struct_uniform class for block struct uniforms in shaders
         */
        class block_struct_uniform;

        /* using declaration for block array uniform types
         *
         * simplify common types of block array uniforms a bit
         */
        using block_uniform_array_uniform = block_array_uniform<block_uniform>;
        using block_struct_array_uniform = block_array_uniform<block_struct_uniform>;
        using block_uniform_2d_array_uniform = block_array_uniform<block_array_uniform<block_uniform>>;
        using block_struct_2d_array_uniform = block_array_uniform<block_array_uniform<block_struct_uniform>>;



        /* class program : object
         *
         * shader program
         */
        class program;

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

        /* struct is_uniform_pure
         *
         * is_uniform_pure::value is true if the type is a pure uniform
         */
        template<class T> struct is_uniform_pure;

        /* struct is_uniform_block
         *
         * is_uniform_block::value is true if the type is a block uniform
         */
        template<class T> struct is_uniform_block;
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

/* specialisations for struct uniforms, as they need to be known as inheriting from struct_uniform before their definition */
template<> struct glh::meta::aquire_uniform_base<glh::core::pure_struct_uniform>
{
    typedef core::struct_uniform type;
};
template<> struct glh::meta::aquire_uniform_base<glh::core::block_struct_uniform>
{
    typedef core::struct_uniform type;
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

/* specialisations for struct uniforms, as they need to be known as derived before their definition */ 
template<> struct glh::meta::is_uniform_derived<glh::core::pure_struct_uniform> : std::true_type {};
template<> struct glh::meta::is_uniform_derived<glh::core::block_struct_uniform> : std::true_type {};




/* IS_UNIFORM_PURE DEFINITION */

/* struct is_uniform_pure
 *
 * is_uniform_pure::value is true if the type is a pure uniform
 */
template<class T> struct glh::meta::is_uniform_pure : std::false_type {};
template<> struct glh::meta::is_uniform_pure<glh::core::pure_uniform> : std::true_type {};
template<> struct glh::meta::is_uniform_pure<glh::core::pure_struct_uniform> : std::true_type {};
template<> template<class _T> struct glh::meta::is_uniform_pure<glh::core::pure_array_uniform<_T>> : glh::meta::is_uniform_pure<_T> {};



/* IS_UNIFORM_BLOCK DEFINITION */

/* struct is_uniform_block
 *
 * is_uniform_block::value is true if the type is a block uniform
 */
template<class T> struct glh::meta::is_uniform_block : std::false_type {};
template<> struct glh::meta::is_uniform_block<glh::core::block_uniform> : std::true_type {};
template<> struct glh::meta::is_uniform_block<glh::core::block_struct_uniform> : std::true_type {};
template<> template<class _T> struct glh::meta::is_uniform_block<glh::core::block_array_uniform<_T>> : glh::meta::is_uniform_block<_T> {};



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
     * _prog: the program the uniform is associated with
     */
    uniform ( const std::string& _name, program& _prog );

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
     * returnd true if it is save to set values
     */
    virtual bool is_set_valid () const = 0;

    /* assert_is_set_valid
     *
     * throws if the uniform is not set valid
     * 
     * operation: description of the operation
     */
    virtual void assert_is_set_valid ( const std::string& operation = "" ) const = 0;



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
     * _prog: the program ths uniform is associated with 
     */
    array_uniform ( const std::string& _name, program& _prog )
        : name { _name }
        , prog { _prog }
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

};




/* UNIFORM_STORAGE DEFINITION */

/* class uniform_storage : uniform_storage
 *
 * stores hard copies of derived uniform types
 */
template<class T> class glh::core::uniform_storage
{

    /* static_assert that T is a derived uniform type */
    static_assert ( meta::is_uniform_derived<T>::value, "class uniform_storage must have a template parameter of a derived uniform type" );

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
     * _prog: the program associated with the uniform
     */
    pure_uniform ( const std::string& _name, program& _prog )
        : uniform { _name, _prog }
    {}

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
        { assert_is_program_in_use ( "set float" ); glUniform1f ( location, v0 ); }
    void set_float ( const GLfloat v0, const GLfloat v1 )  const
        { assert_is_program_in_use ( "set float" ); glUniform2f ( location, v0, v1 ); }
    void set_float ( const GLfloat v0, const GLfloat v1, const GLfloat v2 ) const
        { assert_is_program_in_use ( "set float" ); glUniform3f ( location, v0, v1, v2 ); }
    void set_float ( const GLfloat v0, const GLfloat v1, const GLfloat v2, const GLfloat v3 ) const
        { assert_is_program_in_use ( "set float" ); glUniform4f ( location, v0, v1, v2, v3 ); }

    /* set_int
     *
     * set uniform based on integer value(s)
     * program must be already in use
     * 
     * v0...: the value(s) to set the uniform to
     */
    void set_int ( const GLint v0 ) const
        { assert_is_program_in_use ( "set int" ); glUniform1i ( location, v0 ); }
    void set_int ( const GLint v0, const GLint v1 ) const
        { assert_is_program_in_use ( "set int" ); glUniform2i ( location, v0, v1 ); }
    void set_int ( const GLint v0, const GLint v1, const GLint v2 ) const
        { assert_is_program_in_use ( "set int" ); glUniform3i ( location, v0, v1, v2 ); }
    void set_int ( const GLint v0, const GLint v1, const GLint v2, const GLint v3 ) const
        { assert_is_program_in_use ( "set int" ); glUniform4i ( location, v0, v1, v2, v3 ); }

    /* set_uint
     *
     * set uniform based on unsigned integer value(s)
     * program must be already in use
     * 
     * v0...: the value(s) to set the uniform to
     */
    void set_uint ( const GLuint v0 ) const
        { assert_is_program_in_use ( "set uint" ); glUniform1ui ( location, v0 ); }
    void set_uint ( const GLuint v0, const GLuint v1 ) const
        { assert_is_program_in_use ( "set uint" ); glUniform2ui ( location, v0, v1 ); }
    void set_uint ( const GLuint v0, const GLuint v1, const GLuint v2 ) const
        { assert_is_program_in_use ( "set uint" ); glUniform3ui ( location, v0, v1, v2 ); }
    void set_uint ( const GLuint v0, const GLuint v1, const GLuint v2, const GLuint v3 ) const
        { assert_is_program_in_use ( "set uint" ); glUniform4ui ( location, v0, v1, v2, v3 ); }

    /* set_matrix
     *
     * set uniform based on a matrix
     * program must be already in use
     * 
     * name: the name of the uniform
     * v0: the matrix to set the uniform to
     */
    void set_matrix ( const glh::math::mat2& v0 ) const
        { assert_is_program_in_use ( "set mat2" ); glUniformMatrix2fv ( location, 1, GL_FALSE, v0.export_data ().data () ); }
    void set_matrix ( const glh::math::mat3& v0 ) const
        { assert_is_program_in_use ( "set mat3" ); glUniformMatrix3fv ( location, 1, GL_FALSE, v0.export_data ().data () ); }
    void set_matrix ( const glh::math::mat4& v0 ) const
        { assert_is_program_in_use ( "set mat4" ); glUniformMatrix4fv ( location, 1, GL_FALSE, v0.export_data ().data () ); }

    /* set_vector
     *
     * set uniform based on a vector
     * the same as the set_float functions, except the parameters are packed into a vector
     * program must be already in use
     * 
     * v0: the vector to set the uniform to
     */
    void set_vector ( const glh::math::vec1& v0 ) const
        { assert_is_program_in_use ( "set vec1" ); glUniform1f ( location, v0.at ( 0 ) ); }
    void set_vector ( const glh::math::vec2& v0 ) const
        { assert_is_program_in_use ( "set vec2" ); glUniform2f ( location, v0.at ( 0 ), v0.at ( 1 ) ); }
    void set_vector ( const glh::math::vec3& v0 ) const
        { assert_is_program_in_use ( "set vec3" ); glUniform3f ( location, v0.at ( 0 ), v0.at ( 1 ), v0.at ( 2 ) ); }
    void set_vector ( const glh::math::vec4& v0 ) const
        { assert_is_program_in_use ( "set vec4" ); glUniform4f ( location, v0.at ( 0 ), v0.at ( 1 ), v0.at ( 2 ), v0.at ( 3 ) ); }


    

    /* is_set_valid
     *
     * a pure uniform is always set valid, so return true
     */
    bool is_set_valid () const { return true; }

    /* assert_is_set_valid
     *
     * does nothing, as is always set valid
     * this is required due to pure virtial method in base
     */
    void assert_is_set_valid ( const std::string& operation = "" ) const {}

};



/* PURE_ARRAY_UNIFORM DEFINITION */

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
        : array_uniform<typename meta::aquire_uniform_base<T>::type> { _name, _prog }
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

    /* storage of uniforms in the array */
    uniform_storage<T> uniforms;

};



/* PURE_STRUCT_UNIFORM DEFINITION */

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
        : struct_uniform { _name, _prog }
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

    /* (..._)uniforms
     *
     * storage of uniforms in the program
     */
    uniform_storage<pure_uniform> uniforms;
    uniform_storage<pure_struct_uniform> struct_uniforms;
    uniform_storage<pure_uniform_array_uniform> uniform_array_uniforms;
    uniform_storage<pure_struct_array_uniform> struct_array_uniforms;
    uniform_storage<pure_uniform_2d_array_uniform> uniform_2d_array_uniforms;
    uniform_storage<pure_struct_2d_array_uniform> struct_2d_array_uniforms;

};



/* BLOCK_UNIFORM DEFINITION */

/* class block_uniform : uniform
 *
 * derivation of uniform class for block uniforms in shaders
 */
class glh::core::block_uniform : public uniform
{
public:

    /* constructor
     *
     * construct from name, block name, block index and containing program
     *
     * _prefix: the prefix for any member uniforms
     * _prog: the program the uniform storage is associated with
     */
    block_uniform ( const std::string& _name, program& _prog )
        : uniform { _name, _prog }
    {}

    /* deleted zero-parameter constructor */
    block_uniform () = delete;

    /* default copy constructor */
    block_uniform ( const block_uniform& other ) = default;

    /* deleted copy assignment operator */
    block_uniform& operator= ( const block_uniform& other ) = delete;

    /* default destructor */
    ~block_uniform () = default;



    /* set_float
     *
     * set uniform based on float value(s)
     * 
     * v0...: the value(s) to set the uniform to
     */
    void set_float ( const GLfloat v0 ) const;
    void set_float ( const GLfloat v0, const GLfloat v1 )  const;
    void set_float ( const GLfloat v0, const GLfloat v1, const GLfloat v2 ) const;
    void set_float ( const GLfloat v0, const GLfloat v1, const GLfloat v2, const GLfloat v3 ) const;
    
    /* set_int
     *
     * set uniform based on integer value(s)
     * 
     * v0...: the value(s) to set the uniform to
     */
    void set_int ( const GLint v0 ) const;
    void set_int ( const GLint v0, const GLint v1 ) const;
    void set_int ( const GLint v0, const GLint v1, const GLint v2 ) const;
    void set_int ( const GLint v0, const GLint v1, const GLint v2, const GLint v3 ) const;

    /* set_uint
     *
     * set uniform based on unsigned integer value(s)
     * 
     * v0...: the value(s) to set the uniform to
     */
    void set_uint ( const GLuint v0 ) const;
    void set_uint ( const GLuint v0, const GLuint v1 ) const;
    void set_uint ( const GLuint v0, const GLuint v1, const GLuint v2 ) const;
    void set_uint ( const GLuint v0, const GLuint v1, const GLuint v2, const GLuint v3 ) const;

    /* set_matrix
     *
     * set uniform based on a matrix
     * 
     * name: the name of the uniform
     * v0: the matrix to set the uniform to
     */
    void set_matrix ( const glh::math::mat2& v0 ) const;
    void set_matrix ( const glh::math::mat3& v0 ) const;
    void set_matrix ( const glh::math::mat4& v0 ) const;

    /* set_vector
     *
     * set uniform based on a vector
     * the same as the set_float functions, except the parameters are packed into a vector
     * 
     * v0: the vector to set the uniform to
     */
    void set_vector ( const glh::math::vec1& v0 ) const;
    void set_vector ( const glh::math::vec2& v0 ) const;
    void set_vector ( const glh::math::vec3& v0 ) const;
    void set_vector ( const glh::math::vec4& v0 ) const;


    

    /* is_set_valid
     *
     * returns true if is able to set the uniform value
     */
    bool is_set_valid () const;

    /* assert_is_set_valid
     *
     * throws if is not able to set uniform value
     */
    void assert_is_set_valid ( const std::string& operation = "" ) const;

};



/* BLOCK_ARRAY_UNIFORM DEFINITION */

/* class block_array_uniform : array_uniform
 *
 * derivation of array_uniform class for block array uniforms in shaders
 */
template<class T> class glh::core::block_array_uniform
    : public array_uniform<typename meta::aquire_uniform_base<T>::type>
{

    /* static assert that T is a boock uniform */
    static_assert ( meta::is_uniform_block<T>::value, "class block_array_uniform must have a template paramater of a block uniform type" );

public:

    /* constructor
     *
     * construct from name, block name, block index and containing program
     *
     * _prefix: the prefix for any member uniforms
     * _prog: the program the uniform storage is associated with
     */
    block_array_uniform ( const std::string& _name, program& _prog )
        : array_uniform<typename meta::aquire_uniform_base<T>::type> { _name, _prog }
        , uniforms { _name, _prog }
    {}

    /* deleted zero-parameter constructor */
    block_array_uniform () = delete;

    /* default copy constructor */
    block_array_uniform ( const block_array_uniform& other ) = default;

    /* deleted copy assignment operator */
    block_array_uniform& operator= ( const block_array_uniform& other ) = delete;

    /* default destructor */
    ~block_array_uniform () = default;



    /* typedef of T */
    typedef T type;



    /* at/operator[]
     *
     * return the uniform at an index
     */
    typename meta::aquire_uniform_base<T>::type& at ( const unsigned i ) { return uniforms.get ( "[" + std::to_string ( i ) + "]" ); }
    const typename meta::aquire_uniform_base<T>::type& at ( const unsigned i ) const { return uniforms.get ( "[" + std::to_string ( i ) + "]" ); }



private:

    /* storage of uniforms in the array */
    uniform_storage<T> uniforms;

};



/* BlOCK_STRUCT_UNIFORM DEFINITION */

/* class block_struct_uniform : struct_uniform
 *
 * derivation of struct_uniform class for block struct uniforms in shaders
 */
class glh::core::block_struct_uniform : public struct_uniform
{
public:

    /* constructor
     *
     * construct from name, block name, block index and containing program
     *
     * _prefix: the prefix for any member uniforms
     * _prog: the program the uniform storage is associated with
     */
    block_struct_uniform ( const std::string& _name, program& _prog )
        : struct_uniform { _name, _prog }
        , uniforms { _name + ".", _prog }, struct_uniforms { _name + ".", _prog }
        , uniform_array_uniforms { _name + ".", _prog }, struct_array_uniforms { _name + ".", _prog }
        , uniform_2d_array_uniforms { _name + ".", _prog }, struct_2d_array_uniforms { _name + ".", _prog }
    {}

    /* deleted zero-parameter constructor */
    block_struct_uniform () = delete;

    /* default copy constructor */
    block_struct_uniform ( const block_struct_uniform& other ) = default;

    /* deleted copy assignment operator */
    block_struct_uniform& operator= ( const block_struct_uniform& other ) = delete;

    /* default destructor */
    ~block_struct_uniform () = default;


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

    /* (..._)uniforms
     *
     * storage of uniforms in the program
     */
    uniform_storage<block_uniform> uniforms;
    uniform_storage<block_struct_uniform> struct_uniforms;
    uniform_storage<block_uniform_array_uniform> uniform_array_uniforms;
    uniform_storage<block_struct_array_uniform> struct_array_uniforms;
    uniform_storage<block_uniform_2d_array_uniform> uniform_2d_array_uniforms;
    uniform_storage<block_struct_2d_array_uniform> struct_2d_array_uniforms;

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



    /* get_pure_(struct_)uniform
     *
     * return a pure uniform object based on a name
     * 
     * name: the name of the uniform
     */
    uniform& get_pure_uniform ( const std::string& name ) { return pure_uniforms.get ( name ); }
    const uniform& get_pure_uniform ( const std::string& name ) const { return pure_uniforms.get ( name ); }
    struct_uniform& get_pure_struct_uniform ( const std::string& name ) { return pure_struct_uniforms.get ( name ); }
    const struct_uniform& get_pure_struct_uniform ( const std::string& name ) const { return pure_struct_uniforms.get ( name ); }
    
    /* get_pure_..._array_uniform
     *
     * return a pure uniform array object based on a name
     * 
     * name: the name of the uniform
     */
    uniform_array_uniform& get_pure_uniform_array_uniform ( const std::string& name ) { return pure_uniform_array_uniforms.get ( name ); }
    const uniform_array_uniform& get_pure_uniform_array_uniform ( const std::string& name ) const { return pure_uniform_array_uniforms.get ( name ); }
    struct_array_uniform& get_pure_struct_array_uniform ( const std::string& name ) { return pure_struct_array_uniforms.get ( name ); }
    const struct_array_uniform& get_pure_struct_array_uniform ( const std::string& name ) const { return pure_struct_array_uniforms.get ( name ); }
    uniform_2d_array_uniform& get_pure_uniform_2d_array_uniform ( const std::string& name ) { return pure_uniform_2d_array_uniforms.get ( name ); }
    const uniform_2d_array_uniform& get_pure_uniform_2d_array_uniform ( const std::string& name ) const { return pure_uniform_2d_array_uniforms.get ( name ); }
    struct_2d_array_uniform& get_pure_struct_2d_array_uniform ( const std::string& name ) { return pure_struct_2d_array_uniforms.get ( name ); }
    const struct_2d_array_uniform& get_pure_struct_2d_array_uniform ( const std::string& name ) const { return pure_struct_2d_array_uniforms.get ( name ); }



    /* get_block_(struct_)uniform
     *
     * return a pure uniform object based on a name
     * 
     * name: the name of the uniform
     */
    uniform& get_block_uniform ( const std::string& name ) { return block_uniforms.get ( name ); }
    const uniform& get_block_uniform ( const std::string& name ) const { return block_uniforms.get ( name ); }
    struct_uniform& get_block_struct_uniform ( const std::string& name ) { return block_struct_uniforms.get ( name ); }
    const struct_uniform& get_block_struct_uniform ( const std::string& name ) const { return block_struct_uniforms.get ( name ); }
    
    /* get_block_..._array_uniform
     *
     * return a pure uniform array object based on a name
     * 
     * name: the name of the uniform
     */
    uniform_array_uniform& get_block_uniform_array_uniform ( const std::string& name ) { return block_uniform_array_uniforms.get ( name ); }
    const uniform_array_uniform& get_block_uniform_array_uniform ( const std::string& name ) const { return block_uniform_array_uniforms.get ( name ); }
    struct_array_uniform& get_block_struct_array_uniform ( const std::string& name ) { return block_struct_array_uniforms.get ( name ); }
    const struct_array_uniform& get_block_struct_array_uniform ( const std::string& name ) const { return block_struct_array_uniforms.get ( name ); }
    uniform_2d_array_uniform& get_block_uniform_2d_array_uniform ( const std::string& name ) { return block_uniform_2d_array_uniforms.get ( name ); }
    const uniform_2d_array_uniform& get_block_uniform_2d_array_uniform ( const std::string& name ) const { return block_uniform_2d_array_uniforms.get ( name ); }
    struct_2d_array_uniform& get_block_struct_2d_array_uniform ( const std::string& name ) { return block_struct_2d_array_uniforms.get ( name ); }




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
     * storage of pure uniforms in the program
     */
    uniform_storage<pure_uniform> pure_uniforms;
    uniform_storage<pure_struct_uniform> pure_struct_uniforms;
    uniform_storage<pure_uniform_array_uniform> pure_uniform_array_uniforms;
    uniform_storage<pure_struct_array_uniform> pure_struct_array_uniforms;
    uniform_storage<pure_uniform_2d_array_uniform> pure_uniform_2d_array_uniforms;
    uniform_storage<pure_struct_2d_array_uniform> pure_struct_2d_array_uniforms;

    /* block_(..._)uniforms
     *
     * storage of block uniforms in the program
     */
    uniform_storage<block_uniform> block_uniforms;
    uniform_storage<block_struct_uniform> block_struct_uniforms;
    uniform_storage<block_uniform_array_uniform> block_uniform_array_uniforms;
    uniform_storage<block_struct_array_uniform> block_struct_array_uniforms;
    uniform_storage<block_uniform_2d_array_uniform> block_uniform_2d_array_uniforms;
    uniform_storage<block_struct_2d_array_uniform> block_struct_2d_array_uniforms;

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



/* PURE_UNIFORM_STORAGE IMPLEMENTATION */

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