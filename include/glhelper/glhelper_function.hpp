/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * include/glhelper/glhelper_function.hpp
 * 
 * constructs for representing/using mathematical functions
 * 
 * notable constructs include:
 * 
 * 
 * 
 * CLASS GLH::MATH::GENERIC_FUNCTION
 * 
 * a base class for any type of no-input, single, or multi-variate function
 * it can be called using the overloaded oeprator() or through the apply() function
 * 
 * 
 * 
 * CLASS GLH::MATH::PASSTHROUGH_FUNCTION
 * 
 * a function which does nothing but return it's single input
 * 
 * 
 * 
 * CLASS GLH::MATH::SUMMATION_FUNCTION
 * 
 * a function which takes any number of inputs and returns the sum
 * 
 * 
 * 
 * CLASS GLH::MATH::BINOMIAL_FUNCTION
 * 
 * a single-variable function which stores the coefficients for positive and negative powers of x
 * the sum of all of these terms are then returned
 * 
 * 
 * 
 * CLASS GLH::MATH::GAUSSIAN_FUNCTION
 * 
 * a gaussian function or bell curve
 * 
 * 
 * 
 * CLASS GLH::MATH::GLSL_FUNCTION
 * 
 * allows the transfer of a function shape to be used in glsl
 * this is done by sampling the function a certain number of times, and storing the results in a texture
 * the values can then be linearly interpolated between, in order to aquire the rough shape of the function
 * the resolution of the texture can be chosen by the user - higher resolution means more accurate, but with more memory usage
 * the glsl struct which this class interfaces with is shown below:
 * 
 * 
 * 
 * GLSL STRUCT FUNCTION_STRUCT
 * 
 * struct function_struct_nD
 * {
 *     samplerBuffer outputs;
 *     
 *     float domain_multiplier;
 *     float domain_translation;
 * }
 *
 * where 1 <= n <= 3
 * 
 * outputs: a sampler for the outputs of the function
 * domain_mult/domain_trans: together form a linear transformations, such that:
 *     the lower bound of the domain will transform to the integer 0
 *     the upper bound of the domain will transform to the integer 1
 * 
 */



/* HEADER GUARD */
#ifndef GLHELPER_FUNCTION_HPP_INCLUDED
#define GLHELPER_FUNCTION_HPP_INCLUDED



/* INCLUDES */

/* include core headers */
#include <cmath>
#include <functional>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <tuple>
#include <type_traits>

/* include glhelper_core.hpp */
#include <glhelper/glhelper_core.hpp>

/* include glhelper_texture.hpp */
#include <glhelper/glhelper_texture.hpp>

/* include glhelper_shader.hpp */
#include <glhelper/glhelper_shader.hpp>



/* NAMESPACE DECLARATIONS */

namespace glh
{
    namespace function
    {
        /* class generic_function
         *
         * a base class for any no-input, single or multivariate function
         * 
         * Rt: the return type of the function
         * Ps: the parameter type(s) of the function
         */
        template<class Rt, class ...Ps> class generic_function;

        /* class passthrough_function
         *
         * a simple single-variable function, which takes one input and returns it
         * 
         * P: the parameter type and return type
         */
        template<class P> class passthrough_function;

        /* class summation_function
         *
         * a simple multi-variate function, which takes multiple inputs and finds the sum
         * 
         * Rt: the return type of the function
         * Ps: the parameter type(s) of the function
         */
        template<class Rt, class ...Ps> class summation_function;

        /* class binomial_function : generic_function
         *
         * a simple single-variable binomial function in the form ax^n + bx^(n+1)..., where n can be any real constant
         * 
         * Rt: the return type of the function
         * P: the parameter type of the function (the type of x)
         * K: the constant type of the function (the type of a,b,...)
         */
        template<class Rt, class P, class K = P> class binomial_function;

        /* class gaussian_function : generic_function
         *
         * a single-variable gaussian function in the form  a exp ( -( x - b )^2 / 2c^2 )
         *
         * Rt: the return type of the function
         * P: the parameter type of the function (the type of x)
         * K: the constant type of the function (the type of a,b,c)
         */
        template<class Rt, class P, class K = P> class gaussian_function;

        /* class glsl_function
         *
         * class to handle the transfer of a function's output to glsl
         * 
         * D: the dimension of the function (no. of inputs)
         */
        template<unsigned D> class glsl_function;
    }

    namespace meta
    {
        /* struct is_function
         *
         * value will be true if the type supplied is a function
         */
        template<class T, class = void> struct is_function;
    }
}



