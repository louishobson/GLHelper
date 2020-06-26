/*
 * vertex.model.glsl
 * 
 * model vertex shader
 */



/* DEFINITIONS */

/* maximum color sets */
#define MAX_COLOR_SETS 1

/* maximum number of UV channels */
#define MAX_UV_CHANNELS 2



/* vertices, texture coords and normal */
layout ( location = 0 ) in vec3 pos;
layout ( location = 1 ) in vec3 in_normal;
layout ( location = 2 ) in vec4 in_vcolor [ MAX_COLOR_SETS ];
layout ( location = 3 ) in vec3 in_texcoords [ MAX_UV_CHANNELS ];



/* texture coords, normal, vcolor */
out VS_OUT
{
    vec3 fragpos;
    vec3 normal;
    vec4 vcolor [ MAX_COLOR_SETS ];
    vec3 texcoords [ MAX_UV_CHANNELS ];
} vs_out;



/* camera matrices */
uniform camera_struct camera;

/* model matrix */
uniform mat4 model_matrix;



/* main */
void main ()
{
    /* set the position to be the same as the attribute, with an alpha of 1.0 */
    gl_Position = camera.view_proj * model_matrix * vec4 ( pos, 1.0 );
    /* set texcoords to in_texcoords */
    vs_out.texcoords = in_texcoords;
    /* set normal to normal matrix * in_normal */
    vs_out.normal = normalize ( transpose ( inverse ( mat3 ( model_matrix ) ) ) * in_normal );
    /* set fragpos */
    vs_out.fragpos = vec3 ( model_matrix * vec4 ( pos, 1.0 ) );
    /* set vcolor */
    vs_out.vcolor = in_vcolor;
}