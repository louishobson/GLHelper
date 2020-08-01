/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * include/glhelper/glhelper_vertices.hpp
 * 
 * defines several arrays of vertex and element data for different 3d shapes
 *
 */



#ifndef GLHELPER_SHAPES_HPP_INCLUDED
#define GLHELPER_SHAPES_HPP_INCLUDED



/* INCLUDES */

/* include core headers */
#include <array>
#include <iostream>
#include <vector>

/* include glhelper_core.hpp */
#include <glhelper/glhelper_core.hpp>

/* include glhelper_buffer.hpp */
#include <glhelper/glhelper_buffer.hpp>



/* NAMESPACE DECLARATIONS */

namespace glh
{
    namespace vertices
    {
        /* square_vertex_data
         * square_vertex_normal_data
         * square_vertex_normal_texcoord_data
         * 
         * arrays containing vertex (+ normal (+ texcoord)) data for a square
         * all sides are length 2, z component is 0.0 for all vertices, normals point along +z axis
         */
        extern const std::array<GLfloat, 3 * 4 * 1> square_vertex_data;
        extern const std::array<GLfloat, 6 * 4 * 1> square_vertex_normal_data;
        extern const std::array<GLfloat, 8 * 4 * 1> square_vertex_normal_texcoord_data;

        /* square_element_data
         *
         * elements for square vertex data
         */
        extern const std::array<GLuint, 3 * 2 * 1> square_element_data;

        /* cube_vertex_data
         * cube_reverse_vertex_data
         * cube_vertex_normal_data
         * cube_reverse_vertex_normal_data
         * cube_vertex_normal_texcoord_data
         * cube_reverse_vertex_normal_texcoord_data
         *
         * arrays containing vertex (+ normal (+ texcoord)) data for a cube
         * all sides are length 2, centred at the origin
         */
        extern const std::array<GLfloat, 3 * 4 * 6> cube_vertex_data;
        extern const std::array<GLfloat, 3 * 4 * 6> cube_reverse_vertex_data;
        extern const std::array<GLfloat, 6 * 4 * 6> cube_vertex_normal_data;
        extern const std::array<GLfloat, 6 * 4 * 6> cube_reverse_vertex_normal_data;
        extern const std::array<GLfloat, 9 * 4 * 6> cube_vertex_normal_texcoord_data;
        extern const std::array<GLfloat, 9 * 4 * 6> cube_reverse_vertex_normal_texcoord_data;

        /* cube_element_data
         *
         * elements for cube vertex data
         */
        extern const std::array<GLuint, 3 * 2 * 6> cube_element_data;
    }
}



/* #ifndef GLHELPER_SHAPES_HPP_INCLUDED */
#endif