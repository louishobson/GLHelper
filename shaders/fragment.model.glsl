/*
 * fragment.model.glsl
 * 
 * model fragment shader
 */

#version 330 core

/* maximum number of UV channels */
#define MAX_UV_CHANNELS 8

/* maximum number of textures in texture stack */
#define MAX_TEX_STACK_SIZE 8

/* maximum number of lights */
#define MAX_NUM_LIGHTS 8

/* structure for a texture */
struct texture_stack_level_struct
{
    int blend_operation;
    float blend_strength;
    int uvwsrc;
    sampler2D texunit;
};

/* structure for a texture stack */
struct texture_stack_struct
{
    int stack_size;
    vec3 base_color;
    texture_stack_level_struct levels [ MAX_TEX_STACK_SIZE ];
};

/* structure for a material */
struct material_struct
{
    texture_stack_struct ambient_stack;
    texture_stack_struct diffuse_stack;
    texture_stack_struct specular_stack;

    int blending_mode;

    float shininess;
    float shininess_strength;

    float opacity;
};

/* structure for a light (of any type) */
struct light_struct
{
    vec3 position;
    vec3 direction;
    float inner_cone;
    float outer_cone;

    float att_const;
    float att_linear;
    float att_quad;

    vec3 ambient_color;
    vec3 diffuse_color;
    vec3 specular_color;

    bool enabled;
};

/* structure for storing multiple lights */
struct light_collection_struct
{
    int size;
    light_struct lights [ MAX_NUM_LIGHTS ];
};

/* structure for storing multiple collections of lights */
struct light_system_struct
{
    light_collection_struct dircoll;
    light_collection_struct pointcoll;
    light_collection_struct spotcoll;
};

/* transformations structure */
struct trans_struct
{
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 viewpos;
};

/* texture coords, normal vector, position, vcolor */
in vec2 texcoords [ MAX_UV_CHANNELS ];
in vec3 normal;
in vec3 fragpos;
in vec4 vcolor;

/* output color */
out vec4 fragcolor;

/* material and lighting uniforms */
uniform material_struct material;
uniform light_system_struct light_system;

/* transformation matrices */
uniform trans_struct trans;

/* boolean for whether to render opaque or transparent fragments */
uniform bool transparent_mode;



/* evaluate_stack
 *
 * evaluate multiple stacked textures
 *
 * material: the material the stack if part of
 * stack: the stack to evaluate
 *
 * return: the overall color of the fragment of the stack
 */
vec4 evaluate_stack ( material_struct mat, texture_stack_struct stack )
{
    /* get base color */
    vec4 stack_color = vec4 ( stack.base_color, material.opacity );

    /* loop through the stack */
    for ( int i = 0; i < stack.stack_size; ++i )
    {
        /* get the level color from the texture */
        vec4 level_color = texture ( stack.levels [ i ].texunit, texcoords [ stack.levels [ i ].uvwsrc ] );
        /* multiply by blend strength */
        level_color *= stack.levels [ i ].blend_strength;
        /* add to the stack */
        if ( stack.levels [ i ].blend_operation == 0 ) stack_color = stack_color * level_color; else
        if ( stack.levels [ i ].blend_operation == 1 ) stack_color = stack_color + level_color; else
        if ( stack.levels [ i ].blend_operation == 2 ) stack_color = stack_color - level_color; else
        if ( stack.levels [ i ].blend_operation == 3 ) stack_color = stack_color / level_color; else
        if ( stack.levels [ i ].blend_operation == 4 ) stack_color = ( stack_color + level_color ) - ( stack_color * level_color ); else
        if ( stack.levels [ i ].blend_operation == 5 ) stack_color = stack_color + ( level_color - 0.5 );   
    }

    /* return the stack color */
    return stack_color;
}



/* compute_attenuation
 *
 * compute the multiple of attenuation
 *
 * dist: the distance from the fragment to the light source
 * att_const: constant part
 * att_linear: linear part
 * att_quad: quadratic part
 *
 * return: the multiple of attenuation
 */
