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

/* generate_evaluate_stack_definition
 *
 * defines the function evaluate_stack_[swizzle]
 *
 * return_type: the type to return
 * swizzle_mask: the swizzle pattern to apply to the stack color e.g. xyzw for 4 components
 *
 * the function that it defines is described bellow:
 *
 *
 *
 * evaluate_stack_[swizzle]
 *
 * stack: the stack to evaluate
 * texcoords: array of texture coords for the fragment
 *
 * return: the final color of the stack
 */
#define generate_evaluate_stack_definition( return_type, swizzle_mask ) \
return_type evaluate_stack_ ## swizzle_mask ( const texture_stack_struct stack, const vec2 texcoords [ MAX_TEXTURE_STACK_SIZE ] ) \
{ \
    /* set the output color to the base color of the stack */ \
    return_type stack_color = stack.base_color.swizzle_mask; \
    \
    /* loop through the stack */ \
    for ( int i = 0; i < stack.stack_size; ++i ) \
    { \
        /* add to the stack through the appropriate operation */ \
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
    /* return the stack color */ \
    return stack_color; \
}



/* define the evaluate_stack functions */
generate_evaluate_stack_definition ( float, x )
generate_evaluate_stack_definition ( vec2, xy )
generate_evaluate_stack_definition ( vec3, xyz )
generate_evaluate_stack_definition ( vec4, xyzw )
generate_evaluate_stack_definition ( float, w )



/* evaluate_normal_macro
 *
 * evaluates the normal from a normal stack
 *
 * stack: the normal stack
 * texcoords: array of texture coords for the fragment
 * tbn_matrix: the TBN matrix to use to transform the normal, if map is present
 * 
 * prototype:
 *
 * void evaluate_normal ( texture_stack_struct stack, vec2 texcoords [ MAX_TEXTURE_STACK_SIZE ], mat3 tbn_matrix )
 */
#define evaluate_normal( stack, texcoords, tbn_matrix ) \
    /* if the size of the stack is greater than zero, sample the stack, and transform the output to its vector form
     * then use the tbn matrix to transform the normal to tangent space
     */ \
    ( stack.stack_size > 0 ? tbn_matrix * normalize ( evaluate_stack_xyz ( stack, texcoords ) * 2.0 - 1.0 ) \
    /* otherwise just return the original normal, extracted from the tbn matrix */ \
    : tbn_matrix [ 2 ] )
