/*
 * materials.glsl
 *
 * defines structures for the model material system
 */



/* DEFINITIONS */

/* maximum number of textures in texture stack */
#define MAX_TEXTURE_STACK_SIZE 2



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
    texture_stack_level_struct levels [ MAX_TEXTURE_STACK_SIZE ];
    sampler2DArray textures;
};

/* structure for a material */
struct material_struct
{
    texture_stack_struct ambient_stack;
    texture_stack_struct diffuse_stack;
    texture_stack_struct specular_stack;
    texture_stack_struct emission_stack;
    texture_stack_struct normal_stack;

    int blending_mode;

    float shininess;
    float shininess_strength;

    float opacity;
    bool definitely_opaque;
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
vec4 evaluate_stack ( const texture_stack_struct stack, const vec2 texcoords [ MAX_TEXTURE_STACK_SIZE ] )
{
    /* set stack color to base color */
    vec4 stack_color = stack.base_color;

    /* loop through the stack */
    for ( int i = 0; i < stack.stack_size; ++i )
    {
        /* get the level color from the texture multiplied by the stength */
        const vec4 level_color = texture ( stack.textures, vec3 ( texcoords [ stack.levels [ i ].uvwsrc ], i ) ) * stack.levels [ i ].blend_strength;

        /* add to the stack through the appropriate operation */
        switch ( stack.levels [ i ].blend_operation )
        {
            case 0: stack_color *= level_color; break;
            case 1: stack_color += level_color; break;
            case 2: stack_color -= level_color; break;
            case 3: stack_color /= level_color; break;
            case 4: stack_color = ( stack_color + level_color ) - ( stack_color * level_color ); break;
            case 5: stack_color = stack_color + ( level_color - 0.5 ); break;
            default: stack_color *= level_color; break;
        }
    }

    /* return the stack color */
    return stack_color;
}

/* evaluate_stack_transparency
 *
 * useful for when shadow mapping
 * is much more stripped down than evaluate_stack
 *
 * stack: the stack to evaluate
 * texcoords: array of texture coords for the fragment
 * transparency_cutoff: if the transparency is less than this value, return true
 *
 * return: boolean, true if has alpha < transparency_cutoff
 *
 * prototype: bool evaluate_stack_transparency ( texture_stack_struct stack, vec2 texcoords [ MAX_TEXTURE_STACK_SIZE ], float transparency_cutoff )
 */
#define evaluate_stack_transparency( stack, texcoords, transparency_cutoff ) \
    ( stack.base_color.a < transparency_cutoff || ( stack.stack_size != 0 && texture ( stack.textures, vec3 ( texcoords [ stack.levels [ 0 ].uvwsrc ], 0 ) ).a < transparency_cutoff ) )
    /* if base transparency < 1.0, return true
     * if has no textures, return false
     * if has texture, return true if alpha is less than 1.0
     */

/* evaluate_normal
 *
 * calculate the new normal from a normal map, doing nothing if one is not present
 *
 * stack: the normal stack
 * texcoords: array of texture coords for the fragment
 * tbn_matrix: the TBN matrix to use to transform the normal, if map is present
 *
 * return: the new normal, or the original if no map is present
 *
 * prototype: vec3 evaluate_normal ( texture_stack_struct stack, vec2 texcoords [ MAX_TEXTURE_STACK_SIZE ], mat3 tbn_matrix )
 */
#define evaluate_normal( stack, texcoords, tbn_matrix ) \
    ( stack.stack_size == 0 ? tbn_matrix [ 2 ] : tbn_matrix * normalize ( ( evaluate_stack ( stack, texcoords ).xyz * 2.0 - 1.0 ) ) )
    /* if no normal, return original normal
     * else sample the map and return the transformed normal
     */
