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
         * trans: the transformation matrix to stretch
         * axis: the axis to stretch along (row/column beginning 0)
         * sf: scale factor of stretch
         * 
         * return: the new transformation matrix
         */
        template<unsigned M> matrix<M> stretch ( const matrix<M>& trans, const unsigned axis, const float sf );

        /* enlarge
         *
         * enlarge by a scale factor
         * 
         * trans: the transformation matrix to add an enlargement to
         * sf: scale factor of enlargement
         * 
         * return: the new transformation matrix
         */
        template<unsigned M> matrix<M> enlarge ( const matrix<M>& trans, const float sf );

        /* rotate
         *
         * rotate by a number of radians
         *
         * trans: the transformation matrix to rotate
         * axis0,1: plane over which to rotate
         * arg: the anticlockwise angle to rotate by in radians
         * 
         * returnL the new transformation matrix
         */
        template<unsigned M> matrix<M> rotate ( const matrix<M>& trans, const unsigned axis0, const unsigned axis1, const float arg );
    }
}



/* FUNCTION IMPLEMENTATIONS */

/* zero_matrix
 *
 * produce a zero square matrix
 */
template<unsigned M> glh::math::matrix<M, M> glh::math::zero_matrix ()
{
    /* return a default matrix */
    return math::matrix<M, M> {};
}

/* identity_matrix
 *
 * template function to produce an identity matrix
 */
template<unsigned M> glh::math::matrix<M, M> glh::math::identity ()
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
 * trans: the transformation matrix to stretch
 * axis: the axis to stretch along (row/column beginning 0)
 * sf: scale factor of stretch
 * 
 * return: the new transformation matrix
 */
template<unsigned M> glh::math::matrix<M> glh::math::stretch ( const matrix<M>& trans, const unsigned axis, const float sf )
{
    /* create the new matrix */
    math::matrix<M> result { trans };

    /* multiply the appropriate row by the scale factor */
    for ( unsigned iti = 0; iti < M; ++iti ) result.at ( iti, axis ) *= sf;

    /* return result */
    return result;
}

/* enlarge
 *
 * enlarge by a scale factor
 * 
 * trans: the transformation matrix to add an enlargement to
 * sf: scale factor of enlargement
 * 
 * return: the new transformation matrix
 */
template<unsigned M> glh::math::matrix<M> glh::math::enlarge ( const matrix<M>& trans, const float sf )
{
    /* return trans multiplied by the scale factor */
    return trans * sf;
}

/* rotate
 *
 * rotate by a number of radians
 *
 * trans: the transformation matrix to rotate
 * axis0,1: plane over which to rotate
 * arg: the anticlockwise angle to rotate by in radians
 * 
 * returnL the new transformation matrix
 */
template<unsigned M> glh::math::matrix<M> glh::math::rotate ( const matrix<M>& trans, const unsigned axis0, const unsigned axis1, const float arg )
{
    /* create identity matrix */
    math::matrix<M> rot = identity<M> ();

    /* assign the cosine and sine values */
    rot.at ( axis0, axis0 ) = std::cos ( arg );
    rot.at ( axis1, axis1 ) = std::cos ( arg );
    rot.at ( axis0, axis1 ) = std::sin ( arg );
    rot.at ( axis1, axis0 ) = -std::sin ( arg );

    /* return trans * rot */
    return trans * rot;
}



/* #ifndef GLHELPER_TRANSFORM_HPP_INCLUDED */
#endif