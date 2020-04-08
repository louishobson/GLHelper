/*
 * fragment.glsl
 * 
 * fragment shader
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
};

/* structure for storing multiple lights of each type */
struct lighting_struct
{
    int directional_size;
    light_struct directional [ MAX_NUM_LIGHTS ];

    int point_size;
    light_struct point [ MAX_NUM_LIGHTS ];

    int spotlight_size;
    light_struct spotlight [ MAX_NUM_LIGHTS ];
};

/* transformations structure */
struct trans_struct
{
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 viewpos;
};

/* texture coords, normal vector and position */
in vec2 texcoords [ MAX_UV_CHANNELS ];
in vec3 normal;
in vec3 fragpos;

/* output color */
out vec4 fragcolor;

/* material and lighting uniforms */
uniform material_struct material;
uniform lighting_struct lighting;

/* transformation matrices */
uniform trans_struct trans;



/* evaluate_stack
 *
 * evaluate multiple stacked textures
 *
 * material: the material the stack if part of
 * stack: the stack to evaluate
 *
 * return: the overall color of the fragment of the stack
 */
vec3 evaluate_stack ( material_struct mat, texture_stack_struct stack )
{
    /* get base color */
    vec3 stack_color = stack.base_color;

    /* loop through the stack */
    for ( int i = 0; i < stack.stack_size; ++i )
    {
        /* get the level color from the texture */
        vec3 level_color = texture ( stack.levels [ i ].texunit, texcoords [ stack.levels [ i ].uvwsrc ] ).rgb;
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
 * take a material and compute the ambient component of the light hitting the fragment
 *
 * mat: the material of the fragment
 * lights: lighting_struct containing all of the light sources
 *
 * return: the ambient component of the fragment
 */
vec3 compute_ambient_component ( material_struct mat, lighting_struct lights )
{
    /* evaluate the ambient stack */
    vec3 base_color = evaluate_stack ( mat, mat.ambient_stack );

    /* if base colour is black, return */
    if ( base_color == vec3 ( 0.0, 0.0, 0.0 ) ) return base_color;

    /* ambient fragment color */
    vec3 ambient_color = vec3 ( 0.0, 0.0, 0.0 );

    /* loop over lights and apply attenuation where necesarry */
    for ( int i = 0; i < lights.directional_size; ++i )
    {
        /* add ambient light from directional source */
        ambient_color += base_color * lights.directional [ i ].ambient_color;
    }

    for ( int i = 0; i < lights.point_size; ++i )
    {
        /* add ambient light from point source, including attenuation */
        ambient_color += base_color * lights.point [ i ].ambient_color * compute_attenuation
        (
            length ( lights.point [ i ].position - fragpos ),
            lights.point [ i ].att_const,
            lights.point [ i ].att_linear,
            lights.point [ i ].att_quad
        );
    }

    /* return ambient component */
    return ambient_color;
}



/* compute_diffuse_component
 *
 * take a material and compute the diffuse component of the light hitting the fragment
 *
 * mat: the material of the fragment
 * lights: lighting_struct containing all of the light sources
 *
 * return: the diffuse component of the fragment
 */
vec3 compute_diffuse_component ( material_struct mat, lighting_struct lights )
{
    /* evaluate the diffuse stack */
    vec3 base_color = evaluate_stack ( mat, mat.diffuse_stack );

    /* if base colour is black, return */
    if ( base_color == vec3 ( 0.0, 0.0, 0.0 ) ) return base_color;

    /* diffuse fragment color */
    vec3 diffuse_color = vec3 ( 0.0, 0.0, 0.0 );

    /* loop over lights and apply attenuation where necesarry */
    for ( int i = 0; i < lights.directional_size; ++i )
    {
        /* get diffuse multiplier */
        float diff = max ( dot ( normal, -lights.directional [ i ].direction ), 0.0 );
        /* add diffuse light from directional source */
        diffuse_color += base_color * lights.directional [ i ].diffuse_color * diff;
    }

    for ( int i = 0; i < lights.point_size; ++i )
    {
        /* get normalised vector from fragment to light */
        vec3 fraglightdir = normalize ( lights.point [ i ].position - fragpos );
        /* get diffuse multiplier */
        float diff = max ( dot ( normal, fraglightdir ), 0.0 );
        /* add diffuse light from point source, including attenuation */
        diffuse_color += base_color * lights.point [ i ].diffuse_color * diff * compute_attenuation
        (
            length ( lights.point [ i ].position - fragpos ),
            lights.point [ i ].att_const,
            lights.point [ i ].att_linear,
            lights.point [ i ].att_quad
        );
    }

    /* return diffuse component */
    return diffuse_color;
}



/* compute_specular_component
 *
 * take a material and compute the specular component of the light hitting the fragment
 *
 * mat: the material of the fragment
 * lights: lighting_struct containing all of the light sources
 *
 * return: the specular component of the fragment
 */
vec3 compute_specular_component ( material_struct mat, lighting_struct lights )
{
    /* evaluate the specular stack */
    vec3 base_color = evaluate_stack ( mat, mat.specular_stack );

    /* if base colour is black, return */
    if ( base_color == vec3 ( 0.0, 0.0, 0.0 ) ) return base_color;

    /* specular fragment color */
    vec3 specular_color = vec3 ( 0.0, 0.0, 0.0 );

    /* get normaalised vector from fragment to viewer */
    vec3 fragviewdir = normalize ( trans.viewpos - fragpos );

    /* loop over lights and apply attenuation where necesarry */
    for ( int i = 0; i < lights.directional_size; ++i )
    {
        /* reflect the light off of the fragment */
        vec3 reflectlightdir = normalize ( reflect ( -lights.directional [ i ].direction, normal ) );
        /* get specular multiplier */
        float spec = pow ( max ( dot ( fragviewdir, reflectlightdir ), 0.0 ), material.shininess );
        /* add specular light from directional source */
        specular_color += base_color * lights.directional [ i ].specular_color * spec;
    }

    for ( int i = 0; i < lights.point_size; ++i )
    {
        /* get normalised vector from fragment to light */
        vec3 fraglightdir = normalize ( lights.point [ i ].position - fragpos );
        /* reflect the light off of the fragment */
        vec3 reflectlightdir = normalize ( reflect ( fraglightdir, normal ) );
        /* get specular multiplier */
        float spec = pow ( max ( dot ( fragviewdir, reflectlightdir ), 0.0 ), material.shininess );
        /* add specular light from point source, including attenuation */
        specular_color += base_color * lights.point [ i ].specular_color * spec * compute_attenuation
        (
            length ( lights.point [ i ].position - fragpos ),
            lights.point [ i ].att_const,
            lights.point [ i ].att_linear,
            lights.point [ i ].att_quad
        );
    }

    /* return specilar component multiplied by shininess_strength */
    return specular_color * material.shininess_strength;
}



/* main */
void main ()
{
    vec3 ambient = compute_ambient_component ( material, lighting );
    vec3 diffuse = compute_diffuse_component ( material, lighting );
    vec3 specular = compute_specular_component ( material, lighting );

    fragcolor = vec4 ( ambient + diffuse + specular, 1.0 );
    //fragcolor = vec4 ( evaluate_stack ( material, material.specular_stack ), 1.0 );
}