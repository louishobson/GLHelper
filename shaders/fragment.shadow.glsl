/*
 * fragment.shadow.glsl
 * 
 * shadow fragment shader
 */



/* input linear depth */
in float depth;


/* main */
void main ()
{
    gl_FragDepth = depth;
}