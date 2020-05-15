/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * include/glhelper/glhelper_transform.hpp
 * 
 * implements functions to aid transformation calculations
 * notable functions include (all in namespace glh::math)
 * 
 * PI: multiply a double by pi
 * RAD: convert degrees to radians
 * DEG: convert radians to degrees
 * ZERO_MATRIX: return a zero matrix of a supplied size
 * IDENTITY: return an identity matrix of a supplied size
 * RESIZE: increase of decrease the size of a square matrix, filling in the diagonal with 1s when increasing
 * STRETCH: generic stretching of a matrix or vector
 * STRETCH3D: strecthing specifically in 3d space
 * ENLARGE: generic enlarging of a matrix or vector
 * ENLARGE3D: enlarging specifically in 3d space
 * ROTATE: generic rotation transformation of a matrix or vector
 * ROTATE3D: rotation transformation specifically in 3d space
 * TRANSLATE: generic translation of a vector or matrix (assime affine matrix)
 * TRANSLATE3D: translation specifically in 3d space
 * REFLECT3D: reflection specifically in 3d space
 * PERSPECTIVE: generate a perspective projection matrix based on the position and size of near and far planes
 * PERSPECTIVE_FOV: generate a perspective projection matrix based on an fov angle
 * CAMERA: generate a view matrix based on a camera position and unit axis
 * LOOK_AT: generate a view matrix based on a camera position, focus point and world up unit vector
 * LOOK_ALONG: generate a view matrix based on a camera position, direction of viewing and world up unit vector
 * NORMAL: generate a normal matrix based on a model-view matrix
 * OPERATOR*: for multiplying vectors by matrices to apply transformations 
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

/* include glhelper_matrix.hpp */
#include <glhelper/glhelper_matrix.hpp>

/* include glhelper_vector.hpp */
#include <glhelper/glhelper_vector.hpp>



/* NAMESPACE DECLARATIONS */

namespace glh
{
    namespace math
    {
        /* pi
         *
         * return: the value of pi multiplied by a constant
         */
        inline double pi ( const double k = 1.0 ) { return k * acos ( -1. ); }

        /* rad
         *
         * convert degrees to radians
         * 
         * return: a conversion of deg into radians
         */
        inline double rad ( const double degrees ) { return pi ( degrees / 180 ); }

        /* deg
         *
         * convert radians to degrees
         * 
         * return: a conversion of deg into radians
         */
        inline double deg ( const double radians ) { return ( radians / pi () ) * 180; }

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

        /* resize
         *
         * promote/demote a matrix' size
         * a size promotion leaves the new rows/columns as like in an identity matrix
         */
        template<unsigned _M, unsigned M> matrix<_M> resize ( const matrix<M>& trans );

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
         * 
         * return: the new transformation matrix/vector
         */
        template<unsigned M> matrix<M> stretch ( const matrix<M>& trans, const vector<M>& sfs );
        template<unsigned M> vector<M> stretch ( const vector<M>& vec, const vector<M>& sfs );

        /* stretch3d
         *
         * stretch functions specifically for 3d systems
         * 
         * trans/vec: the transformation matrix/vector to stretch
         * sfs: the stretches to apply to each axis
         * 
         * return: the new transformation matrix/vector
         */
        matrix<3> stretch3d ( const matrix<3>& trans, const vector<3>& sfs );
        vector<3> stretch3d ( const vector<3>& vec, const vector<3>& sfs );
        matrix<4> stretch3d ( const matrix<4>& trans, const vector<3>& sfs );
        vector<4> stretch3d ( const vector<4>& vec, const vector<3>& sfs );

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

        /* enlarge3d
         *
         * enlarge 3d coordinate system by scale factor
         * 
         * trans/vec: the transformation matrix/vector to stretch
         * sfs: the stretches to apply to each axis
         * 
         * return: the new transformation matrix/vector
         */
        matrix<3> enlarge3d ( const matrix<3>& trans, const double sf );
        vector<3> enlarge3d ( const vector<3>& vec, const double sf );
        matrix<4> enlarge3d ( const matrix<4>& trans, const double sf );
        vector<4> enlarge3d ( const vector<4>& vec, const double sf );
        
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
    
        /* rotate3d
         *
         * rotate along several axis simultaneously
         * only for 3d coordinate systems
         * 
         * trans/vec: the transformation matrix/vector to rotate
         * arg: the angle to rotate by in radians
         * axis: the unit axis to rotate in
         * 
         * return: the new transformation matrix/vector
         */
        matrix<3> rotate3d ( const matrix<3>& trans, const double arg, const vector<3>& axis );
        vector<3> rotate3d ( const vector<3>& vec, const double arg, const vector<3>& axis );
        matrix<4> rotate3d ( const matrix<4>& trans, const double arg, const vector<3>& axis );
        vector<4> rotate3d ( const vector<4>& vec, const double arg, const vector<3>& axis );

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

