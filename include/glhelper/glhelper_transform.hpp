/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * include/glhelper/glhelper_transform.hpp
 * 
 * implements functions to aid transformation calculations
 * 
 */



/* HEADER GUARD */
#ifndef GLHELPER_TRANSFORM_HPP_INCLUDED
#define GLHELPER_TRANSFORM_HPP_INCLUDED



/* INCLUDES */

/* include core headers */
#include <iostream>

/* include glhelper_core.hpp */
#include <glhelper/glhelper_core.hpp>

/* include glhelper_exception.hpp */
#include <glhelper/glhelper_exception.hpp>

/* include glhelper_matrix.hpp */
#include <glhelper/glhelper_matrix.hpp>

/* include glhelper_vector.hpp */
#include <glhelper/glhelper_vector.hpp>



/* NAMESPACE FORWARD DECLARATIONS */

namespace glh
{
    namespace math
    {
        /* pi
         *
         * return: the value of pi multiplied by a constant
         */
        double pi ( const double k ) { return k * acos ( -1. ); }

        /* rad
         *
         * convert degrees to radians
         * 
         * return: a conversion of deg into radians
         */
        double rad ( const double deg ) { return pi ( deg / 180 ); }

        /* zero_matrix
         *
         * produce a zero square matrix
         * 
         * return: zero matrix of size MxM
         */
        template<unsigned M> matrix<M> zero_matrix ();

        /* identity
         *
         * template function to produce an identity matrix
         * 
         * return: identity matrix of size MxM
         */
        template<unsigned M> matrix<M> identity ();

        /* stretch
         *
         * stretch along one axis
         * 
         * trans/vec: the transformation matrix/vector to stretch
         * sf: scale factor of stretch
         * axis: the axis to stretch along (row/column beginning 0)
         * 
         * return: the new transformation matrix/vector
         */
        template<unsigned M> matrix<M> stretch ( const matrix<M>& trans, const double sf, const unsigned axis );
        template<unsigned M> vector<M> stretch ( const vector<M>& vec, const double sf, const unsigned axis );

        /* enlarge
         *
         * enlarge by a scale factor
         * 
         * trans/vec: the transformation matrix/vector to enlarge
         * sf: scale factor of enlargement
         * 
         * return: the new transformation matrix/vector
         */
        template<unsigned M> matrix<M> enlarge ( const matrix<M>& trans, const double sf );
        template<unsigned M> vector<M> enlarge ( const vector<M>& vec, const double sf );

        /* rotate
         *
         * rotate around a plane by a number of radians
         *
         * tran/vec: the transformation matrix/vector to rotate
         * arg: the anticlockwise angle to rotate by in radians
         * axis0,1: plane over which to rotate
         * 
         * return: the new transformation matrix/vector
         */
        template<unsigned M> matrix<M> rotate ( const matrix<M>& trans, const double arg, const unsigned axis0, const unsigned axis1 );
        template<unsigned M> vector<M> rotate ( const vector<M>& vec, const double arg, const unsigned axis0, const unsigned axis1 );
    }
}



/* VECTOR-MATRIX OPERATOR DECLARATIONS */

/* operator*
 *
 * multiplication of a matrix before a vector
 */
template<unsigned M, unsigned N> glh::math::vector<M> operator* ( const glh::math::matrix<M, N>& lhs, const glh::math::vector<N>& rhs );

/* operator*=
 *
 * takes a vector and multiplies it by a matrix on its left
 */
template<unsigned M> glh::math::vector<M>& operator*= ( const glh::math::vector<M>& lhs, const glh::math::matrix<M>& rhs );



/* FUNCTION IMPLEMENTATIONS */

/* zero_matrix
 *
 * produce a zero square matrix
 */
template<unsigned M> inline glh::math::matrix<M, M> glh::math::zero_matrix ()
{
    /* return a default matrix */
    return math::matrix<M, M> { 0. };
}

/* identity_matrix
 *
 * template function to produce an identity matrix
 */
template<unsigned M> inline glh::math::matrix<M, M> glh::math::identity ()
{
    /* create new matrix */
    math::matrix<M, M> identity;

    /* set values */
    for ( unsigned iti = 0; iti < M; ++iti ) identity.at ( iti, iti ) = 1.;

    /* return identity matrix */
    return identity;
}

