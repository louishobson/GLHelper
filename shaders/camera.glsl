/*
 * camera.glsl
 *
 * defines structures for cameras
 *
 */



/* STRUCTURES */

/* structure for a camera */
struct camera_struct
{
    mat4 view;
    mat4 proj;
    mat4 view_proj;

    mat4 view_inverse;
    mat4 proj_inverse;
    mat4 view_proj_inverse;
    
    vec3 viewpos;
};