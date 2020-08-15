/*
 * fragment.forward_model.glsl
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
layout ( location = 1 ) out vec3 emission_fragcolor;



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
//uniform int transparent_mode;



/* MAIN */

/* main */
void main ()
{
    /* evaluate color stacks */
    const vec4 diffuse = evaluate_stack_xyzw ( material.diffuse_stack, vs_out.texcoords );
    const float specular = evaluate_stack_x ( material.specular_stack, vs_out.texcoords );

    /* discard if opacity is less than 0.02 */
    //if ( diffuse.w * material.opacity < 0.02 ) discard;

    /* discard if opaque/transparent depending on transparent_mode */
    /*
    switch ( transparent_mode )
    {
        case 0: break;
        case 1: if ( diffuse.w * material.opacity > 0.98 ) discard; break;
        case 2: if ( diffuse.w * material.opacity < 0.98 ) discard; break;
        default: break;
    }
    */

    /* evaluate normal */
    const vec3 normal = evaluate_normal ( material.normal_stack, vs_out.texcoords, vs_out.tbn_matrix );

    /* main output color */
    main_fragcolor.xyz = compute_lighting
    ( 
        diffuse.xyz, 
        diffuse.xyz, 
        specular.xxx, 
        material.shininess, 
        material.shininess_strength, 
        vs_out.fragpos, 
        camera.viewpos, 
        normal, 
        light_system 
    );
    main_fragcolor.w = diffuse.a * material.opacity;

    /* emission output color */
    emission_fragcolor = evaluate_stack_xyz ( material.emission_stack, vs_out.texcoords );
}