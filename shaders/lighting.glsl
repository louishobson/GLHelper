/*
 * lighting.glsl
 *
 * defines structures for lighting
 *
 */



/* DEFINITIONS */

#define MAX_NUM_DIRLIGHTS 1
#define MAX_NUM_POINTLIGHTS 1
#define MAX_NUM_SPOTLIGHTS 1



/* STRUCTURES */

/* structures for types of light */
struct dirlight_struct
{
    vec3 direction;

    vec3 ambient_color;
    vec3 diffuse_color;
    vec3 specular_color;

    bool enabled;
    bool shadow_mapping_enabled;

    mat4 shadow_trans;
    
    float shadow_bias;
    float shadow_depth_range_mult;
};
struct pointlight_struct
{
    vec3 position;

    float att_const;
    float att_linear;
    float att_quad;

    vec3 ambient_color;
    vec3 diffuse_color;
    vec3 specular_color;

    bool enabled;
    bool shadow_mapping_enabled;

    mat4 shadow_trans [ 6 ];
    
    float shadow_bias;
    float shadow_depth_range_mult;
};
struct spotlight_struct
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
    bool shadow_mapping_enabled;

    mat4 shadow_trans;
    
    float shadow_bias;
    float shadow_depth_range_mult;
};

/* structure for storing multiple collections of lights */
struct light_system_struct
{
    int dirlights_size;
    dirlight_struct dirlights [ MAX_NUM_DIRLIGHTS ];

    int pointlights_size;
    pointlight_struct pointlights [ MAX_NUM_POINTLIGHTS ];

    int spotlights_size;
    spotlight_struct spotlights [ MAX_NUM_SPOTLIGHTS ];

    sampler2DArrayShadow shadow_maps_2d;
    samplerCubeArrayShadow shadow_maps_cube;
};



/* FUNCTIONS */

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
float compute_attenuation ( const float dist, const float att_const, const float att_linear, const float att_quad )
{
    return 1.0 / ( att_const + ( att_linear * dist ) + ( att_quad * dist * dist ) );
}

/* compute_lighting
 *
 * compute the output color of a fragment from its colors, position, viewer position and light system
 *
 * ambient/diffuse/specular_color: the color components of the fragment
 * shininess/shininess_strength: the shininess and shininess_strength of the material
 * fragpos: position of the fragment
 * viewpos: position of the viewer
 * normal: normal vector to the fragment
 * light_system: the light system to use
 *
 * return: the final color of the fragment
 */
