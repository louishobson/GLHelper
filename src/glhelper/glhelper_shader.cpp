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



/* UNIFORM IMPLEMENTATION */

/* constructor
 *
 * construct from location and containing program
 *
 * _name: the name of the uniform
 * _prog: the program associated with the uniform
 */
glh::core::uniform::uniform ( const std::string& _name, program& _prog )
    : name { _name }
    , prog { _prog }
    , location { _prog.get_uniform_location ( _name ) }
    , index { _prog.get_uniform_index ( _name ) }
    , size { _prog.get_active_uniform_iv ( _name, GL_UNIFORM_SIZE ) }
    , offset { _prog.get_active_uniform_iv ( _name, GL_UNIFORM_OFFSET ) }
    , block_index { _prog.get_active_uniform_iv ( _name, GL_UNIFORM_BLOCK_INDEX ) }
    , uniform_type { _prog.get_active_uniform_iv ( _name, GL_UNIFORM_TYPE ) }
{}

/* set_...
 *
 * set the uniform to varying values
 */
void glh::core::uniform::set_int ( const GLint v0 ) const
{
    /* check that the type is correct */
    if 
    ( 
        uniform_type != GL_INT && uniform_type && uniform_type != GL_BOOL &&
        uniform_type != GL_SAMPLER_1D && uniform_type != GL_SAMPLER_2D && uniform_type != GL_SAMPLER_3D && 
        uniform_type != GL_SAMPLER_CUBE && 
        uniform_type != GL_SAMPLER_1D_SHADOW && uniform_type != GL_SAMPLER_2D_SHADOW && 
        uniform_type != GL_SAMPLER_1D_ARRAY && uniform_type != GL_SAMPLER_2D_ARRAY && 
        uniform_type != GL_SAMPLER_1D_ARRAY_SHADOW && uniform_type != GL_SAMPLER_2D_ARRAY_SHADOW && 
        uniform_type != GL_SAMPLER_2D_MULTISAMPLE && 
        uniform_type != GL_SAMPLER_2D_MULTISAMPLE_ARRAY && 
        uniform_type != GL_SAMPLER_CUBE_SHADOW && 
        uniform_type != GL_SAMPLER_BUFFER && 
        uniform_type != GL_SAMPLER_2D_RECT && 
        uniform_type != GL_SAMPLER_2D_RECT_SHADOW
    )
    {
        if ( uniform_type == GL_UNSIGNED_INT ) throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with a single signed integer type on a uniform with name " + name + " (the uniform has type uint)" };
        else if ( uniform_type == GL_FLOAT ) throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with a single signed integer type on a uniform with name " + name + " (the uniform has type float)" };
        else throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with a single signed integer type on a uniform with name " + name };
    }

    /* apply the uniform */
    if ( block_index == -1 ) default_set ( glUniform1i, v0 );
    else ubo_set<GLint> ( v0 );
}
void glh::core::uniform::set_int ( const GLint v0, const GLint v1 ) const
{
    /* check that the type is correct */
    if ( uniform_type != GL_INT_VEC2 && uniform_type != GL_BOOL_VEC2 )
    {
        if ( uniform_type == GL_UNSIGNED_INT_VEC2 ) throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with two signed integer types on a uniform with name " + name + " (the uniform has type uint vec2)" };
        if ( uniform_type == GL_FLOAT_VEC2 ) throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with two signed integer types on a uniform with name " + name + " (the uniform has type float vec2)" };
        else throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with two signed integer types on a uniform with name " + name };
    }

    /* apply the uniform */
    if ( block_index == -1 ) default_set ( glUniform2i, v0, v1 );
    else ubo_set<GLint> ( v0, v1 );
}
void glh::core::uniform::set_int ( const GLint v0, const GLint v1, const GLint v2 ) const
{
    /* check that the type is correct */
    if ( uniform_type != GL_INT_VEC3 && uniform_type != GL_BOOL_VEC3 )
    {
        if ( uniform_type == GL_UNSIGNED_INT_VEC3 ) throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with three signed integer types on a uniform ith name " + name + " (the uniform has type uint vec3)" };
        else if ( uniform_type == GL_FLOAT_VEC3 ) throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with three signed integer types on a uniform with name " + name + " (the uniform has type float vec3)" };
        else throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with three signed integer types on a uniform with name " + name };
    }

    /* apply the uniform */
    if ( block_index == -1 ) default_set ( glUniform3i, v0, v1, v2 );
    else ubo_set<GLint> ( v0, v1, v2 );
}
void glh::core::uniform::set_int ( const GLint v0, const GLint v1, const GLint v2, const GLint v3 ) const
{
    /* check that the type is correct */
    if ( uniform_type != GL_INT_VEC4 && uniform_type != GL_BOOL_VEC4 )
    {
        if ( uniform_type == GL_UNSIGNED_INT_VEC4 ) throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with four signed integer types on a uniform ith name " + name + " (the uniform has type uint vec4)" };
        else if ( uniform_type == GL_FLOAT_VEC4 ) throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with four signed integer types on a uniform with name " + name + " (the uniform has type float vec4)" };
        else throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with three signed integer types on a uniform with name " + name };
    }

    /* apply the uniform */
    if ( block_index == -1 ) default_set ( glUniform4i, v0, v1, v2, v3 );
    else ubo_set<GLint> ( v0, v1, v2, v3 );
}
void glh::core::uniform::set_uint ( const GLuint v0 ) const
{
    /* check that the type is correct */
    if 
    ( 
        uniform_type != GL_UNSIGNED_INT && uniform_type && uniform_type != GL_BOOL &&
        uniform_type != GL_SAMPLER_1D && uniform_type != GL_SAMPLER_2D && uniform_type != GL_SAMPLER_3D && 
        uniform_type != GL_SAMPLER_CUBE && 
        uniform_type != GL_SAMPLER_1D_SHADOW && uniform_type != GL_SAMPLER_2D_SHADOW && 
        uniform_type != GL_SAMPLER_1D_ARRAY && uniform_type != GL_SAMPLER_2D_ARRAY && 
        uniform_type != GL_SAMPLER_1D_ARRAY_SHADOW && uniform_type != GL_SAMPLER_2D_ARRAY_SHADOW && 
        uniform_type != GL_SAMPLER_2D_MULTISAMPLE && 
        uniform_type != GL_SAMPLER_2D_MULTISAMPLE_ARRAY && 
        uniform_type != GL_SAMPLER_CUBE_SHADOW && 
        uniform_type != GL_SAMPLER_BUFFER && 
        uniform_type != GL_SAMPLER_2D_RECT && 
        uniform_type != GL_SAMPLER_2D_RECT_SHADOW
    )
    {
        if ( uniform_type == GL_INT ) throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with a single unsigned integer type on a uniform with name " + name + " (the uniform has type int)" };
        else if ( uniform_type == GL_FLOAT ) throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with a single unsigned integer type on a uniform with name " + name + " (the uniform has type float)" };
        else throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with a single unsigned integer type on a uniform with name " + name };
    } 

    /* apply the uniform */
    if ( block_index == -1 ) default_set ( glUniform1ui, v0 );
    else ubo_set<GLuint> ( v0 );
}
void glh::core::uniform::set_uint ( const GLuint v0, const GLuint v1 ) const
{
    /* check that the type is correct */
    if ( uniform_type != GL_UNSIGNED_INT_VEC2 && uniform_type != GL_BOOL_VEC2 )
    {
        if ( uniform_type == GL_INT_VEC2 ) throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with two unsigned integer types on a uniform with name " + name + " (the uniform has type int vec2)" };
        if ( uniform_type == GL_FLOAT_VEC2 ) throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with two unsigned integer types on a uniform with name " + name + " (the uniform has type float vec2)" };
        else throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with two unsigned integer types on a uniform with name " + name };
    }

    /* apply the uniform */
    if ( block_index == -1 ) default_set ( glUniform2ui, v0, v1 );
    else ubo_set<GLuint> ( v0, v1 );
}
void glh::core::uniform::set_uint ( const GLuint v0, const GLuint v1, const GLuint v2 ) const
{
    /* check that the type is correct */
    if ( uniform_type != GL_UNSIGNED_INT_VEC3 && uniform_type != GL_BOOL_VEC3 )
    {
        if ( uniform_type == GL_INT_VEC3 ) throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with three unsigned integer types on a uniform with name " + name + " (the uniform has type int vec3)" };
        if ( uniform_type == GL_FLOAT_VEC3 ) throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with three unsigned integer types on a uniform with name " + name + " (the uniform has type float vec3)" };
        else throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with three unsigned integer types on a uniform with name " + name };
    }

    /* apply the uniform */
    if ( block_index == -1 ) default_set ( glUniform3ui, v0, v1, v2 );
    else ubo_set<GLuint> ( v0, v1, v2 );
}
void glh::core::uniform::set_uint ( const GLuint v0, const GLuint v1, const GLuint v2, const GLuint v3 ) const
{
    /* check that the type is correct */
    if ( uniform_type != GL_UNSIGNED_INT_VEC4 && uniform_type != GL_BOOL_VEC4 )
    {
        if ( uniform_type == GL_INT_VEC4 ) throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with four unsigned integer types on a uniform with name " + name + " (the uniform has type int vec4)" };
        if ( uniform_type == GL_FLOAT_VEC4 ) throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with four unsigned integer types on a uniform with name " + name + " (the uniform has type float vec4)" };
        else throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with four unsigned integer types on a uniform with name " + name };
    }

    /* apply the uniform */
    if ( block_index == -1 ) default_set ( glUniform4ui, v0, v1, v2, v3 );
    else ubo_set<GLuint> ( v0, v1, v2, v3 );
}
void glh::core::uniform::set_float ( const GLfloat v0 ) const
{
    /* check that the type is correct */
    if ( uniform_type != GL_FLOAT ) 
        throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with a single float type on a uniform with name " + name };   

    /* apply the uniform */
    if ( block_index == -1 ) default_set ( glUniform1f, v0 );
    else ubo_set<GLfloat> ( v0 );
}
void glh::core::uniform::set_float ( const GLfloat v0, const GLfloat v1 ) const
{
    /* check that the type is correct */
    if ( uniform_type != GL_FLOAT_VEC2 ) 
        throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with two float types on a uniform with name " + name };

    /* apply the uniform */
    if ( block_index == -1 ) default_set ( glUniform2f, v0, v1 );
    else ubo_set<GLfloat> ( v0, v1 );
}
void glh::core::uniform::set_float ( const GLfloat v0, const GLfloat v1, const GLfloat v2 ) const
{
    /* check that the type is correct */
    if ( uniform_type != GL_FLOAT_VEC3 ) 
        throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with three float types on a uniform with name " + name };    

    /* apply the uniform */
    if ( block_index == -1 ) default_set ( glUniform3f, v0, v1, v2 );
    else ubo_set<GLfloat> ( v0, v1, v2 );        
}
void glh::core::uniform::set_float ( const GLfloat v0, const GLfloat v1, const GLfloat v2, const GLfloat v3 ) const
{
    /* check that the type is correct */
    if ( uniform_type != GL_FLOAT_VEC4 ) 
        throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with four float types on a uniform with name " + name };        

    /* apply the uniform */
    if ( block_index == -1 ) default_set ( glUniform4f, v0, v1, v2, v3 );
    else ubo_set<GLfloat> ( v0, v1, v2, v3 );
}
void glh::core::uniform::set_vector ( const math::vec2& v0 ) const
{
    /* check that the type is correct */
    if ( uniform_type != GL_FLOAT_VEC2 )
        throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with vec2 type on a uniform with name " + name };

    /* apply the uniform */
    if ( block_index == -1 ) default_set ( glUniform2f, v0.at ( 0 ), v0.at ( 1 ) );
    else ubo_set<GLfloat> ( v0.at ( 0 ), v0.at ( 1 ) );
}
void glh::core::uniform::set_vector ( const math::vec3& v0 ) const
{
    /* check that the type is correct */
    if ( uniform_type != GL_FLOAT_VEC3 )
        throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with vec3 type on a uniform with name " + name };

    /* apply the uniform */
    if ( block_index == -1 ) default_set ( glUniform3f, v0.at ( 0 ), v0.at ( 1 ), v0.at ( 2 ) );
    else ubo_set<GLfloat> ( v0.at ( 0 ), v0.at ( 1 ), v0.at ( 2 ) );
}
void glh::core::uniform::set_vector ( const math::vec4& v0 ) const
{
    /* check that the type is correct */
    if ( uniform_type != GL_FLOAT_VEC4 )
        throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with vec4 type on a uniform with name " + name };

    /* apply the uniform */
    if ( block_index == -1 ) default_set ( glUniform4f, v0.at ( 0 ), v0.at ( 1 ), v0.at ( 2 ), v0.at ( 3 ) );
    else ubo_set<GLfloat> ( v0.at ( 0 ), v0.at ( 1 ), v0.at ( 2 ), v0.at ( 3 ) );
}
void glh::core::uniform::set_matrix ( const math::matrix<2, 2>& v0 ) const
{
    /* check that the type is correct */
    if ( uniform_type != GL_FLOAT_MAT2 ) 
        throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with mat2x2 type on a uniform with name " + name };

    /* apply the uniform */
    if ( block_index == -1 ) default_set ( glUniformMatrix2fv, 1, GL_FALSE, v0.export_data ().data () );
    else ubo_set_array ( v0.export_data ().data (), 4 );
}
void glh::core::uniform::set_matrix ( const math::matrix<2, 3>& v0 ) const
{
    /* check that the type is correct */
    if ( uniform_type != GL_FLOAT_MAT2x3 ) 
        throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with mat2x3 type on a uniform with name " + name };

    /* apply the uniform */
    if ( block_index == -1 ) default_set ( glUniformMatrix2x3fv, 1, GL_FALSE, v0.export_data ().data () );
    else ubo_set_array ( v0.export_data ().data (), 6 );
}
void glh::core::uniform::set_matrix ( const math::matrix<2, 4>& v0 ) const
{
    /* check that the type is correct */
    if ( uniform_type != GL_FLOAT_MAT2x4 ) 
        throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with mat2x4 type on a uniform with name " + name };

    /* apply the uniform */
    if ( block_index == -1 ) default_set ( glUniformMatrix2x4fv, 1, GL_FALSE, v0.export_data ().data () );
    else ubo_set_array ( v0.export_data ().data (), 8 );
}
void glh::core::uniform::set_matrix ( const math::matrix<3, 2>& v0 ) const
{
    /* check that the type is correct */
    if ( uniform_type != GL_FLOAT_MAT3x2 ) 
        throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with mat3x2 type on a uniform with name " + name };

    /* apply the uniform */
    if ( block_index == -1 ) default_set ( glUniformMatrix3x2fv, 1, GL_FALSE, v0.export_data ().data () );
    else ubo_set_array ( v0.export_data ().data (), 6 );
}
void glh::core::uniform::set_matrix ( const math::matrix<3, 3>& v0 ) const
{
    /* check that the type is correct */
    if ( uniform_type != GL_FLOAT_MAT3 ) 
        throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with mat3x3 type on a uniform with name " + name };

    /* apply the uniform */
    if ( block_index == -1 ) default_set ( glUniformMatrix3fv, 1, GL_FALSE, v0.export_data ().data () );
    else ubo_set_array ( v0.export_data ().data (), 9 );
}
void glh::core::uniform::set_matrix ( const math::matrix<3, 4>& v0 ) const
{
    /* check that the type is correct */
    if ( uniform_type != GL_FLOAT_MAT3x4 ) 
        throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with mat4x4 type on a uniform with name " + name };

    /* apply the uniform */
    if ( block_index == -1 ) default_set ( glUniformMatrix3x4fv, 1, GL_FALSE, v0.export_data ().data () );
    else ubo_set_array ( v0.export_data ().data (), 12 );
}
void glh::core::uniform::set_matrix ( const math::matrix<4, 2>& v0 ) const
{
    /* check that the type is correct */
    if ( uniform_type != GL_FLOAT_MAT4x2 ) 
        throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with mat4x2 type on a uniform with name " + name };
    
    /* apply the uniform */
    if ( block_index == -1 ) default_set ( glUniformMatrix4x2fv, 1, GL_FALSE, v0.export_data ().data () );
    else ubo_set_array ( v0.export_data ().data (), 8 );
}
void glh::core::uniform::set_matrix ( const math::matrix<4, 3>& v0 ) const
{
    /* check that the type is correct */
    if ( uniform_type != GL_FLOAT_MAT4x3 ) 
        throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with mat4x3 type on a uniform with name " + name };

    /* apply the uniform */
    if ( block_index == -1 ) default_set ( glUniformMatrix4x3fv, 1, GL_FALSE, v0.export_data ().data () );
    else ubo_set_array ( v0.export_data ().data (), 12 );
}
void glh::core::uniform::set_matrix ( const math::matrix<4, 4>& v0 ) const
{
    /* check that the type is correct */
    if ( uniform_type != GL_FLOAT_MAT4 ) 
        throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with mat4x4 type on a uniform with name " + name };

    /* apply the uniform */
    if ( block_index == -1 ) default_set ( glUniformMatrix4fv, 1, GL_FALSE, v0.export_data ().data () );
    else ubo_set_array ( v0.export_data ().data (), 16 );
}