        /* translate3d
         *
         * translate 3d coordinates
         * only for homogenous coordinate systems
         * 
         * trans/vec: the transformation matrix/vector to translate
         * translation: vector for the translation
         * 
         * return: the new transformation matrix/vector
         */
        vector<3> translate3d ( const vector<3>& vec, const vector<3>& translation );
        matrix<4> translate3d ( const matrix<4>& trans, const vector<3>& translation );
        vector<4> translate3d ( const vector<4>& vec, const vector<3>& translation );

        /* reflect3d
         *
         * reflect in a plane at pos described by a normal vector
         * 
         * trans/vec: the transformation matrix/vector to reflect
         * norm: a normal unit vector on the plane
         * pos: position of the plane
         *
         * return: the new transformation matrix/vector
         */
        matrix<3> reflect3d ( const matrix<3>& trans, const vector<3>& norm );
        vector<3> reflect3d ( const vector<3>& vec, const vector<3>& norm, const vector<3>& pos = vector<3> { 0.0 } );
        matrix<4> reflect3d ( const matrix<4>& trans, const vector<3>& norm, const vector<3>& pos = vector<3> { 0.0 } );
        vector<4> reflect3d ( const vector<4>& vec, const vector<3>& norm, const vector<3>& pos = vector<3> { 0.0 } );



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
         * fbx: if the direction of viewing is along wup, this x unit vector will be used to determine the rotation
         * 
         * return: camera matrix based on vectors provided
         */
        matrix<4> look_at ( const vector<3>& p, const vector<3>& t, const vector<3>& wup, const vector<3>& fbx = vector<3> { 1.0, 0.0, 0.0 } );

        /* look_along
         *
         * produce a camera matrix based on a camera position, direction of viewing and up vector
         * 
         * p: camera (non-unit) position vector
         * d: direction unit vector
         * wup: WORLD up unit vector
         * fbx: if the direction of viewing is along wup, this x unit vector will be used to determine the rotation
         * 
         * return: camera matrix based on vectors provided
         */
        matrix<4> look_along ( const vector<3>& p, const vector<3>& d, const vector<3>& wup, const vector<3>& fbx = vector<3> { 1.0, 0.0, 0.0 } );

        /* normal
         *
         * produce a normal matrix from a model-view matrix
         * 
         * trans: the model-view matrix to change to a normal matrix
         * 
         * return: the mormal matrix
         */
        matrix<3> normal ( const matrix<4>& trans );


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
    for ( unsigned i = 0; i < M; ++i ) identity.at ( i, i ) = 1.;

