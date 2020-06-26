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
    vec3 texcoords [ MAX_TEXTURE_STACK_SIZE ];
} vs_out [ 3 ];

/* output triangle strip */
layout ( triangle_strip, max_vertices = 18 ) out;

/* output texcoords and depth */
out GS_OUT
{
    vec3 texcoords [ MAX_TEXTURE_STACK_SIZE ];
    float depth;
} gs_out;



/* UNIFORMS */

/* light system */
uniform light_system_struct light_system;



/* main */
void main ()
{
    /* loop through lights, and continue if shadow mapping is disabled */
    for ( int i = 0; i < light_system.dirlights_size; ++i )
    {
        /* continue if light is disabled */
        if ( !light_system.dirlights [ i ].enabled || !light_system.dirlights [ i ].shadow_mapping_enabled ) continue;
        gl_Layer = i;
        gl_Position = light_system.dirlights [ i ].shadow_camera.view_proj * gl_in [ 0 ].gl_Position; gs_out.texcoords = vs_out [ 0 ].texcoords; gs_out.depth = gl_Position.z * 0.5 + 0.5; EmitVertex ();
        gl_Position = light_system.dirlights [ i ].shadow_camera.view_proj * gl_in [ 1 ].gl_Position; gs_out.texcoords = vs_out [ 1 ].texcoords; gs_out.depth = gl_Position.z * 0.5 + 0.5; EmitVertex ();
        gl_Position = light_system.dirlights [ i ].shadow_camera.view_proj * gl_in [ 2 ].gl_Position; gs_out.texcoords = vs_out [ 2 ].texcoords; gs_out.depth = gl_Position.z * 0.5 + 0.5; EmitVertex ();
        EndPrimitive ();
    }
    for ( int i = 0; i < light_system.pointlights_size; ++i )
    {
        /* continue if light is disabled */
        if ( !light_system.pointlights [ i ].enabled || !light_system.pointlights [ i ].shadow_mapping_enabled ) continue;

        /* precalculate view-space position and depth for vertices 0-3
         * the depth linear by using the distance to the light, and mapped to 0-1 by dividing my shadow_max_dist
         */
        const vec4 v0_view = light_system.pointlights [ i ].shadow_camera.view * gl_in [ 0 ].gl_Position;
        const vec4 v1_view = light_system.pointlights [ i ].shadow_camera.view * gl_in [ 1 ].gl_Position;
        const vec4 v2_view = light_system.pointlights [ i ].shadow_camera.view * gl_in [ 2 ].gl_Position;
        const float v0_depth = length ( light_system.pointlights [ i ].position - gl_in [ 0 ].gl_Position.xyz ) / light_system.pointlights [ i ].shadow_max_dist;
        const float v1_depth = length ( light_system.pointlights [ i ].position - gl_in [ 1 ].gl_Position.xyz ) / light_system.pointlights [ i ].shadow_max_dist;
        const float v2_depth = length ( light_system.pointlights [ i ].position - gl_in [ 2 ].gl_Position.xyz ) / light_system.pointlights [ i ].shadow_max_dist;

        /* repeatedly set the layer, transform the vertices, and emit the primatives */
        gl_Layer = light_system.dirlights_size + i * 6 + 0;
        gl_Position = light_system.pointlights [ i ].shadow_camera.proj * POS_X_ROTATION * v0_view; gs_out.texcoords = vs_out [ 0 ].texcoords; gs_out.depth = v0_depth; EmitVertex ();
        gl_Position = light_system.pointlights [ i ].shadow_camera.proj * POS_X_ROTATION * v1_view; gs_out.texcoords = vs_out [ 1 ].texcoords; gs_out.depth = v1_depth; EmitVertex ();
        gl_Position = light_system.pointlights [ i ].shadow_camera.proj * POS_X_ROTATION * v2_view; gs_out.texcoords = vs_out [ 2 ].texcoords; gs_out.depth = v2_depth; EmitVertex ();
        EndPrimitive ();
        gl_Layer = light_system.dirlights_size + i * 6 + 1;
        gl_Position = light_system.pointlights [ i ].shadow_camera.proj * NEG_X_ROTATION * v0_view; gs_out.texcoords = vs_out [ 0 ].texcoords; gs_out.depth = v0_depth; EmitVertex ();
        gl_Position = light_system.pointlights [ i ].shadow_camera.proj * NEG_X_ROTATION * v1_view; gs_out.texcoords = vs_out [ 1 ].texcoords; gs_out.depth = v1_depth; EmitVertex ();
        gl_Position = light_system.pointlights [ i ].shadow_camera.proj * NEG_X_ROTATION * v2_view; gs_out.texcoords = vs_out [ 2 ].texcoords; gs_out.depth = v2_depth; EmitVertex ();
        EndPrimitive ();
        gl_Layer = light_system.dirlights_size + i * 6 + 2;
        gl_Position = light_system.pointlights [ i ].shadow_camera.proj * POS_Y_ROTATION * v0_view; gs_out.texcoords = vs_out [ 0 ].texcoords; gs_out.depth = v0_depth; EmitVertex ();
        gl_Position = light_system.pointlights [ i ].shadow_camera.proj * POS_Y_ROTATION * v1_view; gs_out.texcoords = vs_out [ 1 ].texcoords; gs_out.depth = v1_depth; EmitVertex ();
        gl_Position = light_system.pointlights [ i ].shadow_camera.proj * POS_Y_ROTATION * v2_view; gs_out.texcoords = vs_out [ 2 ].texcoords; gs_out.depth = v2_depth; EmitVertex ();
        EndPrimitive ();
        gl_Layer = light_system.dirlights_size + i * 6 + 3;
        gl_Position = light_system.pointlights [ i ].shadow_camera.proj * NEG_Y_ROTATION * v0_view; gs_out.texcoords = vs_out [ 0 ].texcoords; gs_out.depth = v0_depth; EmitVertex ();
        gl_Position = light_system.pointlights [ i ].shadow_camera.proj * NEG_Y_ROTATION * v1_view; gs_out.texcoords = vs_out [ 1 ].texcoords; gs_out.depth = v1_depth; EmitVertex ();
        gl_Position = light_system.pointlights [ i ].shadow_camera.proj * NEG_Y_ROTATION * v2_view; gs_out.texcoords = vs_out [ 2 ].texcoords; gs_out.depth = v2_depth; EmitVertex ();
        EndPrimitive ();
        gl_Layer = light_system.dirlights_size + i * 6 + 4;
        gl_Position = light_system.pointlights [ i ].shadow_camera.proj * POS_Z_ROTATION * v0_view; gs_out.texcoords = vs_out [ 0 ].texcoords; gs_out.depth = v0_depth; EmitVertex ();
        gl_Position = light_system.pointlights [ i ].shadow_camera.proj * POS_Z_ROTATION * v1_view; gs_out.texcoords = vs_out [ 1 ].texcoords; gs_out.depth = v1_depth; EmitVertex ();
        gl_Position = light_system.pointlights [ i ].shadow_camera.proj * POS_Z_ROTATION * v2_view; gs_out.texcoords = vs_out [ 2 ].texcoords; gs_out.depth = v2_depth; EmitVertex ();
        EndPrimitive ();
        gl_Layer = light_system.dirlights_size + i * 6 + 5;
        gl_Position = light_system.pointlights [ i ].shadow_camera.proj * NEG_Z_ROTATION * v0_view; gs_out.texcoords = vs_out [ 0 ].texcoords; gs_out.depth = v0_depth; EmitVertex ();
        gl_Position = light_system.pointlights [ i ].shadow_camera.proj * NEG_Z_ROTATION * v1_view; gs_out.texcoords = vs_out [ 1 ].texcoords; gs_out.depth = v1_depth; EmitVertex ();
        gl_Position = light_system.pointlights [ i ].shadow_camera.proj * NEG_Z_ROTATION * v2_view; gs_out.texcoords = vs_out [ 2 ].texcoords; gs_out.depth = v2_depth; EmitVertex ();
        EndPrimitive ();
    }
    for ( int i = 0; i < light_system.spotlights_size; ++i )
    {
        /* continue if light is disabled */
        if ( !light_system.spotlights [ i ].enabled || !light_system.spotlights [ i ].shadow_mapping_enabled ) continue;

        /* set the layer */
        gl_Layer = light_system.dirlights_size * light_system.pointlights_size * 6 + i;

        /* set the positions and depths of all of the vertices, then emit the primative
         * the depth linear by using the distance to the light, and mapped to 0-1 by dividing my shadow_max_dist
         */
        gl_Position = light_system.spotlights [ i ].shadow_camera.view_proj * gl_in [ 0 ].gl_Position; gs_out.texcoords = vs_out [ 0 ].texcoords;
        gs_out.depth = length ( light_system.spotlights [ i ].position - gl_in [ 0 ].gl_Position.xyz ) / light_system.spotlights [ i ].shadow_max_dist; EmitVertex ();
        gl_Position = light_system.spotlights [ i ].shadow_camera.view_proj * gl_in [ 1 ].gl_Position; gs_out.texcoords = vs_out [ 1 ].texcoords;
        gs_out.depth = length ( light_system.spotlights [ i ].position - gl_in [ 1 ].gl_Position.xyz ) / light_system.spotlights [ i ].shadow_max_dist; EmitVertex ();
        gl_Position = light_system.spotlights [ i ].shadow_camera.view_proj * gl_in [ 2 ].gl_Position; gs_out.texcoords = vs_out [ 2 ].texcoords;
        gs_out.depth = length ( light_system.spotlights [ i ].position - gl_in [ 2 ].gl_Position.xyz ) / light_system.spotlights [ i ].shadow_max_dist; EmitVertex ();
        EndPrimitive ();
    }
}