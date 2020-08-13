/*
 * fragment.deferred_model.glsl
 * 
 * model fragment shader for deferred shading
 */



/* INPUT AND OUTPUT */

/* input fragpos, tbn_matrix, vcolor and texcoords */
in VS_OUT
{
    vec3 fragpos;
    mat3 tbn_matrix;
    vec4 vcolor;
    vec2 texcoords [ MAX_TEXTURE_STACK_SIZE ];
} vs_out;

/* fragment position and shininess */
layout ( location = 0 ) out vec4 output_positionshininess;

/* fragment normal and shininess strength */
layout ( location = 1 ) out vec4 output_normalsstrength;

/* albedo and specular output colors */
layout ( location = 2 ) out vec4 output_albedospec;

/* emission output color */
layout ( location = 3 ) out vec3 output_emission;



/* UNIFORMS */

/* material uniform */
uniform material_struct material;

/* transparency modes are: 
 *
 * 0: render everything
 * 1: only transparent
 * 2: only opaque
 */
uniform int transparent_mode;



/* main */
void main ()
{
    /* set output_positionshininess */
    output_positionshininess = vec4 ( vs_out.fragpos, material.shininess );

    /* set output_normal */
    output_normalsstrength.xyz = evaluate_normal ( material.normal_stack, vs_out.texcoords, vs_out.tbn_matrix );
    output_normalsstrength.w = material.shininess_strength;

    /* set albedo part of output_albedospec */
    output_albedospec = evaluate_stack_xyzw ( material.diffuse_stack, vs_out.texcoords );

    /* discard if opacity is less than 0.02 */
    if ( output_albedospec.w * material.opacity < 0.02 ) discard;

    /* discard if opaque/transparent depending on transparent_mode */
    switch ( transparent_mode )
    {
        case 0: break;
        case 1: if ( output_albedospec.w * material.opacity > 0.98 ) discard; break;
        case 2: if ( output_albedospec.w * material.opacity < 0.98 ) discard; break;
        default: break;
    }

    /* set specular part of output_albedospec */
    output_albedospec.w = evaluate_stack_x ( material.specular_stack, vs_out.texcoords );

    /* set output_emission */
    output_emission = evaluate_stack_xyz ( material.emission_stack, vs_out.texcoords );
}

