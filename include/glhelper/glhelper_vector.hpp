/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * include/glhelper/glhelper_vector.hpp
 * 
 * implements vector mathematics
 * notable constructs include:
 * 
 * 
 * 
 * CLASS GLH::MATH::VECTOR
 * 
 * template class to represent a vector of any given size
 * the template parameter M form a vector of size M
 * 
 * 
 * 
 * MATRIX NON-MEMBER FUNCTIONS
 * 
 * non-member functions include (all in namespace glh::math):
 * 
 * OPERATORS*+-/: for matrix-matrix and matrix-scalar combinations
 * CONCATENATE: concatenate two vectors, two floats, or any combination thereof to form one vector
 * DOT: dot/scalar product of two vectors of the same dimension
 * CROSS: cross product of two 3d vectors
 * MODULUS: find the modulus (length) of a vector
 * NORMALISE: normalise a vector (to create a normal vector of the same size)
 * ANGLE: find the angle between two vectors
 * 
 * 
 * 
 * CLASS GLH::EXCEPTION::VECTOR_EXCEPTION
 * 
 * thrown when an error occurs in one of the vector methods or non-member functions (e.g. out of bounds when getting a component)
 * 
 */



/* HEADER GUARD */
#ifndef GLHELPER_VECTOR_HPP_INCLUDED
#define GLHELPER_VECTOR_HPP_INCLUDED



/* INCLUDES */

/* include core headers */
#include <array>
#include <cmath>
#include <initializer_list>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>

/* include glhelper_core.hpp */
#include <glhelper/glhelper_core.hpp>



/* NAMESPACE DECLARATIONS */

namespace glh
{
    namespace meta
    {
        /* struct is_vector
         *
         * is_vector::value is true if the type supplied is a vector
         */
        template<class T> struct is_vector;
    }

    namespace math
    {
        /* TYPES AND CLASSES */

        /* class vector
         *
         * class to represent a vector of M size
         */
        template<unsigned M, class T> class vector;

        /* default vector types */
        template<unsigned M> using fvector = vector<M, GLfloat>;
        template<unsigned M> using dvector = vector<M, GLdouble>;
        template<unsigned M> using ivector = vector<M, GLint>;
        using fvec2 = fvector<2>;
        using fvec3 = fvector<3>;
        using fvec4 = fvector<4>;
        using dvec2 = dvector<2>;
        using dvec3 = dvector<3>;
        using dvec4 = dvector<4>;
        using ivec2 = ivector<2>;
        using ivec3 = ivector<3>;
        using ivec4 = ivector<4>;
        using vec2 = dvec2;
        using vec3 = dvec3;
        using vec4 = dvec4;

        

        /* VECTOR MODIFIER FUNCTIONS DECLARATIONS */

        /* concatenate
         *
         * concatenate two vectors, two Ts or a combination into one vector
         */
        template<unsigned M0, unsigned M1, class T> vector<M0 + M1, T> concatenate ( const vector<M0, T>& lhs, const vector<M1, T>& rhs );
        template<unsigned M, class T, class _T> std::enable_if_t<std::is_convertible<_T, T>::value, vector<M + 1, T>> concatenate ( const vector<M, T>& lhs, const _T& rhs );
        template<unsigned M, class T, class _T> std::enable_if_t<std::is_convertible<_T, T>::value, vector<M + 1, T>> concatenate ( const _T& lhs, const vector<M, T>& rhs );
        template<class T> std::enable_if_t<std::is_arithmetic<T>::value, vector<2, T>> concatenate ( const T& lhs, const T& rhs );

        /* dot
         *
         * find the dot product of two vectors
         */
        template<unsigned M, class T> double dot ( const vector<M, T>& lhs, const vector<M, T>& rhs );

        /* cross
         *
         * find the cross product of a 3d vector
         */
        template<class T> vector<3, T> cross ( const vector<3, T>& lhs, const vector<3, T>& rhs );

        /* modulus
         *
         * find the modulus of a vector
         */
        template<unsigned M, class T> double modulus ( const vector<M, T>& vec );

        /* square_modulus
         *
         * find the modulus of the vector without square-rooting it
         */
        template<unsigned M, class T> double square_modulus ( const vector<M, T>& vec );

        /* normalise
         *
         * convert to a unit vector
         */
        template<unsigned M, class T> vector<M, T> normalise ( const vector<M, T>& vec );

        /* angle
         *
         * find the angle between two vectors
         */
        template<unsigned M, class T> double angle ( const vector<M, T>& lhs, const vector<M, T>& rhs );

    }

