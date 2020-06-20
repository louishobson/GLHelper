/*
 * vertex.shadow.glsl
 * 
 * shadow vertex shader
 */

#version 460 core

/* maximum number of color sets */
#define MAX_COLOR_SETS 1

/* maximum number of UV channels */
#define MAX_UV_CHANNELS 2



/* vertices, texture coords and normal */
layout ( location = 0 ) in vec3 pos;
layout ( location = 1 ) in vec3 in_normal;
layout ( location = 2 ) in vec4 in_vcolor [ MAX_COLOR_SETS ];
layout ( location = 3 ) in vec3 in_texcoords [ MAX_UV_CHANNELS ];



/* model transformation matrix */
uniform mat4 model_matrix;



/* main */
void main ()
{
    /* set the position to be the same as the attribute, with an alpha of 1.0 */
    gl_Position = model_matrix * vec4 ( pos, 1.0 );
}