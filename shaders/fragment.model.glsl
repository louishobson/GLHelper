/*
 * fragment.model.glsl
 * 
 * model fragment shader
 */

/* maximum number of color sets */
#define MAX_COLOR_SETS 1



/* texture coords, normal vector, position, vcolor */
in VS_OUT
{
    vec3 fragpos;
    vec3 normal;
    vec4 vcolor [ MAX_COLOR_SETS ];
    vec3 texcoords [ MAX_UV_CHANNELS ];
} fs_in;



/* output color */
out vec4 fragcolor;



/* material and lighting uniforms */
uniform material_struct material;
uniform light_system_struct light_system;

/* camera matrices */
uniform camera_struct camera;

/* modes are: 
 *
 * 0: render everything
 * 1: only transparent
 * 2: only opaque
 */
uniform int transparent_mode;



uniform samplerCube skybox;

/* main */
void main ()
{
    /* evaluate stacks */
    vec4 ambient;
    if ( material.ambient_stack.stack_size > 0 ) ambient = evaluate_stack ( material.ambient_stack, fs_in.texcoords );
    else ambient = evaluate_stack ( material.diffuse_stack, fs_in.texcoords );
    vec4 diffuse = evaluate_stack ( material.diffuse_stack, fs_in.texcoords );
    vec4 specular = evaluate_stack ( material.specular_stack, fs_in.texcoords );

    /* if there were no textures for any stack, multiply by the vertex color */
    if ( material.ambient_stack.stack_size == 0 && material.diffuse_stack.stack_size == 0 && material.specular_stack.stack_size == 0 )
    {
        ambient *= fs_in.vcolor [ 0 ];
        diffuse *= fs_in.vcolor [ 0 ];
        specular *= fs_in.vcolor [ 0 ];
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
        compute_lighting ( ambient.xyz, diffuse.xyz, specular.xyz, material.shininess, material.shininess_strength, fs_in.fragpos, camera.viewpos, fs_in.normal, light_system ),
        diffuse.a * material.opacity
    );

}