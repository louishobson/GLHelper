/*
 * fragment.shadow.glsl
 * 
 * shadow fragment shader
 */



/* INPUTS AND OUTPUTS */

/* input texcoords and depth */
in GS_OUT
{
    vec3 texcoords [ MAX_TEXTURE_STACK_SIZE ];
    float depth;
} gs_out;



/* UNIFORMS */

/* material uniform */
uniform material_struct material;



/* main */
void main ()
{
    /* sample diffuse stack, and discard if at all transparent */
    if ( !material.definitely_opaque && material.opacity >= 1.0 && evaluate_stack_transparency ( material.diffuse_stack, gs_out.texcoords, 0.99 ) ) discard;

    /* set the depth */
    gl_FragDepth = gs_out.depth;
}