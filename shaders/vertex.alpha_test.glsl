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

/* output vcolor_alpha and texcoords */
out VS_OUT
{
    float vcolor_alpha;
    vec2 texcoords [ MAX_TEXTURE_STACK_SIZE ];
} vs_out;



/* MAIN */

/* main */
void main ()
{
    /* set position */
    gl_Position = vec4 ( in_pos, 1.0 );

    /* set vcolor_alpha */
    vs_out.vcolor_alpha = in_vcolor.w;
    
    /* set texcoords to in_texcoords */
    vs_out.texcoords = in_texcoords;
}