    namespace exception
    {
        /* class vector_exception : exception
         *
         * for exceptions related to vectors
         */
        class vector_exception;
    }
}



/* IS_VECTOR DEFINITION */

/* struct is_vector
 *
 * is_vector::value is true if the type supplied is a vector
 */
template<class T> struct glh::meta::is_vector : std::false_type {};
template<> template<unsigned M, class _T> struct glh::meta::is_vector<glh::math::vector<M, _T>> : std::true_type {};



/* VECTOR DEFINITION */

/* class vector
 *
 * class to represent a vector of M size
 */
template<unsigned M, class T> class glh::math::vector
{

    /* assert that M > 0 */
    static_assert ( M > 0, "a vector can not have a dimension of 0" );

    /* static assert that T is arithmetic */
    static_assert ( std::is_arithmetic<T>::value, "a vector cannot be instantiated from a non-arithmetic type" );

public:

    /* single value constructor
     *
     * sets all values to the value provided, defaulting to 0
     */
    explicit vector ( const T& val = 0 ) { data.fill ( val ); }

    /* resize constructor
     *
     * construct from any other vector size but same type
     * smaller vectors will be promoted, and the rest of this vector will be filled with 0s
     * larger vectors will be demoted, and their excessive elements will be ignored
     */
    template<unsigned _M> explicit vector ( const vector<_M, T>& other );

    /* retype constructor
     *
     * construct from a vector of the same size but different type
     */
    template<class _T> vector ( const vector<M, _T>& other );

    /* compound constructor
     *
     * construct from a mix of vectors, swizzle_vectors and integral types
     * the sum of the components of the parameters must be equal to the size of this vector (M)
     * 
     * the constructor is delegated onwards until vs... resolves to nothing, at which point the copy constructor should be called
     */
    template<class T0, class T1, class... Ts> vector ( const T0& v0, const T1& v1, const Ts&... vs )
        : vector { concatenate ( v0, v1 ), vs... }
    {}

    /* any type vector assignment operator */
    template<class _T> vector& operator= ( const vector<M, _T>& other );

    /* default destructor */
    ~vector () = default;



    /* the size of the vector */
    static const unsigned size = M;
    
    /* the type of the vector */
    typedef T value_type;



    /* at
     *
     * get values out of the vector
     */
    T& at ( const unsigned i );
    const T& at ( const unsigned i ) const;

    /* swizzle
     *
     * swizzle a vector
     * the variadic template parameters define the indices of the vector being swizzled
     */
    template<unsigned V0, unsigned V1, unsigned... Vs> vector<2 + sizeof... ( Vs ), T> swizzle () const { return concatenate ( at ( V0 ), swizzle<V1, Vs...> () ); }
    template<unsigned V0> vector<1, T> swizzle () const { return vector<1, T> { at ( V0 ) }; }



    /* internal_ptr
     *
     * return: pointer to the internal array of data
     */
    T * internal_ptr () { return data.data (); }
    const T * internal_ptr () const { return data.data (); }

private:

    /* data
     *
     * the actual data of the vector
     */
    std::array<T, M> data;

};



/* VECTOR OPERATORS DECLARATIONS */

/* operator== and operator!=
 *
 * compare the values of two vectors of the same size
 */
template<unsigned M, class T> bool operator== ( const glh::math::vector<M, T>& lhs, const glh::math::vector<M, T>& rhs );
template<unsigned M, class T> bool operator!= ( const glh::math::vector<M, T>& lhs, const glh::math::vector<M, T>& rhs );

/* operator+(=)
 *
 * addition operations on vectors include:
 * 
 * vector + vector
 * vector += vector
 */
template<unsigned M, class T> glh::math::vector<M, T> operator+ ( const glh::math::vector<M, T>& lhs, const glh::math::vector<M, T>& rhs );
template<unsigned M, class T> glh::math::vector<M, T>& operator+= ( glh::math::vector<M, T>& lhs, const glh::math::vector<M, T>& rhs );

/* operator-(=)
 *
 * subtraction operations on vectors include:
 * 
 * vector - vector
 * vector -= vector
 */
template<unsigned M, class T> glh::math::vector<M, T> operator- ( const glh::math::vector<M, T>& lhs, const glh::math::vector<M, T>& rhs );
template<unsigned M, class T> glh::math::vector<M, T>& operator-= ( glh::math::vector<M, T>& lhs, const glh::math::vector<M, T>& rhs );

