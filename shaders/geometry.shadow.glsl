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
layout ( triangle_strip, max_vertices = 18 ) out;

/* output texcoords and depth */
out GS_OUT
{
    vec2 texcoords [ MAX_TEXTURE_STACK_SIZE ];
    float depth;
} gs_out;



/* UNIFORMS */

/* light system */
uniform light_system_struct light_system;



/* main */
void main ()
{
    /* loop through dirlights */
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
    for ( int i = 0; i < light_system.pointlights_size; ++i )
    {
        /* continue if light is disabled */
        if ( !light_system.pointlights [ i ].enabled || !light_system.pointlights [ i ].shadow_mapping_enabled ) continue;

        /* set to render to the front texture
         * the front texture is defined by all points where z >= 0
         */
        gl_Layer = light_system.dirlights_size + i * 2;

        /* three vectors in preparation for caching position vectors */
        vec3 cached_positions [ 3 ];

        /* vector in preparation for cachine depths */
        vec3 cached_depths;

        /* for each vertex, set cached_positions to the vertex position transformed to the pointlight's view-space using its position as a translation vector
         * cache the length of this vector to cached_depths and set the output depth to this value
         * normalize the cached vertex position
         * now set the output position's x and y components to the cached vertex position' x and y components divided by 1.0 PLUS the z component
         * the output position's x and y components are now the ndc coordinates
         * the output position's w component can be set to 1.0 to stop the perspective divide
         * set the texcoords and emit the vertex
         * after all vertices are processed, end the primative
         */
        cached_positions [ 0 ] = gl_in [ 0 ].gl_Position.xyz - light_system.pointlights [ i ].position;
        gs_out.depth = cached_depths.x = length ( cached_positions [ 0 ] ) * light_system.pointlights [ i ].shadow_depth_range_mult;
        cached_positions [ 0 ] = normalize ( cached_positions [ 0 ] );
        gl_Position.xy = cached_positions [ 0 ].xy / ( 1.0 + cached_positions [ 0 ].z ); 
        gl_Position.z = cached_positions [ 0 ].z * 1.9 - 0.95; gl_Position.w = 1.0; gs_out.texcoords = vs_out [ 0 ].texcoords; 
        EmitVertex ();

        cached_positions [ 1 ] = gl_in [ 1 ].gl_Position.xyz - light_system.pointlights [ i ].position;
        gs_out.depth = cached_depths.y = length ( cached_positions [ 1 ] ) * light_system.pointlights [ i ].shadow_depth_range_mult;
        cached_positions [ 1 ] = normalize ( cached_positions [ 1 ] ); 
        gl_Position.xy = cached_positions [ 1 ].xy / ( 1.0 + cached_positions [ 1 ].z ); 
        gl_Position.z = cached_positions [ 1 ].z * 1.9 - 0.95; gl_Position.w = 1.0; gs_out.texcoords = vs_out [ 1 ].texcoords; 
        EmitVertex ();

        cached_positions [ 2 ] = gl_in [ 2 ].gl_Position.xyz - light_system.pointlights [ i ].position;
        gs_out.depth = cached_depths.z = length ( cached_positions [ 2 ] ) * light_system.pointlights [ i ].shadow_depth_range_mult;
        cached_positions [ 2 ] = normalize ( cached_positions [ 2 ] ); 
        gl_Position.xy = cached_positions [ 2 ].xy / ( 1.0 + cached_positions [ 2 ].z ); 
        gl_Position.z = cached_positions [ 2 ].z * 1.9 - 0.95; gl_Position.w = 1.0; gs_out.texcoords = vs_out [ 2 ].texcoords; 
        EmitVertex ();
        
        EndPrimitive ();
        
        /* set to render to the back texture */
        gl_Layer = light_system.dirlights_size + i * 2 + 1;
        
        /* do similar to the above, except using the cached depths where appropriate
         * the x and y components of the cached position is now divided by 1.0 MINUS the cached z component, since z should be <= 0.0
         * the z component of the output position is scaled differently, so that any z component more than 0.0 will be culled
         */
        gs_out.depth = cached_depths.x;
        gl_Position.xy = cached_positions [ 0 ].xy / ( 1.0 - cached_positions [ 0 ].z );
        gl_Position.z = cached_positions [ 0 ].z * 1.9 + 0.95; gl_Position.w = 1.0; gs_out.texcoords = vs_out [ 0 ].texcoords;
        EmitVertex ();
        
        gs_out.depth = cached_depths.y;
        gl_Position.xy = cached_positions [ 1 ].xy / ( 1.0 - cached_positions [ 1 ].z );
        gl_Position.z = cached_positions [ 1 ].z * 1.9 + 0.95; gl_Position.w = 1.0; gs_out.texcoords = vs_out [ 1 ].texcoords;
        EmitVertex ();

        gs_out.depth = cached_depths.z;
        gl_Position.xy = cached_positions [ 2 ].xy / ( 1.0 - cached_positions [ 2 ].z );
        gl_Position.z = cached_positions [ 2 ].z * 1.9 + 0.95; gl_Position.w = 1.0; gs_out.texcoords = vs_out [ 2 ].texcoords;
        EmitVertex ();

        EndPrimitive ();
    }
    
    /* loop through dirlights */
    for ( int i = 0; i < light_system.spotlights_size; ++i )
    {
        /* continue if light is disabled */
        if ( !light_system.spotlights [ i ].enabled || !light_system.spotlights [ i ].shadow_mapping_enabled ) continue;

        /* set the layer */
        gl_Layer = light_system.dirlights_size + light_system.pointlights_size * 2 + i;

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