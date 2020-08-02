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
    vec2 texcoords [ MAX_TEXTURE_STACK_SIZE ];
} vs_out;

/* main output color */
layout ( location = 0 ) out vec4 main_fragcolor;

/* the emission output color */
layout ( location = 1 ) out vec4 emission_fragcolor;



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
    /* evaluate color stacks */
    vec4 diffuse; evaluate_stack_macro ( diffuse, material.diffuse_stack, vs_out.texcoords );
    vec4 specular; evaluate_stack_macro ( specular, material.specular_stack, vs_out.texcoords );

    /* discard if opaque/transparent depending on transparent_mode */
    switch ( transparent_mode )
    {
        case 0: break;
        case 1: if ( diffuse.a * material.opacity >= 1.0 ) discard; break;
        case 2: if ( diffuse.a * material.opacity < 0.98 ) discard; break;
        default: break;
    }

    /* evaluate normal */
    vec3 normal; evaluate_normal_macro ( normal, material.normal_stack, vs_out.texcoords, vs_out.tbn_matrix );

    /* main output color */
    compute_lighting_macro ( main_fragcolor.xyz, diffuse.xyz, diffuse.xyz, specular.xyz, material.shininess, material.shininess_strength, vs_out.fragpos, camera.viewpos, normal, light_system );
    //compute_lighting_macro ( main_fragcolor.xyz, ambient.xyz, diffuse.xyz, 0.5.xxx, 128, 0.5, vs_out.fragpos, camera.viewpos, normal, light_system );
    main_fragcolor.w = diffuse.a * material.opacity;

    /* emission output color */
    evaluate_stack_macro ( emission_fragcolor, material.emission_stack, vs_out.texcoords );
}