/* operator*(=)
 *
 * multiplication operations on vectors include:
 * 
 * vector * vector (SEE BELOW)
 * vector * scalar == scalar * vector
 * vector *= vector
 * vector *= scalar (SEE BELOW)
 * 
 * NOTE: vector multiplication finds the component wise product
 *       use the cross_product and dot_product functions for other multiplication types
 */
template<unsigned M, class T> glh::math::vector<M, T> operator* ( const glh::math::vector<M, T>& lhs, const glh::math::vector<M, T>& rhs );
template<unsigned M, class T> glh::math::vector<M, T> operator* ( const glh::math::vector<M, T>& lhs, const double& rhs );
template<unsigned M, class T> glh::math::vector<M, T> operator* ( const double& lhs, const glh::math::vector<M, T>& rhs );
template<unsigned M, class T> glh::math::vector<M, T>& operator*= ( glh::math::vector<M, T>& lhs, const glh::math::vector<M, T>& rhs );
template<unsigned M, class T> glh::math::vector<M, T>& operator*= ( glh::math::vector<M, T>& lhs, const double& rhs );

/* operator/(=)
 *
 * division operations on vectors include:
 * 
 * vector / vector (SEE BELOW)
 * vector / scalar
 * vector /= vector (SEE BELOW)
 * vector /= scalar
 * 
 * NOTE: vector division finds the component wise dividend
 */
template<unsigned M, class T> glh::math::vector<M, T> operator/ ( const glh::math::vector<M, T>& lhs, const glh::math::vector<M, T>& rhs );
template<unsigned M, class T> glh::math::vector<M, T> operator/ ( const glh::math::vector<M, T>& lhs, const double& rhs );
template<unsigned M, class T> glh::math::vector<M, T>& operator/= ( glh::math::vector<M, T>& lhs, const glh::math::vector<M, T>& rhs );
template<unsigned M, class T> glh::math::vector<M, T>& operator/= ( glh::math::vector<M, T>& lhs, const double& rhs );

/* unary plus operator */
template<unsigned M, class T> glh::math::vector<M, T> operator+ ( const glh::math::vector<M, T>& lhs );
/* unary minus operator */
template<unsigned M, class T> glh::math::vector<M, T> operator- ( const glh::math::vector<M, T>& lhs );

/* pow operator */
template<unsigned M, class T> glh::math::vector<M, T> std::pow ( const glh::math::vector<M, T>& lhs, const glh::math::vector<M, T>& rhs );

/* operator<<
 *
 * format as a one-line string
 */
template<unsigned M, class T> std::ostream& operator<< ( std::ostream& os, const glh::math::vector<M, T>& _vector );



/* VECTOR_EXCEPTION DECLARATION */

/* class vector_exception : exception
 *
 * for exceptions related to vectors
 */
class glh::exception::vector_exception : public exception
{
public:

    /* full constructor
     *
     * __what: description of the exception
     */
    explicit vector_exception ( const std::string& __what )
        : exception ( __what )
    {}

    /* default zero-parameter constructor
     *
     * construct vector_exception with no descrption
     */
    vector_exception () = default;

    /* default everything else and inherits what () function */

};



/* VECTOR IMPLEMENTATION */

/* resize constructor
 *
 * construct from any other size of vector of the same type
 * smaller vectors will be promoted, and the rest of this vector will be filled with 0s
 * larger vectors will be demoted, and their excessive elements will be ignored
 */
template<unsigned M, class T>
template<unsigned _M> inline glh::math::vector<M, T>::vector ( const vector<_M, T>& other )
    : vector { 0 }
{
    /* loop for whichever vector is smaller, copying values accordingly */
    for ( unsigned i = 0; i < M && i < _M; ++i ) at ( i ) = other.at ( i );
}

/* retype constructor
 *
 * construct from a vector of the same size but different type
 */
template<unsigned M, class T>
template<class _T> glh::math::vector<M, T>::vector ( const vector<M, _T>& other )
{
    /* loop for each element and copy them */
    for ( unsigned i = 0; i < M; ++i ) at ( i ) = static_cast<T> ( other.at ( i ) );
}


/* any type vector assignment operator */
template<unsigned M, class T>
template<class _T> inline glh::math::vector<M, T>& glh::math::vector<M, T>::operator= ( const vector<M, _T>& other )
{
    /* loop for each element and copy them */
    for ( unsigned i = 0; i < M; ++i ) at ( i ) = static_cast<T> ( other.at ( i ) );

    /* return * this */
    return * this;
}

