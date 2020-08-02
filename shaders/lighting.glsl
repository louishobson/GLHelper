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

    int pcf_samples;
    float pcf_radius;
    mat2 pcf_rotation;
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
    
    float shadow_bias;
    float shadow_depth_range_mult;

    int pcf_samples;
    float pcf_radius;
    mat2 pcf_rotation;
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

    int pcf_samples;
    float pcf_radius;
    mat2 pcf_rotation;
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
 *
 * prototype: float compute_attenuation ( float dist, float att_const, float att_linear, float att_quad )
 */
#define compute_attenuation( dist, att_const, att_linear, att_quad ) \
    ( 1.0 / ( att_const + att_linear * dist + att_quad * dist * dist ) )

/* compute_diffuse_constant
 *
 * compute the diffuse component of lighting a fragment
 *
 * lightdir: unit vector from fragment to light position
 * normal: normal of the fragment
 *
 * return: the multiple of diffuse lighting
 *
 * prototype: float compute_diffuse_constant ( vec3 lightdir, vec3 normal )
 */
#define compute_diffuse_constant( lightdir, normal ) \
    ( clamp ( dot ( lightdir, normal ), 0.0, 1.0 ) )

/* compute_specular_constant
 *
 * compute the specular component of lighting a fragment
 *
 * viewdir: unit vector from fragment to viewer position
 * lightdir: unit vector from fragment to light position
 * normal: normal of the fragment
 * shininess: shininess of the material
 *
 * return: the multiple of specular lighting
 *
 * prototype: float compute_specular_constant ( vec3 viewdir, vec3 lightdir, vec3 normal, float shininess )
 */
#define compute_specular_constant( viewdir, lightdir, normal, shininess ) \
    ( pow ( clamp ( dot ( normalize ( viewdir + lightdir ), normal ), 0.0, 1.0 ), shininess ) )

/* calculate_spotlight_constant
 *
 * compute the spotlight constant for lighting a fragment
 *
 * direction: unit vector for the central direction of the spotlight
 * lightdir: unit vector from fragment to light position
 * inner/outer_cone: inner and outer cone in radians for the spotlight
 * 
 * return: the multiple for spotlight lighting
 *
 * prototype: float compute_spotlight_constant ( vec3 directiom, vec3 lightdir, float inner_cone, float outer_cone )
 */
#define compute_spotlight_constant( direction, lightdir, inner_cone, outer_cone ) \
    ( 1.0 - smoothstep ( inner_cone, outer_cone, acos ( dot ( direction, -lightdir ) ) ) )
    /* set spotlight_constant to 1.0, unless inbetween inner and outer cone
     * in this case, the arccos of the dot product of lightdir and -direction of the light gives the angle
     * we can then use Hermite interpolation to create a smooth gradient between the inner and outer come
     */



/* compute_lighting_macro
 *
 * compute the output color of a fragment from its colors, position, viewer position and light system
 *
 * final_color: modifiable vec3 lvalue for the output color
 * ambient/diffuse/specular_light: the color components of the fragment
 * shininess/shininess_strength: the shininess and shininess_strength of the material
 * fragpos: position of the fragment
 * viewpos: position of the viewer
 * normal: normal vector to the fragment
 * light_system: the light system to use
 *
 * prototype:
 *
 * vec3 compute_lighting ( out vec3 final_color, const vec3 ambient_light, const vec3 diffuse_light, const vec3 specular_light, 
 *                         const float shininess, const float shininess_strength, 
 *                         const vec3 fragpos, const vec3 viewpos, const vec3 normal, 
 *                         const light_system_struct light_system )
 */