/* IS_FUNCTION DEFINITION */

/* struct is_function
 *
 * value will be true if the type supplied is a function
 */
template<class T, class> struct glh::meta::is_function : std::false_type {};
template<class Rt, class ...Ps> template<class T> 
struct glh::meta::is_function<T, std::enable_if_t<std::is_base_of<glh::function::generic_function<Rt, Ps...>, T>::value>> : std::true_type {};




/* GENERIC_FUNCTION DEFINITION */

/* class generic_function
 *
 * a base class for any no-input, single or multivariate function
 * 
 * Rt: the return type of the function
 * Ps: the parameter type(s) of the function
 */
template<class Rt, class ...Ps> class glh::function::generic_function
{
public:

    /* no user-defined constructors or destructors, just pure virtual methods */

    /* allow access to the types of the function */
    using return_type = Rt;
    static const unsigned param_count = sizeof...( Ps );

    /* pure virtual apply and operator()
     *
     * apply the function to some input parameters
     */
    virtual Rt apply ( const Ps&... ps ) const = 0;
    virtual Rt operator() ( const Ps&... ps ) const { return apply ( ps... ); }

    /* duplicate
     *
     * copy the function onto the heap and return a base class pointer to it
     */
    virtual generic_function * duplicate () const = 0;

};



/* PASSTHROUGH_FUNCTION DEFINITION */

/* class passthrough_function
*
* a simple single-variable function, which takes one input and returns it
* 
* P: the parameter type and return type
*/
template<class P> class glh::function::passthrough_function : public generic_function<P, P>
{
public:

    /* default everuthing */
    passthrough_function () = default;
    passthrough_function ( const passthrough_function& other ) = default;
    passthrough_function& operator= ( const passthrough_function& other ) = default;
    ~passthrough_function () = default;

    
    
    /* apply method */
    P apply ( const P& p ) const { return p; }

    /* duplicate method */
    generic_function<P, P> * duplicate () const { return new passthrough_function { * this }; }

};



/* SUMMATION_FUNCTION DEFINITION */

/* class summation_function
 *
 * a simple multi-variate function, which takes multiple inputs and finds the sum
 * 
 * Rt: the return type of the function
 * Ps: the parameter type(s) of the function
 */
template<class Rt, class ...Ps> class glh::function::summation_function : public generic_function<Rt, Ps...>
{

    /* assert that all of Rt and Ps... are arithmetic */
    static_assert ( std::is_arithmetic<Rt>::value && std::conjunction<std::is_arithmetic<Ps>...>::value, "summation function cannot be formed of non-arithmetic types" );

public:

    /* default everuthing */
    summation_function () = default;
    summation_function ( const summation_function& other ) = default;
    summation_function& operator= ( const summation_function& other ) = default;
    ~summation_function () = default;



    /* apply method */
    Rt apply ( const Ps&... p ) const { return __apply ( p... ); }

    /* duplicate method */
    generic_function<Rt, Ps...> * duplicate () const { return new summation_function { * this }; }



private:

    /* __apply
     *
     * helper method for apply
     */
    template<class P_, class ...Ps_> Rt __apply ( const P_& p, const Ps_&... ps ) const { return p + __apply ( ps... ); }
    Rt __apply () const { return Rt { 0 }; }

};



/* BINOMIAL FUNCTION DEFINITION */

/* class binomial_function : generic_function
 *
 * a simple single-variable binomial function in the form ax^n + bx^(n+1)..., where n can be any real constant
 * 
 * Rt: the return type of the function
 * P: the parameter type of the function (the type of x)
 * K: the constant type of the function (the type of a,b,...)
 */
template<class Rt, class P, class K> class glh::function::binomial_function : public generic_function<Rt, P>
{

    /* assert that all of Rt, P and K are arithmetic */
    static_assert ( std::is_arithmetic<Rt>::value && std::is_arithmetic<P>::value && std::is_arithmetic<K>::value, "binomial function cannot be formed of non-arithmetic types" );

public:

    /* default zero-parameter constructor */
    binomial_function () = default;

    /* full constructor
     *
     * the parameters supplied are the constants for a binomial in ascending order starting at n
     */
    template<class ...Ks> binomial_function ( const int n, const K& k, const Ks&... ks )
        : binomial_function { n + 1, ks... }
    { constants.insert ( std::make_pair ( n, k ) ); }

    /* do-nothing constructor, such that full constructor stops its recursion */
    binomial_function ( const int n ) {};

    /* default copy constructor */
    binomial_function ( const binomial_function& other ) = default;

    /* default copy assignment operator */
    binomial_function& operator= ( const binomial_function& other ) = default;

    /* default destructor */
    ~binomial_function () = default;



    /* apply method */
    Rt apply ( const P& p ) const;

    /* duplicate method */
    generic_function<Rt, P> * duplicate () const { return new binomial_function { * this }; }



    /* get/set_constant
     *
     * get/set the constant at the specified power of x
     */
    K get_constant ( const int n );
    void set_constant ( const int n, const K& k );



private:

    /* map of powers of x to their constants
     * contant assumed to be 0 if not present
     */
    std::map<int, K> constants;

};