    /* return identity matrix */
    return identity;
}

/* resize
 *
 * promote/demote a matrix' size
 * a size promotion leaves the new rows/columns as like in an identity matrix
 */
template<unsigned _M, unsigned M> inline glh::math::matrix<_M> glh::math::resize ( const matrix<M>& trans )
{
    /* result matrix */
    matrix<_M> result { identity<_M> () };

    /* iterate over the smaller of _M and _N and copy values */
    for ( unsigned i = 0; i < std::min ( _M, M ); ++i ) for ( unsigned j = 0; j < std::min ( _M, M ); ++j )
    {
        result.at ( i, j ) = trans.at ( i, j );
    }

    /* return result */
    return result;
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
    for ( unsigned i = 0; i < M; ++i ) result.at ( i, axis ) *= sf;

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
    for ( unsigned i = 0; i < M; ++i ) for ( unsigned j = 0; j < M; ++j )
    {
        result.at ( i, j ) *= sfs.at ( j );
    }

    /* return result */
    return result;
}
template<unsigned M> inline glh::math::vector<M> glh::math::stretch ( const vector<M>& vec, const vector<M>& sfs )
{
    /* multiply vectors and return */
    return vec * sfs;
}

/* stretch3d
 *
 * stretch functions specifically for 3d systems
 * 
 * trans/vec: the transformation matrix/vector to stretch
 * sfs: the stretches to apply to each axis
 * 
 * return: the new transformation matrix/vector
 */
inline glh::math::matrix<3> glh::math::stretch3d ( const matrix<3>& trans, const vector<3>& sfs )
{
    /* the same as the default stretch function */
    return stretch ( trans, sfs );
}
inline glh::math::vector<3> glh::math::stretch3d ( const vector<3>& vec, const vector<3>& sfs )
{
    /* same as the default stretch function */
    return stretch ( vec, sfs );
}
inline glh::math::matrix<4> glh::math::stretch3d ( const matrix<4>& trans, const vector<3>& sfs )
{
    /* create new matrix */
    matrix<4> result { trans };

    /* add stretches to 3x3 region of matrix */
    for ( unsigned i = 0; i < 3; ++i ) for ( unsigned j = 0; j < 3; ++j )
    {
        result.at ( i, j ) *= sfs.at ( j );
    }

    /* return result */
    return result;
}
inline glh::math::vector<4> glh::math::stretch3d ( const vector<4>& vec, const vector<3>& sfs )
{
    /* multiply vectors, adding component to sfs */
    return vec * vector<4> { sfs, 1.0 };
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

/* enlarge3d
 *
 * enlarge 3d coordinate system by scale factor
 * 
 * trans/vec: the transformation matrix/vector to stretch
 * sfs: the stretches to apply to each axis
 * 
 * return: the new transformation matrix/vector
 */
inline glh::math::matrix<3> glh::math::enlarge3d ( const matrix<3>& trans, const double sf )
{
    /* same as normal enlarge function */
    return enlarge ( trans, sf );
}
inline glh::math::vector<3> glh::math::enlarge3d ( const vector<3>& vec, const double sf )
{
    /* same as normal enlarge function */
    return enlarge ( vec, sf );
}
inline glh::math::matrix<4> glh::math::enlarge3d ( const matrix<4>& trans, const double sf )
{
    /* create new matrix */
    matrix<4> result { trans };

    /* multiply upper left 3x3 region by scale factor */
    for ( unsigned i = 0; i < 3; ++i ) for ( unsigned j = 0; j < 3; ++j ) result.at ( i, j ) *= sf;

    /* return result */
    return result;
}
inline glh::math::vector<4> glh::math::enlarge3d ( const vector<4>& vec, const double sf )
{
    /* multiply vec by sf */
    return vec * vector<4> { sf, sf, sf, 1.0 };
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

/* rotate3d
 *
 * rotate along several axis simultaneously
 * only for 3d coordinate systems
 * 
 * trans/vec: the transformation matrix/vector to rotate
 * arg: the angle to rotate by in radians
 * axis: the unit axis to rotate in
 * 
 * return: the new transformation matrix/vector
 */
inline glh::math::matrix<3> glh::math::rotate3d ( const matrix<3>& trans, const double arg, const vector<3>& axis )
{
    /* return the new transformation matrix */
    return matrix<3>
    {
        ( std::cos ( arg ) ) + ( axis.at ( 0 ) * axis.at ( 0 )  * ( 1 - std::cos ( arg ) ) ),
        ( axis.at ( 0 ) * axis.at ( 1 ) * ( 1 - std::cos ( arg ) ) ) - ( axis.at ( 2 ) * std::sin ( arg ) ),
        ( axis.at ( 0 ) * axis.at ( 2 ) * ( 1 - std::cos ( arg ) ) ) + ( axis.at ( 1 ) * std::sin ( arg ) ),

        ( axis.at ( 1 ) * axis.at ( 0 ) * ( 1 - std::cos ( arg ) ) ) + ( axis.at ( 2 ) * std::sin ( arg ) ),
        ( std::cos ( arg ) ) + ( axis.at ( 1 ) * axis.at ( 1 ) * ( 1 - std::cos ( arg ) ) ),
        ( axis.at ( 1 ) * axis.at ( 2 ) * ( 1 - std::cos ( arg ) ) ) - ( axis.at ( 0 ) * std::sin ( arg ) ),

        ( axis.at ( 2 ) * axis.at ( 0 ) * ( 1 - std::cos ( arg ) ) ) - ( axis.at ( 1 ) * std::sin ( arg ) ),
        ( axis.at ( 2 ) * axis.at ( 1 ) * ( 1 - std::cos ( arg ) ) ) + ( axis.at ( 0 ) * std::sin ( arg ) ),
        ( std::cos ( arg ) ) + ( axis.at ( 2 ) * axis.at ( 2 ) * ( 1 - std::cos ( arg ) ) ) 
    } * trans;
}
inline glh::math::vector<3> glh::math::rotate3d ( const vector<3>& vec, const double arg, const vector<3>& axis )
{
    /* return the vector multiplied by the rotational matrix */
    return rotate3d ( identity<3> (), arg, axis ) * vec;
}
inline glh::math::matrix<4> glh::math::rotate3d ( const matrix<4>& trans, const double arg, const vector<3>& axis )
{
    /* return the new transformation matrix */
    return matrix<4>
    {
        ( std::cos ( arg ) ) + ( axis.at ( 0 ) * axis.at ( 0 )  * ( 1 - std::cos ( arg ) ) ),
        ( axis.at ( 0 ) * axis.at ( 1 ) * ( 1 - std::cos ( arg ) ) ) - ( axis.at ( 2 ) * std::sin ( arg ) ),
        ( axis.at ( 0 ) * axis.at ( 2 ) * ( 1 - std::cos ( arg ) ) ) + ( axis.at ( 1 ) * std::sin ( arg ) ),
        0,

        ( axis.at ( 1 ) * axis.at ( 0 ) * ( 1 - std::cos ( arg ) ) ) + ( axis.at ( 2 ) * std::sin ( arg ) ),
        ( std::cos ( arg ) ) + ( axis.at ( 1 ) * axis.at ( 1 ) * ( 1 - std::cos ( arg ) ) ),
        ( axis.at ( 1 ) * axis.at ( 2 ) * ( 1 - std::cos ( arg ) ) ) - ( axis.at ( 0 ) * std::sin ( arg ) ),
        0,

        ( axis.at ( 2 ) * axis.at ( 0 ) * ( 1 - std::cos ( arg ) ) ) - ( axis.at ( 1 ) * std::sin ( arg ) ),
        ( axis.at ( 2 ) * axis.at ( 1 ) * ( 1 - std::cos ( arg ) ) ) + ( axis.at ( 0 ) * std::sin ( arg ) ),
        ( std::cos ( arg ) ) + ( axis.at ( 2 ) * axis.at ( 2 ) * ( 1 - std::cos ( arg ) ) ),
        0,

        0, 0, 0, 1
    } * trans;
}
inline glh::math::vector<4> glh::math::rotate3d ( const vector<4>& vec, const double arg, const vector<3>& axis )
{
    /* return the vector multiplied by the rotational matrix */
    return rotate3d ( identity<4> (), arg, axis ) * vec;
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
    for ( unsigned i = 0; i < M - 1; ++i ) result.at ( i, M - 1 ) += translation.at ( i );

    /* return result */
    return result;
}
template<unsigned M> inline glh::math::vector<M> glh::math::translate ( const vector<M>& vec, const vector<M>& translation )
{
    /* return the sum of the two vectors */
    return vec + translation;
}

/* translate3d
 *
 * translate 3d coordinates
 * only for homogenous coordinate systems
 * 
 * trans/vec: the transformation matrix/vector to translate
 * translation: vector for the translation
 * 
 * return: the new transformation matrix/vector
 */
inline glh::math::vector<3> glh::math::translate3d ( const vector<3>& vec, const vector<3>& translation )
{
    /* return the sum of the two vectors */
    return vec + translation;
}
inline glh::math::matrix<4> glh::math::translate3d ( const matrix<4>& trans, const vector<3>& translation )
{
    /* create new matrix */
    matrix<4> result { trans };

    /* add translations */
    result.at ( 0, 3 ) = translation.at ( 0 );
    result.at ( 1, 3 ) = translation.at ( 1 );
    result.at ( 2, 3 ) = translation.at ( 2 );

    /* return new matrix */
    return result;
}
inline glh::math::vector<4> glh::math::translate3d ( const vector<4>& vec, const vector<3>& translation )
{
    /* return the sum of the two vectors */
    return vec + vector<4> { translation, 0.0 };
}

/* reflect3d
 *
 * reflect in a plane at pos described by a normal vector
 * 
 * trans/vec: the transformation matrix/vector to reflect
 * norm: a normal unit vector on the plane
 * pos: position of the plane
 *
 * return: the new transformation matrix/vector
 */
inline glh::math::matrix<3> glh::math::reflect3d ( const matrix<3>& trans, const vector<3>& norm )
{
    /* return the reflection matrix */
    return matrix<3>
    {
        1 - ( 2 * norm.at ( 0 ) * norm.at ( 0 ) ),
        - ( 2 * norm.at ( 0 ) * norm.at ( 1 ) ),
        - ( 2 * norm.at ( 0 ) * norm.at ( 2 ) ),

        - ( 2 * norm.at ( 0 ) * norm.at ( 1 ) ),
        1 - ( 2 * norm.at ( 1 ) * norm.at ( 1 ) ),
        - ( 2 * norm.at ( 1 ) * norm.at ( 2 ) ),

        - ( 2 * norm.at ( 0 ) * norm.at ( 2 ) ),
        - ( 2 * norm.at ( 1 ) * norm.at ( 2 ) ),
        1 - ( 2 * norm.at ( 2 ) * norm.at ( 2 ) )
    } * trans;
}
inline glh::math::vector<3> glh::math::reflect3d ( const vector<3>& vec, const vector<3>& norm, const vector<3>& pos )
{
    /* reflect using matrix */
    return reflect3d ( vector<4> { vec, 1.0 }, norm, pos ).swizzle<0, 1, 2> ();
}
inline glh::math::matrix<4> glh::math::reflect3d ( const matrix<4>& trans, const vector<3>& norm, const vector<3>& pos )
{
    /* get the value of d */
    const double d = dot ( -pos, norm );

    /* return the reflection matrix */
    return matrix<4>
    {
        1 - ( 2 * norm.at ( 0 ) * norm.at ( 0 ) ),
        - ( 2 * norm.at ( 0 ) * norm.at ( 1 ) ),
        - ( 2 * norm.at ( 0 ) * norm.at ( 2 ) ),
        - ( 2 * d * norm.at ( 0 ) ),

        - ( 2 * norm.at ( 0 ) * norm.at ( 1 ) ),
        1 - ( 2 * norm.at ( 1 ) * norm.at ( 1 ) ),
        - ( 2 * norm.at ( 1 ) * norm.at ( 2 ) ),
        - ( 2 * d * norm.at ( 1 ) ),

        - ( 2 * norm.at ( 0 ) * norm.at ( 2 ) ),
        - ( 2 * norm.at ( 1 ) * norm.at ( 2 ) ),
        1 - ( 2 * norm.at ( 2 ) * norm.at ( 2 ) ),
        - ( 2 * d * norm.at ( 2 ) ),

        0, 0, 0, 1
    } * trans;
}
inline glh::math::vector<4> glh::math::reflect3d ( const vector<4>& vec, const vector<3>& norm, const vector<3>& pos )
{
    /* reflect using matrix */
    return reflect3d ( identity<4> (), norm, pos ) * vec;
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
    const double r = n * std::tan ( fov / 2 );
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
 * fbx: if the direction of viewing is along wup, this x unit vector will be used to determine the rotation
 * 
 * return: camera matrix based on vectors provided
 */
inline glh::math::matrix<4> glh::math::look_at ( const vector<3>& p, const vector<3>& t, const vector<3>& wup, const vector<3>& fbx )
{
    /* z = norm ( p - t )
     * if ( z.wup < 1.0 ) X = norm ( wup x z ) else X = fbx
     * y = z x X
     */
    const vector<3> z = normalise ( t - p );
    const vector<3> x = ( std::abs ( dot ( z, wup ) ) < 1.0 ? cross ( wup, z ) : fbx );
    const vector<3> y = cross ( z, x );
    /* return the camera matrix */
    return camera ( p, x, y, z );
}

/* look_along
 *
 * produce a camera matrix based on a camera position, direction of viewing and up vector
 * 
 * p: camera (non-unit) position vector
 * d: direction unit vector
 * wup: WORLD up unit vector
 * fbx: if the direction of viewing is along wup, this x unit vector will be used to determine the rotation
 * 
 * return: camera matrix based on vectors provided
 */
inline glh::math::matrix<4> glh::math::look_along ( const vector<3>& p, const vector<3>& d, const vector<3>& wup, const vector<3>& fbx )
{

    /* z = -d
     * if ( z.wup < 1.0 ) X = norm ( wup x z ) else X = fbx
     * y = z x X
     */
    const vector<3> z = -d;
    const vector<3> x = ( std::abs ( dot ( z, wup ) ) < 1.0 ? cross ( wup, z ) : fbx );
    const vector<3> y = cross ( z, x );
    /* return the camera matrix */
    return camera ( p, x, y, z );
}

/* normal
 *
 * produce a normal matrix from a model-view matrix
 * 
 * trans: the model-view matrix to change to a normal matrix
 * 
 * return: the mormal matrix
 */
inline glh::math::matrix<3> glh::math::normal ( const matrix<4>& trans )
{
    /* return the transpose of the inverse of the top left 3x3 submatrix of trans */
    return transpose ( inverse ( resize<3> ( trans ) ) );
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
    for ( unsigned i = 0; i < M; ++i ) for ( unsigned j = 0; j < N; ++j )
    {
        /* keep adding to the values in result */
        result.at ( i ) += lhs.at ( i, j ) * rhs.at ( j );
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