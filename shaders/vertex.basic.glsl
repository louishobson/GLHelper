/*
 * vertex.basic.glsl
 * 
 * basic vertex shader
 */

#version 460 core

/* transformations structure */
struct trans_struct
{
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 viewpos;
};

/* vertices, texture coords and normal */
layout ( location = 0 ) in vec3 pos;
layout ( location = 1 ) in vec3 in_normal;
layout ( location = 2 ) in vec3 in_texcoords;

/* fragpos, normal and texture coords */
out vec3 fragpos;
out vec3 normal;
out vec3 texcoords;

/* transformation matrices */
uniform trans_struct trans;

/* main */
void main ()
{
    /* set the position to be the same as the attribute, with an alpha of 1.0 */
    gl_Position = trans.proj * trans.view * trans.model * vec4 ( pos, 1.0 );
    /* set texcoords to in_texcoords */
    texcoords = in_texcoords;
    /* set normal to normal matrix * in_normal */
    normal = normalize ( transpose ( inverse ( mat3 ( trans.model ) ) ) * in_normal );
    /* set fragpos */
    fragpos = vec3 ( trans.model * vec4 ( pos, 1.0 ) );
}