/* at
 *
 * get values out of the vector
 */
template<unsigned M, class T> inline T& glh::math::vector<M, T>::at ( const unsigned i )
{
    /* check bounds then return if valid */
    if ( i >= M ) throw exception::vector_exception { "vector indices are out of bounds" };
    return data.at ( i );
}
template<unsigned M, class T> inline const T& glh::math::vector<M, T>::at ( const unsigned i ) const
{
    /* check bounds then return if valid */
    if ( i >= M ) throw exception::vector_exception { "vector indices are out of bounds" };
    return data.at ( i );
}



/* VECTOR MODIFIER FUNCTIONS IMPLEMENTATIONS */

/* concatenate
 *
 * concatenate two vectors, doubles or a combination into one vector
 */
template<unsigned M0, unsigned M1, class T> inline glh::math::vector<M0 + M1, T> glh::math::concatenate ( const vector<M0, T>& lhs, const vector<M1, T>& rhs )
{
    /* create the new vector */
    glh::math::vector<M0 + M1, T> conc;
    
    /* loop for the vectors */
    unsigned conci = 0;
    for ( unsigned i = 0; i < M0; ++i ) conc.at ( conci++ ) = lhs.at ( i );
    for ( unsigned i = 0; i < M1; ++i ) conc.at ( conci++ ) = rhs.at ( i );

    /* return new vector */
    return conc;
}
template<unsigned M, class T, class _T> inline std::enable_if_t<std::is_convertible<_T, T>::value, glh::math::vector<M + 1, T>> glh::math::concatenate ( const vector<M, T>& lhs, const _T& rhs )
{
    /* create the new vector */
    glh::math::vector<M + 1, T> conc;

    /* loop for the vector */
    unsigned conci = 0;
    for ( unsigned i = 0; i < M; ++i ) conc.at ( conci++ ) = lhs.at ( i );
    conc.at ( conci++ ) = rhs;

    /* return new vector */
    return conc;
}
template<unsigned M, class T, class _T> inline std::enable_if_t<std::is_convertible<_T, T>::value, glh::math::vector<M + 1, T>> glh::math::concatenate ( const _T& lhs, const vector<M, T>& rhs )
{
    /* create the new vector */
    glh::math::vector<M + 1, T> conc;

    /* loop for the vector */
    unsigned conci  = 0;
    conc.at ( conci++ ) = lhs;
    for ( unsigned i = 0; i < M; ++i ) conc.at ( conci++ ) = rhs.at ( i );

    /* return new vector */
    return conc;
}
template<class T> inline std::enable_if_t<std::is_arithmetic<T>::value, glh::math::vector<2, T>> glh::math::concatenate ( const T& lhs, const T& rhs )
{
    /* create the new vector */
    glh::math::vector<2, T> conc;

    /* set the values */
    conc.at ( 0 ) = lhs;
    conc.at ( 1 ) = rhs;

    /* return new vector */
    return conc;
}

/* dot
 *
 * find the dot product of two vectors
 */
template<unsigned M, class T> inline double glh::math::dot ( const vector<M, T>& lhs, const vector<M, T>& rhs )
{
    /* store cross product */
    double result = 0.0;

    /* loop to calculate */
    for ( unsigned i = 0; i < M; ++i ) result += ( lhs.at ( i ) * rhs.at ( i ) );

    /* return result */
    return result;
}

/* cross
 *
 * find the cross product of a 3d vector
 */
template<class T> inline glh::math::vector<3, T> glh::math::cross ( const vector<3, T>& lhs, const vector<3, T>& rhs )
{
    /* return cross product */
    return glh::math::vector<3, T> 
    { 
        ( lhs.at ( 1 ) * rhs.at ( 2 ) ) - ( lhs.at ( 2 ) * rhs.at ( 1 ) ),
        ( lhs.at ( 2 ) * rhs.at ( 0 ) ) - ( lhs.at ( 0 ) * rhs.at ( 2 ) ),
        ( lhs.at ( 0 ) * rhs.at ( 1 ) ) - ( lhs.at ( 1 ) * rhs.at ( 0 ) )
    };
}

/* modulus
 *
 * find the modulus of a vector
 */
template<unsigned M, class T> inline double glh::math::modulus ( const vector<M, T>& vec )
{
    /* store the modulus */
    double mod = 0.0;

    /* keep adding to the modulus */
    for ( unsigned i = 0; i < M; ++i ) mod += vec.at ( i ) * vec.at ( i );

    /* return the sqrt of the modulus */
    return std::sqrt ( mod );
}

