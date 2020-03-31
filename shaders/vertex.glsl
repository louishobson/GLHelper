/*
 * vertex.glsl
 * 
 * vertex shader for testing
 */

#version 330 core

/* vertices, texture coords and normal */
layout ( location = 0 ) in vec3 aPos;
layout ( location = 1 ) in vec2 aTexCoord;
layout ( location = 2 ) in vec3 aNormVec;

/* texture coords, normal */
out vec2 TexCoord;
out vec3 NormVec;
out vec3 FragPos;

/* transformation matrices */
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Proj;
uniform mat3 NormMat;

/* main */
void main ()
{
    /* set the position to be the same as the attribute, with an alpha of 1.0 */
    gl_Position = Proj * View * Model * vec4 ( aPos, 1.0f );
    /* set TexCoord to aTex */
    TexCoord = aTexCoord;
    /* set NormVec to NormMat * aNormVec */
    NormVec = normalize ( NormMat * aNormVec );
    /* set FragPos */
    FragPos = vec3 ( Model * vec4 ( aPos, 1.0 ) );
}