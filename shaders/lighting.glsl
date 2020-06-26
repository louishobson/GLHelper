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

#define POS_X_ROTATION ( mat4 (  0,  0, -1,  0,    0,  1,  0,  0,    1,  0,  0,  0,    0,  0,  0,  1 ) )
#define NEG_X_ROTATION ( mat4 (  0,  0,  1,  0,    0,  1,  0,  0,   -1,  0,  0,  0,    0,  0,  0,  1 ) )
#define POS_Y_ROTATION ( mat4 (  1,  0,  0,  0,    0,  0, -1,  0,    0,  1,  0,  0,    0,  0,  0,  1 ) )
#define NEG_Y_ROTATION ( mat4 (  1,  0,  0,  0,    0,  0,  1,  0,    0, -1,  0,  0,    0,  0,  0,  1 ) )
#define POS_Z_ROTATION ( mat4 ( -1,  0,  0,  0,    0,  1,  0,  0,    0,  0, -1,  0,    0,  0,  0,  1 ) )
#define NEG_Z_ROTATION ( mat4 (  1,  0,  0,  0,    0,  1,  0,  0,    0,  0,  1,  0,    0,  0,  0,  1 ) )

#define POS_X_ROTATION_INVERSE NEG_X_ROTATION
#define NEG_X_ROTATION_INVERSE POS_X_ROTATION
#define POS_Y_ROTATION_INVERSE NEG_Y_ROTATION
#define NEG_Y_ROTATION_INVERSE POS_Y_ROTATION
#define POS_Z_ROTATION_INVERSE POS_Z_ROTATION
#define NEG_Z_ROTATION_INVERSE NEG_Z_ROTATION



/* STRUCTURES */

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

    bool enabled;
    bool shadow_mapping_enabled;

    camera_struct shadow_camera;
    
    float shadow_bias;
    float shadow_max_dist;
};

/* structure for storing multiple collections of lights */
struct light_system_struct
{
    int dirlights_size;
    light_struct dirlights [ MAX_NUM_DIRLIGHTS ];

    int pointlights_size;
    light_struct pointlights [ MAX_NUM_POINTLIGHTS ];

    int spotlights_size;
    light_struct spotlights [ MAX_NUM_SPOTLIGHTS ];

    sampler2DArrayShadow shadow_maps;
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

    /* check if any colors supplied are black, or shininess_strength is 0, as steps can be ignored if so */
    const bool has_ambient_component = ( ambient_color != vec3 ( 0.0, 0.0, 0.0 ) );
    const bool has_diffuse_component = ( diffuse_color != vec3 ( 0.0, 0.0, 0.0 ) );
    const bool has_specular_component = ( specular_color != vec3 ( 0.0, 0.0, 0.0 ) && shininess_strength > 0.0 );

    /* calculate unit vector from fragment to viewer */
    const vec3 viewdir = normalize ( viewpos - fragpos ); 

    /* iterate through directional lighting */
    for ( uint i = 0; i < light_system.dirlights_size; ++i )
    {
        /* continue if not enabled */
        if ( !light_system.dirlights [ i ].enabled ) continue;

        /* add ambient component */
        if ( has_ambient_component ) base_color += light_system.dirlights [ i ].ambient_color * ambient_color; 

        /* default shadow constant to 1.0, unless shadow mapping is enabled */
        float shadow_constant = 1.0;
        if ( light_system.dirlights [ i ].shadow_mapping_enabled )
        {
            /* transform the fragment position using the light's shadow matrices */
            vec4 fragpos_light_proj = light_system.dirlights [ i ].shadow_camera.view_proj * vec4 ( fragpos, 1.0 );

            /* map to range 0-1 */
            fragpos_light_proj.xyz = fragpos_light_proj.xyz * 0.5 + 0.5;

            /* calculate z through z-component * the shadow bias, taking into account the angle of the surface
             * the closer the surface is to perpandicular to the light, the less bias */
            const float depth = fragpos_light_proj.z - max ( light_system.dirlights [ i ].shadow_bias * ( 1.0 - dot ( normal, -light_system.dirlights [ i ].direction ) ), 0.001 );

            /* sample the shadow map to get the shadow constant */
            shadow_constant = texture ( light_system.shadow_maps, vec4 ( fragpos_light_proj.xy, i, depth ) );

            /* if shadow_constant is 0.0, continue */
            if ( shadow_constant == 0.0 ) continue;
        }

        /* add diffuse component */
        if ( has_diffuse_component ) base_color += max ( dot ( normal, -light_system.dirlights [ i ].direction ), 0.0 ) * shadow_constant * light_system.dirlights [ i ].diffuse_color * diffuse_color;

        /* add specular component */
        if ( has_specular_component )
        {
            /* calculate halfway vector */
            const vec3 halfway = normalize ( viewdir - light_system.dirlights [ i ].direction );

            /* add to base color depending on how close the halfway vector is to the normal */
            base_color += pow ( max ( dot ( normal, halfway ), 0.0 ), shininess ) * shininess_strength * shadow_constant * light_system.dirlights [ i ].specular_color * specular_color;
        }
    }

