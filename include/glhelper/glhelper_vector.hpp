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

        /* class vector_exception : exception
        *
        * for exceptions related to vectors
        */
        class vector_exception;

        

        /* MATRIX MODIFIER FUNCTIONS DECLARATIONS */

        /* concatenate
         *
         * concatenate two vectors, two floats or a combination into one vector
         */
        template<unsigned M0, unsigned M1> vector<M0 + M1> concatenate ( const vector<M0>& v0, const vector<M1>& v1 );
        template<unsigned M> vector<M + 1> concatenate ( const vector<M>& v0, const float v1 );
        template<unsigned M> vector<M + 1> concatenate ( const float v0, const vector<M>& v1 );
        vector<2> concatenate ( const float v0, const float v1 );

        /* dot_product
         *
         * find the dot product of two vectors
         */
        template<unsigned M> float dot_product ( const vector<M>& lhs, const vector<M>& rhs );

        /* cross product
         *
         * find the cross product of a 3d vector
         */
        vector<3> cross_product ( const vector<3>& lhs, const vector<3>& rhs );

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
    vector ()
        : data { { 0. } }
    {}

    /* float constructor
     *
     * sets all values to the float provided
     */
    vector ( const float val )
        : data { { val } }
    {}

    /* default copy constructor */
    vector ( const vector<M>& other ) = default;

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
    float& at ( const unsigned i );
    const float& at ( const unsigned i ) const;

    /* swizzle
     *
     * swizzle a vector
     * the variadic template parameters define the indices of the vector being swizzled
     */
    template<unsigned T0, unsigned T1, unsigned... Ts> vector<2 + sizeof... ( Ts )> swizzle () { return concatenate ( at ( T0 ), swizzle<T1, Ts...> () ); }
    template<unsigned T> vector<1> swizzle () { return at ( T ); }



    /* internal_data
     *
     * return: reference to data
     */
    std::array<float, M>& internal_data () { return data; }
    const std::array<float, M>& internal_data () const { return data; }

    /* internal_ptr
     *
     * return: pointer to the internal array of data
     */
    float * internal_ptr () { return data.data (); }
    const float * internal_ptr () const { return data.data (); }

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
    std::array<float, M> data;

};



/* VECTOR OPERATORS DECLARATIONS */

/* operator+(=)
 *
 * addition operations on vectors
 */
template<unsigned M> glh::math::vector<M> operator+ ( const glh::math::vector<M>& lhs, const glh::math::vector<M>& rhs );
template<unsigned M> glh::math::vector<M>& operator+= ( glh::math::vector<M>& lhs, const glh::math::vector<M>& rhs );

/* operator-(=)
 *
 * subtraction operations on vectors
 */
template<unsigned M> glh::math::vector<M> operator- ( const glh::math::vector<M>& lhs, const glh::math::vector<M>& rhs );
template<unsigned M> glh::math::vector<M>& operator-= ( glh::math::vector<M>& lhs, const glh::math::vector<M>& rhs );

/* operator*(=)
 *
 * multiplication operations on vectors
 * for two vectors, multiplication finds the component-wise product
 */
template<unsigned M> glh::math::vector<M> operator* ( const glh::math::vector<M>& lhs, const glh::math::vector<M>& rhs );
template<unsigned M> glh::math::vector<M> operator* ( const glh::math::vector<M>& lhs, const float rhs );
template<unsigned M> glh::math::vector<M>& operator*= ( glh::math::vector<M>& lhs, const glh::math::vector<M>& rhs );
template<unsigned M> glh::math::vector<M>& operator*= ( glh::math::vector<M>& lhs, const float rhs );

/* operator/(=)
 *
 * division operations on vectors
 * for two vectors, division finds the component-wise dividend
 */
template<unsigned M> glh::math::vector<M> operator/ ( const glh::math::vector<M>& lhs, const glh::math::vector<M>& rhs );
template<unsigned M> glh::math::vector<M> operator/ ( const glh::math::vector<M>& lhs, const float rhs );
template<unsigned M> glh::math::vector<M>& operator/= ( glh::math::vector<M>& lhs, const glh::math::vector<M>& rhs );
template<unsigned M> glh::math::vector<M>& operator/= ( glh::math::vector<M>& lhs, const float rhs );

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
class glh::math::vector_exception : public exception
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
    explicit vector_exception () = default;

    /* default everything else and inherits what () function */

};