/* stretch
 *
 * stretch along one axis
 * 
 * trans/vec: the transformation matrix/vector to stretch
 * sf: scale factor of stretch
 * axis: the axis to stretch along (row/column beginning 0)
 * 
 * return: the new transformation matrix/vector
 */
template<unsigned M> inline glh::math::matrix<M> glh::math::stretch ( const matrix<M>& trans, const unsigned sf, const double axis )
{
    /* create the new matrix */
    math::matrix<M> result { trans };

    /* multiply the appropriate row by the scale factor */
    for ( unsigned iti = 0; iti < M; ++iti ) result.at ( iti, axis ) *= sf;

    /* return result */
    return result;
}
template<unsigned M> inline glh::math::vector<M> glh::math::stretch ( const vector<M>& vec, const unsigned sf, const double axis )
{
    /* create the new vector */
    math::vector<M> result { vec };

    /* multiply the appropriate component by the scale factor */
    result.at ( axis ) *= sf;

    /* return result */
    return result;
}

/* enlarge
 *
 * enlarge by a scale factor
 * 
 * trans/vec: the transformation matrix/vector to enlarge
 * sf: scale factor of enlargement
 * 
 * return: the new transformation matrix/vector
 */
template<unsigned M> inline glh::math::matrix<M> glh::math::enlarge ( const matrix<M>& trans, const double sf )
{
    /* return trans multiplied by the scale factor */
    return trans * sf;
}
template<unsigned M> inline glh::math::vector<M> glh::math::enlarge ( const vector<M>& vec, const double sf )
{
    /* return vec multiplied by the scale factor */
    return vec * sf;
}

/* rotate
 *
 * rotate by a number of radians
 *
 * trans/vec: the transformation matrix/vector to rotate
 * arg: the anticlockwise angle to rotate by in radians
 * axis0,1: plane over which to rotate
 * 
 * return: the new transformation matrix/vector
 */
template<unsigned M> inline glh::math::matrix<M> glh::math::rotate ( const matrix<M>& trans, const unsigned arg, const unsigned axis0, const double axis1 )
{
    /* create identity matrix */
    math::matrix<M> rot = identity<M> ();

    /* assign the cosine and sine values */
    rot.at ( axis0, axis0 ) = std::cos ( arg );
    rot.at ( axis1, axis1 ) = std::cos ( arg );
    rot.at ( axis0, axis1 ) = -std::sin ( arg );
    rot.at ( axis1, axis0 ) = std::sin ( arg );

    /* return rot * trans */
    return rot * trans;
}
template<unsigned M> inline glh::math::vector<M> glh::math::rotate ( const vector<M>& vec, const unsigned arg, const unsigned axis0, const double axis1 )
{
    /* produce a rotational matrix and return the product */
    return glh::math::rotate ( glh::math::identity<M> (), axis0, axis1, arg ) * vec;
}



/* VECTOR-MATRIX OPERATOR IMPLEMENTATIONS */

/* operator*
 *
 * multiplication of a matrix before a vector
 */
template<unsigned M, unsigned N> glh::math::vector<M> operator* ( const glh::math::matrix<M, N>& lhs, const glh::math::vector<N>& rhs )
{
    /* create the new vector */
    glh::math::vector<M> result;

    /* iterate for each value in result, and then each value in a row of the matrix */
    for ( unsigned iti = 0; iti < M; ++iti ) for ( unsigned itj = 0; itj < N; ++itj )
    {
        /* keep adding to the values in result */
        result.at ( iti ) += lhs.at ( iti, itj ) * rhs.at ( itj );
    }

    /* return result */
    return result;
}

/* operator*=
 *
 * takes a vector and multiplies it by a matrix on its left
 */
template<unsigned M> glh::math::vector<M>& operator*= ( const glh::math::vector<M>& lhs, const glh::math::matrix<M>& rhs )
{
    /* return lhs = rhs * lhs */
    return ( lhs = rhs * lhs );
}




/* #ifndef GLHELPER_TRANSFORM_HPP_INCLUDED */
#endif