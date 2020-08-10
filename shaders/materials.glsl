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

/* evaluate_stack_macro_swizzle
 *
 * evaluate multiple stacked textures with a specific swizzle mask
 *
 * stack_color: the name of a modifiable vector lvalue which will be used to store the result
 * stack: the stack to evaluate
 * texcoords: array of texture coords for the fragment
 * swizzle: the swizzle pattern to apply to the stack color e.g. xyzw for 4 components
 *
 * the number of components of stack_color should be the same as the components returned by the swizzle mask supplied
 *
 * prototype:
 *
 * void evaluate_stack_macro ( out vec[1-4] stack_color, texture_stack_struct stack, vec2 texcoords [ MAX_TEXTURE_STACK_SIZE ], MACRO swizzle_mask )
 */
 #define evaluate_stack_macro_swizzle( stack_color, stack, texcoords, swizzle_mask ) \
{ \
    /* set the output color to the base color of the stack */ \
    stack_color = stack.base_color.swizzle_mask; \
    \
    /* loop through the stack */ \
    for ( int i = 0; i < stack.stack_size; ++i ) \
    { \
        /* add to the stack through the appropriate operation 
         * the expression 'texture ( stack.textures, vec3 ( texcoords [ stack.levels [ i ].uvwsrc ], i ) ) * stack.levels [ i ].blend_strength'
         * is equal to the sampled value from the current texture
         * for case 4, this is stored into a temporary variable, since the equation requires its use more than once
         */ \
        switch ( stack.levels [ i ].blend_operation ) \
        { \
            case 0: stack_color *= texture ( stack.textures, vec3 ( texcoords [ stack.levels [ i ].uvwsrc ], i ) ).swizzle_mask * stack.levels [ i ].blend_strength; break; \
            case 1: stack_color += texture ( stack.textures, vec3 ( texcoords [ stack.levels [ i ].uvwsrc ], i ) ).swizzle_mask * stack.levels [ i ].blend_strength; break; \
            case 2: stack_color -= texture ( stack.textures, vec3 ( texcoords [ stack.levels [ i ].uvwsrc ], i ) ).swizzle_mask * stack.levels [ i ].blend_strength; break; \
            case 3: stack_color /= texture ( stack.textures, vec3 ( texcoords [ stack.levels [ i ].uvwsrc ], i ) ).swizzle_mask * stack.levels [ i ].blend_strength; break; \
            case 4: \
                stack_color = ( stack_color + texture ( stack.textures, vec3 ( texcoords [ stack.levels [ i ].uvwsrc ], i ) ).swizzle_mask ) \
                            - ( stack_color * texture ( stack.textures, vec3 ( texcoords [ stack.levels [ i ].uvwsrc ], i ) ).swizzle_mask ); \
                break; \
            case 5:  stack_color += texture ( stack.textures, vec3 ( texcoords [ stack.levels [ i ].uvwsrc ], i ) ).swizzle_mask * stack.levels [ i ].blend_strength - 0.5; break; \
            default: stack_color *= texture ( stack.textures, vec3 ( texcoords [ stack.levels [ i ].uvwsrc ], i ) ).swizzle_mask * stack.levels [ i ].blend_strength; break; \
        } \
    } \
}

/* evaluate_stack_macro_[x/xy/xyz/xyzw/w]
 *
 * evaluate multiple stacked texture with the number of components supplied
 *
 * stack_color: the name of an modifiable vec4 lvalue which will be used to store the result
 * stack: the stack to evaluate
 * texcoords: array of texture coords for the fragment
 *
 * the number of components of stack_color should be the same as the components returned by the swizzle mask in the macro name
 *
 * prototype:
 *
 * void evaluate_stack_macro_[x/xy/xyz/xyzw/w] ( out vec[1-4] stack_color, texture_stack_struct stack, vec2 texcoords [ MAX_TEXTURE_STACK_SIZE ] )
 */
#define evaluate_stack_macro_x( stack_color, stack, texcoords )    evaluate_stack_macro_swizzle ( stack_color, stack, texcoords, x )
#define evaluate_stack_macro_xy( stack_color, stack, texcoords )   evaluate_stack_macro_swizzle ( stack_color, stack, texcoords, xy )
#define evaluate_stack_macro_xyz( stack_color, stack, texcoords )  evaluate_stack_macro_swizzle ( stack_color, stack, texcoords, xyz )
#define evaluate_stack_macro_xyzw( stack_color, stack, texcoords ) evaluate_stack_macro_swizzle ( stack_color, stack, texcoords, xyzw )
#define evaluate_stack_macro_w( stack_color, stack, texcoords )    evaluate_stack_macro_swizzle ( stack_color, stack, texcoords, w )
#define evaluate_stack_macro( stack_color, stack, texcoords )      evaluate_stack_macro_swizzle ( stack_color, stack, texcoords, xyzw )



/* evaluate_normal_macro
 *
 * macro version of evaluate_normal
 *
 * normal: a modifiable lvalue for the output normal
 * stack: the normal stack
 * texcoords: array of texture coords for the fragment
 * tbn_matrix: the TBN matrix to use to transform the normal, if map is present
 * 
 * prototype:
 *
 * void evaluate_normal_macro ( out vec3 normal, texture_stack_struct stack, vec2 texcoords [ MAX_TEXTURE_STACK_SIZE ], mat3 tbn_matrix )
 */
#define evaluate_normal_macro( normal, stack, texcoords, tbn_matrix ) \
{ \
    /* if the size of the stack is greater than zero, sample the stack, and transform the output to its vector form
     * then use the tbn matrix to transform the normal to tangent space
     */ \
    if ( stack.stack_size > 0 ) { evaluate_stack_macro_xyz ( normal, stack, texcoords ); normal = tbn_matrix * normalize ( normal * 2.0 - 1.0 ); } \
    /* otherwise just return the original normal, extracted from the tbn matrix */ \
    else normal = tbn_matrix [ 2 ]; \
}

