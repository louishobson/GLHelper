/*
 * geometry.alpha_test.glsl
 * 
 * geometry shader to query about primative alpha values
 */



/* INPUTS AND OUTPUTS */

/* recieve triangles from vertex shader */
layout ( triangles ) in;

/* input normal, tangent, vcolor_alpha and texcoords */
in VS_OUT
{
    float vcolor_alpha;
    vec2 texcoords [ MAX_TEXTURE_STACK_SIZE ];
} vs_out [ 3 ];

/* output triangle strip */
layout ( triangle_strip, max_vertices = 3 ) out;

/* output vcolor_alpha and texcoords */
out GS_OUT
{
    float vcolor_alpha;
    vec2 texcoords [ MAX_TEXTURE_STACK_SIZE ];
} gs_out;



/* UNIFORMS */

/* material uniforms */
uniform material_struct material;



/* MAIN */

/* main */
void main ()
{
    /* create inverse of the tbn matrix for primative
     * the tangent is assumed to be the vector from vertex 0 to vertex 1
     * the normal is calculated from the cross product between the vectors from 0 to 1 and 0 to 2
     * the bitangent is then calculated from the cross product of the normal and the tangent
     */
    mat3 inv_tbn_matrix = mat3
    ( 
        normalize ( gl_in [ 1 ].gl_Position.xyz - gl_in [ 0 ].gl_Position.xyz ),
        vec3 ( 0.0 ),
        normalize ( cross ( gl_in [ 1 ].gl_Position.xyz - gl_in [ 0 ].gl_Position.xyz, gl_in [ 2 ].gl_Position.xyz - gl_in [ 0 ].gl_Position.xyz ) )
    );
    inv_tbn_matrix [ 1 ] = normalize ( cross ( inv_tbn_matrix [ 2 ], inv_tbn_matrix [ 0 ] ) );
    inv_tbn_matrix = inverse ( inv_tbn_matrix );

    /* loop through the texture coords for each level of the diffuse stack
     * for each level, find the area of the triangle created by the texture coords
     * finish with the maximum of all of the levels
     */
    float max_texcoords_area = 0.0;
    for ( int i = 0; i < material.diffuse_stack.stack_size; ++i )
    {
        max_texcoords_area = max ( max_texcoords_area, length ( cross 
        (
            vec3 ( vs_out [ 1 ].texcoords [ material.diffuse_stack.levels [ i ].uvwsrc ] - vs_out [ 0 ].texcoords [ material.diffuse_stack.levels [ i ].uvwsrc ], 0.0 ),
            vec3 ( vs_out [ 2 ].texcoords [ material.diffuse_stack.levels [ i ].uvwsrc ] - vs_out [ 0 ].texcoords [ material.diffuse_stack.levels [ i ].uvwsrc ], 0.0 )
        ) ) );
    }

    /* multiply each input vertex by the inverse tbn matrix, then lose the z component
     * the effect of this transformation is to rotate the face to be in a plane parallel to the xy plane
     * in other words, the z components of all of the vertices should be the same
     */
    vec2 positions [ 3 ] =
    {
        ( inv_tbn_matrix * gl_in [ 0 ].gl_Position.xyz ).xy,
        ( inv_tbn_matrix * gl_in [ 1 ].gl_Position.xyz ).xy,
        ( inv_tbn_matrix * gl_in [ 2 ].gl_Position.xyz ).xy
    };

    /* find the minimum x and y value */
    const vec2 min_positions = min ( positions [ 0 ], min ( positions [ 1 ], positions [ 2 ] ) );

    /* use this vector to translate the primative such that all points are within the positive quadrant */
    positions [ 0 ] -= min_positions; positions [ 1 ] -= min_positions; positions [ 2 ] -= min_positions;

    /* find maximum x and y value */
    const vec2 max_positions = max ( positions [ 0 ], max ( positions [ 1 ], positions [ 2 ] ) );
    
    /* scale positions to the range [0-1] */
    float scale = 1.0 / max ( max_positions.x, max_positions.y );
    positions [ 0 ] *= scale; positions [ 1 ] *= scale; positions [ 2 ] *= scale;

    /* the scale is to map the positions in at most the range of [0-2]:
     * the "max_texcoords_area / length ( cross ( vec3 ( positions [ 1 ] - positions [ 0 ], 0.0 ), vec3 ( positions [ 2 ] - positions [ 0 ], 0.0 ) ) )" part finds the fraction of the area of the texcoords over the area of positions
     * this is then square rooted, since for the area to be increased by some constant, the lengths must be increased by the sqrt of that constant
     * this value can, however, be no larger than one since the triangle formes by positions is already at its maximum size
     * this is then multipled by two, since positions must be mapped to range [0-2]
     */
    scale = 2.0 * clamp ( sqrt ( max_texcoords_area / length ( cross ( vec3 ( positions [ 1 ] - positions [ 0 ], 0.0 ), vec3 ( positions [ 2 ] - positions [ 0 ], 0.0 ) ) ) ), 0.1, 1.0 );

    /* emit vertices */
    gl_Position = vec4 ( clamp ( positions [ 0 ] * scale - 1.0, -1.0, 1.0 ), 0.0, 1.0 ); gs_out.vcolor_alpha = vs_out [ 0 ].vcolor_alpha; gs_out.texcoords = vs_out [ 0 ].texcoords; gl_PrimitiveID = gl_PrimitiveIDIn; EmitVertex ();
    gl_Position = vec4 ( clamp ( positions [ 1 ] * scale - 1.0, -1.0, 1.0 ), 0.0, 1.0 ); gs_out.vcolor_alpha = vs_out [ 1 ].vcolor_alpha; gs_out.texcoords = vs_out [ 1 ].texcoords; gl_PrimitiveID = gl_PrimitiveIDIn; EmitVertex ();
    gl_Position = vec4 ( clamp ( positions [ 2 ] * scale - 1.0, -1.0, 1.0 ), 0.0, 1.0 ); gs_out.vcolor_alpha = vs_out [ 2 ].vcolor_alpha; gs_out.texcoords = vs_out [ 2 ].texcoords; gl_PrimitiveID = gl_PrimitiveIDIn; EmitVertex ();

    /* end primitive */
    EndPrimitive ();
}