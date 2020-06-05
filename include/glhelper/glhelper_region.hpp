/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * include/glhelper/glhelper_region.hpp
 * 
 * defines constructs to represent regions in multi-dimensional space
 * notable constructs include:
 * 
 * 
 * 
 * STRUCT GLH::REGION::UNIFORM_REGION
 * 
 * represents a region centred on a point with a given radius
 * there are several typedefs with intuitive names for common dimensions
 * 
 */



/* HEADER GUARD */
#ifndef GLHELPER_REGION_HPP_INCLUDED
#define GLHELPER_REGION_HPP_INCLUDED



/* INCLUDES */

/* include core headers */
#include <algorithm>
#include <array>
#include <iostream>
#include <type_traits>

/* include glhelper_core.hpp */
#include <glhelper/glhelper_core.hpp>

/* include glhelper_math.hpp */
#include <glhelper/glhelper_math.hpp>



/* NAMESPACE DECLARATIONS */

namespace glh
{
    namespace region
    {
        /* TYPES AND CLASSES */

        /* struct uniform_region
         *
         * a region with a centre and radius
         * 1D = segment, 2D = circle, 3D = sphere, etc.
         */
        template<unsigned M, class T = double> struct uniform_region;

        /* typedefs for uniform regions */
        template<class T = double> using segment_region = uniform_region<1, T>;
        template<class T = double> using circular_region = uniform_region<2, T>;
        template<class T = double> using spherical_region = uniform_region<3, T>;


        
        /* MODIFIER FUNCTIONS DECLARATIONS */

        /* is_contained
         *
         * returns true if lhs is fully contained inside rhs
         * this function is not commutative
         */
        template<unsigned M, class T0, class T1> bool is_contained ( const uniform_region<M, T0>& lhs, const uniform_region<M, T1>& rhs );

        /* is_overlapping
         *
         * returns true if either regions overlap
         * this function is commutative
         */
        template<unsigned M, class T0, class T1> bool is_overlapping ( const uniform_region<M, T0>& lhs, const uniform_region<M, T1>& rhs );

        /* combine
         *
         * combine two regions to create a region which encompasses both of them
         */
        template<unsigned M, class T0, class T1> uniform_region<M, meta::pat_t<T0, T1>> combine ( const uniform_region<M, T0>& lhs, const uniform_region<M, T1>& rhs );
    }
}

/* UNIFORM_REGION OPERATORS DECLARATIONS */

/* operator==
 *
 * returns true if the regions are the same
 */
template<unsigned M, class T0, class T1> bool operator== ( const glh::region::uniform_region<M, T0>& lhs, const glh::region::uniform_region<M, T0>& rhs );

/* operator*
 *
 * calculates a new region based on a transformation matrix
 */
template<unsigned M, class T0, class T1> glh::region::uniform_region<M, glh::meta::pat_t<T0, T1>> operator* ( const glh::math::matrix<M, M, T0>& lhs, const glh::region::uniform_region<M, T1>& rhs );
template<unsigned M, class T0, class T1> glh::region::uniform_region<M, glh::meta::pat_t<T0, T1>> operator* ( const glh::math::matrix<M + 1, M + 1, T0>& lhs, const glh::region::uniform_region<M, T1>& rhs );



/* UNIFORM_REGION DEFINITION */

/* class uniform_region
 *
 * a region with a centre and radius
 * 1D = segment, 2D = circle, 3D = sphere, etc.
 */
template<unsigned M, class T> struct glh::region::uniform_region
{

    /* static assert that M > 0 */
    static_assert ( M > 0, "a region must cannot have a dimension of 0" );

    /* assert that T is arithmetic */
    static_assert ( std::is_arithmetic<T>::value, "a region cannot be instantiated from a non-arithmetic type" );

public:

    /* full constructor
     *
     * set centre and radius
     * 
     * _centre: centre of the region
     * _radius: radius of the region
     */
    uniform_region ( const math::vector<M, T>& _centre, const T& _radius )
        : centre { _centre }
        , radius { _radius }
    {}

    /* zero-parameter constructor
     *
     * set centre and radius to origin and zero respectively
     */
    uniform_region ()
        : centre { 0 }
        , radius { 0 }
    {}

    /* copy constructor
     *
     * can be from a different type of T
     */
    template<class _T> uniform_region ( const uniform_region<M, _T>& other )
        : centre { other.centre }
        , radius { other.radius }
    {}

    /* copy assignment operator
     *
     * can be from a different type of T
     */
    template<class _T> uniform_region& operator= ( const uniform_region<M, _T>& other )
    { centre = other.centre; radius = other.radius; }

    /* default destructor */
    ~uniform_region () = default;



    /* the centre of the region */
    math::vector<M, T> centre;

    /* the radius of the region */
    T radius;

};



