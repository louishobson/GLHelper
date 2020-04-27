/*
 * fragment.basic.glsl
 * 
 * basic fragment shader
 */

#version 330 core

/* texture coords, normal vector, position, vcolor */
in vec3 texcoords;
in vec3 normal;
in vec3 fragpos;

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