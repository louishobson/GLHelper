/*
 * vertex.model.glsl
 * 
 * model vertex shader
 */



/* INPUTS AND OUTPUTS */

/* input vertex position, normal, vertex color, and texture coords */
layout ( location = 0 ) in vec3 in_pos;
layout ( location = 1 ) in vec3 in_normal;
layout ( location = 2 ) in vec4 in_vcolor;
layout ( location = 3 ) in vec3 in_texcoords [ MAX_TEXTURE_STACK_SIZE ];

/* output fragpos, normal, vcolor and texcoords */
out VS_OUT
{
    vec3 fragpos;
    vec3 normal;
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

    /* pass through normal */
    vs_out.normal = in_normal;

    /* set fragpos */
    vs_out.fragpos = in_pos;

    /* set vcolor */
    vs_out.vcolor = in_vcolor;
    
    /* set texcoords to in_texcoords */
    vs_out.texcoords = in_texcoords;
}