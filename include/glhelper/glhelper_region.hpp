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

    }
}



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



/* #ifndef GLHELPER_REGION_HPP_INCLUDED */
#endif