/* use_program
 *
 * use the associated program
 */
void glh::core::uniform::use_program () const
{
    /* use program */
    prog.use ();
}

/* is_program_in_use
 *
 * return a boolean for if the associated program is in use
 */
bool glh::core::uniform::is_program_in_use () const
{
    return prog.is_in_use ();
}

/* assert_is_program_in_use
 *
 * will throw if the associated program is not in use
 */
void glh::core::uniform::assert_is_program_in_use ( const std::string& operation ) const
{
    /* if program is not in use, throw */
    if ( !is_program_in_use () ) 
    {
        if ( operation.size () > 0 ) throw exception::uniform_exception { "attempted to perform " + operation + " operation on uniform without program being in use" };
        else throw exception::uniform_exception { "attempted to perform operation on uniform without program being in use" };
    }
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
    /* try to find the uniform location from the map */
    auto it = uniform_locations.find ( name );
    if ( it != uniform_locations.end () ) return it->second;

    /* otherwise add uniform location to map */
    else
    {
        /* try to get location */
        const GLint location = glGetUniformLocation ( id, name.c_str () );
        
        /* add uniform location to map */
        uniform_locations.insert ( { name, location } );
        
        /* return the location */
        return location;
    }   
}

/* get_uniform_index
 *
 * get the index of a uniform
 * 
 * name: the name of the uniform
 * 
 * return: the index of the uniform 
 */
