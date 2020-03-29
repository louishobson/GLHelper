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
        inline double pi ( const double k ) { return k * acos ( -1. ); }

        /* rad
         *
         * convert degrees to radians
         * 
         * return: a conversion of deg into radians
         */
        inline double rad ( const double deg ) { return pi ( deg / 180 ); }

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

        /* stretch with vector
         *
         * stretch along several axis simultaneously
         * 
         * trans/vec: the transformation matrix/vector to stretch
         * sfs: the stretches to apply to each axis
         * return: the new transformation matrix/vector
         */
        template<unsigned M> matrix<M> stretch ( const matrix<M>& trans, const vector<M>& sfs );
        template<unsigned M> vector<M> stretch ( const vector<M>& vec, const vector<M>& sfs );

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
    
        /* rotate with vector
         *
         * rotate along several axis simultaneously
         * only for 3D space (inc. 4D vectors for homogeneus coords)
         * rotate around an arbitrary unit axis
         * 
         * trans/vec: the transformation matrix/vector to rotate
         * arg: the angle to rotate by in radians
         * axis: the unit axis to rotate in
         * 
         * return: the new transformation matrix/vector
         */
        matrix<3> rotate ( const matrix<3>& trans, const double arg, const vector<3>& axis );
        vector<3> rotate ( const vector<3>& vec, const double arg, const vector<3>& axis );
        matrix<4> rotate ( const matrix<4>& trans, const double arg, const vector<3>& axis );
        vector<4> rotate ( const vector<4>& vec, const double arg, const vector<3>& axis );

        /* translate
         *
         * translate a matrix or vector
         * 
         * trans/vec: the translation matrix/vector to translate
         * translation: the amount to translate by
         * axis: the axis of translation
         * 
         * return: the new transformation matrix/vector 
         */
        template<unsigned M> matrix<M> translate ( const matrix<M>& trans, const double translation, const double axis );
        template<unsigned M> vector<M> translate ( const vector<M>& vec, const double translation, const double axis );

        /* translate with vector
         *
         * translate along all axis simultaneously
         * matrices are assumed to have a homogenmous coordinate associated with them
         * 
         * trans/vec: the transformation matrix/vector to translate
         * translation: vector for the translation
         * 
         * return: the new transformation matrix/vector
         */
        template<unsigned M> matrix<M> translate ( const matrix<M>& trans, const vector<M - 1>& translation );
        template<unsigned M> vector<M> translate ( const vector<M>& vec, const vector<M>& translation );



        /* perspective
         *
         * create a perspective projection matrix
         * 
         * l: the (x) position of the LEFT of the near rectangle of the frustum
         * r: the (x) position of the RIGHT of the near rectangle of the frustum
         * b: the (y) position of the BOTTOM of the near rectangle of the frustum
         * t: the (y) position of the TOP of the near rectangle of the frustum
         * n: the (z) position of the NEAR rectangle of the frustum
         * f: the (z) position of the FAR rectangle of the frustum
         * 
         * return: the perspective projection matrix
         */
        matrix<4> perspective ( const double l, const double r, const double b, const double t, const double n, const double f );

        /* perspective_fov
         *
         * create a perspective projection matrix from a field of view
         * 
         * fov: the field of view in radians
         * aspect: the aspect ratio of the screen
         * n: the distance to the near rectangle
         * f: the distance to the far rectangle
         * 
         * return: the perspective projection matrix
         */
        matrix<4> perspective_fov ( const double fov, const double aspect, const double n, const double f );

        /* camera
         *
         * produce a camera matrix from vectors
         * 
         * p: position (non-unit) vector
         * x,y,z: unit axis for the camera
         * 
         * return: camera matrix based on vectors provided
         */
        matrix<4> camera ( const vector<3>& p, const vector<3>& x, const vector<3>& y, const vector<3>& z );

        /* look_at
         *
         * produce a camera matrix based on a camera position, target position and an up vector
         * 
         * p: camera (non-unit) position vector
         * t: target (non-unit) position vector
         * wup: WORLD up unit vector
         * 
         * return: camera matrix based on vectors provided
         */
        matrix<4> look_at ( const vector<3>& p, const vector<3>& t, const vector<3>& wup );

        /* look_towards
         *
         * produce a camera matrix based on a camera position, direction of viewing and up vector
         * 
         * p: camera (non-unit) position vector
         * d: direction unit vector
         * wup: WORLD up unit vector
         * 
         * return: camera matrix based on vectors provided
         */
        matrix<4> look_towards ( const vector<3>& p, const vector<3>& d, const vector<3>& wup );        

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
    return matrix<M, M> { 0. };
}