vec3 compute_lighting ( const vec3 ambient_color, const vec3 diffuse_color, const vec3 specular_color, 
                        const float shininess, const float shininess_strength, 
                        const vec3 fragpos, const vec3 viewpos, const vec3 normal, 
                        const light_system_struct light_system )
{
    /* start with a black base color */
    vec3 base_color = vec3 ( 0.0, 0.0, 0.0 );

    /* calculate unit vector from fragment to viewer */
    const vec3 viewdir = normalize ( viewpos - fragpos ); 

    /* iterate through directional lighting */
    for ( uint i = 0; i < light_system.dirlights_size; ++i )
    {
        /* continue if disabled */
        if ( !light_system.dirlights [ i ].enabled ) continue;

        /* calculate diffuse constant */
        const float diffuse_constant = max ( dot ( normal, -light_system.dirlights [ i ].direction ), 0.0 );

        /* calculate specular constant from halfway vector */
        //const float specular_constant = max ( dot ( normal, normalize ( viewdir - light_system.dirlights [ i ].direction ) ), 0.0 );

        /* calculate shadow_constant */
        float shadow_constant = 1.0;
        if ( light_system.dirlights [ i ].shadow_mapping_enabled )
        {
            /* transform the fragment position using the light's shadow matrices */
            vec4 fragpos_light_proj = light_system.dirlights [ i ].shadow_trans * vec4 ( fragpos, 1.0 );

            /* map to range 0-1 */
            fragpos_light_proj.xyz = fragpos_light_proj.xyz * 0.5 + 0.5;

            /* calculate z through z-component * the shadow bias, taking into account the angle of the surface
             * the closer the surface is to perpandicular to the light, the less bias */
            fragpos_light_proj.z -= max ( light_system.dirlights [ i ].shadow_bias * ( 1.0 - diffuse_constant ), 0.001 );

            /* sample the shadow map to get the shadow constant */
            shadow_constant = texture ( light_system.shadow_maps_2d, vec4 ( fragpos_light_proj.xy, i, fragpos_light_proj.z ) );
        }

        /* add ambient component */
        base_color += light_system.dirlights [ i ].ambient_color * ambient_color; 

        /* add diffuse component */
        base_color += diffuse_constant * shadow_constant * light_system.dirlights [ i ].diffuse_color * diffuse_color;

        /* add specular component */
        //base_color += pow ( specular_constant, shininess ) * shininess_strength * shadow_constant * light_system.dirlights [ i ].specular_color * specular_color;
        //base_color += pow ( max ( dot ( normal, normalize ( viewdir - light_system.dirlights [ i ].direction ) ), 0.0 ), shininess ) * shininess_strength * shadow_constant * light_system.dirlights [ i ].specular_color * specular_color;
        base_color += pow ( max ( dot ( normal, normalize ( viewdir - light_system.dirlights [ i ].direction ) ), 0.0 ), 256.0 ) * 0.75 * light_system.dirlights [ i ].specular_color;
    }

    /* iterate through point lighting */
    for ( uint i = 0; i < light_system.pointlights_size; ++i )
    {
        /* continue if disabled */
        if ( !light_system.pointlights [ i ].enabled ) continue;

        /* get vector from fragment to light */
        vec3 lightdir = light_system.pointlights [ i ].position - fragpos;

        /* get the distance from the fragment to the light */
        const float lightdist = length ( lightdir );

        /* normalize lightdir for use with dot product */
        lightdir = normalize ( lightdir );

        /* use distance to light to calculate the attenuation */
        const float attenuation = compute_attenuation ( lightdist, light_system.pointlights [ i ].att_const, light_system.pointlights [ i ].att_linear, light_system.pointlights [ i ].att_quad );

        /* calculate diffuse constant */
        const float diffuse_constant = max ( dot ( normal, lightdir ), 0.0 );

        /* calculate specular constant from halfway vector */
        //const float specular_constant = max ( dot ( normal, normalize ( viewdir + lightdir ) ), 0.0 );        
        
        /* calculate shadow_constant */
        float shadow_constant = 1.0;
        if ( light_system.pointlights [ i ].shadow_mapping_enabled )
        {
            /* sample the shadow map to get the shadow constant */
            shadow_constant = texture 
            ( 
                light_system.shadow_maps_cube, vec4 ( -lightdir, i ), 
                lightdist * light_system.pointlights [ i ].shadow_depth_range_mult -
                    max ( light_system.pointlights [ i ].shadow_bias * ( 1.0 - diffuse_constant ), 0.001 )
            );
        }

        /* add ambient component */
        base_color += attenuation * light_system.pointlights [ i ].ambient_color * ambient_color; 

        /* add diffuse component */
        base_color += diffuse_constant * shadow_constant * attenuation * light_system.pointlights [ i ].diffuse_color * diffuse_color;

        /* add specular component */
        //base_color += pow ( specular_constant, shininess ) * shininess_strength * shadow_constant * attenuation * light_system.pointlights [ i ].specular_color * specular_color;
        base_color += pow ( max ( dot ( normal, normalize ( viewdir + lightdir ) ), 0.0 ), shininess ) * shininess_strength * shadow_constant * attenuation * light_system.pointlights [ i ].specular_color * specular_color;
    }

    /* iterate through spot lighting */
    for ( uint i = 0; i < light_system.spotlights_size; ++i )
    {
        /* continue if disabled */
        if ( !light_system.spotlights [ i ].enabled ) continue;

        /* get vector from fragment to light */
        vec3 lightdir = light_system.spotlights [ i ].position - fragpos;

        /* get the distance from the fragment to the light */
        const float lightdist = length ( lightdir );

        /* normalize lightdir for use with dot product */
        lightdir = normalize ( lightdir );

        /* get arccos of the dot product of vector to light and direction of the light
         * this equals the cosine of the angle between the light and the fragment
         */
        //const float lighttheta = acos ( dot ( light_system.spotlights [ i ].direction, -lightdir ) );

        /* set spotlight_constant to 1.0, unless inbetween inner and outer cone */
        //const float spotlight_constant = 1.0 - smoothstep ( light_system.spotlights [ i ].inner_cone, light_system.spotlights [ i ].outer_cone, lighttheta );
        const float spotlight_constant = 1.0 - smoothstep ( light_system.spotlights [ i ].inner_cone, light_system.spotlights [ i ].outer_cone, acos ( dot ( light_system.spotlights [ i ].direction, -lightdir ) ) );

        /* use distance to light to calculate the attenuation */
        const float attenuation = compute_attenuation ( lightdist, light_system.spotlights [ i ].att_const, light_system.spotlights [ i ].att_linear, light_system.spotlights [ i ].att_quad );

        /* calculate diffuse constant */
        const float diffuse_constant = max ( dot ( normal, lightdir ), 0.0 );

        /* calculate specular constant from halfway vector */
        //const float specular_constant = max ( dot ( normal, normalize ( viewdir + lightdir ) ), 0.0 );

        /* calculate shadow_constant */
        float shadow_constant = 1.0;
        if ( light_system.spotlights [ i ].shadow_mapping_enabled )
        {
            /* transform the fragment position using the light's shadow matrices */
            vec4 fragpos_light_proj = light_system.spotlights [ i ].shadow_trans * vec4 ( fragpos, 1.0 );

            /* divide by w component */
            fragpos_light_proj.xy /= fragpos_light_proj.w;

            /* map to range 0-1 */
            fragpos_light_proj.xy = fragpos_light_proj.xy * 0.5 + 0.5;

            /* create the depth linearly using the distance from the fragment to the light
             * by multiplying by shadow_depth_range_mult, the depth will be in range 0-1
             * also take into account the angle of the surface to the light's position
             */
            fragpos_light_proj.z = lightdist * light_system.spotlights [ i ].shadow_depth_range_mult -
                max ( light_system.spotlights [ i ].shadow_bias * ( 1.0 - diffuse_constant ), 0.001 );

            /* sample the shadow map to get the shadow constant */
            shadow_constant = texture ( light_system.shadow_maps_2d, vec4 ( fragpos_light_proj.xy, light_system.dirlights_size + i, fragpos_light_proj.z ) );
        }

        /* add ambient component */
        base_color += light_system.spotlights [ i ].ambient_color * ambient_color; 

        /* add diffuse component */
        base_color += diffuse_constant * spotlight_constant * shadow_constant * attenuation * light_system.spotlights [ i ].diffuse_color * diffuse_color;

        /* add specular component */
        //base_color += pow ( specular_constant, shininess ) * shininess_strength * spotlight_constant * shadow_constant * attenuation * light_system.spotlights [ i ].specular_color * specular_color;
        base_color += pow ( max ( dot ( normal, normalize ( viewdir + lightdir ) ), 0.0 ), shininess ) * shininess_strength * spotlight_constant * shadow_constant * attenuation * light_system.spotlights [ i ].specular_color * specular_color;
    }

    /* return the base color */
    return base_color;
}