float compute_attenuation ( float dist, float att_const, float att_linear, float att_quad )
{
    return 1.0 / ( att_const + ( att_linear * dist ) + ( att_quad * dist * dist ) );
}



/* compute_ambient_component
 *
 * take a base color, material and light system and compute the ambient component of the light hitting the fragment
 *
 * base_color: the color of the fragment the light is hitting
 * mat: the material of the fragment
 * lighting: light_system_struct containing all of the light sources
 *
 * return: the ambient component of the fragment
 */
vec3 compute_ambient_component ( vec3 base_color, material_struct mat, light_system_struct lighting )
{
    /* if base colour is black, return */
    if ( base_color == vec3 ( 0.0, 0.0, 0.0 ) ) return base_color;

    /* ambient fragment color */
    vec3 ambient_color = vec3 ( 0.0, 0.0, 0.0 );

    /* loop over lights and apply attenuation where necesarry */
    for ( int i = 0; i < lighting.dircoll.size; ++i )
    {
        /* continue if disabled */
        if ( !lighting.dircoll.lights [ i ].enabled ) continue;
        
        /* add ambient light from directional source */
        ambient_color += base_color * lighting.dircoll.lights [ i ].ambient_color;
    }

    for ( int i = 0; i < lighting.pointcoll.size; ++i )
    {
        /* continue if disabled */
        if ( !lighting.pointcoll.lights [ i ].enabled ) continue;

        /* add ambient light from point source, including attenuation */
        ambient_color += base_color * lighting.pointcoll.lights [ i ].ambient_color * compute_attenuation
        (
            length ( lighting.pointcoll.lights [ i ].position - fragpos ),
            lighting.pointcoll.lights [ i ].att_const,
            lighting.pointcoll.lights [ i ].att_linear,
            lighting.pointcoll.lights [ i ].att_quad
        );
    }

    /* return ambient component */
    return ambient_color;
}



/* compute_diffuse_component
 *
 * take a base color, material and light system and compute the diffuse component of the light hitting the fragment
 *
 * base_color: the color of the fragment the light is hitting
 * mat: the material of the fragment
 * lighting: light_system_struct containing all of the light sources
 *
 * return: the diffuse component of the fragment
 */
vec3 compute_diffuse_component ( vec3 base_color, material_struct mat, light_system_struct lighting )
{
    /* if base colour is black, return */
    if ( base_color == vec3 ( 0.0, 0.0, 0.0 ) ) return base_color;

    /* diffuse fragment color */
    vec3 diffuse_color = vec3 ( 0.0, 0.0, 0.0 );

    /* loop over lights and apply attenuation where necesarry */
    for ( int i = 0; i < lighting.dircoll.size; ++i )
    {
        /* continue if disabled */
        if ( !lighting.dircoll.lights [ i ].enabled ) continue;

        /* get diffuse multiplier */
        float diff = max ( dot ( normal, -lighting.dircoll.lights [ i ].direction ), 0.0 );
        /* add diffuse light from directional source */
        diffuse_color += base_color * lighting.dircoll.lights [ i ].diffuse_color * diff;
    }

    for ( int i = 0; i < lighting.pointcoll.size; ++i )
    {
        /* continue if disabled */
        if ( !lighting.pointcoll.lights [ i ].enabled ) continue;

        /* get normalised vector from fragment to light */
        vec3 fraglightdir = normalize ( lighting.pointcoll.lights [ i ].position - fragpos );
        /* get diffuse multiplier */
        float diff = max ( dot ( normal, fraglightdir ), 0.0 );
        /* add diffuse light from point source, including attenuation */
        diffuse_color += base_color * lighting.pointcoll.lights [ i ].diffuse_color * diff * compute_attenuation
        (
            length ( lighting.pointcoll.lights [ i ].position - fragpos ),
            lighting.pointcoll.lights [ i ].att_const,
            lighting.pointcoll.lights [ i ].att_linear,
            lighting.pointcoll.lights [ i ].att_quad
        );
    }

    /* return diffuse component */
    return diffuse_color;
}



