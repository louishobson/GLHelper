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
    : object { glh::object_manager::generate_shader ( _target ) }
    , target { _target }
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
    if ( source.empty () ) throw shader_exception { "imported shader source is empty at path " + path };

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
        throw shader_exception { "shader compilation failed for shader at path " + path };
    }
}



/* PROGRAM IMPLEMENTATION */

/* three-shader constructor
 *
 * link all three shaders into a program
 * NOTE: the shader program remains valid even when linked shaders are destroyed
 */
glh::program::program ( const vshader& vs, const gshader& gs, const fshader& fs )
    : object { glh::object_manager::generate_program () }
{
    /* check shaders are valid */
    if ( !vs.is_valid () || gs.is_valid () || !fs.is_valid () ) throw shader_exception { "cannot create shader program from invalid shaders" };

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
        /* throw exception */
        throw shader_exception { "program linking failed" };
    }
}

/* get_(struct_/array_)uniform
 *
 * return a uniform object based on a name
 * 
 * name: the name of the uniform
 * 
 * return: unfirom object
 */
glh::uniform glh::program::get_uniform ( const std::string& name )
{
    /* return the uniform */
    return uniform { get_uniform_location ( name ), * this };
}
const glh::uniform glh::program::get_unfiorm ( const std::string& name ) const
{
    /* return the uniform */
    return uniform { get_uniform_location ( name ), const_cast<program&> ( * this ) };
}
glh::struct_uniform glh::program::get_struct_uniform ( const std::string& name )
{
    /* return the struct uniform */
    return struct_uniform { name, * this };
}
const glh::struct_uniform glh::program::get_struct_uniform ( const std::string& name ) const
{
    /* return the struct uniform */
    return struct_uniform { name, const_cast<program&> ( * this ) };
}

/* get_..._array_uniform
 *
 * simplified versions of get_array_uniform
 */
glh::uniform_array_uniform glh::program::get_uniform_array_uniform ( const std::string& name )
{
    /* return the array uniform */
    return uniform_array_uniform { name, * this };
}
const glh::uniform_array_uniform glh::program::get_uniform_array_uniform ( const std::string& name ) const
{
    /* return the array uniform */
    return uniform_array_uniform { name, const_cast<program&> ( * this ) };
}
glh::struct_array_uniform glh::program::get_struct_array_uniform ( const std::string& name )
{
    /* return the array uniform */
    return struct_array_uniform { name, * this };
}
const glh::struct_array_uniform glh::program::get_struct_array_uniform ( const std::string& name ) const
{
    /* return the array uniform */
    return struct_array_uniform { name, const_cast<program&> ( * this ) };
}
glh::uniform_2d_array_uniform glh::program::get_uniform_2d_array_uniform ( const std::string& name )
{
    /* return the array uniform */
    return uniform_2d_array_uniform { name, * this };
}
const glh::uniform_2d_array_uniform glh::program::get_uniform_2d_array_uniform ( const std::string& name ) const
{
    /* return the array uniform */
    return uniform_2d_array_uniform { name, const_cast<program&> ( * this ) };
}
glh::struct_2d_array_uniform glh::program::get_struct_2d_array_uniform ( const std::string& name )
{
    /* return the array uniform */
    return struct_2d_array_uniform { name, * this };
}
const glh::struct_2d_array_uniform glh::program::get_struct_2d_array_uniform ( const std::string& name ) const
{
    /* return the array uniform */
    return struct_2d_array_uniform { name, const_cast<program&> ( * this ) };
}

/* get_uniform_location
 *
 * get the location of a uniform
 * 
 * return: location of the uniform
 */
GLint glh::program::get_uniform_location ( const std::string& name ) const
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
        if ( location == -1 ) throw uniform_exception { "failed to find uniform with name " + name };
        /* add uniform to map */
        uniform_locations.insert ( { name, location } );
        /* return the location */
        return location;
    }   
}



/* UNIFORM IMPLEMENTATION */

/* check_is_program_in_use
 *
 * will throw if the associated program is not in use
 */
void glh::uniform::check_is_program_in_use () const 
{
    /* if not in use, throw */ 
    if ( !prog.is_in_use () ) throw uniform_exception { "associated program of uniform is not in use" };
}



/* STRUCT_UNIFORM IMPLEMENTATION */

/* get_(struct_/array_)uniform
 *
 * get a member of the struct
 */
glh::uniform glh::struct_uniform::get_uniform ( const std::string& member )
{
    /* return uniform */
    return prog.get_uniform ( name + "." + member );
}
const glh::uniform glh::struct_uniform::get_uniform ( const std::string& member ) const
{
    /* return uniform */
    return prog.get_uniform ( name + "." + member );
}
glh::struct_uniform glh::struct_uniform::get_struct_uniform ( const std::string& member )
{
    /* return uniform */
    return prog.get_struct_uniform ( name + "." + member );
}
const glh::struct_uniform glh::struct_uniform::get_struct_uniform ( const std::string& member ) const
{
    /* return uniform */
    return prog.get_struct_uniform ( name + "." + member );
}

/* get_..._array_uniform
 *
 * simplified versions of get_array_uniform
 */
glh::uniform_array_uniform glh::struct_uniform::get_uniform_array_uniform ( const std::string& member )
{
    /* return the array uniform */
    return uniform_array_uniform { name + "." + member, prog };
}
const glh::uniform_array_uniform glh::struct_uniform::get_uniform_array_uniform ( const std::string& member ) const
{
    /* return the array uniform */
    return uniform_array_uniform { name + "." + member, prog };
}
glh::struct_array_uniform glh::struct_uniform::get_struct_array_uniform ( const std::string& member )
{
    /* return the array uniform */
    return struct_array_uniform { name + "." + member, prog };
}
const glh::struct_array_uniform glh::struct_uniform::get_struct_array_uniform ( const std::string& member ) const
{
    /* return the array uniform */
    return struct_array_uniform { name + "." + member, prog };
}
glh::uniform_2d_array_uniform glh::struct_uniform::get_uniform_2d_array_uniform ( const std::string& member )
{
    /* return the array uniform */
    return uniform_2d_array_uniform { name + "." + member, prog };
}
const glh::uniform_2d_array_uniform glh::struct_uniform::get_uniform_2d_array_uniform ( const std::string& member ) const
{
    /* return the array uniform */
    return uniform_2d_array_uniform { name + "." + member, prog };
}
glh::struct_2d_array_uniform glh::struct_uniform::get_struct_2d_array_uniform ( const std::string& member )
{
    /* return the array uniform */
    return struct_2d_array_uniform { name + "." + member, prog };
}
const glh::struct_2d_array_uniform glh::struct_uniform::get_struct_2d_array_uniform ( const std::string& member ) const
{
    /* return the array uniform */
    return struct_2d_array_uniform { name + "." + member, prog };
}