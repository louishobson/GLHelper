/*
 * materials.glsl
 *
 * defines structures for the model material system
 */



/* DEFINITIONS */

/* maximum number of textures in texture stack */
#define MAX_TEX_STACK_SIZE 2

/* maximum number of UV channels */
#define MAX_UV_CHANNELS 2



/* STRUCTURES */

/* structure for a texture */
struct texture_stack_level_struct
{
    int blend_operation;
    float blend_strength;
    int uvwsrc;
};

/* structure for a texture stack */
struct texture_stack_struct
{
    vec3 base_color;
    int stack_size;
    texture_stack_level_struct levels [ MAX_TEX_STACK_SIZE ];
    sampler2DArray textures;
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



/* FUNCTIONS */

/* evaluate_stack
 *
 * evaluate multiple stacked textures
 *
 * material: the material the stack if part of
 * stack: the stack to evaluate
 * texcoords: array of texture coords for the fragment
 *
 * return: the overall color of the fragment of the stack
 */
vec4 evaluate_stack ( const material_struct material, const texture_stack_struct stack, const vec3 texcoords [ MAX_UV_CHANNELS ] )
{
    /* get base color */
    vec4 stack_color = vec4 ( stack.base_color, material.opacity );

    /* loop through the stack */
    for ( int i = 0; i < stack.stack_size; ++i )
    {
        /* get the level color from the texture */
        vec4 level_color = texture ( stack.textures, vec3 ( texcoords [ stack.levels [ i ].uvwsrc ].xy, i ) );
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