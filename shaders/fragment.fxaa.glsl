/*
 * fragment.fxaa.glsl
 * 
 * fragment shader to apply FXAA
 *
 * see https://catlikecoding.com/unity/tutorials/advanced-rendering/fxaa/ for explanation of maths
 */



/* MACROS */

/* NUM_EDGE_SAMPLES
 *
 * the number of samples left and right of the centre texel to sample
 */
#define NUM_EDGE_SAMPLES 20

/* EDGE_SAMPLE_POSITIONS
 *
 * the positions of edge samples
 */
#define EDGE_SAMPLE_POSITIONS 1.0, 1.5, 2.0, 2.5, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 12.0, 14.0, 16.0, 18.0, 20.0, 24.0, 28.0, 32.0, 40.0



/* INPUTS AND OUTPUTS */

/* input texcoords */
in VS_OUT
{
    vec2 texcoords;
} vs_out;

/* output fragcolor */
out vec3 fragcolor;



/* UNIFORMS */

/* the texture to apply FXAA to */
uniform sampler2D fxaa_texture;
 
/* the constant and relative thresholds for contrast */
uniform float contrast_constant_threshold;
uniform float contrast_relative_threshold;

/* an array of edge sampl positions */
uniform float edge_sample_positions [ NUM_EDGE_SAMPLES + 1 ] = { EDGE_SAMPLE_POSITIONS };



/* STRUCTURES */

/* neighborhood_info_struct
 *
 * stores information about the neighborhood texels
 */
struct neighborhood_info_struct
{
    /* luminances */
    float centre;

    /* the luminances of the texels directly above, below and to the side of the centre texel */
    vec4 neighbors;
    
    /* the luminances of the texels diagonal to the centre texel */
    vec4 dneighbors;

    /* the neighborhood contrast */
    float contrast;

    /* the pixel blend factor */
    float pixel_blend_factor;

    /* boolean for if the edge is horizontal */
    bool is_horizontal;

    /* two vectors to step acros the edge and to step along the edge */
    vec2 step_across;
    vec2 step_along;

    /* the texcoordinates of the fragment plus half of step_across */
    vec2 edge_texcoords;

    /* the edge luminance and gradient threshold */
    float edge_luminance;
    float gradient_threshold;
};



/* FUNCTIONS */

/* compute_luminance
 *
 * computes the luminance of a color
 *
 * color: the color to get the luminance of
 *
 * return: a scalar representing the luminance
 *
 * prototype: float compute_luminance ( const vec3 color )
 */
#define compute_luminance( color ) ( dot ( color, vec3 ( 0.2126729f,  0.7151522f, 0.0721750f ) ) )

/* sample_luminance
 *
 * sample the luminance from a sampler2D
 *
 * color_tetxure: the texture to sample
 * texcoords: the texture coords to sample with
 *
 * return: the luminance of the sampled texel
 *
 * prototype: float sample_luminance ( sampler2D color_texture, const vec2 texcoords )
 */
#define sample_luminance( color_texture, texcoords ) ( compute_luminance ( texture ( color_texture, texcoords ).xyz ) )

/* sample_luminance_offset
 *
 * sample the luminance from a sampler2D using an offset
 *
 * color_tetxure: the texture to sample
 * texcoords: the texture coords to sample with
 * offset: the offset to use
 *
 * return: the luminance of the sampled texel
 *
 * prototype: float sample_luminance ( sampler2D color_texture, const vec2 texcoords, const ivec2 offset )
 */
#define sample_luminance_offset( color_texture, texcoords, offset ) ( compute_luminance ( textureOffset ( color_texture, texcoords, offset ).xyz ) )




/* MAIN */

