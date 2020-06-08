/*
 * geometry.model.glsl
 * 
 * model geometry shader
 */

#version 330 core
#extension GL_ARB_arrays_of_arrays : enable



/* maximum number of color sets */
#define MAX_COLOR_SETS 1

/* maximum number of UV channels */
#define MAX_UV_CHANNELS 4



/* recieve triangles from vertex shader */
layout ( triangles ) in;

/* output triangle strip */
layout ( triangle_strip, max_vertices = 3 ) out; 



/* texture coords, normal, vcolor */
in VS_OUT
{
    vec3 fragpos;
    vec3 normal;
    vec4 vcolor [ MAX_COLOR_SETS ];
    vec3 texcoords [ MAX_UV_CHANNELS ];
} gs_in [ 3 ];

out GS_OUT
{
    vec3 fragpos;
    vec3 normal;
    vec4 vcolor [ MAX_COLOR_SETS ];
    vec3 texcoords [ MAX_UV_CHANNELS ];
} gs_out;


/* time uniform */
uniform float time;




/* forward vertex */
void forward_vertex ( const int index )
{
    gl_Position = gl_in [ index ].gl_Position;
    gs_out.fragpos = gs_in [ index ].fragpos;
    gs_out.normal = gs_in [ index ].normal;
    for ( int i = 0; i < MAX_COLOR_SETS; ++i ) gs_out.vcolor [ i ] = gs_in [ index ].vcolor [ i ];
    for ( int i = 0; i < MAX_UV_CHANNELS; ++i ) gs_out.texcoords [ i ] = gs_in [ index ].texcoords [ i ];
    EmitVertex ();
}



/* main */
void main ()
{
    forward_vertex ( 0 );
    forward_vertex ( 1 );
    forward_vertex ( 2 );

    EndPrimitive ();
}