/* MODIFIER FUNCTIONS DECLARATIONS */

/* is_contained
 *
 * returns true if lhs is fully contained inside rhs
 * this function is not commutative
 */
template<unsigned M, class T0, class T1> inline bool glh::region::is_contained ( const uniform_region<M, T0>& lhs, const uniform_region<M, T1>& rhs )
{
    /* return true if the distance between lhs and rhs + the radius of lhs is less than or equal to the radius of rhs */
    return ( math::modulus ( rhs.centre - lhs.centre ) + lhs.radius <= rhs.radius );
}

/* is_overlapping
 *
 * returns true if either regions overlap
 * this function is commutative
 */
template<unsigned M, class T0, class T1> inline bool glh::region::is_overlapping ( const uniform_region<M, T0>& lhs, const uniform_region<M, T1>& rhs )
{
    /* return true if the distance is less than the sum of the radii */
    return ( math::modulus ( rhs.centre - lhs.centre ) < lhs.radius + rhs.radius );
}

/* combine
 *
 * combine two regions to create a region which encompasses both of them
 */
template<unsigned M, class T0, class T1> inline glh::region::uniform_region<M, glh::meta::pat_t<T0, T1>> glh::region::combine ( const uniform_region<M, T0>& lhs, const uniform_region<M, T1>& rhs )
{
    /* if either is contained within the other, return the encompassing region */
    if ( is_contained ( lhs, rhs ) ) return rhs;
    if ( is_contained ( rhs, lhs ) ) return lhs;

    /* get a vector from  lhs to rhs, and find the length */
    const auto difference = rhs.centre - lhs.centre;
    const auto distance = math::modulus ( difference );

    /* normalise difference */
    const auto norm_difference = math::normalise ( difference );

    /* create the new region and return it */
    return uniform_region<M, meta::pat_t<T0, T1>>
    {
        lhs.centre - ( norm_difference * lhs.radius ) + ( ( ( lhs.radius + distance + rhs.radius ) / 2.0 ) * norm_difference ),
        ( lhs.radius + distance + rhs.radius ) / 2.0
    };
}



/* operator*
 *
 * calculates a new region based on a transformation matrix
 */
template<unsigned M, class T0, class T1> inline glh::region::uniform_region<M, glh::meta::pat_t<T0, T1>> operator* ( const glh::math::matrix<M, M, T0>& lhs, const glh::region::uniform_region<M, T1>& rhs )
{
    /* get the stretch of each axis */
    std::array<glh::meta::pat_t<T0, T1>, M> stretches;
    for ( unsigned i = 0; i < M; ++i ) stretches.at ( i ) = glh::math::modulus ( glh::math::promote_vector<M, T0, T1> ( glh::math::column_vector ( lhs, i ) ) );

    /* apply the transformation to the centre of the region and alter the radius by the maximum stretch */
    return glh::region::uniform_region<M, glh::meta::pat_t<T0, T1>> 
    {
        lhs * rhs.centre,
        std::max_element ( stretches.begin (), stretches.end () ) * rhs.radius
    };
}
template<unsigned M, class T0, class T1> inline glh::region::uniform_region<M, glh::meta::pat_t<T0, T1>> operator* ( const glh::math::matrix<M + 1, M + 1, T0>& lhs, const glh::region::uniform_region<M, T1>& rhs )
{
    /* get the stretch of each axis */
    std::array<glh::meta::pat_t<T0, T1>, M> stretches;
    for ( unsigned i = 0; i < M; ++i ) stretches.at ( i ) = glh::math::modulus ( glh::math::promote_vector<M, T0, T1> ( glh::math::column_vector ( lhs, i ) ) );

    /* apply the transformation to the centre of the region and alter the radius by the maximum stretch */
    return glh::region::uniform_region<M, glh::meta::pat_t<T0, T1>> 
    {
        lhs * rhs.centre,
        std::max_element ( stretches.begin (), stretches.end () ) * rhs.radius
    };
}






/* #ifndef GLHELPER_REGION_HPP_INCLUDED */
#endif