/* VECTOR IMPLEMENTATION */

/* at
 *
 * get values out of the vector
 */
template<unsigned M> inline float& glh::math::vector<M>::at ( const unsigned i )
{
    /* check bounds then return if valid */
    if ( i >= M ) throw vector_exception { "vector indices are out of bounds" };
    return data.at ( i );
}
template<unsigned M> inline const float& glh::math::vector<M>::at ( const unsigned i ) const
{
    /* check bounds then return if valid */
    if ( i >= M ) throw matrix_exception { "vector indices are out of bounds" };
    return data.at ( i );
}



/* MATRIX MODIFIER FUNCTIONS IMPLEMENTATIONS */

/* concatenate
 *
 * concatenate two vectors, floats or a combination into one vector
 */
template<unsigned M0, unsigned M1> inline glh::math::vector<M0 + M1> glh::math::concatenate ( const vector<M0>& v0, const vector<M1>& v1 )
{
    /* create the new vector */
    glh::math::vector<M0 + M1> conc;
    
    /* loop for the vectors */
    unsigned conci = 0;
    for ( unsigned iti = 0; iti < M0; ++iti ) conc.at ( conci++ ) = v0.at ( iti );
    for ( unsigned iti = 0; iti < M1; ++iti ) conc.at ( conci++ ) = v1.at ( iti );

    /* return new vector */
    return conc;
}
template<unsigned M> inline glh::math::vector<M + 1> glh::math::concatenate ( const vector<M>& v0, const float v1 )
{
    /* create the new vector */
    glh::math::vector<M + 1> conc;

    /* loop for the vector */
    unsigned conci  = 0;
    for ( unsigned iti = 0; iti < M; ++iti ) conc.at ( conci++ ) = v0.at ( iti );
    conc.at ( conci++ ) = v1;

    /* return new vector */
    return conc;
}
template<unsigned M> inline glh::math::vector<M + 1> glh::math::concatenate ( const float v0, const vector<M>& v1 )
{
    /* create the new vector */
    glh::math::vector<M + 1> conc;

    /* loop for the vector */
    unsigned conci  = 0;
    conc.at ( conci++ ) = v0;
    for ( unsigned iti = 0; iti < M; ++iti ) conc.at ( conci++ ) = v1.at ( iti );

    /* return new vector */
    return conc;
}
glh::math::vector<2> inline glh::math::concatenate ( const float v0, const float v1 )
{
    /* create the new vector */
    glh::math::vector<2> conc;

    /* set the values */
    conc.at ( 0 ) = v0;
    conc.at ( 1 ) = v1;

    /* return new vector */
    return conc;
}

/* dot_product
 *
 * find the dot product of two vectors
 */
template<unsigned M> float glh::math::dot_product ( const vector<M>& lhs, const vector<M>& rhs )
{
    /* store cross product */
    float result;

    /* loop to calculate */
    for ( unsigned iti = 0; iti < M; ++iti ) result += ( lhs.at ( iti ) * rhs.at ( iti ) );

    /* return result */
    return result;
}

/* cross product
 *
 * find the cross product of a 3d vector
 */
glh::math::vector<3> inline glh::math::cross_product ( const vector<3>& lhs, const vector<3>& rhs )
{
    /* return cross product */
    return glh::math::vector<3> 
    { 
        ( lhs.at ( 1 ) * rhs.at ( 2 ) ) - ( lhs.at ( 2 ) * rhs.at ( 1 ) ),
        ( lhs.at ( 2 ) * rhs.at ( 0 ) ) - ( lhs.at ( 0 ) * rhs.at ( 2 ) ),
        ( lhs.at ( 0 ) * rhs.at ( 1 ) ) - ( lhs.at ( 1 ) * rhs.at ( 0 ) )
    };
}



/* VECTOR OPERATORS IMPLEMENTATION */

