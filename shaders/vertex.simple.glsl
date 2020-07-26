/*
 * vertex.simple.glsl
 * 
 * simple vertex shader for rendering a single quad onto the screen
 */



 /* INPUTS AND OUTPUTS */

/* input vertex position, normal and texture coords */
layout ( location = 0 ) in vec3 in_pos;
layout ( location = 1 ) in vec3 in_normal;
layout ( location = 2 ) in vec2 in_texcoords;

/* output fragpos */
out VS_OUT
{
    vec3 fragpos;
    vec2 texcoords;
} vs_out;



/* MAIN */

/* main */
void main ()
{
    /* set gl_Position to in_pos */
    gl_Position = in_pos;

    /* set fragpos to in_pos and texcoords to in_texcoords */
    vs_out.fragpos = in_pos;
    vs_out.texcoords = in_texcoords;
}






 