/* main */
void main ()
{
    /* set fragcolor to the color of the fragement */
    fragcolor = texture ( fxaa_texture, vs_out.texcoords ).xyz;

    /* create the neighborhood_info_struct */
    neighborhood_info_struct nb_info;

    /* gather the luminances */
    nb_info.centre = sample_luminance ( fxaa_texture, vs_out.texcoords );
    nb_info.neighbors = vec4
    (
        sample_luminance_offset ( fxaa_texture, vs_out.texcoords, ivec2 ( -1, 0 ) ), sample_luminance_offset ( fxaa_texture, vs_out.texcoords, ivec2 ( 0,  1 ) ),
        sample_luminance_offset ( fxaa_texture, vs_out.texcoords, ivec2 (  1, 0 ) ), sample_luminance_offset ( fxaa_texture, vs_out.texcoords, ivec2 ( 0, -1 ) )
    );
    nb_info.dneighbors = vec4 
    (
        sample_luminance_offset ( fxaa_texture, vs_out.texcoords, ivec2 ( -1,  1 ) ), sample_luminance_offset ( fxaa_texture, vs_out.texcoords, ivec2 (  1,  1 ) ),
        sample_luminance_offset ( fxaa_texture, vs_out.texcoords, ivec2 (  1, -1 ) ), sample_luminance_offset ( fxaa_texture, vs_out.texcoords, ivec2 ( -1, -1 ) )
    );

    /* get the min/max luminances */
    const vec2 minmax_luminance = vec2 
    (
        min ( nb_info.centre, min ( nb_info.neighbors.x, min ( nb_info.neighbors.y, min ( nb_info.neighbors.z, nb_info.neighbors.w ) ) ) ), 
        max ( nb_info.centre, max ( nb_info.neighbors.x, max ( nb_info.neighbors.y, max ( nb_info.neighbors.z, nb_info.neighbors.w ) ) ) ) 
    );

    /* set the contrast */
    nb_info.contrast = ( ( minmax_luminance.y - minmax_luminance.x ) < max ( contrast_constant_threshold, minmax_luminance.y * contrast_relative_threshold ) ? 0 : ( minmax_luminance.y - minmax_luminance.x ) );

    /* return if contrast is 0 */
    if ( nb_info.contrast == 0.0 ) return;

    /* set the pixel blend factor */
    nb_info.pixel_blend_factor = smoothstep ( 0.0, 1.0, clamp ( abs ( ( dot ( nb_info.neighbors, vec4 ( 2.0 ) ) + dot ( nb_info.dneighbors, vec4 ( 1.0 ) ) ) / 12.0 - nb_info.centre ) / nb_info.contrast, 0.0, 1.0 ) );
    nb_info.pixel_blend_factor *= nb_info.pixel_blend_factor;

    /* compute if is horizontal */
    nb_info.is_horizontal =
    (
        dot ( abs ( vec3 ( nb_info.neighbors.y + nb_info.neighbors.w - 2 * nb_info.centre, nb_info.dneighbors.x + nb_info.dneighbors.w - 2 * nb_info.neighbors.x, nb_info.dneighbors.y + nb_info.dneighbors.z - 2 * nb_info.neighbors.z ) ), vec3 ( 2.0, 1.0, 1.0 ) ) >=
        dot ( abs ( vec3 ( nb_info.neighbors.x + nb_info.neighbors.z - 2 * nb_info.centre, nb_info.dneighbors.x + nb_info.dneighbors.y - 2 * nb_info.neighbors.y, nb_info.dneighbors.w + nb_info.dneighbors.z - 2 * nb_info.neighbors.w ) ), vec3 ( 2.0, 1.0, 1.0 ) )
    );

    /* get the texel size */
    const vec2 texel_size = 1.0 / textureSize ( fxaa_texture, 0 );

    /* depending which of the four sides the edge is on...
     * find the step along vector (which is actually only dependent on whether the edge is horizontal or not)
     * the vector to step across the edge
     * the luminance of the edge
     * and the threshold gradient
     */
    if ( nb_info.is_horizontal )
    {
        nb_info.step_along = vec2 ( texel_size.x, 0.0 );
        if ( abs ( nb_info.neighbors.y - nb_info.centre ) > abs ( nb_info.neighbors.w - nb_info.centre ) )
        {
            nb_info.step_across = vec2 ( 0.0, texel_size.y );
            nb_info.edge_luminance = ( nb_info.centre + nb_info.neighbors.y ) * 0.5;
            nb_info.gradient_threshold = abs ( nb_info.neighbors.y - nb_info.centre ) * 0.25;
        } else
        {
            nb_info.step_across = vec2 ( 0.0, -texel_size.y );
            nb_info.edge_luminance = ( nb_info.centre + nb_info.neighbors.w ) * 0.5;
            nb_info.gradient_threshold = abs ( nb_info.neighbors.w - nb_info.centre ) * 0.25;
        }
    } else
    {
        nb_info.step_along = vec2 ( 0.0, texel_size.y );
        if ( abs ( nb_info.neighbors.x - nb_info.centre ) > abs ( nb_info.neighbors.z - nb_info.centre ) )
        {
            nb_info.step_across = vec2 ( -texel_size.x, 0.0 );
            nb_info.edge_luminance = ( nb_info.centre + nb_info.neighbors.x ) * 0.5;
            nb_info.gradient_threshold = abs ( nb_info.neighbors.x - nb_info.centre ) * 0.25;
        } else
        {
            nb_info.step_across = vec2 ( texel_size.x, 0.0 );
            nb_info.edge_luminance = ( nb_info.centre + nb_info.neighbors.z ) * 0.5;
            nb_info.gradient_threshold = abs ( nb_info.neighbors.z - nb_info.centre ) * 0.25;
        }
    }

    /* set the edge texcoords */
    nb_info.edge_texcoords = vs_out.texcoords + nb_info.step_across * 0.5;

    /* loop over the edge to find the ends */
    bvec2 end_of_edge = bvec2 ( false );
    vec2 end_position = vec2 ( 0.0 );
    vec2 end_luminance = vec2 ( nb_info.edge_luminance );
    vec2 end_luminance_delta = vec2 ( 0.0 );
    for ( int i = 0; i < NUM_EDGE_SAMPLES; ++i )
    {
        if ( !end_of_edge.x )
        {
            end_position.x = edge_sample_positions [ i ];
            end_luminance.x = sample_luminance ( fxaa_texture, edge_sample_positions [ i ] * nb_info.step_along + nb_info.edge_texcoords );
            end_luminance_delta.x = end_luminance.x - nb_info.edge_luminance;
            end_of_edge.x = abs ( end_luminance_delta.x ) >= nb_info.gradient_threshold;
        }
        if ( !end_of_edge.y )
        {
            end_position.y = edge_sample_positions [ i ];
            end_luminance.y = sample_luminance ( fxaa_texture, -edge_sample_positions [ i ] * nb_info.step_along + nb_info.edge_texcoords );
            end_luminance_delta.y = end_luminance.y - nb_info.edge_luminance;
            end_of_edge.y = abs ( end_luminance_delta.y ) >= nb_info.gradient_threshold;
        } 
    }

    /* if has not reached edge, guess where edge is */
    end_position.x = ( end_of_edge.x ? end_position.x : edge_sample_positions [ NUM_EDGE_SAMPLES ] );
    end_position.y = ( end_of_edge.y ? end_position.y : edge_sample_positions [ NUM_EDGE_SAMPLES ] );

    /* don't blend in the wrong direction */
    if ( ( end_position.x < end_position.y ? sign ( end_luminance_delta.x ) : sign ( end_luminance_delta.y ) ) == sign ( nb_info.centre - nb_info.edge_luminance ) ) return;

    /* get the blend factor */
    const float blend_factor = max ( nb_info.pixel_blend_factor, 0.5 - min ( end_position.x, end_position.y ) / ( end_position.x + end_position.y - 1.0 ) );

    /* set fragcolor */
    fragcolor = texture ( fxaa_texture, vs_out.texcoords + nb_info.step_across * blend_factor ).xyz;
}
