/*
 * fragment.bloom.glsl
 * 
 * bloom fragment shader
 */



/* INPUT AND OUTPUT */

/* input texcoords */
in VS_OUT
{
    vec2 texcoords;
} vs_out;

/* output fragcolor */
out vec3 fragcolor;



/* UNIFORMS */

/* the texture to bloom */
uniform sampler2D bloom_texture;

/* bloom modes are
 *
 * 0: blur horizontally
 * 1: blur virtically
 */
uniform int bloom_mode;

/* bloom function */
uniform function_struct_1D bloom_function;

/* bloom radius
 * the number of pixels to bloom by
 * each texel will incur an increase of 1.0 in the bloom_function
 * a bloom_radius of 0 will cause no bloom, 1 will cause one pixel of bloom etc.
 */
uniform int bloom_radius;



/* MAIN */

/* main */
void main ()
{
    /* find the texoffset: the texcoord offset between texels of the texture */
    vec2 texoffset = 1.0 / textureSize ( bloom_texture, 0 );

    /* set fragcolor to the color of this texel multiplied the weight given by bloom_function */
    fragcolor = textureLod ( bloom_texture, vs_out.texcoords, 0.0 ).xyz * sample_function ( bloom_function, 0.0 ).x; 

    /* switch between bloom modes */
    if ( bloom_mode == 0 )
    {
        for ( int i = 1; i <= bloom_radius; ++i )
        {
            /* assume the bloom function is not symmetrical */
            /*
            fragcolor += textureLod ( bloom_texture, vs_out.texcoords + vec2 ( i * texoffset.x, 0.0 ), 0.0 ).xyz * sample_function ( bloom_function,  i ).x
                       + textureLod ( bloom_texture, vs_out.texcoords - vec2 ( i * texoffset.y, 0.0 ), 0.0 ).xyz * sample_function ( bloom_function, -i ).x;
            */
            /* assume the bloom function is symmetrical */
            fragcolor += ( textureLod ( bloom_texture, vs_out.texcoords + vec2 ( i * texoffset.x, 0.0 ), 0.0 ).xyz + textureLod ( bloom_texture, vs_out.texcoords - vec2 ( i * texoffset.y, 0.0 ), 0.0 ).xyz )
                       * sample_function ( bloom_function, i ).x;
        }
    } else
    {
        for ( int i = 1; i <= bloom_radius; ++i )
        {
            /* assume the bloom function is not symmetrical */
            /*
            fragcolor += textureLod ( bloom_texture, vs_out.texcoords + vec2 ( 0.0, i * texoffset.x ) ).xyz * sample_function ( bloom_function,  i ).x
                       + textureLod ( bloom_texture, vs_out.texcoords - vec2 ( 0.0, i * texoffset.y ) ).xyz * sample_function ( bloom_function, -i ).x;
             */
            /* assume the bloom function is symmetrical */
            fragcolor += ( textureLod ( bloom_texture, vs_out.texcoords + vec2 ( 0.0, i * texoffset.x ), 0.0 ).xyz + textureLod ( bloom_texture, vs_out.texcoords - vec2 ( 0.0, i * texoffset.y ), 0.0 ).xyz )
                       * sample_function ( bloom_function, i ).x;
        }
    }
}