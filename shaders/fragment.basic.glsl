/*
 * fragment.basic.glsl
 * 
 * basic fragment shader
 */

#version 330 core

/* texture coords, normal vector and position */
in vec2 texcoords [ 8 ];
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
    fragcolor = pow ( texture ( texunit, texcoords [ 0 ] ) + vec4 ( 0.03, 0.03, 0.03, 0.0 ), vec4 ( 0.8, 0.8, 0.8, 1 ) );
    //fragcolor = texture ( texunit, texcoords [ 0 ] );
}