/* GAUSSIAN_FUNCTION DEFINITION */

/* class gaussian_function : generic_function
 *
 * a single-variable gaussian function in the form  a exp ( -( x - b )^2 / 2c^2 )
 *
 * Rt: the return type of the function
 * P: the parameter type of the function (the type of x)
 * K: the constant type of the function (the type of a,b,c)
 */
template<class Rt, class P, class K> class glh::function::gaussian_function : public generic_function<Rt, P>
{

    /* assert that all of Rt, P and K are arithmetic */
    static_assert ( std::is_arithmetic<Rt>::value && std::is_arithmetic<P>::value && std::is_arithmetic<K>::value, "gaussian function cannot be formed of non-arithmetic types" );

public:

    /* zero-parameter constructor */
    gaussian_function ()
        : a { 1 }
        , b { 0 }
        , c { 1 }
    {}

    /* full constructor */
    gaussian_function ( const K& _a, const K& _b, const K& _c )
        : a { _a }
        , b { _b }
        , c { _c }
    {}

    /* default copy constructor */
    gaussian_function ( const gaussian_function& other ) = default;

    /* default copy assignment operator */
    gaussian_function& operator= ( const gaussian_function& other ) = default;

    /* default destructor */
    ~gaussian_function () = default;



    /* apply method */
    Rt apply ( const P& p ) const { return a * std::exp ( -( ( p - b ) * ( p - b ) ) / ( 2 * c * c ) ); }

    /* duplicate method */
    generic_function<Rt, P> * duplicate () const { return new gaussian_function { * this }; }



    /* get/set_a/b/c
     *
     * set the constants a, b and c
     */
    const K& get_a () const { return a; };
    const K& get_b () const { return b; };
    const K& get_c () const { return c; };
    void set_a ( const K& _a ) { a = _a; }
    void set_b ( const K& _b ) { a = _b; }
    void set_c ( const K& _c ) { a = _c; }



private:

    /* constants for a, b and c */
    K a;
    K b;
    K c;

};



/* GLSL_FUNCTION DEFINITION */

/* class glsl_function
 *
 * class to handle the transfer of a function's output to glsl
 * 
 * D: the dimension of the function (no. of inputs)
 */
template<> class glh::function::glsl_function<1>
{
public:

    /* reset_function constructor
     * 
     * see reset_function for details
     */
    template<class Rt, class P0,
        std::enable_if_t<std::conjunction<std::is_convertible<Rt, float>, std::is_convertible<P0, float>>::value>...
    > glsl_function ( const generic_function<Rt, P0>& func, const unsigned resolution, const double domain_lower, const double domain_higher )
        : last_resolution { 0 }
        , last_output_internal_format { GL_NONE }
    { reset_function ( func, resolution, domain_lower, domain_higher ); }

    /* deleted zero-parameter constructor */
    glsl_function () = delete;



    /* cache_uniforms
     *
     * function_uni: the function uniform to cache
     */
    void cache_uniforms ( core::struct_uniform& function_uni );

    /* apply
     * 
     * apply the function to the uniforms
     */
    void apply () const;
    void apply ( core::struct_uniform& function_uni );



    /* reset_function
     *
     * takes a generic function and saves its outputs to output_tex
     */
    /* when a scalar is returned */
    template<class Rt, class P0,
        std::enable_if_t<std::conjunction<std::is_convertible<Rt, float>, std::is_convertible<P0, float>>::value>...
    > void reset_function ( const generic_function<Rt, P0>& func, const unsigned resolution, const double domain_lower, const double domain_higher );

    /* when a vector is returned */
    template<unsigned M, class T, class P0,
        std::enable_if_t<( M >= 1 ) && ( M <= 4 )>...,
        std::enable_if_t<std::conjunction<std::is_convertible<T, float>, std::is_convertible<P0, float>>::value>...
    > void reset_function ( const generic_function<math::vector<M, T>, P0>& func, const unsigned resolution, const double domain_lower, const double domain_higher );



    /* clamp_to_edge
     *
     * will cause inputs outside the domain to clamp to the edge
     */
    void clamp_to_edge () { output_tex.set_wrap ( GL_CLAMP_TO_EDGE ); }

    /* repeat
     * 
     * will cause inputs outside the domain to repeat
     * useful for oscilating functions like trig functions
     */
    void repeat () { output_tex.set_wrap ( GL_REPEAT ); }



private:

    /* buffer texture for transfering the outputs to shaders */
    core::texture1d output_tex;

    /* domain muliplier and translation */
    unsigned domain_multiplier;
    unsigned domain_translation;

    /* the settings of the last cached function */
    unsigned last_resolution;
    GLenum last_output_internal_format;



    /* cached uniforms struct */
    struct cached_uniforms_struct
    {
        core::struct_uniform& function_uni;
        core::uniform& outputs_uni;
        core::uniform& domain_multiplier_uni;
        core::uniform& domain_translation_uni;
    };

    /* cached uniforms */
    std::unique_ptr<cached_uniforms_struct> cached_uniforms;
};



