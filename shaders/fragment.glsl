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
    int uvwsrc;
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

/* texture coords, normal vector and position */
in vec2 texcoords [ 8 ];
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
vec3 evaluate_stack ( material_struct mat, texture_stack_struct stack )
{
    /* get base colour */
    vec3 stack_colour = stack.base_colour;

    /* loop through the stack */
    for ( int i = 0; i < stack.stack_size; ++i )
    {
        /* get the colour */
        vec3 level_colour = texture ( stack.levels [ i ].texunit, texcoords [ stack.levels [ i ].uvwsrc ] ).xyz * stack.levels [ i ].blend_strength;
        /* add to the stack */
        if ( stack.levels [ i ].blend_operation == 0 ) stack_colour = stack_colour * level_colour; else
        if ( stack.levels [ i ].blend_operation == 1 ) stack_colour = stack_colour + level_colour; else
        if ( stack.levels [ i ].blend_operation == 2 ) stack_colour = stack_colour - level_colour; else
        if ( stack.levels [ i ].blend_operation == 3 ) stack_colour = stack_colour / level_colour; else
        if ( stack.levels [ i ].blend_operation == 4 ) stack_colour = ( stack_colour + level_colour ) - ( stack_colour * level_colour ); else
        if ( stack.levels [ i ].blend_operation == 5 ) stack_colour = stack_colour + ( level_colour - 0.5 );   
    }

    /* return the stack colour */
    return stack_colour;
}



/* main */
void main ()
{
    fragcolour = vec4 ( evaluate_stack ( material, material.ambient_stack )
                 + evaluate_stack ( material, material.diffuse_stack )
                 + evaluate_stack ( material, material.specular_stack ), 1.0 );
}