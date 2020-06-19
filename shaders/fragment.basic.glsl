/*
 * fragment.basic.glsl
 * 
 * basic fragment shader
 */

#version 460 core

/* fragpos, normal and texture coords */
in vec3 fragpos;
in vec3 normal;
in vec3 texcoords;

/* output color */
out vec4 fragcolor;

/* sampler uniform */
uniform sampler2D texunit;



/* main */
void main ()
{
    /* sample texture and return */
    fragcolor = texture ( texunit, texcoords.xy );
}