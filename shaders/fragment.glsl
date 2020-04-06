/*
 * fragment.glsl
 * 
 * fragment shader
 */

#version 330 core

#define MAX_STACK_SIZE 8

/* structure for a texture */
struct texture_stack_level_struct
{
    int blend_operation;
    float blend_strength;
    sampler2D texunit;
};

/* structure for a texture stack */
struct texture_stack_struct
{
    int stack_size;
    vec3 base_colour;
    texture_stack_level_struct levels [ MAX_STACK_SIZE ];
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

/* texture coord, normal vector and position */
in vec2 texcoord;
in vec3 norm;
in vec3 fragpos;

/* output colour */
out vec4 fragcolour;

/* viewpos, material and lighting uniforms */
uniform vec3 viewpos;
uniform material_struct material;



/* evaluate_stack
 *
 * evaluate multiple stacked textures
 *
 * material: the material the stack if part of
 * stack: the stack to evaluate
 *
 * return: the overall colour of the fragment of the stack
 */
vec4 evaluate_stack ( material_struct mat, texture_stack_struct stack )
{
    /* get base colour */
    vec4 stack_colour = vec4 ( stack.base_colour, 1.0 );

    /* loop through the stack */
    for ( int i = 0; i < stack.stack_size; ++i )
    {
        /* get the colour */
        stack_colour += texture ( stack.levels [ i ].texunit, texcoord ) * stack.levels [ i ].blend_strength;
        /* add to the stack */
    }

    /* return the stack colour */
    return stack_colour;
}



/* main */
void main ()
{
    fragcolour = vec4 ( vec3 ( evaluate_stack ( material, material.ambient_stack )
                 + evaluate_stack ( material, material.diffuse_stack )
                 + evaluate_stack ( material, material.specular_stack ) ).xyz / 4, 1.0 );
}