    /* iterate through point lighting */
    for ( uint i = 0; i < light_system.pointlights_size; ++i )
    {
        /* continue if not enabled */
        if ( !light_system.pointlights [ i ].enabled ) continue;

        /* get vector from fragment to light */
        vec3 lightdir = light_system.pointlights [ i ].position - fragpos;

        /* get the distance from the fragment to the light */
        const float lightdist = length ( lightdir );

        /* use that distance to calculate the attenuation */
        const float attenuation = compute_attenuation ( lightdist, light_system.pointlights [ i ].att_const, light_system.pointlights [ i ].att_linear, light_system.pointlights [ i ].att_quad );

        /* normalize lightdir for use with dot product */
        lightdir = normalize ( lightdir );

        /* add ambient component */
        if ( has_ambient_component ) base_color += attenuation * light_system.pointlights [ i ].ambient_color * ambient_color; 

        /* default shadow constant to 1.0, unless shadow mapping is enabled */
        float shadow_constant = 1.0;
        if ( light_system.pointlights [ i ].shadow_mapping_enabled )
        {
            /* calculate shadow constant by sampling texture */
            vec4 fragpos_light_view = light_system.pointlights [ i ].shadow_camera.view * vec4 ( fragpos, 1.0 );
            
            /* get signs of fragpos */
            const bvec3 fragpos_light_view_signs = bvec3 ( fragpos_light_view.x >= 0.0, fragpos_light_view.y >= 0.0, fragpos_light_view.z >= 0.0 );

            /* get abs of fragpos */
            const vec3 fragpos_light_view_abs = abs ( fragpos_light_view.xyz );

            /* transform fragpos to be on the -z side of the light */
            uint face_index;
            if ( fragpos_light_view_signs.x && fragpos_light_view_abs.x >= fragpos_light_view_abs.y && fragpos_light_view_abs.x >= fragpos_light_view_abs.z ) 
                { fragpos_light_view = POS_X_ROTATION * fragpos_light_view; face_index = 0; } else
            if ( !fragpos_light_view_signs.x && fragpos_light_view_abs.x >= fragpos_light_view_abs.y && fragpos_light_view_abs.x >= fragpos_light_view_abs.z ) 
                { fragpos_light_view = NEG_X_ROTATION * fragpos_light_view; face_index = 1; } else
            if ( fragpos_light_view_signs.y && fragpos_light_view_abs.y >= fragpos_light_view_abs.x && fragpos_light_view_abs.y >= fragpos_light_view_abs.z ) 
                { fragpos_light_view = POS_Y_ROTATION * fragpos_light_view; face_index = 2; } else
            if ( !fragpos_light_view_signs.y && fragpos_light_view_abs.y >= fragpos_light_view_abs.x && fragpos_light_view_abs.y >= fragpos_light_view_abs.z ) 
                { fragpos_light_view = NEG_Y_ROTATION * fragpos_light_view; face_index = 3; } else
            if ( fragpos_light_view_signs.z && fragpos_light_view_abs.z >= fragpos_light_view_abs.x && fragpos_light_view_abs.z >= fragpos_light_view_abs.y ) 
                { fragpos_light_view = POS_Z_ROTATION * fragpos_light_view; face_index = 4; }
            else
                { fragpos_light_view = NEG_Z_ROTATION * fragpos_light_view; face_index = 5; }

            /* project the transformed fragpos using the light's projection matrix */
            vec4 fragpos_light_proj = light_system.pointlights [ i ].shadow_camera.proj * fragpos_light_view;

            /* map x and y to range 0-1 */
            fragpos_light_proj.xy = ( fragpos_light_proj.xy / fragpos_light_proj.w ) * 0.5 + 0.5;

            /* create the depth linearly using the distance from the fragment to the light
             * by dividing by shadow_max_dist, the depth will be in range 0-1
             * also take into account the angle of the surface to the light's position
             */
            const float depth = lightdist / light_system.pointlights [ i ].shadow_max_dist -
                max ( light_system.pointlights [ i ].shadow_bias * ( 1.0 - dot ( normal, -light_system.pointlights [ i ].direction ) ), 0.001 );

            /* sample the shadow map to get the shadow constant */
            shadow_constant = texture ( light_system.shadow_maps,  vec4 ( fragpos_light_proj.xy, light_system.dirlights_size + i * 6 + face_index, depth ) );

            /* if shadow_constant is 0.0, continue */
            if ( shadow_constant == 0.0 ) continue;
        }

        /* add diffuse component */
        if ( has_diffuse_component ) base_color += max ( dot ( normal, lightdir ), 0.0 ) * shadow_constant * attenuation * light_system.pointlights [ i ].diffuse_color * diffuse_color;

        /* add specular component */
        if ( has_specular_component )
        {
            /* calculate halfway vector */
            const vec3 halfway = normalize ( viewdir + lightdir );

            /* add to base color depending on how close the halfway vector is to the normal */
            base_color += pow ( max ( dot ( normal, halfway ), 0.0 ), shininess ) * shininess_strength * shadow_constant * attenuation * light_system.pointlights [ i ].specular_color * specular_color;
        }
    }

    /* return the base color */
    return base_color;
}