/* operator+(=)
 *
 * addition operations on vectors
 */
template<unsigned M> inline glh::math::vector<M> operator+ ( const glh::math::vector<M>& lhs, const glh::math::vector<M>& rhs )
{
    /* create the new vector */
    glh::math::vector<M> result;

    /* iterate for each value in result */
    for ( unsigned iti = 0; iti < M; ++iti ) result.at ( iti ) = lhs.at ( iti ) + rhs.at ( iti );

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
 * subtraction operations on vectors
 */
template<unsigned M> inline glh::math::vector<M> operator- ( const glh::math::vector<M>& lhs, const glh::math::vector<M>& rhs )
{
    /* create the new vector */
    glh::math::vector<M> result;

    /* iterate for each value in result */
    for ( unsigned iti = 0; iti < M; ++iti ) result.at ( iti ) = lhs.at ( iti ) - rhs.at ( iti );

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
 * multiplication operations on vectors
 * for two vectors, multiplication finds the component-wise product
 */
template<unsigned M> inline glh::math::vector<M> operator* ( const glh::math::vector<M>& lhs, const glh::math::vector<M>& rhs )
{
    /* create the new vector */
    glh::math::vector<M> result;

    /* iterate for each value in result */
    for ( unsigned iti = 0; iti < M; ++iti ) result.at ( iti ) = lhs.at ( iti ) * rhs.at ( iti );

    /* return the new vector */
    return result;
}
template<unsigned M> inline glh::math::vector<M> operator* ( const glh::math::vector<M>& lhs, const float rhs )
{
    /* create the new vector */
    glh::math::vector<M> result;

    /* iterate for each value in result */
    for ( unsigned iti = 0; iti < M; ++iti ) result.at ( iti ) = lhs.at ( iti ) * rhs;

    /* return the new vector */
    return result;
}
template<unsigned M> inline glh::math::vector<M>& operator*= ( glh::math::vector<M>& lhs, const glh::math::vector<M>& rhs )
{
    /* set lhs to equal lhs * rhs */
    return ( lhs = lhs * rhs );
}
template<unsigned M> inline glh::math::vector<M>& operator*= ( glh::math::vector<M>& lhs, const float rhs )
{
    /* set lhs to equal lhs * rhs */
    return ( lhs = lhs * rhs );
}

/* operator/(=)
 *
 * division operations on vectors
 * for two vectors, division finds the component-wise dividend
 */
template<unsigned M> inline glh::math::vector<M> operator/ ( const glh::math::vector<M>& lhs, const glh::math::vector<M>& rhs )
{
    /* create the new vector */
    glh::math::vector<M> result;

    /* iterate for each value in result */
    for ( unsigned iti = 0; iti < M; ++iti ) result.at ( iti ) = lhs.at ( iti ) / rhs.at ( iti );

    /* return the new vector */
    return result;
}
template<unsigned M> inline glh::math::vector<M> operator/ ( const glh::math::vector<M>& lhs, const float rhs )
{
    /* create the new vector */
    glh::math::vector<M> result;

    /* iterate for each value in result */
    for ( unsigned iti = 0; iti < M; ++iti ) result.at ( iti ) = lhs.at ( iti ) / rhs;

    /* return the new vector */
    return result;
}
template<unsigned M> inline glh::math::vector<M> operator/= ( glh::math::vector<M>& lhs, const glh::math::vector<M>& rhs )
{
    /* set lhs to equal lhs / rhs */
    return ( lhs = lhs / rhs );
}
template<unsigned M> inline glh::math::vector<M> operator/= ( glh::math::vector<M>& lhs, const float rhs )
{
    /* set lhs to equal lhs * rhs */
    return ( lhs = lhs / rhs );
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
    for ( unsigned iti = 0; iti < M; ++iti )
    {
        /* stream the value */
        os << _vector.at ( iti );
        /* if not end of stream, stream comma */
        if ( iti + 1 < M ) os << ",";
    }
    /* stream closing } */
    os << "}";
    /* return os */
    return os;
}


/* #ifndef GLHELPER_VECTOR_HPP_INCLUDED */
#endif