/*
 * lighting.glsl
 *
 * defines structures for lighting
 */



/* DEFINITIONS */

 /* maximum number of lights */
#define MAX_NUM_LIGHTS 16



/* STRUCTURES */

/* structure for a light (of any type) */
struct light_struct
{
    vec3 position;
    vec3 direction;
    float inner_cone;
    float outer_cone;

    float att_const;
    float att_linear;
    float att_quad;

    vec3 ambient_color;
    vec3 diffuse_color;
    vec3 specular_color;

    bool enabled;
    bool shadow_mapping_enabled;

    mat4 shadow_view;
    mat4 shadow_proj;
};

/* structure for storing multiple collections of lights */
struct light_system_struct
{
    int dirlights_size;
    light_struct dirlights [ MAX_NUM_LIGHTS ];

    int pointlights_size;
    light_struct pointlights [ MAX_NUM_LIGHTS ];

    int spotlights_size;
    light_struct spotlights [ MAX_NUM_LIGHTS ];

    sampler2DArrayShadow shadow_maps_2d;
    samplerCubeArrayShadow shadow_maps_cube;

    mat4 shadow_maps_cube_rotations [ 6 ];
};



/* FUNCTIONS */

/* compute_attenuation
 *
 * compute the multiple of attenuation
 *
 * dist: the distance from the fragment to the light source
 * att_const: constant part
 * att_linear: linear part
 * att_quad: quadratic part
 *
 * return: the multiple of attenuation
 */
float compute_attenuation ( float dist, float att_const, float att_linear, float att_quad )
{
    return 1.0 / ( att_const + ( att_linear * dist ) + ( att_quad * dist * dist ) );
}