GLuint glh::core::program::get_uniform_index ( const std::string& name ) const
{
    /* try to find the uniform index from the map */
    auto it = uniform_indices.find ( name );
    if ( it != uniform_indices.end () ) return it->second;

    /* otherwise add uniform index to map */
    else
    {
        /* try to get index */
        GLuint index;
        const char * name_ptr = name.c_str ();
        glGetUniformIndices ( id, 1, &name_ptr, &index );

        /* if -1, throw */
        if ( index == GL_INVALID_INDEX ) throw exception::uniform_exception { "failed to get index of uniform with name " + name };
        
        /* add uniform index to map */
        uniform_indices.insert ( { name, index } );
        
        /* return the index */
        return index;
    }   
}

/* get_uniform_block_index
 *
 * get the index of a uniform block from the program
 * 
 * name: the name of the uniform block
 */
GLuint glh::core::program::get_uniform_block_index ( const std::string& name ) const
{
    /* try to find the uniform block index from the map */
    auto it = uniform_block_indices.find ( name );
    if ( it != uniform_block_indices.end () ) return it->second;

    /* otherwise add uniform block index to map */
    else
    {
        /* try to get index */
        GLuint index = glGetUniformBlockIndex ( id, name.c_str () );

        /* if -1, throw */
        if ( index == GL_INVALID_INDEX ) throw exception::uniform_exception { "failed to get index of uniform block with name " + name };
        
        /* add uniform block index to map */
        uniform_block_indices.insert ( { name, index } );
        
        /* return the index */
        return index;
    }   
}

/* get_active_uniform_iv
 *
 * get integer information about a uniform
 * 
 * index/name: the index/name of the uniform
 * target: the target piece of information
 */
GLint glh::core::program::get_active_uniform_iv ( const GLuint index, const GLenum target ) const
{
    /* get information and return */
    GLint param = 0;
    glGetActiveUniformsiv ( id, 1, &index, target, &param );
    return param;
}
GLint glh::core::program::get_active_uniform_iv ( const std::string& name, const GLenum target ) const
{
    /* get index and call overload */
    return get_active_uniform_iv ( get_uniform_index ( name ), target );
}