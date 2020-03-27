/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * src/glhelper/glhelper_shader.cpp
 * 
 * implements include/glhelper/glhelper_shader.hpp
 * 
 */



/* INCLUDES */

/* include glhelper_shader.hpp */
#include <glhelper/glhelper_shader.hpp>



/* SHADER IMPLEMENTATION */

/* constructor */
glh::shader::shader ( const GLenum _target, const std::string& _path )
    : target { _target }
    , path { _path }
{
    /* try to open the shader */
    std::ifstream shader_file { path, std::ios_base::in };
    /* throw exception for failure */
    if ( !shader_file ) throw shader_exception { "could not open shader file" };
    /* otherwise read shader as string */
    source.assign ( std::istreambuf_iterator<char> ( shader_file ), std::istreambuf_iterator<char> () );
    /* close the file */
    shader_file.close ();
    /* throw exception if shader source is empty */
    if ( source.empty () ) throw shader_exception { "imported shader source is empty" };

    /* generate shader object, attach the source and compile */
    id = glCreateShader ( target );
    const char * source_ptr = source.c_str ();
    glShaderSource ( id, 1, &source_ptr, NULL );
    glCompileShader ( id );

    /* check compilation success */
    GLint comp_success;
    glGetShaderiv ( id, GL_COMPILE_STATUS, &comp_success );
    if ( !comp_success ) 
    {
        /* compilation failed
         * first get log info */
        char comp_log [ GLH_SHADER_LOG_SIZE ];
        glGetShaderInfoLog ( id, GLH_SHADER_LOG_SIZE, NULL, comp_log );
        /* print log info to stderr */
        std::cerr << comp_log;
        /* destroy the shader and throw */
        destroy ();
        throw shader_exception { "shader compilation failed" };
    }
}

/* destroy
 *
 * destroys the shader, setting its id to 0
 * any program using this shader will still function
 */
void glh::shader::destroy ()
{
    /* if is valid */
    if ( is_valid () )
    {
        /* destroy and set id to 0 */
        glDeleteShader ( id );
        id = 0;
    }   
}



/* PROGRAM IMPLEMENTATION */

/* three-shader constructor
 *
 * link all three shaders into a program
 * NOTE: the shader program remains valid even when linked shaders are destroyed
 */
glh::program::program ( const vshader& vs, const gshader& gs, const fshader& fs )
{
    /* check shaders are valid */
    if ( !vs.is_valid () || gs.is_valid () || !fs.is_valid () ) throw shader_exception { "cannot create shader program from invalid shaders" };

    /* generate program */
    id = glCreateProgram ();

    /* attach shaders */
    glAttachShader ( id, vs.internal_id () );
    glAttachShader ( id, gs.internal_id () );
    glAttachShader ( id, fs.internal_id () );

    /* link the program */
    glLinkProgram ( id );

    /* check linking success */
    int link_success;
    glGetProgramiv ( id, GL_LINK_STATUS, &link_success );
    if ( !link_success )
    {
        /* linking failed
         * first get log info */
        char link_log [ GLH_SHADER_LOG_SIZE ];
        glGetProgramInfoLog ( id, GLH_SHADER_LOG_SIZE, NULL, link_log );
        /* print log info to stderr */
        std::cerr << link_log;
        /* destroy the program and throw */
        destroy ();
        throw shader_exception { "program linking failed" };
    }
}

/* two-shader constructor
 *
 * link vertex and fragment shaders into a program
 * uses the default geometry shader
 * NOTE: the shader program remains valid even when linked shaders are destroyed
 */
glh::program::program ( const vshader& vs, const fshader& fs )
{
    /* check shaders are valid */
    if ( !vs.is_valid () || !fs.is_valid () ) throw shader_exception { "cannot create shader program from invalid shaders" };

    /* generate program */
    id = glCreateProgram ();

    /* attach shaders */
    glAttachShader ( id, vs.internal_id () );
    glAttachShader ( id, fs.internal_id () );

    /* link the program */
    glLinkProgram ( id );

    /* check linking success */
    int link_success;
    glGetProgramiv ( id, GL_LINK_STATUS, &link_success );
    if ( !link_success )
    {
        /* linking failed
         * first get log info */
        char link_log [ GLH_SHADER_LOG_SIZE ];
        glGetProgramInfoLog ( id, GLH_SHADER_LOG_SIZE, NULL, link_log );
        /* print log info to stderr */
        std::cerr << link_log;
        /* destroy the program and throw */
        destroy ();
        throw shader_exception { "program linking failed" };
    }
}

/* get_uniform
 *
 * return a uniform object based on a name
 * 
 * name: the name of the uniform
 * 
 * return: unfirom object
 */
glh::uniform glh::program::get_uniform ( const std::string& name ) const
{ 
    /* get the location and return the new uniform */
    return uniform { get_uniform_location ( name ), * this }; 
}


/* destroy
 *
 * destroys the shader program, setting id to 0
 */
void glh::program::destroy ()
{
    /* if is valid */
    if ( is_valid () )
    {
        /* destroy and set id to 0 */
        glDeleteProgram ( id );
        id = 0;
    }    
}

/* use
 *
 * use the shader program for the following OpenGL function calls
 * will not call glUseProgram if already in use
 */
void glh::program::use () const
{ 
    /* if program is not valid, throw error */
    if ( !is_valid () ) throw shader_exception { "cannot use invalid shader program" };

    /* use program, if not already in use */
    if ( !is_in_use () ) glUseProgram ( id );
}

/* is_in_use
 *
 * return: boolean for if the program is in use
 */
bool glh::program::is_in_use () const 
{
    /* get program currently in use */
    GLint program_in_use;
    glGetIntegerv ( GL_CURRENT_PROGRAM, &program_in_use ); 
    /* return boolean for if is valid and is in use */
    return ( is_valid () && program_in_use == id ); 
}

/* get_uniform_location
 *
 * get the location of a uniform
 * 
 * return: location of the uniform
 */
GLint glh::program::get_uniform_location ( const std::string& name ) const
{
    /* try to get location */
    const GLint location = glGetUniformLocation ( id, name.c_str () );
    /* if -1, throw */
    if ( location == -1 ) throw shader_exception { "failed to find uniform" };
    /* return location */
    return location;
}



/* UNIFORM IMPLEMENTATION */

/* check_is_program_in_use
 *
 * will throw if the associated program is not in use
 */
void glh::uniform::check_is_program_in_use () const 
{
    /* if not in use, throw */ 
    if ( !prog.is_in_use () ) throw shader_exception { "associated program of shader is not in use" };
}