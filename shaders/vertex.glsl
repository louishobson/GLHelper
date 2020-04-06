/*
 * vertex.glsl
 * 
 * vertex shader for testing
 */

#version 330 core

/* transformations structure */
struct trans_struct
{
    mat4 model;
    mat4 view;
    mat4 proj;
    mat3 norm;
};

/* vertices, texture coords and normal */
layout ( location = 0 ) in vec3 pos;
layout ( location = 1 ) in vec3 in_norm;
layout ( location = 2 ) in vec2 in_texcoord;

/* texture coords, normal */
out vec2 texcoord;
out vec3 norm;
out vec3 fragpos;

/* transformation matrices */
uniform trans_struct trans;

/* main */
void main ()
{
    /* set the position to be the same as the attribute, with an alpha of 1.0 */
    gl_Position = trans.proj * trans.view * trans.model * vec4 ( pos, 1.0f );
    /* set texcoord to in_texcoords */
    texcoord = in_texcoord;
    /* set norm to trans.norm * in_norm */
    norm = normalize ( trans.norm * in_norm );
    /* set fragpos */
    fragpos = vec3 ( trans.model * vec4 ( pos, 1.0 ) );
}