/*
 * fragment.glsl
 * 
 * fragment shader
 */

#version 330 core

/* structure for a material */
struct MaterialStruct
{
    sampler2D Diffuse;
    sampler2D Specular;
    float Shininess;
};

/* structure for light properties */
struct LightPropertiesStruct
{
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
};

/* structure for attenuation values */
struct LightAttenuationStruct
{
    float Constant;
    float Linear;
    float Quadratic;
};

/* structure for point light */
struct PointLightStruct
{
    vec3 Position;
    LightAttenuationStruct Attenuation;
    LightPropertiesStruct Properties;
};

/* texture coords, normal vector and position */
in vec2 TexCoords;
in vec3 NormVec;
in vec3 FragPos;

/* output colour */
out vec4 FragColor;

/* viewpos, material and lighting uniforms */
uniform vec3 ViewPos;
uniform MaterialStruct Material;
uniform PointLightStruct Lighting [ 2 ];



/* function to calculate point light colour
 *
 * Light: a PointLightStruct for the light
 * Material: a MaterialStruct for the material
 * Normal: the normal to the surface
 * Fragment: the coordinates of the fragment
 * Position: the position of the camera
 * 
 * return: the colour of the fragment
 */
vec3 CalcPointLight ( PointLightStruct Light )
{
    /* CALCULATE AMBIENT COMPONENT */

    vec3 Ambient = vec3 ( texture ( Material.Diffuse, TexCoords ) ) * Light.Properties.Ambient;

    /* CALCULATE DIFFUSE COMPONENT */

    /* calculate light direction */
    vec3 LightDir = normalize ( Light.Position - FragPos );
    /* calculate diffuse component */
    float Diff = max ( dot ( NormVec, LightDir ), 0.0 );
    vec3 Diffuse = vec3 ( texture ( Material.Diffuse, TexCoords ) ) * Diff * Light.Properties.Diffuse;

    /* CALCULATE SPECULAR COMPONENT */

    /* calculate the direction of viewing */
    vec3 ViewDir = normalize ( ViewPos - FragPos );
    /* calculate the direction of the reflected light */
    vec3 ReflectDir = reflect ( - LightDir, NormVec );
    /* calculate the specular component */
    float Spec = pow ( max ( dot ( ViewDir, ReflectDir ), 0.0 ), Material.Shininess );
    vec3 Specular = vec3 ( texture ( Material.Specular, TexCoords ) ) * Spec * Light.Properties.Specular;

    /* ATTENUATE */

    /* calculate attenuation */
    float Distance = length ( Light.Position - FragPos );
    float Attenuation = 1.0 / ( Light.Attenuation.Constant + ( Light.Attenuation.Linear * Distance ) + ( Light.Attenuation.Quadratic * Distance * Distance ) );

    /* apply attenuation */
    Ambient *= Attenuation;
    Diffuse *= Attenuation;
    Specular *= Attenuation;

    /* SET THE FRAGMENT COLOUR */

    /* set to a combination of the ambient, diffuce and specular components */
    return Diffuse + Specular;
}



/* main */
void main ()
{
    FragColor = vec4 ( CalcPointLight ( Lighting [ 0 ] ) + CalcPointLight ( Lighting [ 1 ] ), 1.0 );
}