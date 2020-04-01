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

/* structure for lighting */
struct LightingStruct
{
    vec3 ViewPos;
    vec3 LightPos;
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
};

/* texture coords, normal vector and position */
in vec2 TexCoords;
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

    vec3 Ambient = vec3 ( texture ( Material.Diffuse, TexCoords ) ) * Lighting.Ambient;

    /* CALCULATE DIFFUSE COMPONENT */

    /* calculate light direction */
    vec3 LightDir = normalize ( FragPos - Lighting.LightPos );
    /* calculate diffuse component */
    float Diff = max ( dot ( NormVec, -LightDir ), 0.0 );
    vec3 Diffuse = vec3 ( texture ( Material.Diffuse, TexCoords ) ) * Diff * Lighting.Diffuse;

    /* CALCULATE SPECULAR COMPONENT */

    /* calculate the direction of viewing */
    vec3 ViewDir = normalize ( Lighting.ViewPos - FragPos );
    /* calculate the direction of the reflected light */
    vec3 ReflectDir = reflect ( LightDir, NormVec );
    /* calculate the specular component */
    float Spec = pow ( max ( dot ( ViewDir, ReflectDir ), 0.0 ), Material.Shininess );
    vec3 Specular = vec3 ( texture ( Material.Specular, TexCoords ) ) * Spec * Lighting.Specular;

    /* SET THE FRAGMENT COLOUR */

    /* set to a combination of the ambient, diffuce and specular components */
    FragColor = vec4 ( Ambient + Diffuse + Specular, 1.0 );
}