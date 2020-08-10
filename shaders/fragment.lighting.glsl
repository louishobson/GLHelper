/*
 * fragment.lighting.glsl
 * 
 * fragment shader for lighting calculations using a gbuffer
 */



/* INPUTS AND OUTPUTS */

/* input texcoords */
in VS_OUT
{
    vec2 texcoords;
} vs_out;

/* output fragcolor */
out vec3 fragcolor;



/* UNIFORMS */

/* fragment positions and shininess */
uniform sampler2D gbuffer_positionshininess;

/* fragment normals and shininess strengths */
uniform sampler2D gbuffer_normalsstrength;

/* fragment albedo and specular colors */
uniform sampler2D gbuffer_albedospec;

/* material and lighting uniforms */
uniform material_struct material;
uniform light_system_struct light_system;

/* camera matrices */
uniform camera_struct camera;



/* MAIN */

/* main */
void main ()
{
    /* sample from gbuffer */
    const vec4 positionshininess = texture ( gbuffer_positionshininess, vs_out.texcoords );
    const vec4 normalsstrength = texture ( gbuffer_normalsstrength, vs_out.texcoords );
    const vec4 albedospec = texture ( gbuffer_albedospec, vs_out.texcoords );

    /* calculate lighting */
    compute_lighting_macro 
    ( 
        fragcolor, 
        albedospec.xyz,
        albedospec.xyz,
        albedospec.www,
        positionshininess.w,
        normalsstrength.w,
        positionshininess.xyz,
        camera.viewpos,
        normalsstrength.xyz,
        light_system
    );
}