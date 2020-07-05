/*
 * vertex.model.glsl
 * 
 * model vertex shader
 */



/* INPUTS AND OUTPUTS */

/* input vertex position, normal, vertex color, and texture coords */
layout ( location = 0 ) in vec3 in_pos;
layout ( location = 1 ) in vec3 in_normal;
layout ( location = 2 ) in vec3 in_tangent;
layout ( location = 3 ) in vec4 in_vcolor;
layout ( location = 4 ) in vec3 in_texcoords [ MAX_TEXTURE_STACK_SIZE ];

/* output fragpos, tbn_matrix, vcolor and texcoords */
out VS_OUT
{
    vec3 fragpos;
    mat3 tbn_matrix;
    vec4 vcolor;
    vec3 texcoords [ MAX_TEXTURE_STACK_SIZE ];
} vs_out;



/* UNIFORMS */

/* camera matrices */
uniform camera_struct camera;



/* MAIN */

/* main */
void main ()
{
    /* transform position based on camera */
    gl_Position = camera.view_proj * vec4 ( in_pos, 1.0 );

    /* set fragpos */
    vs_out.fragpos = in_pos;

    /* set tbn matrix */
    vs_out.tbn_matrix = mat3 ( in_tangent, cross ( in_normal, in_tangent ), in_normal );

    /* set vcolor */
    vs_out.vcolor = in_vcolor;
    
    /* set texcoords to in_texcoords */
    vs_out.texcoords = in_texcoords;
}