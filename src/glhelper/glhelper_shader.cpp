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
glh::core::shader::shader ( const GLenum _target, const std::string& _path )
    : object { object_manager::generate_shader ( _target ) }
    , target { _target }
    , path { _path }
{
    /* try to open the shader */
    std::ifstream shader_file { path, std::ios_base::in };
    /* throw exception for failure */
    if ( !shader_file ) throw exception::shader_exception { "could not open shader file" };
    /* otherwise read shader as string */
    source.assign ( std::istreambuf_iterator<char> ( shader_file ), std::istreambuf_iterator<char> () );
    /* close the file */
    shader_file.close ();
    /* throw exception if shader source is empty */
    if ( source.empty () ) throw exception::shader_exception { "imported shader source is empty at path " + path };

    /* generate shader object, attach the source and compile */
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
        throw exception::shader_exception { "shader compilation failed for shader at path " + path };
    }
}



/* PURE_UNIFORM IMPLEMENTATION */

/* constructor
 *
 * construct from location and containing program
 *
 * _name: the name of the uniform
 * _location: the location of the uniform
 * _prog: the program associated with the uniform
 */
glh::core::pure_uniform::pure_uniform ( const std::string& _name, program& _prog )
    : uniform { _name }
    , prog { _prog }
    , location { prog.get_uniform_location ( name ) }
{}

/* use_program
 *
 * use the associated program
 */
void glh::core::pure_uniform::use_program () const
{
    /* use program */
    prog.use ();
}

/* is_program_in_use
 *
 * return a boolean for if the associated program is in use
 */
bool glh::core::pure_uniform::is_program_in_use () const
{
    return prog.is_in_use ();
}

/* assert_is_program_in_use
 *
 * will throw if the associated program is not in use
 */
void glh::core::pure_uniform::assert_is_program_in_use () const
{
    /* if program is not in use, throw */
    if ( !is_program_in_use () ) throw exception::uniform_exception { "attempted to perform pure uniform setting operation without program being in use" };
}



/* PROGRAM IMPLEMENTATION */

/* three-shader constructor
 *
 * link all three shaders into a program
 * NOTE: the shader program remains valid even when linked shaders are destroyed
 */
glh::core::program::program ( const vshader& vs, const gshader& gs, const fshader& fs )
    : object { object_manager::generate_program () }
    , uniforms { "", * this }, struct_uniforms { "", * this }
    , uniform_array_uniforms { "", * this }, struct_array_uniforms { "", * this }
    , uniform_2d_array_uniforms { "", * this }, struct_2d_array_uniforms { "", * this }
{
    /* check shaders are valid */
    if ( !vs.is_object_valid () || gs.is_object_valid () || !fs.is_object_valid () ) throw exception::shader_exception { "cannot create shader program from invalid shaders" };

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
        /* throw exception */
        throw exception::shader_exception { "program linking failed" };
    }
}

/* two-shader constructor
 *
 * link vertex and fragment shaders into a program
 * uses the default geometry shader
 * NOTE: the shader program remains valid even when linked shaders are destroyed
 */
glh::core::program::program ( const vshader& vs, const fshader& fs )
    : object { object_manager::generate_program () }
    , uniforms { "", * this }, struct_uniforms { "", * this }
    , uniform_array_uniforms { "", * this }, struct_array_uniforms { "", * this }
    , uniform_2d_array_uniforms { "", * this }, struct_2d_array_uniforms { "", * this }
{
    /* check shaders are valid */
    if ( !vs.is_object_valid () || !fs.is_object_valid () ) throw exception::shader_exception { "cannot create shader program from invalid shaders" };

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
        /* throw exception */
        throw exception::shader_exception { "program linking failed" };
    }
}

/* get_uniform_location
 *
 * get the location of a uniform
 * 
 * name: the name of the uniform
 * 
 * return: location of the uniform
 */
GLint glh::core::program::get_uniform_location ( const std::string& name ) const
{
    /* try to get the uniform from the map */
    try
    {
        return uniform_locations.at ( name );
    }
    /* catch out of range error and add uniform to map */
    catch ( const std::out_of_range& ex )
    {
        /* try to get location */
        const GLint location = glGetUniformLocation ( id, name.c_str () );
        /* if -1, throw */
        if ( location == -1 ) throw exception::uniform_exception { "failed to find uniform with name " + name };
        /* add uniform to map */
        uniform_locations.insert ( { name, location } );
        /* return the location */
        return location;
    }   
}