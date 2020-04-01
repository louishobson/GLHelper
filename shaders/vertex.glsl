/*
 * vertex.glsl
 * 
 * vertex shader for testing
 */

#version 330 core

/* transformations structure */
struct TransStructure
{
    mat4 Model;
    mat4 View;
    mat4 Proj;
    mat3 NormMat;
};

/* vertices, texture coords and normal */
layout ( location = 0 ) in vec3 aPos;
layout ( location = 1 ) in vec2 aTexCoords;
layout ( location = 2 ) in vec3 aNormVec;

/* texture coords, normal */
out vec2 TexCoords;
out vec3 NormVec;
out vec3 FragPos;

/* transformation matrices */
uniform TransStructure Trans;

/* main */
void main ()
{
    /* set the position to be the same as the attribute, with an alpha of 1.0 */
    gl_Position = Trans.Proj * Trans.View * Trans.Model * vec4 ( aPos, 1.0f );
    /* set TexCoords to aTexCoords */
    TexCoords = aTexCoords;
    /* set NormVec to NormMat * aNormVec */
    NormVec = normalize ( Trans.NormMat * aNormVec );
    /* set FragPos */
    FragPos = vec3 ( Trans.Model * vec4 ( aPos, 1.0 ) );
}