#define compute_lighting_macro( final_color, ambient_light, diffuse_light, specular_light, shininess, shininess_strength, fragpos, viewpos, normal, light_system ) \
{ \
    /* start with a black base color */ \
    final_color = vec3 ( 0.0, 0.0, 0.0 ); \
    \
    /* calculate unit vector from fragment to viewer */ \
    const vec3 viewdir = normalize ( viewpos - fragpos ); \
    \
    /* iterate through directional lighting */ \
    for ( int i = 0; i < light_system.dirlights_size; ++i ) \
    { \
        /* continue if disabled */ \
        if ( !light_system.dirlights [ i ].enabled ) continue; \
        \
        /* calculate shadow_constant */ \
        float shadow_constant = 1.0; \
        if ( light_system.dirlights [ i ].shadow_mapping_enabled ) \
        { \
            /* transform the fragment position using the light's shadow matrices 
             * then transform into the range [0-1]
             */ \
            vec3 fragpos_light_proj = ( light_system.dirlights [ i ].shadow_trans * vec4 ( fragpos, 1.0 ) ).xyz * 0.5 + 0.5; \
            \
            /* calculate z through z-component * the shadow bias, taking into account the angle of the surface
             * the closer the surface is to perpandicular to the light, the less bias */ \
            fragpos_light_proj.z -= max ( light_system.dirlights [ i ].shadow_bias * ( 1.0 - compute_diffuse_constant ( -light_system.dirlights [ i ].direction, normal ) ), 0.001 ); \
            \
            /* switch depending on if pcf samples are enabled */ \
            if ( light_system.dirlights [ i ].pcf_samples == 0 ) \
            { \
                /* sample the shadow map to get the shadow constant */ \
                shadow_constant = texture ( light_system.shadow_maps, vec4 ( fragpos_light_proj.xy, i, fragpos_light_proj.z ) ); \
            } else \
            { \
                /* set the initial sample offset */ \
                vec2 pcf_sample_offset = vec2 ( light_system.dirlights [ i ].pcf_radius, 0.0 ); \
                \
                /* loop through pcf samples */ \
                shadow_constant = 0.0; \
                for ( int j = 0; j < light_system.dirlights [ i ].pcf_samples; ++j ) \
                { \
                    /* sample shadow map and rotate offset */ \
                    shadow_constant += texture ( light_system.shadow_maps, vec4 ( fragpos_light_proj.xy + pcf_sample_offset, i, fragpos_light_proj.z ) ); \
                    pcf_sample_offset = light_system.dirlights [ i ].pcf_rotation * pcf_sample_offset; \
                    shadow_constant += texture ( light_system.shadow_maps, vec4 ( fragpos_light_proj.xy + pcf_sample_offset * 0.5, i, fragpos_light_proj.z ) ); \
                    pcf_sample_offset = light_system.dirlights [ i ].pcf_rotation * pcf_sample_offset; \
                } \
                \
                /* divide shadow_constant by the number of pcf samples taken */ \
                shadow_constant /= light_system.dirlights [ i ].pcf_samples; \
            } \
        } \
        \
        /* add ambient component */ \
        final_color += light_system.dirlights [ i ].ambient_color * ambient_light; \
        \
        /* add diffuse component */ \
        final_color += compute_diffuse_constant ( -light_system.dirlights [ i ].direction, normal ) * shadow_constant * light_system.dirlights [ i ].diffuse_color * diffuse_light; \
        \
        /* add specular component */ \
        final_color += compute_specular_constant ( viewdir, -light_system.dirlights [ i ].direction, normal, shininess ) * shininess_strength * shadow_constant \
                     * light_system.dirlights [ i ].specular_color * specular_light; \
    } \
    \
    /* iterate through point lighting */ \
    for ( int i = 0; i < light_system.pointlights_size; ++i ) \
    { \
        /* continue if disabled */ \
        if ( !light_system.pointlights [ i ].enabled ) continue; \
        \
        /* get vector from fragment to light */ \
        const vec3 lightdir = normalize ( light_system.pointlights [ i ].position - fragpos ); \
        \
        /* get the distance from the fragment to the light */ \
        const float lightdist = length ( light_system.pointlights [ i ].position - fragpos ); \
        \
        /* use distance to light to calculate the attenuation */ \
        const float attenuation = compute_attenuation ( lightdist, light_system.pointlights [ i ].att_const, light_system.pointlights [ i ].att_linear, light_system.pointlights [ i ].att_quad ); \
         \
        /* calculate shadow_constant */ \
        float shadow_constant = 1.0; \
        if ( light_system.pointlights [ i ].shadow_mapping_enabled ) \
        { \
            /* transform the fragment position for dual-parabolic sampling 
             * lightdir.xy is negated, since lightdir is from the fragment to the light - opposite to the incident ray
             * the absolute of lightdir.z is used, since this will be correct or whichever side of the dual-paraboloid map is chosen to be used
             * the expression 'int ( -lightdir.z <= 0.0 )' equates to 1 or 0, depending on whether the front or back paraboloid should be sampled from
             */ \
            vec4 fragpos_light_proj = vec4 ( ( -lightdir.xy / ( 1.0 + abs ( lightdir.z ) ) ) * 0.5 + 0.5, light_system.dirlights_size + i * 2 + int ( -lightdir.z <= 0.0 ), \
                lightdist * light_system.pointlights [ i ].shadow_depth_range_mult - \
                max ( light_system.pointlights [ i ].shadow_bias * ( 1.0 - compute_diffuse_constant ( lightdir, normal ) ), 0.001 ) ); \
            \
            /* switch depending on if pcf samples are enabled */ \
            if ( light_system.pointlights [ i ].pcf_samples == 0 ) \
            { \
                /* sample the shadow map to get the shadow constant */ \
                shadow_constant = texture ( light_system.shadow_maps, fragpos_light_proj ); \
            } else \
            { \
                /* set the initial sample offset */ \
                vec2 pcf_sample_offset = vec2 ( light_system.pointlights [ i ].pcf_radius, 0.0 ); \
                \
                /* loop through pcf samples */ \
                shadow_constant = 0.0; \
                for ( int j = 0; j < light_system.pointlights [ i ].pcf_samples; j += 2 ) \
                { \
                    /* sample the shadow map using the radius and rotate */ \
                    shadow_constant += texture ( light_system.shadow_maps, \
                        vec4 ( fragpos_light_proj.xy + pcf_sample_offset * 1.0, fragpos_light_proj.zw ) ); \
                    pcf_sample_offset = light_system.pointlights [ i ].pcf_rotation * pcf_sample_offset; \
                    \
                    /* sample the shadow map with half the radius and rotate */ \
                    shadow_constant += texture ( light_system.shadow_maps, \
                        vec4 ( fragpos_light_proj.xy + pcf_sample_offset * 0.5, fragpos_light_proj.zw ) ); \
                    pcf_sample_offset = light_system.pointlights [ i ].pcf_rotation * pcf_sample_offset; \
                } \
                \
                /* divide shadow_constant by the number of pcf samples taken */ \
                shadow_constant /= light_system.pointlights [ i ].pcf_samples; \
            } \
        } \
        \
        /* add ambient component */ \
        final_color += attenuation * light_system.pointlights [ i ].ambient_color * ambient_light; \
        \
        /* add diffuse component */ \
        final_color += compute_diffuse_constant ( lightdir, normal ) * shadow_constant * attenuation * light_system.pointlights [ i ].diffuse_color * diffuse_light; \
        \
        /* add specular component */ \
        final_color += compute_specular_constant ( viewdir, lightdir, normal, shininess ) * shininess_strength * shadow_constant * attenuation * light_system.pointlights [ i ].specular_color * specular_light; \
    } \
    \
    /* iterate through spot lighting */ \
    for ( int i = 0; i < light_system.spotlights_size; ++i ) \
    { \
        /* continue if disabled */ \
        if ( !light_system.spotlights [ i ].enabled ) continue; \
        \
        /* get vector from fragment to light */ \
        const vec3 lightdir = normalize ( light_system.spotlights [ i ].position - fragpos ); \
        \
        /* get the distance from the fragment to the light */ \
        const float lightdist = length ( light_system.spotlights [ i ].position - fragpos ); \
        \
        /* compute spotlight constant */ \
        const float spotlight_constant = compute_spotlight_constant ( light_system.spotlights [ i ].direction, lightdir, light_system.spotlights [ i ].inner_cone, light_system.spotlights [ i ].outer_cone ); \
        \
        /* use distance to light to calculate the attenuation */ \
        const float attenuation = compute_attenuation ( lightdist, light_system.spotlights [ i ].att_const, light_system.spotlights [ i ].att_linear, light_system.spotlights [ i ].att_quad ); \
        \
        /* calculate shadow_constant */ \
        float shadow_constant = 1.0; \
        if ( light_system.spotlights [ i ].shadow_mapping_enabled ) \
        { \
            /* transform the fragment position using the light's shadow matrices */ \
            vec4 fragpos_light_proj = light_system.spotlights [ i ].shadow_trans * vec4 ( fragpos, 1.0 ); \
            \
            /* divide x and y components by w component and map to range [0-1] 
             * create the depth linearly using the distance from the fragment to the light
             * by multiplying by shadow_depth_range_mult, the depth will be in range [0-1]
             * also take into account the angle of the surface to the light's position
             */ \
            fragpos_light_proj.xyz = vec3 ( ( fragpos_light_proj.xy / fragpos_light_proj.w ) * 0.5 + 0.5, lightdist * light_system.spotlights [ i ].shadow_depth_range_mult - \
                max ( light_system.spotlights [ i ].shadow_bias * ( 1.0 - compute_diffuse_constant ( lightdir, normal ) ), 0.001 ) ); \
            \
            /* switch depending on if pcf samples are enabled */ \
            if ( light_system.spotlights [ i ].pcf_samples == 0 ) \
            { \
                /* sample the shadow map to get the shadow constant */ \
                shadow_constant = texture ( light_system.shadow_maps, vec4 ( fragpos_light_proj.xy, light_system.dirlights_size + light_system.pointlights_size * 2 + i, fragpos_light_proj.z ) ); \
            } else \
            { \
                /* set the initial sample offset */ \
                vec2 pcf_sample_offset = vec2 ( light_system.spotlights [ i ].pcf_radius, 0.0 ); \
                \
                /* loop through pcf samples */ \
                shadow_constant = 0.0; \
                for ( int j = 0; j < light_system.spotlights [ i ].pcf_samples; j += 2 ) \
                { \
                    /* sample shadow map with the full radius and rotate */ \
                    shadow_constant += texture ( light_system.shadow_maps, \
                        vec4 ( fragpos_light_proj.xy + pcf_sample_offset, light_system.dirlights_size + light_system.pointlights_size * 2 + i, fragpos_light_proj.z ) ); \
                    pcf_sample_offset = light_system.spotlights [ i ].pcf_rotation * pcf_sample_offset; \
                    \
                    /* sample the shadow map with half the radius and rotate */ \
                    shadow_constant += texture ( light_system.shadow_maps, \
                        vec4 ( fragpos_light_proj.xy + pcf_sample_offset * 0.5, light_system.dirlights_size + light_system.pointlights_size * 2 + i, fragpos_light_proj.z ) ); \
                    pcf_sample_offset = light_system.spotlights [ i ].pcf_rotation * pcf_sample_offset; \
                } \
                \
                /* divide shadow_constant by the number of pcf samples taken */ \
                shadow_constant /= light_system.spotlights [ i ].pcf_samples; \
            } \
        } \
        \
        /* add ambient component */ \
        final_color += light_system.spotlights [ i ].ambient_color * ambient_light; \
        \
        /* add diffuse component */ \
        final_color += compute_diffuse_constant ( lightdir, normal ) * spotlight_constant * shadow_constant * attenuation * light_system.spotlights [ i ].diffuse_color * diffuse_light; \
        \
        /* add specular component */ \
        final_color += compute_specular_constant ( viewdir, lightdir, normal, shininess ) * shininess_strength * spotlight_constant * shadow_constant * attenuation \
                     * light_system.spotlights [ i ].specular_color * specular_light; \
    } \
}