/* compute_specular_component
 *
 * take a base color, material and light system and compute the specular component of the light hitting the fragment
 *
 * base_color: the color of the fragment the light is hitting
 * mat: the material of the fragment
 * lighting: lighting_system_struct containing all of the light sources
 *
 * return: the specular component of the fragment
 */
vec3 compute_specular_component ( vec3 base_color, material_struct mat, light_system_struct lighting )
{
    /* if base colour is black, return */
    if ( base_color == vec3 ( 0.0, 0.0, 0.0 ) ) return base_color;

    /* specular fragment color */
    vec3 specular_color = vec3 ( 0.0, 0.0, 0.0 );

    /* get normaalised vector from fragment to viewer */
    vec3 fragviewdir = normalize ( trans.viewpos - fragpos );

    /* loop over lights and apply attenuation where necesarry */
    for ( int i = 0; i < lighting.dircoll.size; ++i )
    {
        /* continue if disabled */
        if ( !lighting.dircoll.lights [ i ].enabled ) continue;

        /* reflect the light off of the fragment */
        vec3 reflectlightdir = normalize ( reflect ( -lighting.dircoll.lights [ i ].direction, normal ) );
        /* get specular multiplier */
        float spec = pow ( max ( dot ( fragviewdir, reflectlightdir ), 0.0 ), material.shininess );
        /* add specular light from directional source */
        specular_color += base_color * lighting.dircoll.lights [ i ].specular_color * spec;
    }

    for ( int i = 0; i < lighting.pointcoll.size; ++i )
    {
        /* continue if disabled */
        if ( !lighting.pointcoll.lights [ i ].enabled ) continue;

        /* get normalised vector from fragment to light */
        vec3 fraglightdir = normalize ( lighting.pointcoll.lights [ i ].position - fragpos );
        /* reflect the light off of the fragment */
        vec3 reflectlightdir = normalize ( reflect ( fraglightdir, normal ) );
        /* get specular multiplier */
        float spec = pow ( max ( dot ( fragviewdir, reflectlightdir ), 0.0 ), material.shininess );
        /* add specular light from point source, including attenuation */
        specular_color += base_color * lighting.pointcoll.lights [ i ].specular_color * spec * compute_attenuation
        (
            length ( lighting.pointcoll.lights [ i ].position - fragpos ),
            lighting.pointcoll.lights [ i ].att_const,
            lighting.pointcoll.lights [ i ].att_linear,
            lighting.pointcoll.lights [ i ].att_quad
        );
    }

    /* return specilar component multiplied by shininess_strength */
    return specular_color * material.shininess_strength;
}



/* main */
void main ()
{
    /* evaluate stacks */
    vec4 ambient = evaluate_stack ( material, material.ambient_stack );
    vec4 diffuse = evaluate_stack ( material, material.diffuse_stack );
    vec4 specular = evaluate_stack ( material, material.specular_stack );

    /* if there were no textures for any stack, multiply by the vertex color */
    if ( material.ambient_stack.stack_size == 0 ) ambient *= vcolor;
    if ( material.diffuse_stack.stack_size == 0 ) diffuse *= vcolor;
    if ( material.specular_stack.stack_size == 0 ) specular *= vcolor;

    /* if is completely transparent, discard regardless of whether in transparent mode */
    if ( ambient.a + diffuse.a == 0.0 ) discard;

    /* discard if opaque/transparent */
    if ( transparent_mode )
    {
        if ( ambient.a + diffuse.a >= 1.0 ) discard;
    } else
    {
        if ( ambient.a + diffuse.a < 1.0 ) discard;
    }

    /* calculate lighting colors */
    ambient.rgb = compute_ambient_component ( ambient.rgb, material, light_system );
    diffuse.rgb = compute_diffuse_component ( diffuse.rgb, material, light_system );
    specular.rgb = compute_specular_component ( specular.rgb, material, light_system );

    /* set output color */
    if ( transparent_mode ) fragcolor = vec4 ( ambient.rgb + diffuse.rgb + specular.rgb, ambient.a + diffuse.a );
    else fragcolor = vec4 ( ambient.rgb + diffuse.rgb + specular.rgb, 1.0 );
}