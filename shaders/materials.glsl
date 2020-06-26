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
    vec4 base_color;
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
 * stack: the stack to evaluate
 * texcoords: array of texture coords for the fragment
 *
 * return: the overall color of the fragment of the stack
 */
vec4 evaluate_stack ( const texture_stack_struct stack, const vec3 texcoords [ MAX_UV_CHANNELS ] )
{
    /* set stack color color to base color */
    vec4 stack_color = stack.base_color;

    /* loop through the stack */
    for ( int i = 0; i < stack.stack_size; ++i )
    {
        /* get the level color from the texture */
        vec4 level_color = texture ( stack.textures, vec3 ( texcoords [ stack.levels [ i ].uvwsrc ].xy, i ) );
        /* multiply by blend strength */
        level_color *= stack.levels [ i ].blend_strength;
        /* add to the stack */
        switch ( stack.levels [ i ].blend_operation )
        {
            case 0: stack_color = stack_color * level_color; break;
            case 1: stack_color = stack_color + level_color; break;
            case 2: stack_color = stack_color - level_color; break;
            case 3: stack_color = stack_color / level_color; break;
            case 4: stack_color = ( stack_color + level_color ) - ( stack_color * level_color ); break;
            case 5: stack_color = stack_color + ( level_color - 0.5 ); break;
            default: stack_color = stack_color * level_color; break;
        }
    }

    /* return the stack color */
    return stack_color;
}