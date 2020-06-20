/*
 * geometry.shadow.glsl
 * 
 * shadow geometry shader
 */

#version 460 core



/* maximum number of lights */
#define MAX_NUM_LIGHTS 2

/* pi */
#define PI 3.1415926535897932384626433832795



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
    light_struct spotlights_size [ MAX_NUM_LIGHTS ];

    sampler2DArrayShadow shadow_maps_2d;
    samplerCubeArrayShadow shadow_maps_cube;

    mat4 shadow_maps_cube_rotations [ 6 ];
};



/* recieve triangles from vertex shader */
layout ( triangles ) in;

/* output triangle strip */
layout ( triangle_strip, max_vertices = 256 ) out;



/* light system */
uniform light_system_struct light_system;



/* main */
void main ()
{
    /* loop through lights */
    for ( int i = 0; i < light_system.dirlights_size; ++i )
    {
        /* transform the vertices */
        gl_Layer = i;
        gl_Position = light_system.dirlights [ i ].shadow_proj * light_system.dirlights [ i ].shadow_view * gl_in [ 0 ].gl_Position; EmitVertex ();
        gl_Position = light_system.dirlights [ i ].shadow_proj * light_system.dirlights [ i ].shadow_view * gl_in [ 1 ].gl_Position; EmitVertex ();
        gl_Position = light_system.dirlights [ i ].shadow_proj * light_system.dirlights [ i ].shadow_view * gl_in [ 2 ].gl_Position; EmitVertex ();
        EndPrimitive ();
    }
    for ( int i = 0; i < light_system.pointlights_size; ++i )
    {
        for ( int j = 0; j < 6; ++j ) 
        {
            /* transform the vertices */
            gl_Layer = light_system.dirlights_size + i * 6 + j;
            gl_Position = light_system.pointlights [ i ].shadow_proj * light_system.shadow_maps_cube_rotations [ j ] * light_system.pointlights [ i ].shadow_view * gl_in [ 0 ].gl_Position; EmitVertex ();
            gl_Position = light_system.pointlights [ i ].shadow_proj * light_system.shadow_maps_cube_rotations [ j ] * light_system.pointlights [ i ].shadow_view * gl_in [ 1 ].gl_Position; EmitVertex ();
            gl_Position = light_system.pointlights [ i ].shadow_proj * light_system.shadow_maps_cube_rotations [ j ] * light_system.pointlights [ i ].shadow_view * gl_in [ 2 ].gl_Position; EmitVertex ();
            EndPrimitive ();
        }
    }
    for ( int i = 0; i < light_system.spotlights_size; ++i )
    {
        /* transform the vertices */
        gl_Layer = light_system.dirlights_size * light_system.pointlights_size * 6 + i;
        gl_Position = light_system.spotlights [ i ].shadow_proj * light_system.spotlights [ i ].shadow_view * gl_in [ 0 ].gl_Position; EmitVertex ();
        gl_Position = light_system.spotlights [ i ].shadow_proj * light_system.spotlights [ i ].shadow_view * gl_in [ 1 ].gl_Position; EmitVertex ();
        gl_Position = light_system.spotlights [ i ].shadow_proj * light_system.spotlights [ i ].shadow_view * gl_in [ 2 ].gl_Position; EmitVertex ();
        EndPrimitive ();
    }
}