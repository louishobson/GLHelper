/*
 * fragment.model.glsl
 * 
 * model fragment shader
 */



/* INPUT AND OUTPUT */

/* input fragpos, normal, vcolor and texcoords */
in VS_OUT
{
    vec3 fragpos;
    vec3 normal;
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
    vec4 ambient;
    if ( material.ambient_stack.stack_size > 0 ) ambient = evaluate_stack ( material.ambient_stack, vs_out.texcoords );
    else ambient = evaluate_stack ( material.diffuse_stack, vs_out.texcoords );
    vec4 diffuse = evaluate_stack ( material.diffuse_stack, vs_out.texcoords );
    vec4 specular = evaluate_stack ( material.specular_stack, vs_out.texcoords );

    /* if there were no textures for any stack, multiply by the vertex color */
    if ( material.ambient_stack.stack_size == 0 && material.diffuse_stack.stack_size == 0 && material.specular_stack.stack_size == 0 )
    {
        ambient *= vs_out.vcolor;
        diffuse *= vs_out.vcolor;
        specular *= vs_out.vcolor;
    } 

    /* if is completely transparent, discard regardless of whether in transparent mode */
    if ( diffuse.a == 0.0 ) discard;

    /* discard if opaque/transparent */
    switch ( transparent_mode )
    {
        case 0: break;
        case 1: if ( ambient.a >= 1.0 ) discard; break;
        case 2: if ( ambient.a < 1.0 ) discard; break;
        default: break;
    }

    /* calculate lighting color */
    fragcolor = vec4
    (
        compute_lighting ( ambient.xyz, diffuse.xyz, specular.xyz, material.shininess, material.shininess_strength, vs_out.fragpos, camera.viewpos, vs_out.normal, light_system ),
        diffuse.a * material.opacity
    );

}