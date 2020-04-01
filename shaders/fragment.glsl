/*
 * fragment.glsl
 * 
 * fragment shader
 */

#version 330 core

/* structure for a material */
struct MaterialStruct
{
    sampler2D TexUnit;
    float Shininess;
};

/* structure for lighting */
struct LightingStruct
{
    vec3 ViewPos;
    vec3 Ambient;
    vec3 LightPos;
    vec3 Colour;
    float SpecStrength;
};

/* texture coords, normal vector and position */
in vec2 TexCoord;
in vec3 NormVec;
in vec3 FragPos;

/* output colour */
out vec4 FragColor;

/* material and lighting uniforms */
uniform MaterialStruct Material;
uniform LightingStruct Lighting;

/* main */
void main ()
{
    /* CALCULATE AMBIENT COMPONENT */

    vec3 Ambient = Lighting.Ambient;

    /* CALCULATE DIFFUSE COMPONENT */

    /* calculate light direction */
    vec3 LightDir = normalize ( FragPos - Lighting.LightPos );
    /* calculate diffuse component */
    vec3 Diffuse = max ( dot ( NormVec, -LightDir ), 0.0 ) * Lighting.Colour;

    /* CALCULATE SPECULAR COMPONENT */

    /* calculate the direction of viewing */
    vec3 ViewDir = normalize ( Lighting.ViewPos - FragPos );
    /* calculate the direction of the reflected light */
    vec3 ReflectDir = reflect ( LightDir, NormVec );
    /* calculate the specular component */
    vec3 Specular = pow ( max ( dot ( ViewDir, ReflectDir ), 0.0 ), Material.Shininess ) * Lighting.SpecStrength * Lighting.Colour;

    /* SET THE FRAGMENT COLOUR */

    /* set to a combination of the ambient, diffuce and specular components */
    FragColor = texture ( Material.TexUnit, TexCoord ) * vec4 ( Ambient + Diffuse + Specular, 1.0 );
}