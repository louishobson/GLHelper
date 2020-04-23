/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * include/glhelper/glhelper_vector.hpp
 * 
 * implements vector mathematics
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

/* include glhelper_core.hpp */
#include <glhelper/glhelper_core.hpp>

/* include glhelper_exception.hpp */
#include <glhelper/glhelper_exception.hpp>



/* NAMESPACE FORWARD DECLARATIONS */

namespace glh
{
    namespace math
    {
        /* TYPES AND CLASSES */

        /* class vector
         *
         * class to represent a vector of M size
         */
        template<unsigned M> class vector;

        /* typedef vector<M> vecM
        *
        * default vector sizes
        */
        typedef vector<1> vec1;
        typedef vector<2> vec2;
        typedef vector<3> vec3;
        typedef vector<4> vec4;

        

        /* MATRIX MODIFIER FUNCTIONS DECLARATIONS */

        /* concatenate
         *
         * concatenate two vectors, two doubles or a combination into one vector
         */
        template<unsigned M0, unsigned M1> vector<M0 + M1> concatenate ( const vector<M0>& lhs, const vector<M1>& rhs );
        template<unsigned M> vector<M + 1> concatenate ( const vector<M>& lhs, const double rhs );
        template<unsigned M> vector<M + 1> concatenate ( const double lhs, const vector<M>& rhs );
        vector<2> concatenate ( const double lhs, const double rhs );

        /* dot
         *
         * find the dot product of two vectors
         */
        template<unsigned M> double dot ( const vector<M>& lhs, const vector<M>& rhs );

        /* cross
         *
         * find the cross product of a 3d vector
         */
        vector<3> cross ( const vector<3>& lhs, const vector<3>& rhs );

        /* modulus
         *
         * find the modulus of a vector
         */
        template<unsigned M> double modulus ( const vector<M>& vec );

        /* normalise
         *
         * convert to a unit vector
         */
        template<unsigned M> vector<M> normalise ( const vector<M>& vec );

