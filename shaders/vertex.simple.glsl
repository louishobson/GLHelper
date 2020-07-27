/*
 * vertex.simple.glsl
 * 
 * simple vertex shader for rendering a single quad onto the screen
 */



 /* INPUTS AND OUTPUTS */

/* input vertex position */
layout ( location = 0 ) in vec3 in_pos;

/* output texcoords */
out VS_OUT
{
    vec2 texcoords;
} vs_out;



/* MAIN */

/* main */
void main ()
{
    /* set gl_Position to in_pos */
    gl_Position = vec4 ( in_pos, 1.0 );

    /* set texcoords to in_pos.xy * 0.5 + 0.5  */
    vs_out.texcoords = in_pos.xy * 0.5 + 0.5;
}






 