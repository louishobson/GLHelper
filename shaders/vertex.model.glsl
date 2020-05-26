/*
 * vertex.model.glsl
 * 
 * model vertex shader
 */

#version 330 core

/* maximum number of color sets */
#define MAX_COLOR_SETS 1

/* maximum number of UV channels */
#define MAX_UV_CHANNELS 2

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
layout ( location = 2 ) in vec4 in_vcolor [ MAX_COLOR_SETS ];
layout ( location = 3 ) in vec3 in_texcoords [ MAX_UV_CHANNELS ];

/* texture coords, normal, vcolor */
out vec3 fragpos;
out vec3 normal;
out vec4 vcolor [ MAX_COLOR_SETS ];
out vec3 texcoords [ MAX_UV_CHANNELS ];

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
    /* set vcolor */
    vcolor = in_vcolor;
}