/* square_modulus
 *
 * find the modulus of the vector without square-rooting it
 */
template<unsigned M, class T> inline double glh::math::square_modulus ( const vector<M, T>& vec )
{
    /* store the modulus */
    double mod = 0.0;

    /* keep adding to the modulus */
    for ( unsigned i = 0; i < M; ++i ) mod += vec.at ( i ) * vec.at ( i );

    /* return without square rooting */
    return mod;
}

/* normalise
 *
 * convert to a unit vector
 */
template<unsigned M, class T> inline glh::math::vector<M, T> glh::math::normalise ( const vector<M, T>& vec )
{
    /* return the vector divided by its modulus */
    return vec / modulus ( vec );
}

/* angle
 *
 * find the angle between two vectors
 */
template<unsigned M, class T> inline double glh::math::angle ( const vector<M, T>& lhs, const vector<M, T>& rhs )
{
    /* return the arccos of ( lhs.rhs / |lhs|.|rhs| ) */
    return std::acos ( dot ( lhs, rhs ) / ( modulus ( lhs ) * modulus ( rhs ) ) );
}



/* VECTOR OPERATORS IMPLEMENTATION */

/* operator== and operator!=
 *
 * compare the values of two vectors of the same size
 */
template<unsigned M, class T> inline bool operator== ( const glh::math::vector<M, T>& lhs, const glh::math::vector<M, T>& rhs )
{
    /* return false if any elements differ */
    for ( unsigned i = 0; i < M; ++i ) if ( lhs.at ( i ) != rhs.at ( i ) ) return false;

    /* else return true */
    return true;
}
template<unsigned M, class T> inline bool operator!= ( const glh::math::vector<M, T>& lhs, const glh::math::vector<M, T>& rhs )
{
    /* return true if any elements differ */
    for ( unsigned i = 0; i < M; ++i ) if ( lhs.at ( i ) != rhs.at ( i ) ) return true;

    /* else return false */
    return false;
}

/* operator+(=)
 *
 * addition operations on vectors include:
 * 
 * vector + vector
 * vector += vector
 */
template<unsigned M, class T> inline glh::math::vector<M, T> operator+ ( const glh::math::vector<M, T>& lhs, const glh::math::vector<M, T>& rhs )
{
    /* create the new vector */
    glh::math::vector<M, T> result;

    /* iterate for each value in result */
    for ( unsigned i = 0; i < M; ++i ) result.at ( i ) = lhs.at ( i ) + rhs.at ( i );

    /* return the new vector */
    return result;
}
template<unsigned M, class T> inline glh::math::vector<M, T>& operator+= ( glh::math::vector<M, T>& lhs, const glh::math::vector<M, T>& rhs )
{
    /* set lhs to equal lhs + rhs */
    return ( lhs = lhs + rhs );
}

/* operator-(=)
 *
 * subtraction operations on vectors include:
 * 
 * vector - vector
 * vector -= vector
 */
template<unsigned M, class T> inline glh::math::vector<M, T> operator- ( const glh::math::vector<M, T>& lhs, const glh::math::vector<M, T>& rhs )
{
    /* create the new vector */
    glh::math::vector<M, T> result;

    /* iterate for each value in result */
    for ( unsigned i = 0; i < M; ++i ) result.at ( i ) = lhs.at ( i ) - rhs.at ( i );

    /* return the new vector */
    return result;
}
template<unsigned M, class T> inline glh::math::vector<M, T>& operator-= ( glh::math::vector<M, T>& lhs, const glh::math::vector<M, T>& rhs )
{
    /* set lhs to equal lhs - rhs */
    return ( lhs = lhs - rhs );
}

/* operator*(=)
 *
 * multiplication operations on vectors include:
 * 
 * vector * vector (SEE BELOW)
 * vector * scalar == scalar * vector
 * vector *= vector
 * vector *= scalar (SEE BELOW)
 * 
 * NOTE: vector multiplication finds the component wise product
 *       use the cross_product and dot_product functions for other multiplication types
 */
