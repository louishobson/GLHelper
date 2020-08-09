/*
 * vertex.alpha_test.glsl
 * 
 * vertex shader to query about primative alpha values
 */



/* INPUTS AND OUTPUTS */

/* input vertex position, vertex color, and texture coords */
layout ( location = 0 ) in vec3 in_pos;
layout ( location = 3 ) in vec4 in_vcolor;
layout ( location = 4 ) in vec2 in_texcoords [ MAX_TEXTURE_STACK_SIZE ];

/* output vcolor and texcoords */
out VS_OUT
{
    vec4 vcolor;
    vec2 texcoords [ MAX_TEXTURE_STACK_SIZE ];
} vs_out;



/* MAIN */

/* main */
void main ()
{
    /* set position */
    gl_Position = vec4 ( in_pos, 1.0 );

    /* set vcolor */
    vs_out.vcolor = in_vcolor;
    
    /* set texcoords to in_texcoords */
    vs_out.texcoords = in_texcoords;
}