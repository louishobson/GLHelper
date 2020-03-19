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
    shader_file >> source;
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
    /* delete the shader */
    glDeleteShader ( id );

    /* set id to 0 */
    id = 0;
}