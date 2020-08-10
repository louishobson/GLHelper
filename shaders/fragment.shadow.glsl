/*
 * fragment.shadow.glsl
 * 
 * shadow fragment shader
 */



/* INPUTS AND OUTPUTS */

/* input texcoords and depth */
in GS_OUT
{
    vec2 texcoords [ MAX_TEXTURE_STACK_SIZE ];
    float depth;
} gs_out;



/* UNIFORMS */

/* material uniform */
uniform material_struct material;



/* main */
void main ()
{
    /* set the depth */
    gl_FragDepth = ( material.definitely_opaque || 
        ( material.opacity > 0.99 && material.diffuse_stack.stack_size > 0 && material.diffuse_stack.base_color.w > 0.99 && texture ( material.diffuse_stack.textures, vec3 ( gs_out.texcoords [ material.diffuse_stack.levels [ 0 ].uvwsrc ], 0 ) ).w > 0.99 ) 
        ? gs_out.depth : 1.0 );
}