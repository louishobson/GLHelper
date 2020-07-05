/*
 * vertex.shadow.glsl
 * 
 * shadow vertex shader
 */



/* INPUTS AND OUTPUTS */

/* input vertex position and texture coords */
layout ( location = 0 ) in vec3 in_pos;
layout ( location = 4 ) in vec3 in_texcoords [ MAX_TEXTURE_STACK_SIZE ];

/* output texcoords */
out VS_OUT
{
    vec3 texcoords [ MAX_TEXTURE_STACK_SIZE ];
} vs_out;



/* main */
void main ()
{
    /* pass through vertex position */
    gl_Position = vec4 ( in_pos, 1.0 );

    /* transfer the texcoords */
    vs_out.texcoords = in_texcoords;
}