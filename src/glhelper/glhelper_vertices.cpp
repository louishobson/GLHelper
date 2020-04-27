/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * src/glhelper/glhelper_vertices.cpp
 * 
 * implementation of include/glhelper/glhelper_vertices.hpp
 *
 */



/* INCLUDES */

/* include glhelper_vertices.hpp */
#include <glhelper/glhelper_vertices.hpp>



/* square_vertex_data
 * square_vertex_normal_data
 * square_vertec_normal_texpos_data
 * 
 * arrays containing vertex (+ normal (+ texcoord)) data for a square
 * all sides are length 1, z component is 0.0 for all vertices, normals point along +z axis
 */
const std::array<GLfloat, 3 * 4 * 1> glh::vertices::square_vertex_data
{
    /* position */ 
     0.5,  0.5,  0.0,
    -0.5,  0.5,  0.0,
    -0.5, -0.5,  0.0,
     0.5, -0.5,  0.0
};
const std::array<GLfloat, 6 * 4 * 1> glh::vertices::square_vertex_normal_data
{
    /* position          normal */ 
     0.5,  0.5,  0.0,    0.0,  0.0,  1.0,
    -0.5,  0.5,  0.0,    0.0,  0.0,  1.0,
    -0.5, -0.5,  0.0,    0.0,  0.0,  1.0,
     0.5, -0.5,  0.0,    0.0,  0.0,  1.0
};
const std::array<GLfloat, 9 * 4 * 1> glh::vertices::square_vertex_normal_texcoord_data
{
    /* position          normal              texcoords */
     0.5,  0.5,  0.0,    0.0,  0.0,  1.0,    1.0, 1.0, 0.0,
    -0.5,  0.5,  0.0,    0.0,  0.0,  1.0,    0.0, 1.0, 0.0,
    -0.5, -0.5,  0.0,    0.0,  0.0,  1.0,    0.0, 0.0, 0.0,
     0.5, -0.5,  0.0,    0.0,  0.0,  1.0,    1.0, 0.0, 0.0
};



/* square_element_data
 *
 * elements for square vertex data
 */
const std::array<GLuint, 3 * 2 * 1> glh::vertices::square_element_data
{
    0, 1, 2, 2, 3, 0
};