        /* angle
         *
         * find the angle between two vectors
         */
        template<unsigned M> double angle ( const vector<M>& lhs, const vector<M>& rhs );

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



/* VECTOR DEFINITION */

/* class vector
 *
 * class to represent a vector of M size
 */
template<unsigned M> class glh::math::vector
{

    /* assert that M > 0 */
    static_assert ( M > 0, "a vector can not have a dimension of 0" );

public:

    /* zero-parameter constructor
     * 
     * creates a vector filled with 0s
     */
    vector () { data.fill ( 0.0 ); }

    /* double constructor
     *
     * sets all values to the double provided
     */
    explicit vector ( const double val ) { data.fill ( val ); }

    /* vector constructor
     *
     * construct from any other vector
     * smaller vectors will be promoted, and the rest of this vector will be filled with 0s
     * larger vectors will be demoted, and their excessive elements will be ignored
     */
    template<unsigned _M> explicit vector ( const vector<_M>& other );

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

    /* default assignment operator */
    vector<M>& operator= ( const vector<M>& other ) = default;

    /* default destructor */
    ~vector () = default;



    /* at
     *
     * get values out of the vector
     */
    double& at ( const unsigned i );
    const double& at ( const unsigned i ) const;

    /* swizzle
     *
     * swizzle a vector
     * the variadic template parameters define the indices of the vector being swizzled
     */
    template<unsigned T0, unsigned T1, unsigned... Ts> vector<2 + sizeof... ( Ts )> swizzle () const { return concatenate ( at ( T0 ), swizzle<T1, Ts...> () ); }
    template<unsigned T> vector<1> swizzle () const { return vector<1> { at ( T ) }; }



    /* internal_data
     *
     * return: reference to data
     */
    std::array<double, M>& internal_data () { return data; }
    const std::array<double, M>& internal_data () const { return data; }

    /* internal_ptr
     *
     * return: pointer to the internal array of data
     */
    double * internal_ptr () { return data.data (); }
    const double * internal_ptr () const { return data.data (); }

    /* export_data
     *
     * return: an std::array which can be used with OpenGL
     */
    std::array<float, M> export_data () const;

    /* format_str
     *
     * format the matrix to a string
     */
    std::string format_str () const;

private:

    /* data
     *
     * the actual data of the vector
     */
    std::array<double, M> data;

};



/* VECTOR OPERATORS DECLARATIONS */

/* operator== and operator!=
 *
 * compare the values of two vectors of the same size
 */
template<unsigned M> bool operator== ( const glh::math::vector<M>& lhs, const glh::math::vector<M>& rhs );
template<unsigned M> bool operator!= ( const glh::math::vector<M>& lhs, const glh::math::vector<M>& rhs );

/* operator+(=)
 *
 * addition operations on vectors include:
 * 
 * vector + vector
 * vector += vector
 */
template<unsigned M> glh::math::vector<M> operator+ ( const glh::math::vector<M>& lhs, const glh::math::vector<M>& rhs );
template<unsigned M> glh::math::vector<M>& operator+= ( glh::math::vector<M>& lhs, const glh::math::vector<M>& rhs );

/* operator-(=)
 *
 * subtraction operations on vectors include:
 * 
 * vector - vector
 * vector -= vector
 */
template<unsigned M> glh::math::vector<M> operator- ( const glh::math::vector<M>& lhs, const glh::math::vector<M>& rhs );
template<unsigned M> glh::math::vector<M>& operator-= ( glh::math::vector<M>& lhs, const glh::math::vector<M>& rhs );

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
template<unsigned M> glh::math::vector<M> operator* ( const glh::math::vector<M>& lhs, const glh::math::vector<M>& rhs );
template<unsigned M> glh::math::vector<M> operator* ( const glh::math::vector<M>& lhs, const double rhs );
template<unsigned M> glh::math::vector<M> operator* ( const double lhs, const glh::math::vector<M>& rhs );
template<unsigned M> glh::math::vector<M>& operator*= ( glh::math::vector<M>& lhs, const glh::math::vector<M>& rhs );
template<unsigned M> glh::math::vector<M>& operator*= ( glh::math::vector<M>& lhs, const double rhs );

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
template<unsigned M> glh::math::vector<M> operator/ ( const glh::math::vector<M>& lhs, const glh::math::vector<M>& rhs );
template<unsigned M> glh::math::vector<M> operator/ ( const glh::math::vector<M>& lhs, const double rhs );
template<unsigned M> glh::math::vector<M>& operator/= ( glh::math::vector<M>& lhs, const glh::math::vector<M>& rhs );
template<unsigned M> glh::math::vector<M>& operator/= ( glh::math::vector<M>& lhs, const double rhs );

/* unary plus operator */
template<unsigned M> glh::math::vector<M> operator+ ( const glh::math::vector<M>& lhs );
/* unary minus operator */
template<unsigned M> glh::math::vector<M> operator- ( const glh::math::vector<M>& lhs );

/* operator<<
 *
 * format as a one-line string
 */
template<unsigned M> std::ostream& operator<< ( std::ostream& os, const glh::math::vector<M>& _vector );



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

/* vector constructor
 *
 * construct from any other vector
 * smaller vectors will be promoted, and the rest of this vector will be filled with 0s
 * larger vectors will be demoted, and their excessive elements will be ignored
 */
template<unsigned M>
template<unsigned _M> inline glh::math::vector<M>::vector ( const vector<_M>& other )
    : vector { 0. }
{
    /* loop for whichever vector is smaller, copying values accordingly */
    for ( unsigned i = 0; i < M && i < _M; ++i ) at ( i ) = other.at ( i );
}

/* at
 *
 * get values out of the vector
 */
template<unsigned M> inline double& glh::math::vector<M>::at ( const unsigned i )
{
    /* check bounds then return if valid */
    if ( i >= M ) throw exception::vector_exception { "vector indices are out of bounds" };
    return data.at ( i );
}
template<unsigned M> inline const double& glh::math::vector<M>::at ( const unsigned i ) const
{
    /* check bounds then return if valid */
    if ( i >= M ) throw exception::vector_exception { "vector indices are out of bounds" };
    return data.at ( i );
}

/* export_data
 *
 * return: an std::array which can be used with OpenGL
 */
template<unsigned M> std::array<float, M> glh::math::vector<M>::export_data () const
{
    /* create the new array */
    std::array<float, M> new_data;

    /* copy elements over */
    for ( unsigned i = 0; i < M; ++i ) new_data.at ( i ) = at ( i );

    /* return the float data */
    return new_data;
}



/* MATRIX MODIFIER FUNCTIONS IMPLEMENTATIONS */

/* concatenate
 *
 * concatenate two vectors, doubles or a combination into one vector
 */
template<unsigned M0, unsigned M1> inline glh::math::vector<M0 + M1> glh::math::concatenate ( const vector<M0>& lhs, const vector<M1>& rhs )
{
    /* create the new vector */
    glh::math::vector<M0 + M1> conc;
    
    /* loop for the vectors */
    unsigned conci = 0;
    for ( unsigned i = 0; i < M0; ++i ) conc.at ( conci++ ) = lhs.at ( i );
    for ( unsigned i = 0; i < M1; ++i ) conc.at ( conci++ ) = rhs.at ( i );

    /* return new vector */
    return conc;
}
template<unsigned M> inline glh::math::vector<M + 1> glh::math::concatenate ( const vector<M>& lhs, const double rhs )
{
    /* create the new vector */
    glh::math::vector<M + 1> conc;

    /* loop for the vector */
    unsigned conci  = 0;
    for ( unsigned i = 0; i < M; ++i ) conc.at ( conci++ ) = lhs.at ( i );
    conc.at ( conci++ ) = rhs;

    /* return new vector */
    return conc;
}
template<unsigned M> inline glh::math::vector<M + 1> glh::math::concatenate ( const double lhs, const vector<M>& rhs )
{
    /* create the new vector */
    glh::math::vector<M + 1> conc;

    /* loop for the vector */
    unsigned conci  = 0;
    conc.at ( conci++ ) = lhs;
    for ( unsigned i = 0; i < M; ++i ) conc.at ( conci++ ) = rhs.at ( i );

    /* return new vector */
    return conc;
}
glh::math::vector<2> inline glh::math::concatenate ( const double lhs, const double rhs )
{
    /* create the new vector */
    glh::math::vector<2> conc;

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
template<unsigned M> double glh::math::dot ( const vector<M>& lhs, const vector<M>& rhs )
{
    /* store cross product */
    double result = 0;

    /* loop to calculate */
    for ( unsigned i = 0; i < M; ++i ) result += ( lhs.at ( i ) * rhs.at ( i ) );

    /* return result */
    return result;
}

/* cross
 *
 * find the cross product of a 3d vector
 */
glh::math::vector<3> inline glh::math::cross ( const vector<3>& lhs, const vector<3>& rhs )
{
    /* return cross product */
    return glh::math::vector<3> 
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
template<unsigned M> inline double glh::math::modulus ( const vector<M>& vec )
{
    /* store the modulus */
    double mod = 0;

    /* keep adding to the modulus */
    for ( unsigned i = 0; i < M; ++i ) mod += vec.at ( i ) * vec.at ( i );

    /* return the sqrt of the modulus */
    return std::sqrt ( mod );
}

/* normalise
 *
 * convert to a unit vector
 */
template<unsigned M> inline glh::math::vector<M> glh::math::normalise ( const vector<M>& vec )
{
    /* return the vector divided by its modulus */
    return vec / modulus ( vec );
}

/* angle
 *
 * find the angle between two vectors
 */
template<unsigned M> inline double glh::math::angle ( const vector<M>& lhs, const vector<M>& rhs )
{
    /* return the arccos of ( lhs.rhs / |lhs|.|rhs| ) */
    return acos ( dot ( lhs, rhs ) / ( modulus ( lhs ) * modulus ( rhs ) ) );
}



/* VECTOR OPERATORS IMPLEMENTATION */

/* operator== and operator!=
 *
 * compare the values of two vectors of the same size
 */
template<unsigned M> inline bool operator== ( const glh::math::vector<M>& lhs, const glh::math::vector<M>& rhs )
{
    /* return false if any elements differ */
    for ( unsigned i = 0; i < M; ++i ) if ( lhs.at ( i ) != rhs.at ( i ) ) return false;

    /* else return true */
    return true;
}
template<unsigned M> inline bool operator!= ( const glh::math::vector<M>& lhs, const glh::math::vector<M>& rhs )
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
template<unsigned M> inline glh::math::vector<M> operator+ ( const glh::math::vector<M>& lhs, const glh::math::vector<M>& rhs )
{
    /* create the new vector */
    glh::math::vector<M> result;

    /* iterate for each value in result */
    for ( unsigned i = 0; i < M; ++i ) result.at ( i ) = lhs.at ( i ) + rhs.at ( i );

    /* return the new vector */
    return result;
}
template<unsigned M> inline glh::math::vector<M>& operator+= ( glh::math::vector<M>& lhs, const glh::math::vector<M>& rhs )
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
template<unsigned M> inline glh::math::vector<M> operator- ( const glh::math::vector<M>& lhs, const glh::math::vector<M>& rhs )
{
    /* create the new vector */
    glh::math::vector<M> result;

    /* iterate for each value in result */
    for ( unsigned i = 0; i < M; ++i ) result.at ( i ) = lhs.at ( i ) - rhs.at ( i );

    /* return the new vector */
    return result;
}
template<unsigned M> inline glh::math::vector<M>& operator-= ( glh::math::vector<M>& lhs, const glh::math::vector<M>& rhs )
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
template<unsigned M> inline glh::math::vector<M> operator* ( const glh::math::vector<M>& lhs, const glh::math::vector<M>& rhs )
{
    /* create the new vector */
    glh::math::vector<M> result;

    /* iterate for each value in result */
    for ( unsigned i = 0; i < M; ++i ) result.at ( i ) = lhs.at ( i ) * rhs.at ( i );

    /* return the new vector */
    return result;
}
template<unsigned M> inline glh::math::vector<M> operator* ( const glh::math::vector<M>& lhs, const double rhs )
{
    /* create the new vector */
    glh::math::vector<M> result;

    /* iterate for each value in result */
    for ( unsigned i = 0; i < M; ++i ) result.at ( i ) = lhs.at ( i ) * rhs;

    /* return the new vector */
    return result;
}
template<unsigned M> inline glh::math::vector<M> operator* ( const double lhs, const glh::math::vector<M>& rhs )
{
    /* equivalent to vector * scalar */
    return rhs * lhs;
}
template<unsigned M> inline glh::math::vector<M>& operator*= ( glh::math::vector<M>& lhs, const glh::math::vector<M>& rhs )
{
    /* set lhs to equal lhs * rhs */
    return ( lhs = lhs * rhs );
}
template<unsigned M> inline glh::math::vector<M>& operator*= ( glh::math::vector<M>& lhs, const double rhs )
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
template<unsigned M> inline glh::math::vector<M> operator/ ( const glh::math::vector<M>& lhs, const glh::math::vector<M>& rhs )
{
    /* create the new vector */
    glh::math::vector<M> result;

    /* iterate for each value in result */
    for ( unsigned i = 0; i < M; ++i ) result.at ( i ) = lhs.at ( i ) / rhs.at ( i );

    /* return the new vector */
    return result;
}
template<unsigned M> inline glh::math::vector<M> operator/ ( const glh::math::vector<M>& lhs, const double rhs )
{
    /* create the new vector */
    glh::math::vector<M> result;

    /* iterate for each value in result */
    for ( unsigned i = 0; i < M; ++i ) result.at ( i ) = lhs.at ( i ) / rhs;

    /* return the new vector */
    return result;
}
template<unsigned M> inline glh::math::vector<M> operator/= ( glh::math::vector<M>& lhs, const glh::math::vector<M>& rhs )
{
    /* set lhs to equal lhs / rhs */
    return ( lhs = lhs / rhs );
}
template<unsigned M> inline glh::math::vector<M> operator/= ( glh::math::vector<M>& lhs, const double rhs )
{
    /* set lhs to equal lhs * rhs */
    return ( lhs = lhs / rhs );
}

/* unary plus operator */
template<unsigned M> inline glh::math::vector<M> operator+ ( const glh::math::vector<M>& lhs )
{
    /* return the same vector */
    return lhs;
}
/* unary minus operator */
template<unsigned M> inline glh::math::vector<M> operator- ( const glh::math::vector<M>& lhs )
{
    /* create new vector */
    glh::math::vector<M> result { lhs };

    /* negate all the values */
    for ( unsigned i = 0; i < M; ++i ) result.at ( i ) = - result.at ( i );

    /* return the result */
    return result;
}

/* operator<<
 *
 * format as a one-line string
 */
template<unsigned M> inline std::ostream& operator<< ( std::ostream& os, const glh::math::vector<M>& _vector )
{
    /* stream intro into ostream */
    os << "glh::vector<" << M << ">{";
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