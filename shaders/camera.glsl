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
    
    vec3 viewpos;
};