/* BINOMIAL_FUNCTION IMPLEMENTATION */

/* apply method */
template<class Rt, class P, class K> 
inline Rt glh::function::binomial_function<Rt, P, K>::apply ( const P& p ) const
{
    /* the value to output */
    Rt out { 0 };

    /* get iterator to power of x 0 */
    auto upper = constants.lower_bound ( 0 );

    /* get reverse iterator to the previous power, which will always be less than 0 */
    auto lower = constants.rbegin (); std::advance ( lower, std::distance ( upper, constants.end () ) );

    /* if upper is x^0, add the constant at x^0 multiplied by p to the 0 to out
     * then add one to upper
     * now both upper and lower refer to one above and one below power of 0, respectively
     */
    if ( upper->first == 0 ) 
    {
        out += upper->second * P { 1 };
        ++upper;
    }

    /* iterate through powers, using x to keep track of powers */
    P x { p };
    for ( unsigned i = 1; upper != constants.end () || lower != constants.rend (); ++i )
    {
        /* if the power is currently the same as upper or lower, add that term */
        if ( upper != constants.end () && upper->first == i )
        {
            out += upper->second * x;
            ++upper;
        }
        if ( lower != constants.rend () && lower->first == -i )
        {
            out += lower->second * ( P { 1 } / x );
            ++lower;
        }

        /* multiply x by p */
        x *= p;
    }

    /* return out */
    return out;
}

/* get/set_constant
 *
 * get/set the constant at the specified power of x
 */
template<class Rt, class P, class K> 
inline K glh::function::binomial_function<Rt, P, K>::get_constant ( const int n )
{
    /* find entry */
    auto it = constants.find ( n );

    /* if not found, return 0 constant */
    if ( it == constants.end () ) return K { 0 };

    /* else return the constant */
    else return it->second;
}
template<class Rt, class P, class K> 
inline void glh::function::binomial_function<Rt, P, K>::set_constant ( const int n, const K& k )
{
    /* find entry */
    auto it = constants.find ( n );

    /* if not found, add the entry */
    if ( it == constants.end () ) constants.insert ( std::make_pair ( n, k ) );

    /* otherwise, set the new value */
    else it->second = k;
}



/* GLSL_FUNCTION<1> IMPLEMENTATION */


/* cache_uniforms
 *
 * function_uni: the function uniform to cache
 */
inline void glh::function::glsl_function<1>::cache_uniforms ( core::struct_uniform& function_uni )
{
    /* if uniforms are not already cached, cache the new ones */
    if ( !cached_uniforms || cached_uniforms->function_uni != function_uni )
    {
        cached_uniforms.reset ( new cached_uniforms_struct
        {
            function_uni,
            function_uni.get_uniform ( "outputs" ),
            function_uni.get_uniform ( "domain_multiplier" ),
            function_uni.get_uniform ( "domain_translation" )
        } );
    }
}

/* apply
 * 
 * apply the function to the uniforms
 */
inline void glh::function::glsl_function<1>::apply () const
{
    /* throw if no uniforms cached */
    if ( !cached_uniforms ) throw exception::uniform_exception { "cannot apply glsl_function without a uniform cache" };

    /* set the values */
    cached_uniforms->outputs_uni.set_int ( output_tex.bind_loop () );
    cached_uniforms->domain_multiplier_uni.set_float ( domain_multiplier );
    cached_uniforms->domain_translation_uni.set_float ( domain_translation );
}
void glh::function::glsl_function<1>::apply ( core::struct_uniform& function_uni )
{
    /* cache uniforms then apply */
    cache_uniforms ( function_uni );
    apply ();
}