/* cube_vertex_data
* cube_vertex_normal_data
* cube_vertex_normal_texcoord_data
*
* arrays containing vertex (+ normal (+ texcoord)) data for a cube
*/
const std::array<GLfloat, 3 * 4 * 6> glh::vertices::cube_vertex_data
{
    /* position */
    /* +x */
     0.5,  0.5, -0.5,
     0.5,  0.5,  0.5,
     0.5, -0.5,  0.5,
     0.5, -0.5, -0.5,

    /* -x */
    -0.5,  0.5,  0.5,
    -0.5,  0.5, -0.5,
    -0.5, -0.5, -0.5,
    -0.5, -0.5,  0.5,

    /* +y */
     0.5,  0.5, -0.5,
    -0.5,  0.5, -0.5,
    -0.5,  0.5,  0.5,
     0.5,  0.5,  0.5,

    /* -y */
    -0.5, -0.5, -0.5,
     0.5, -0.5, -0.5,
     0.5, -0.5,  0.5,
    -0.5, -0.5,  0.5,

    /* +z */ 
     0.5,  0.5,  0.5,
    -0.5,  0.5,  0.5,
    -0.5, -0.5,  0.5,
     0.5, -0.5,  0.5,

    /* -z */
    -0.5,  0.5, -0.5,
     0.5,  0.5, -0.5,
     0.5, -0.5, -0.5,
    -0.5, -0.5, -0.5
};
const std::array<GLfloat, 6 * 4 * 6> glh::vertices::cube_vertex_normal_data
{
    /* position          normal */
    /* +x */
     0.5,  0.5, -0.5,    1.0,  0.0,  0.0,
     0.5,  0.5,  0.5,    1.0,  0.0,  0.0,
     0.5, -0.5,  0.5,    1.0,  0.0,  0.0,
     0.5, -0.5, -0.5,    1.0,  0.0,  0.0,

    /* -x */
    -0.5,  0.5,  0.5,   -1.0,  0.0,  0.0,
    -0.5,  0.5, -0.5,   -1.0,  0.0,  0.0,
    -0.5, -0.5, -0.5,   -1.0,  0.0,  0.0,
    -0.5, -0.5,  0.5,   -1.0,  0.0,  0.0,

    /* +y */
     0.5,  0.5, -0.5,    0.0,  1.0,  0.0,
    -0.5,  0.5, -0.5,    0.0,  1.0,  0.0,
    -0.5,  0.5,  0.5,    0.0,  1.0,  0.0,
     0.5,  0.5,  0.5,    0.0,  1.0,  0.0,

    /* -y */
    -0.5, -0.5, -0.5,    0.0, -1.0,  0.0,
     0.5, -0.5, -0.5,    0.0, -1.0,  0.0,
     0.5, -0.5,  0.5,    0.0, -1.0,  0.0,
    -0.5, -0.5,  0.5,    0.0, -1.0,  0.0,

    /* +z */ 
     0.5,  0.5,  0.5,    0.0,  0.0,  1.0,
    -0.5,  0.5,  0.5,    0.0,  0.0,  1.0,
    -0.5, -0.5,  0.5,    0.0,  0.0,  1.0,
     0.5, -0.5,  0.5,    0.0,  0.0,  1.0,

    /* -z */
    -0.5,  0.5, -0.5,    0.0,  0.0, -1.0,
     0.5,  0.5, -0.5,    0.0,  0.0, -1.0,
     0.5, -0.5, -0.5,    0.0,  0.0, -1.0,
    -0.5, -0.5, -0.5,    0.0,  0.0, -1.0
};
const std::array<GLfloat, 9 * 4 * 6> glh::vertices::cube_vertex_normal_texcoord_data
{
    /* position          normal              texcoords */
    /* +x */
     0.5,  0.5, -0.5,    1.0,  0.0,  0.0,    1.0, 1.0, 0.0,
     0.5,  0.5,  0.5,    1.0,  0.0,  0.0,    0.0, 1.0, 0.0,
     0.5, -0.5,  0.5,    1.0,  0.0,  0.0,    0.0, 0.0, 0.0,
     0.5, -0.5, -0.5,    1.0,  0.0,  0.0,    1.0, 0.0, 0.0,

    /* -x */
    -0.5,  0.5,  0.5,   -1.0,  0.0,  0.0,    1.0, 1.0, 0.0,
    -0.5,  0.5, -0.5,   -1.0,  0.0,  0.0,    0.0, 1.0, 0.0,
    -0.5, -0.5, -0.5,   -1.0,  0.0,  0.0,    0.0, 0.0, 0.0,
    -0.5, -0.5,  0.5,   -1.0,  0.0,  0.0,    1.0, 0.0, 0.0,

    /* +y */
     0.5,  0.5, -0.5,    0.0,  1.0,  0.0,    1.0, 1.0, 0.0,
    -0.5,  0.5, -0.5,    0.0,  1.0,  0.0,    0.0, 1.0, 0.0,
    -0.5,  0.5,  0.5,    0.0,  1.0,  0.0,    0.0, 0.0, 0.0,
     0.5,  0.5,  0.5,    0.0,  1.0,  0.0,    1.0, 0.0, 0.0,

    /* -y */
    -0.5, -0.5, -0.5,    0.0, -1.0,  0.0,    1.0, 1.0, 0.0,
     0.5, -0.5, -0.5,    0.0, -1.0,  0.0,    0.0, 1.0, 0.0,
     0.5, -0.5,  0.5,    0.0, -1.0,  0.0,    0.0, 0.0, 0.0,
    -0.5, -0.5,  0.5,    0.0, -1.0,  0.0,    1.0, 0.0, 0.0,

    /* +z */ 
     0.5,  0.5,  0.5,    0.0,  0.0,  1.0,    1.0, 1.0, 0.0,
    -0.5,  0.5,  0.5,    0.0,  0.0,  1.0,    0.0, 1.0, 0.0,
    -0.5, -0.5,  0.5,    0.0,  0.0,  1.0,    0.0, 0.0, 0.0,
     0.5, -0.5,  0.5,    0.0,  0.0,  1.0,    1.0, 0.0, 0.0,

    /* -z */
    -0.5,  0.5, -0.5,    0.0,  0.0, -1.0,    1.0, 1.0, 0.0,
     0.5,  0.5, -0.5,    0.0,  0.0, -1.0,    0.0, 1.0, 0.0,
     0.5, -0.5, -0.5,    0.0,  0.0, -1.0,    0.0, 0.0, 0.0,
    -0.5, -0.5, -0.5,    0.0,  0.0, -1.0,    1.0, 0.0, 0.0
};



/* cube_element_data
 *
 * elements for cube vertex data
 */
const std::array<GLuint, 3 * 2 * 6> glh::vertices::cube_element_data
{
     0,  1,  2,  2,  3,  0,
     4,  5,  6,  6,  7,  4,
     8,  9, 10, 10, 11,  8,
    12, 13, 14, 14, 15, 12,
    16, 17, 18, 18, 19, 16,
    20, 21, 22, 22, 23, 20
};