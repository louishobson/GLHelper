/*
 * camera.glsl
 *
 * defines structures for mathematical functions
 *
 */



/* 1D function */
struct function_struct_1D
{
    sampler1D outputs;
    
    float domain_lower;
    float domain_upper;

    float domain_multiplier;
    float domain_translation;
};

/* 2D function */
struct function_struct_2D
{
    sampler2D outputs;
    
    float domain_lower;
    float domain_upper;

    float domain_multiplier;
    float domain_translation;
};

/* 3D function */
struct function_struct_3D
{
    sampler3D outputs;
    
    float domain_lower;
    float domain_upper;

    float domain_multiplier;
    float domain_translation;
};



/* sample_function
 *
 * samples a mathematical function
 *
 * func: the function to sample
 * params: the parameters to the function
 * 
 * return: the output of the function for the parameters supplied
 *
 * prototypes:
 *
 * vec4 sample_function ( const function_struct_1D, const float params );
 * vec4 sample_function ( const function_struct_2D, const vec2 params );
 * vec4 sample_function ( const function_struct_3D, const vec3 params );
 *
 * the texture function always produces a vec4, so remember to swizzle it if necessary
 */
#define sample_function( func, params ) \
    ( texture ( func.outputs, params * func.domain_multiplier + func.domain_translation ) )