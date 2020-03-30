/*
 * fragment.glsl
 * 
 * fragment shader
 */

#version 330 core

/* texture coords */
in vec2 TexCoord;

/* output colour */
out vec4 FragColor;

/* texture sampler */
uniform sampler2D texture1;

/* main */
void main ()
{
    FragColor = texture ( texture1, TexCoord );
}