template<unsigned M, class T> inline glh::math::vector<M, T> operator* ( const glh::math::vector<M, T>& lhs, const glh::math::vector<M, T>& rhs )
{
    /* create the new vector */
    glh::math::vector<M, T> result;

    /* iterate for each value in result */
    for ( unsigned i = 0; i < M; ++i ) result.at ( i ) = lhs.at ( i ) * rhs.at ( i );

    /* return the new vector */
    return result;
}
template<unsigned M, class T> inline glh::math::vector<M, T> operator* ( const glh::math::vector<M, T>& lhs, const double& rhs )
{
    /* create the new vector */
    glh::math::vector<M, T> result;

    /* iterate for each value in result */
    for ( unsigned i = 0; i < M; ++i ) result.at ( i ) = lhs.at ( i ) * rhs;

    /* return the new vector */
    return result;
}
template<unsigned M, class T> inline glh::math::vector<M, T> operator* ( const double& lhs, const glh::math::vector<M, T>& rhs )
{
    /* equivalent to vector * scalar */
    return rhs * lhs;
}
template<unsigned M, class T> inline glh::math::vector<M, T>& operator*= ( glh::math::vector<M, T>& lhs, const glh::math::vector<M, T>& rhs )
{
    /* set lhs to equal lhs * rhs */
    return ( lhs = lhs * rhs );
}
template<unsigned M, class T> inline glh::math::vector<M, T>& operator*= ( glh::math::vector<M, T>& lhs, const double& rhs )
{
    /* set lhs to equal lhs * rhs */
    return ( lhs = lhs * rhs );
}

/* operator/(=)
 *
 * division operations on vectors include:
 * 
 * vector / vector (SEE BELOW)
 * vector / scalar
 * vector /= vector (SEE BELOW)
 * vector /= scalar
 * 
 * NOTE: vector division finds the component wise dividend
 */
template<unsigned M, class T> inline glh::math::vector<M, T> operator/ ( const glh::math::vector<M, T>& lhs, const glh::math::vector<M, T>& rhs )
{
    /* create the new vector */
    glh::math::vector<M, T> result;

    /* iterate for each value in result */
    for ( unsigned i = 0; i < M; ++i ) result.at ( i ) = lhs.at ( i ) / rhs.at ( i );

    /* return the new vector */
    return result;
}
template<unsigned M, class T> inline glh::math::vector<M, T> operator/ ( const glh::math::vector<M, T>& lhs, const double& rhs )
{
    /* create the new vector */
    glh::math::vector<M, T> result;

    /* iterate for each value in result */
    for ( unsigned i = 0; i < M; ++i ) result.at ( i ) = lhs.at ( i ) / rhs;

    /* return the new vector */
    return result;
}
template<unsigned M, class T> inline glh::math::vector<M, T>& operator/= ( glh::math::vector<M, T>& lhs, const glh::math::vector<M, T>& rhs )
{
    /* set lhs to equal lhs / rhs */
    return ( lhs = lhs / rhs );
}
template<unsigned M, class T> inline glh::math::vector<M, T>& operator/= ( glh::math::vector<M, T>& lhs, const double& rhs )
{
    /* set lhs to equal lhs * rhs */
    return ( lhs = lhs / rhs );
}

/* unary plus operator */
template<unsigned M, class T> inline glh::math::vector<M, T> operator+ ( const glh::math::vector<M, T>& lhs )
{
    /* return the same vector */
    return lhs;
}
/* unary minus operator */
template<unsigned M, class T> inline glh::math::vector<M, T> operator- ( const glh::math::vector<M, T>& lhs )
{
    /* create new vector */
    glh::math::vector<M, T> result { lhs };

    /* negate all the values */
    for ( unsigned i = 0; i < M; ++i ) result.at ( i ) = - result.at ( i );

    /* return the result */
    return result;
}

/* pow operator */
template<unsigned M, class T> inline glh::math::vector<M, T> std::pow ( const glh::math::vector<M, T>& lhs, const glh::math::vector<M, T>& rhs )
{
    /* create new vector */
    glh::math::vector<M, T> result;

    /* pow each of the elements of the vector */
    for ( unsigned i = 0; i < M; ++i ) result.at ( i ) = std::pow ( lhs.at ( i ), rhs.at ( i ) );

    /* return result */
    return result;
}

/* operator<<
 *
 * format as a one-line string
 */
template<unsigned M, class T> inline std::ostream& operator<< ( std::ostream& os, const glh::math::vector<M, T>& _vector )
{
    /* stream intro into ostream */
    os << "vector<" << M << ">{";
    /* loop for each value */
    for ( unsigned i = 0; i < M; ++i )
    {
        /* stream the value */
        os << _vector.at ( i );
        /* if not end of stream, stream comma */
        if ( i + 1 < M ) os << ",";
    }
    /* stream closing } */
    os << "}";
    /* return os */
    return os;
}


/* #ifndef GLHELPER_VECTOR_HPP_INCLUDED */
#endif