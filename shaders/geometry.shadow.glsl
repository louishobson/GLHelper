/*
 * geometry.shadow.glsl
 * 
 * shadow geometry shader
 */



/* pi */
#define PI 3.1415926535897932384626433832795



/* recieve triangles from vertex shader */
layout ( triangles ) in;

/* output triangle strip */
layout ( triangle_strip, max_vertices = 256 ) out;



/* light system */
uniform light_system_struct light_system;



/* main */
void main ()
{
    /* loop through lights, and continue if shadow mapping is disabled */
    for ( int i = 0; i < light_system.dirlights_size; ++i )
    {
        /* transform the vertices */
        if ( !light_system.dirlights [ i ].shadow_mapping_enabled ) continue;
        gl_Layer = i;
        gl_Position = light_system.dirlights [ i ].shadow_proj * light_system.dirlights [ i ].shadow_view * gl_in [ 0 ].gl_Position; EmitVertex ();
        gl_Position = light_system.dirlights [ i ].shadow_proj * light_system.dirlights [ i ].shadow_view * gl_in [ 1 ].gl_Position; EmitVertex ();
        gl_Position = light_system.dirlights [ i ].shadow_proj * light_system.dirlights [ i ].shadow_view * gl_in [ 2 ].gl_Position; EmitVertex ();
        EndPrimitive ();
    }
    for ( int i = 0; i < light_system.pointlights_size; ++i )
    {
        if ( !light_system.pointlights [ i ].shadow_mapping_enabled ) continue;
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
        if ( !light_system.spotlights [ i ].shadow_mapping_enabled ) continue;
        gl_Layer = light_system.dirlights_size * light_system.pointlights_size * 6 + i;
        gl_Position = light_system.spotlights [ i ].shadow_proj * light_system.spotlights [ i ].shadow_view * gl_in [ 0 ].gl_Position; EmitVertex ();
        gl_Position = light_system.spotlights [ i ].shadow_proj * light_system.spotlights [ i ].shadow_view * gl_in [ 1 ].gl_Position; EmitVertex ();
        gl_Position = light_system.spotlights [ i ].shadow_proj * light_system.spotlights [ i ].shadow_view * gl_in [ 2 ].gl_Position; EmitVertex ();
        EndPrimitive ();
    }
}