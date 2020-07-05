/*
 * fragment.model.glsl
 * 
 * model fragment shader
 */



/* INPUT AND OUTPUT */

/* input fragpos, tbn_matrix, vcolor and texcoords */
in VS_OUT
{
    vec3 fragpos;
    mat3 tbn_matrix;
    vec4 vcolor;
    vec3 texcoords [ MAX_TEXTURE_STACK_SIZE ];
} vs_out;

/* output color */
out vec4 fragcolor;



/* UNIFORMS */

/* material and lighting uniforms */
uniform material_struct material;
uniform light_system_struct light_system;

/* camera matrices */
uniform camera_struct camera;

/* transparency modes are: 
 *
 * 0: render everything
 * 1: only transparent
 * 2: only opaque
 */
uniform int transparent_mode;



/* MAIN */

/* main */
void main ()
{
    /* evaluate stacks */
    vec4 ambient = evaluate_stack ( material.diffuse_stack, vs_out.texcoords );
    vec4 diffuse = ambient;
    vec4 specular = evaluate_stack ( material.specular_stack, vs_out.texcoords );

    /* evaluate new normal */
    vec3 normal = evaluate_normal ( material.normal_stack, vs_out.texcoords, vs_out.tbn_matrix );

    /* discard if opaque/transparent */
    switch ( transparent_mode )
    {
        case 0: break;
        case 1: if ( diffuse.a * material.opacity >= 1.0 ) discard; break;
        case 2: if ( diffuse.a * material.opacity < 0.98 ) discard; break;
        default: break;
    }

    /* calculate lighting color */
    fragcolor = vec4
    (
        compute_lighting ( ambient.xyz, diffuse.xyz, specular.xyz, material.shininess, material.shininess_strength, vs_out.fragpos, camera.viewpos, normal, light_system ),
        diffuse.a * material.opacity
    );
}