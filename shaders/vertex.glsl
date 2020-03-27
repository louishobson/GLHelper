/*
 * vertex.glsl
 * 
 * vertex shader for testing
 */

#version 330 core

/* vertices and texture coords */
layout ( location = 0 ) in vec3 pos;
layout ( location = 1 ) in vec2 tex;

/* texture coords */
out vec2 TexCoord;

/* transformation matrix */
uniform mat4 trans;

/* main */
void main ()
{
    /* set the position to be the same as the attribute, with an alpha of 1.0 */
    gl_Position = trans * vec4 ( pos, 1.0f );
    /* set TexCoord to tex */
    TexCoord = tex;
}