/* reset_function
 *
 * takes a generic function and saves its outputs to output_tex
 */
/* when a scalar is returned */
template<class Rt, class P0,
    std::enable_if_t<std::conjunction<std::is_convertible<Rt, float>, std::is_convertible<P0, float>>::value>...
> inline void glh::function::glsl_function<1>::reset_function ( const generic_function<Rt, P0>& func, const unsigned resolution, const double domain_lower, const double domain_higher )
{
    /* reformat the texture if necessary */
    if ( resolution != last_resolution || last_output_internal_format != GL_R16F )
    {
        output_tex.tex_image ( resolution * sizeof ( GLfloat ), GL_R16F, GL_RED, GL_FLOAT );
        last_resolution = resolution; last_output_internal_format = GL_R16F;
    }

    /* set the domain transformation
     * this linear transformation causes [domain_lower-domain_higher]->[0-1]
     */
    domain_multiplier = 1.0 / ( domain_higher - domain_lower );
    domain_translation = -( domain_lower * domain_multiplier );
    
    /* create temporary domain transformation
     * this linear transformation causes [0-resolution]->[domain_lower-domain_higher]
     */
    double temp_domain_multiplier = ( domain_higher - domain_lower ) / resolution;
    double temp_domain_translation = domain_lower;

    /* allocate memory for the raw outputs */
    GLfloat * raw_outputs = new GLfloat [ resolution ];

    /* calculate raw outputs */
    for ( unsigned i = 0; i < resolution; ++i )
        raw_outputs [ i ] = func ( ( i * temp_domain_multiplier ) + temp_domain_translation );        
    
    /* substitute the raw output into the texture */
    output_tex.tex_sub_image ( 0.0, resolution * sizeof ( GLfloat ), GL_RED, GL_FLOAT, raw_outputs );

    /* delete the allocated memory */
    delete[] raw_outputs;
}

/* when a vector is returned */
template<unsigned M, class T, class P0,
    std::enable_if_t<( M >= 1 ) && ( M <= 4 )>...,
    std::enable_if_t<std::conjunction<std::is_convertible<T, float>, std::is_convertible<P0, float>>::value>...
> inline void glh::function::glsl_function<1>::reset_function ( const generic_function<math::vector<M, T>, P0>& func, const unsigned resolution, const double domain_lower, const double domain_higher )
{
    /* get the output format from the value of M */
    const GLenum output_internal_format = ( M == 1 ? GL_R16F : ( M == 2 ? GL_RG16F : ( M == 3 ? GL_RGB16F : GL_RGBA16F ) ) );
    const GLenum output_format = ( M == 1 ? GL_RED : ( M == 2 ? GL_RG : ( M == 3 ? GL_RGB : GL_RGBA ) ) );

    /* reformat the texture if necessary */
    if ( resolution != last_resolution || last_output_internal_format != output_internal_format )
    {
        output_tex.tex_image ( resolution * M * sizeof ( GLfloat ), output_internal_format, output_format, GL_FLOAT );
        last_resolution = resolution; last_output_internal_format = output_internal_format;
    }

    /* set the domain transformation
     * this linear transformation causes [domain_lower-domain_higher]->[0-1]
     */
    domain_multiplier = 1.0 / ( domain_higher - domain_lower );
    domain_translation = -( domain_lower * domain_multiplier );
    
    /* create temporary domain transformation
     * this linear transformation causes [0-resolution]->[domain_lower-domain_higher]
     */
    double temp_domain_multiplier = ( domain_higher - domain_lower ) / resolution;
    double temp_domain_translation = domain_lower;

    /* allocate memory for the raw outputs */
    float * raw_outputs = new math::fvector<M> [ resolution ];

    /* calculate raw outputs */
    for ( unsigned i = 0; i < resolution; ++i )
        raw_outputs [ i ] = math::fvector<M> { func ( ( i * temp_domain_multiplier ) + temp_domain_translation ) };        
    
    /* substitute the raw output into the texture */
    output_tex.tex_sub_image ( 0.0, resolution * M * sizeof ( GLfloat ), output_format, GL_FLOAT, raw_outputs );

    /* delete the allocated memory */
    delete[] raw_outputs;
}





/* #ifndef GLHELPER_FUNCTION_HPP_INCLUDED */
#endif