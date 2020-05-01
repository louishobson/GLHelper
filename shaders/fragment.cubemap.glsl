/*
 * fragment.cubemap.glsl
 * 
 * cubemap fragment shader
 */

#version 330 core

/* transformations structure */
struct trans_struct
{
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 viewpos;
};

/* fragpos, normal and texcoord */
in vec3 fragpos;
in vec3 normal;
in vec3 texcoords;

/* output color */
out vec4 fragcolor;

/* sampler uniform */
uniform samplerCube skybox;

/* transformation matrices */
uniform trans_struct trans;



/* main */
void main ()
{
    /* sample texture and return */
    fragcolor = texture ( skybox, texcoords );
}