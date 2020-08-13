/*
 * fragment.alpha_test.glsl
 * 
 * fragment shader to query about primative alpha values
 */



/* INPUT AND OUTPUT */

/* input vcolor_alpha and texcoords */
in GS_OUT
{
    float vcolor_alpha;
    vec2 texcoords [ MAX_TEXTURE_STACK_SIZE ];
} gs_out;



/* SSBO */

/* output ssbo containing some number of unsigned integers */
layout ( packed, binding = 0 ) buffer output_ssbo
{
    uint output_data [];
};




/* UNIFORMS */

/* material uniforms */
uniform material_struct material;



/* MAIN */

/* main */
void main ()
{
    /* sample the alpha of the diffuse stack */
    const float diffuse_alpha = evaluate_stack_w ( material.diffuse_stack, gs_out.texcoords );

    /* get the bit and element offset */
    uint bit_offset = ( gl_PrimitiveID % 8 ) * 4;
    uint element_offset = gl_PrimitiveID / 8;

    /* create the output mask */
    uint output_mask = 0;
    output_mask |= uint ( gs_out.vcolor_alpha >= 0.98 ) << ( bit_offset );
    output_mask |= uint ( gs_out.vcolor_alpha >= 0.02 && gs_out.vcolor_alpha <= 0.98 ) << ( bit_offset + 1 );
    output_mask |= uint ( diffuse_alpha >= 0.98 ) << ( bit_offset + 2 );
    output_mask |= uint ( diffuse_alpha >= 0.02 && diffuse_alpha <= 0.98 ) << ( bit_offset + 3 );

    /* set the output integer */
    atomicOr ( output_data [ element_offset ], output_mask );
}