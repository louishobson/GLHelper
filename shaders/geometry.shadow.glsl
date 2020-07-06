/*
 * geometry.shadow.glsl
 * 
 * shadow geometry shader
 */



/* INPUTS AND OUTPUTS */

/* recieve triangles from vertex shader */
layout ( triangles ) in;

/* input texcoords */
in VS_OUT
{
    vec2 texcoords [ MAX_TEXTURE_STACK_SIZE ];
} vs_out [ 3 ];

/* output triangle strip */
layout ( triangle_strip, max_vertices = 30 ) out;

/* output texcoords and depth */
out GS_OUT
{
    vec2 texcoords [ MAX_TEXTURE_STACK_SIZE ];
    float depth;
} gs_out;



/* UNIFORMS */

/* light system */
uniform light_system_struct light_system;

/* shadow_mode
 *
 * modes are:
 * 
 * 0: process dirlights and spotlights
 * 1: process pointlights
 */
uniform int shadow_mode;



/* main */
void main ()
{
    /* loop through dirlights */
    if ( shadow_mode == 0 )
    for ( int i = 0; i < light_system.dirlights_size; ++i )
    {
        /* continue if light is disabled */
        if ( !light_system.dirlights [ i ].enabled || !light_system.dirlights [ i ].shadow_mapping_enabled ) continue;
        gl_Layer = i;
        gl_Position = light_system.dirlights [ i ].shadow_trans * gl_in [ 0 ].gl_Position; gs_out.texcoords = vs_out [ 0 ].texcoords; gs_out.depth = gl_Position.z * 0.5 + 0.5; EmitVertex ();
        gl_Position = light_system.dirlights [ i ].shadow_trans * gl_in [ 1 ].gl_Position; gs_out.texcoords = vs_out [ 1 ].texcoords; gs_out.depth = gl_Position.z * 0.5 + 0.5; EmitVertex ();
        gl_Position = light_system.dirlights [ i ].shadow_trans * gl_in [ 2 ].gl_Position; gs_out.texcoords = vs_out [ 2 ].texcoords; gs_out.depth = gl_Position.z * 0.5 + 0.5; EmitVertex ();
        EndPrimitive ();
    }

    /* loop through pointlights */
    if ( shadow_mode == 1 )
    for ( int i = 0; i < light_system.pointlights_size; ++i )
    {
        /* continue if light is disabled */
        if ( !light_system.pointlights [ i ].enabled || !light_system.pointlights [ i ].shadow_mapping_enabled ) continue;

        /* precalculate view-space position and depth for vertices 0-3
         * the depth linear by using the distance to the light, and mapped to 0-1 by multiplying by shadow_depth_range_mult
         */
        const vec3 depths = vec3
        (
            length ( light_system.pointlights [ i ].position - gl_in [ 0 ].gl_Position.xyz ),
            length ( light_system.pointlights [ i ].position - gl_in [ 1 ].gl_Position.xyz ),
            length ( light_system.pointlights [ i ].position - gl_in [ 2 ].gl_Position.xyz )
        ) * light_system.pointlights [ i ].shadow_depth_range_mult;

        /* repeatedly set the layer, transform the vertices, and emit the primatives */
        for ( int j = 0; j < 6; ++j )
        {
            gl_Layer = i * 6 + j;
            gl_Position = light_system.pointlights [ i ].shadow_trans [ j ] * gl_in [ 0 ].gl_Position; gs_out.texcoords = vs_out [ 0 ].texcoords; gs_out.depth = depths [ 0 ]; EmitVertex ();
            gl_Position = light_system.pointlights [ i ].shadow_trans [ j ] * gl_in [ 1 ].gl_Position; gs_out.texcoords = vs_out [ 1 ].texcoords; gs_out.depth = depths [ 1 ]; EmitVertex ();
            gl_Position = light_system.pointlights [ i ].shadow_trans [ j ] * gl_in [ 2 ].gl_Position; gs_out.texcoords = vs_out [ 2 ].texcoords; gs_out.depth = depths [ 2 ]; EmitVertex ();
            EndPrimitive ();
        }
    }
    
    /* loop through dirlights */
    if ( shadow_mode == 0 )
    for ( int i = 0; i < light_system.spotlights_size; ++i )
    {
        /* continue if light is disabled */
        if ( !light_system.spotlights [ i ].enabled || !light_system.spotlights [ i ].shadow_mapping_enabled ) continue;

        /* set the layer */
        gl_Layer = light_system.dirlights_size + i;

        /* set the positions and depths of all of the vertices, then emit the primative
         * the depth linear by using the distance to the light, and mapped to 0-1 by multiplying by shadow_depth_range_mult
         */
        gl_Position = light_system.spotlights [ i ].shadow_trans * gl_in [ 0 ].gl_Position; gs_out.texcoords = vs_out [ 0 ].texcoords;
        gs_out.depth = length ( light_system.spotlights [ i ].position - gl_in [ 0 ].gl_Position.xyz ) * light_system.spotlights [ i ].shadow_depth_range_mult; EmitVertex ();
        gl_Position = light_system.spotlights [ i ].shadow_trans * gl_in [ 1 ].gl_Position; gs_out.texcoords = vs_out [ 1 ].texcoords;
        gs_out.depth = length ( light_system.spotlights [ i ].position - gl_in [ 1 ].gl_Position.xyz ) * light_system.spotlights [ i ].shadow_depth_range_mult; EmitVertex ();
        gl_Position = light_system.spotlights [ i ].shadow_trans * gl_in [ 2 ].gl_Position; gs_out.texcoords = vs_out [ 2 ].texcoords;
        gs_out.depth = length ( light_system.spotlights [ i ].position - gl_in [ 2 ].gl_Position.xyz ) * light_system.spotlights [ i ].shadow_depth_range_mult; EmitVertex ();
        EndPrimitive ();
    }
}