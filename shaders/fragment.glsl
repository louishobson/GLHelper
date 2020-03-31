/*
 * fragment.glsl
 * 
 * fragment shader
 */

#version 330 core

/* texture coords, normal vector and position */
in vec2 TexCoord;
in vec3 NormVec;
in vec3 FragPos;

/* output colour */
out vec4 FragColor;

/* viewing position, ambient light, light position, light colour, specular strength and shininess */
uniform vec3 ViewPos;
uniform vec3 Ambient;
uniform vec3 LightPos;
uniform vec3 LightColour;
uniform float SpecStrength;
uniform float Shininess;

/* texture sampler */
uniform sampler2D TexUnit;

/* main */
void main ()
{
    /* CALCULATE DIFFUSE COMPONENT OF LIGHTING */


    /* calculate light direction */
    vec3 LightDir = normalize ( FragPos - LightPos );
    /* calculate diffuse component */
    vec3 Diffuse = max ( dot ( NormVec, -LightDir ), 0.0 ) * LightColour;

    /* CALCULATE SPECULAR COMPONENTS */

    /* calculate the direction of viewing */
    vec3 ViewDir = normalize ( ViewPos - FragPos );
    /* calculate the direction of the reflected light */
    vec3 ReflectDir = reflect ( LightDir, NormVec );
    /* calculate the specular component */
    vec3 Specular = pow ( max ( dot ( ViewDir, ReflectDir ), 0.0 ), Shininess ) * SpecStrength * LightColour;

    /* SET THE FRAGMENT COLOUR */

    /* set to a combination of the ambient, diffuce and specular components */
    FragColor = texture ( TexUnit, TexCoord ) * vec4 ( Ambient + Diffuse + Specular, 1.0 );
}