/* identity_matrix
 *
 * template function to produce an identity matrix
 */
template<unsigned M> inline glh::math::matrix<M, M> glh::math::identity ()
{
    /* create new matrix */
    matrix<M, M> identity;

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
    matrix<M> result { trans };

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

/* stretch with vector
 *
 * stretch along several axis simultaneously
 * 
 * trans/vec: the transformation matrix/vector to stretch
 * sf: the stretches to apply to each axis
 * return: the new transformation matrix/vector
 */
template<unsigned M> inline glh::math::matrix<M> glh::math::stretch ( const matrix<M>& trans, const vector<M>& sfs )
{
    /* create new matrix */
    matrix<M> result { trans };

    /* add stretches */
    for ( unsigned iti = 0; iti < M; ++iti ) for ( unsigned itj = 0; itj < M; ++itj )
    {
        result.at ( iti, itj ) *= sfs.at ( itj );
    }

    /* return result */
    return result;
}
template<unsigned M> inline glh::math::vector<M> glh::math::stretch ( const vector<M>& vec, const vector<M>& sfs )
{
    /* multiply vectors and return */
    return vec * sfs;
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
template<unsigned M> inline glh::math::matrix<M> glh::math::rotate ( const matrix<M>& trans, const double arg, const unsigned axis0, const unsigned axis1 )
{
    /* create identity matrix */
    matrix<M> rot = identity<M> ();

    /* assign the cosine and sine values */
    rot.at ( axis0, axis0 ) = std::cos ( arg );
    rot.at ( axis1, axis1 ) = std::cos ( arg );
    rot.at ( axis0, axis1 ) = -std::sin ( arg );
    rot.at ( axis1, axis0 ) = std::sin ( arg );

    /* return rot * trans */
    return rot * trans;
}
template<unsigned M> inline glh::math::vector<M> glh::math::rotate ( const vector<M>& vec, const double arg, const unsigned axis0, const unsigned axis1 )
{
    /* produce a rotational matrix and return the product */
    return rotate ( glh::math::identity<M> (), axis0, axis1, arg ) * vec;
}

/* rotate with vector
 *
 * rotate along several axis simultaneously
 * only for 3D space (inc. 4D vectors for homogeneus coords)
 * rotate around an arbitrary unit axis
 * 
 * trans/vec: the transformation matrix/vector to rotate
 * arg: the angle to rotate by in radians
 * axis: the unit axis to rotate in
 * 
 * return: the new transformation matrix/vector
 */
inline glh::math::matrix<3> glh::math::rotate ( const matrix<3>& trans, const double arg, const vector<3>& axis )
{
    /* return the new transformation matrix */
    return matrix<3>
    {
        ( cos ( arg ) ) + ( axis.at ( 0 ) * axis.at ( 0 )  * ( 1 - cos ( arg ) ) ),
        ( axis.at ( 0 ) * axis.at ( 1 ) * ( 1 - cos ( arg ) ) ) - ( axis.at ( 2 ) * sin ( arg ) ),
        ( axis.at ( 0 ) * axis.at ( 2 ) * ( 1 - cos ( arg ) ) ) + ( axis.at ( 1 ) * sin ( arg ) ),

        ( axis.at ( 1 ) * axis.at ( 0 ) * ( 1 - cos ( arg ) ) ) + ( axis.at ( 2 ) * sin ( arg ) ),
        ( cos ( arg ) ) + ( axis.at ( 1 ) * axis.at ( 1 ) * ( 1 - cos ( arg ) ) ),
        ( axis.at ( 1 ) * axis.at ( 2 ) * ( 1 - cos ( arg ) ) ) - ( axis.at ( 0 ) * sin ( arg ) ),

        ( axis.at ( 2 ) * axis.at ( 0 ) * ( 1 - cos ( arg ) ) ) - ( axis.at ( 1 ) * sin ( arg ) ),
        ( axis.at ( 2 ) * axis.at ( 1 ) * ( 1 - cos ( arg ) ) ) + ( axis.at ( 0 ) * sin ( arg ) ),
        ( cos ( arg ) ) + ( axis.at ( 2 ) * axis.at ( 2 ) * ( 1 - cos ( arg ) ) ) 
    } * trans;
}
inline glh::math::vector<3> glh::math::rotate ( const vector<3>& vec, const double arg, const vector<3>& axis )
{
    /* return the vector multiplied by the rotational matrix */
    return rotate ( identity<3> (), arg, axis ) * vec;
}
inline glh::math::matrix<4> glh::math::rotate ( const matrix<4>& trans, const double arg, const vector<3>& axis )
{
    /* return the new transformation matrix */
    return matrix<4>
    {
        ( cos ( arg ) ) + ( axis.at ( 0 ) * axis.at ( 0 )  * ( 1 - cos ( arg ) ) ),
        ( axis.at ( 0 ) * axis.at ( 1 ) * ( 1 - cos ( arg ) ) ) - ( axis.at ( 2 ) * sin ( arg ) ),
        ( axis.at ( 0 ) * axis.at ( 2 ) * ( 1 - cos ( arg ) ) ) + ( axis.at ( 1 ) * sin ( arg ) ),
        0,

        ( axis.at ( 1 ) * axis.at ( 0 ) * ( 1 - cos ( arg ) ) ) + ( axis.at ( 2 ) * sin ( arg ) ),
        ( cos ( arg ) ) + ( axis.at ( 1 ) * axis.at ( 1 ) * ( 1 - cos ( arg ) ) ),
        ( axis.at ( 1 ) * axis.at ( 2 ) * ( 1 - cos ( arg ) ) ) - ( axis.at ( 0 ) * sin ( arg ) ),
        0,

        ( axis.at ( 2 ) * axis.at ( 0 ) * ( 1 - cos ( arg ) ) ) - ( axis.at ( 1 ) * sin ( arg ) ),
        ( axis.at ( 2 ) * axis.at ( 1 ) * ( 1 - cos ( arg ) ) ) + ( axis.at ( 0 ) * sin ( arg ) ),
        ( cos ( arg ) ) + ( axis.at ( 2 ) * axis.at ( 2 ) * ( 1 - cos ( arg ) ) ),
        0,

        0, 0, 0, 1
    } * trans;
}
inline glh::math::vector<4> glh::math::rotate ( const vector<4>& vec, const double arg, const vector<3>& axis )
{
    /* return the vector multiplied by the rotational matrix */
    return rotate ( identity<4> (), arg, axis ) * vec;
}

/* translate
 *
 * translate a matrix or vector
 * 
 * trans/vec: the translation matrix/vector to translate
 * translation: the amount to translate by
 * axis: the axis of translation
 * 
 * return: the new transformation matrix/vector 
 */
template<unsigned M> inline glh::math::matrix<M> glh::math::translate ( const matrix<M>& trans, const double translation, const double axis )
{
    /* create new matrix */
    matrix<M> result { trans };

    /* add the translation */
    result.at ( axis, M - 1 ) += translation;

    /* return the matrix */
    return result;
}
template<unsigned M> inline glh::math::vector<M> glh::math::translate ( const vector<M>& vec, const double translation, const double axis )
{
    /* create new vector */
    vector<M> result { vec };

    /* add the translation */
    vec.at ( axis ) += translation;

    /* return vec */
    return vec;
}

/* translate with vector
 *
 * translate along all axis simultaneously
 * matrices are assumed to have a homogenmous coordinate associated with them
 * 
 * trans/vec: the transformation matrix/vector to translate
 * translation: vector for the translation
 * 
 * return: the new transformation matrix/vector
 */
template<unsigned M> inline glh::math::matrix<M> glh::math::translate ( const matrix<M>& trans, const vector<M - 1>& translation )
{
    /* create new matrix */
    matrix<M> result { trans };

    /* apply translation */
    for ( unsigned iti = 0; iti < M - 1; ++iti ) result.at ( iti, M - 1 ) += translation.at ( iti );

    /* return result */
    return result;
}
template<unsigned M> inline glh::math::vector<M> glh::math::translate ( const vector<M>& vec, const vector<M>& translation )
{
    /* return the sum of the two vectors */
    return vec + translation;
}



/* perspective
 *
 * create a perspective projection matrix
 * 
 * l: the (x) position of the LEFT of the near rectangle of the frustum
 * r: the (x) position of the RIGHT of the near rectangle of the frustum
 * b: the (y) position of the BOTTOM of the near rectangle of the frustum
 * t: the (y) position of the TOP of the near rectangle of the frustum
 * n: the (z) position of the NEAR rectangle of the frustum
 * f: the (z) position of the FAR rectangle of the frustum
 * 
 * return: the perspective projection matrix
 */
inline glh::math::matrix<4> glh::math::perspective ( const double l, const double r, const double b, const double t, const double n, const double f )
{
    /* return the new matrix */
    return matrix<4>
    {
        ( 2 * n ) / ( r - l ),           0,            ( r + l ) / ( r - l ),             0,
                  0,           ( 2 * n ) / ( t - b ),  ( t + b ) / ( t - b ),             0,
                  0,                     0,           -( f + n ) / ( f - n ), -( 2 * f * n ) / ( f - n ), 
                  0,                     0,                       -1,                      0
    };
}

/* perspective_fov
 *
 * create a perspective projection matrix from a field of view
 * 
 * fov: the field of view in radians
 * aspect: the aspect ratio of the screen
 * n: the distance to the near rectangle
 * f: the distance to the far rectangle
 * 
 * return: the perspective projection matrix
 */
inline glh::math::matrix<4> glh::math::perspective_fov ( const double fov, const double aspect, const double n, const double f )
{
    /* calculate the right position */
    const double r = n * tan ( fov / 2 );
    /* call perspective */
    return perspective ( -r, r, - r / aspect, r / aspect, n, f );
}

/* camera
 *
 * produce a camera matrix from vectors
 * 
 * p: position (non-unit) vector
 * x,y,z: unit axis for the camera
 * 
 * return: camera matrix based on vectors provided
 */
inline glh::math::matrix<4> glh::math::camera ( const vector<3>& p, const vector<3>& x, const vector<3>& y, const vector<3>& z )
{
    /* return the camera matrix */
    return matrix<4>
    {
        x.at ( 0 ), x.at ( 1 ), x.at ( 2 ), 0,
        y.at ( 0 ), y.at ( 1 ), y.at ( 2 ), 0,
        z.at ( 0 ), z.at ( 1 ), z.at ( 2 ), 0,
            0     ,     0     ,     0     , 1
    } * matrix<4>
    {
        1, 0, 0, -p.at ( 0 ),
        0, 1, 0, -p.at ( 1 ),
        0, 0, 1, -p.at ( 2 ),
        0, 0, 0,      1
    };
}

/* look_at
 *
 * produce a camera matrix based on a camera position, target position and an up vector
 * 
 * p: camera (non-unit) position vector
 * t: target (non-unit) position vector
 * wup: WORLD up unit vector
 * 
 * return: camera matrix based on vectors provided
 */
inline glh::math::matrix<4> glh::math::look_at ( const vector<3>& p, const vector<3>& t, const vector<3>& wup )
{
    /* d = norm ( p - t )
     * r = norm ( wup x d )
     * u = d x r
     */
    const vector<3> d = norm ( p - t );
    const vector<3> r = cross ( wup, d );
    const vector<3> u = cross ( d, r );
    /* return the camera matrix */
    return camera ( p, d, r, u );
}

/* look_towards
 *
 * produce a camera matrix based on a camera position, direction of viewing and up vector
 * 
 * p: camera (non-unit) position vector
 * d: direction unit vector
 * wup: WORLD up unit vector
 * 
 * return: camera matrix based on vectors provided
 */
inline glh::math::matrix<4> glh::math::look_towards ( const vector<3>& p, const vector<3>& d, const vector<3>& wup )
{
    /* r = wup x d
     * u = d x r
     */
    const vector<3> r = cross ( wup, d );
    const vector<3> u = cross ( d, r );
    /* return the camera matrix */
    return camera ( p, -d, r, u );
}




/* VECTOR-MATRIX OPERATOR IMPLEMENTATIONS */

/* operator*
 *
 * multiplication of a matrix before a vector
 */
template<unsigned M, unsigned N> inline glh::math::vector<M> operator* ( const glh::math::matrix<M, N>& lhs, const glh::math::vector<N>& rhs )
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
template<unsigned M> inline glh::math::vector<M>& operator*= ( const glh::math::vector<M>& lhs, const glh::math::matrix<M>& rhs )
{
    /* return lhs = rhs * lhs */
    return ( lhs = rhs * lhs );
}




/* #ifndef GLHELPER_TRANSFORM_HPP_INCLUDED */
#endif