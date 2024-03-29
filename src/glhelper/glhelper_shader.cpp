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



/* UNIFORM COMPARISION OPERATORS IMPLEMENTATION */

/* comparison operators for uniform types
 * array uniform comparison operator implementations can be found in header
 *
 * returns true if the objects refer to the same uniform
 */
bool operator== ( const glh::core::uniform& lhs, const glh::core::uniform& rhs )
{
    return ( lhs.get_program () == rhs.get_program () && lhs.get_name () == rhs.get_name () );
}
bool operator!= ( const glh::core::uniform& lhs, const glh::core::uniform& rhs )
{
    return !( lhs == rhs );
}
bool operator== ( const glh::core::struct_uniform& lhs, const glh::core::struct_uniform& rhs )
{
    return ( lhs.get_program () == rhs.get_program () && lhs.get_name () == rhs.get_name () );
}
bool operator!= ( const glh::core::struct_uniform& lhs, const glh::core::struct_uniform& rhs )
{
    return !( lhs == rhs );
}



/* SHADER IMPLEMENTATION */

/* multi-source constructor */
glh::core::shader::shader ( const GLenum type, std::initializer_list<std::string> paths )
    : source { "#version 460" }
    , compiled { false }
{
    /* generate shader */
    id = glCreateShader ( type );

    /* include files */
    include_files ( paths );
}

/* destructor */
glh::core::shader::~shader ()
{
    /* destroy shader */
    glDeleteShader ( id );
}



/* include_files
 *
 * add multiple files to the source
 */
void glh::core::shader::include_files ( std::initializer_list<std::string> paths )
{
    /* loop through paths */
    for ( const auto& path: paths )
    {
        /* try to open the shader */
        std::ifstream shader_file { path, std::ios_base::in };

        /* throw exception for failure */
        if ( !shader_file ) throw exception::shader_exception { "could not open shader file" };

        /* otherwise read shader as string */
        source.append ( std::istreambuf_iterator<char> ( shader_file ), std::istreambuf_iterator<char> () );

        /* close the file */
        shader_file.close ();
    }
}

/* include_source
 *
 * add source code directly
 */
void glh::core::shader::include_source ( const std::string& source_to_include )
{
    /* append the source */
    source.append ( source_to_include );
}



/* compile
 * 
 * compiles the shader
 */
void glh::core::shader::compile ()
{
    /* attach the current source and compile */
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
        std::cerr << "\nSHADER TRACEBACK:\n\n" << comp_log;

        /* write into file */
        std::ofstream log_file { GLH_SHADER_LOG_FILE, std::ios::out };
        log_file << source;
        log_file.close ();

        /* throw */
        throw exception::shader_exception { "shader compilation failed" };
    }

    /* set is_compiled to true */
    compiled = true;
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
void glh::core::uniform::set_int ( const GLint v0 )
{
    /* check that the type is correct */
    if 
    ( 
        uniform_type != GL_INT && uniform_type && uniform_type != GL_BOOL &&
        uniform_type != GL_SAMPLER_1D && uniform_type != GL_SAMPLER_2D && uniform_type != GL_SAMPLER_3D && uniform_type != GL_SAMPLER_CUBE &&
        uniform_type != GL_SAMPLER_1D_SHADOW && uniform_type != GL_SAMPLER_2D_SHADOW && uniform_type != GL_SAMPLER_CUBE_SHADOW &&
        uniform_type != GL_SAMPLER_1D_ARRAY && uniform_type != GL_SAMPLER_2D_ARRAY && uniform_type != GL_SAMPLER_CUBE_MAP_ARRAY && 
        uniform_type != GL_SAMPLER_1D_ARRAY_SHADOW && uniform_type != GL_SAMPLER_2D_ARRAY_SHADOW && uniform_type != GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW &&
        uniform_type != GL_SAMPLER_2D_MULTISAMPLE && 
        uniform_type != GL_SAMPLER_2D_MULTISAMPLE_ARRAY && 
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
    if ( block_index == -1 ) 
    {
        prog.bind ();
        glUniform1i ( location, v0 );
    } else
    {   
        /* get ubo for bind point and throw if anything goes wrong, then set the value */
        const int bind_index = prog.get_uniform_block_binding ( block_index );
        if ( bind_index < 0 ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without it's block being bound to an index" };
        auto bound_ubo = ubo::get_bound_ubo_index ( bind_index );
        if ( !bound_ubo ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without a ubo being associated to its block's index" };
        bound_ubo->buffer_sub_data ( offset, sizeof ( v0 ), &v0 );
    }
}
void glh::core::uniform::set_uint ( const GLuint v0 )
{
    /* check that the type is correct */
    if 
    ( 
        uniform_type != GL_INT && uniform_type && uniform_type != GL_BOOL &&
        uniform_type != GL_SAMPLER_1D && uniform_type != GL_SAMPLER_2D && uniform_type != GL_SAMPLER_3D && uniform_type != GL_SAMPLER_CUBE &&
        uniform_type != GL_SAMPLER_1D_SHADOW && uniform_type != GL_SAMPLER_2D_SHADOW && uniform_type != GL_SAMPLER_CUBE_SHADOW &&
        uniform_type != GL_SAMPLER_1D_ARRAY && uniform_type != GL_SAMPLER_2D_ARRAY && uniform_type != GL_SAMPLER_CUBE_MAP_ARRAY && 
        uniform_type != GL_SAMPLER_1D_ARRAY_SHADOW && uniform_type != GL_SAMPLER_2D_ARRAY_SHADOW && uniform_type != GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW &&
        uniform_type != GL_SAMPLER_2D_MULTISAMPLE && 
        uniform_type != GL_SAMPLER_2D_MULTISAMPLE_ARRAY && 
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
    if ( block_index == -1 ) 
    {
        prog.bind ();
        glUniform1ui ( location, v0 );
    } else
    {   
        /* get ubo for bind point and throw if anything goes wrong, then set the value */
        const int bind_index = prog.get_uniform_block_binding ( block_index );
        if ( bind_index < 0 ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without it's block being bound to an index" };
        auto bound_ubo = ubo::get_bound_ubo_index ( bind_index );
        if ( !bound_ubo ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without a ubo being associated to its block's index" };
        bound_ubo->buffer_sub_data ( offset, sizeof ( v0 ), &v0 );
    }
}
void glh::core::uniform::set_float ( const GLfloat v0 )
{
    /* check that the type is correct */
    if ( uniform_type != GL_FLOAT ) 
        throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with a single float type on a uniform with name " + name };   

    /* apply the uniform */
    if ( block_index == -1 ) 
    {
        prog.bind ();
        glUniform1f ( location, v0 );
    } else
    {   
        /* get ubo for bind point and throw if anything goes wrong, then set the value */
        const int bind_index = prog.get_uniform_block_binding ( block_index );
        if ( bind_index < 0 ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without it's block being bound to an index" };
        auto bound_ubo = ubo::get_bound_ubo_index ( bind_index );
        if ( !bound_ubo ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without a ubo being associated to its block's index" };
        bound_ubo->buffer_sub_data ( offset, sizeof ( v0 ), &v0 );
    }
}
void glh::core::uniform::set_vector ( const math::fvec2& v0 )
{
    /* check that the type is correct */
    if ( uniform_type != GL_FLOAT_VEC2 )
        throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with vec2 type on a uniform with name " + name };

    /* apply the uniform */
    if ( block_index == -1 ) 
    {
        prog.bind ();
        glUniform2f ( location, v0.at ( 0 ), v0.at ( 1 ) );
    } else
    {   
        /* get ubo for bind point and throw if anything goes wrong, then set the value */
        const int bind_index = prog.get_uniform_block_binding ( block_index );
        if ( bind_index < 0 ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without it's block being bound to an index" };
        auto bound_ubo = ubo::get_bound_ubo_index ( bind_index );
        if ( !bound_ubo ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without a ubo being associated to its block's index" };
        bound_ubo->buffer_sub_data ( offset, sizeof ( v0 ), v0.internal_ptr () );
    }
    
}
void glh::core::uniform::set_vector ( const math::fvec3& v0 )
{
    /* check that the type is correct */
    if ( uniform_type != GL_FLOAT_VEC3 )
        throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with vec3 type on a uniform with name " + name };

    /* apply the uniform */
    if ( block_index == -1 ) 
    {
        prog.bind ();
        glUniform3f ( location, v0.at ( 0 ), v0.at ( 1 ), v0.at ( 2 ) );
    } else
    {   
        /* get ubo for bind point and throw if anything goes wrong, then set the value */
        const int bind_index = prog.get_uniform_block_binding ( block_index );
        if ( bind_index < 0 ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without it's block being bound to an index" };
        auto bound_ubo = ubo::get_bound_ubo_index ( bind_index );
        if ( !bound_ubo ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without a ubo being associated to its block's index" };
        bound_ubo->buffer_sub_data ( offset, sizeof ( v0 ), v0.internal_ptr () );
    }
}
void glh::core::uniform::set_vector ( const math::fvec4& v0 )
{
    /* check that the type is correct */
    if ( uniform_type != GL_FLOAT_VEC4 )
        throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with vec4 type on a uniform with name " + name };

    /* apply the uniform */
    if ( block_index == -1 ) 
    {
        prog.bind ();
        glUniform4f ( location, v0.at ( 0 ), v0.at ( 1 ), v0.at ( 2 ), v0.at ( 3 ) );
    } else
    {   
        /* get ubo for bind point and throw if anything goes wrong, then set the value */
        const int bind_index = prog.get_uniform_block_binding ( block_index );
        if ( bind_index < 0 ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without it's block being bound to an index" };
        auto bound_ubo = ubo::get_bound_ubo_index ( bind_index );
        if ( !bound_ubo ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without a ubo being associated to its block's index" };
        bound_ubo->buffer_sub_data ( offset, sizeof ( v0 ), v0.internal_ptr () );
    }
}
void glh::core::uniform::set_matrix ( const math::fmat2& v0 )
{
    /* check that the type is correct */
    if ( uniform_type != GL_FLOAT_MAT2 ) 
        throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with mat2x2 type on a uniform with name " + name };

    /* apply the uniform */
    if ( block_index == -1 ) 
    {
        prog.bind ();
        glUniformMatrix2fv ( location, 1, GL_FALSE, v0.internal_ptr () );
    } else
    {   
        /* convert to uniform-aligned matrix */
        math::ua_fmat2 ua_v0 { v0 };
        /* get ubo for bind point and throw if anything goes wrong, then set the value */
        const int bind_index = prog.get_uniform_block_binding ( block_index );
        if ( bind_index < 0 ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without it's block being bound to an index" };
        auto bound_ubo = ubo::get_bound_ubo_index ( bind_index );
        if ( !bound_ubo ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without a ubo being associated to its block's index" };
        bound_ubo->buffer_sub_data ( offset, sizeof ( ua_v0 ), ua_v0.internal_ptr () );
    }
}
void glh::core::uniform::set_matrix ( const math::fmat2x3& v0 )
{
    /* check that the type is correct */
    if ( uniform_type != GL_FLOAT_MAT2x3 ) 
        throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with mat2x3 type on a uniform with name " + name };

    /* apply the uniform */
    if ( block_index == -1 ) 
    {
        prog.bind ();
        glUniformMatrix2x3fv ( location, 1, GL_FALSE, v0.internal_ptr () );
    } else
    {   
        /* convert to uniform-aligned matrix */
        math::ua_fmat2x3 ua_v0 { v0 };
        /* get ubo for bind point and throw if anything goes wrong, then set the value */
        const int bind_index = prog.get_uniform_block_binding ( block_index );
        if ( bind_index < 0 ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without it's block being bound to an index" };
        auto bound_ubo = ubo::get_bound_ubo_index ( bind_index );
        if ( !bound_ubo ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without a ubo being associated to its block's index" };
        bound_ubo->buffer_sub_data ( offset, sizeof ( ua_v0 ), ua_v0.internal_ptr () );
    }
}
void glh::core::uniform::set_matrix ( const math::fmat2x4& v0 )
{
    /* check that the type is correct */
    if ( uniform_type != GL_FLOAT_MAT2x4 ) 
        throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with mat2x4 type on a uniform with name " + name };

    /* apply the uniform */
    if ( block_index == -1 ) 
    {
        prog.bind ();
        glUniformMatrix2x4fv ( location, 1, GL_FALSE, v0.internal_ptr () );
    } else
    {   
        /* convert to uniform-aligned matrix */
        math::ua_fmat2x4 ua_v0 { v0 };
        /* get ubo for bind point and throw if anything goes wrong, then set the value */
        const int bind_index = prog.get_uniform_block_binding ( block_index );
        if ( bind_index < 0 ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without it's block being bound to an index" };
        auto bound_ubo = ubo::get_bound_ubo_index ( bind_index );
        if ( !bound_ubo ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without a ubo being associated to its block's index" };
        bound_ubo->buffer_sub_data ( offset, sizeof ( ua_v0 ), ua_v0.internal_ptr () );
    }
}
void glh::core::uniform::set_matrix ( const math::fmat3x2& v0 )
{
    /* check that the type is correct */
    if ( uniform_type != GL_FLOAT_MAT3x2 ) 
        throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with mat3x2 type on a uniform with name " + name };

    /* apply the uniform */
    if ( block_index == -1 ) 
    {
        prog.bind ();
        glUniformMatrix3x2fv ( location, 1, GL_FALSE, v0.internal_ptr () );
    } else
    {   
        /* convert to uniform-aligned matrix */
        math::ua_fmat3x2 ua_v0 { v0 };
        /* get ubo for bind point and throw if anything goes wrong, then set the value */
        const int bind_index = prog.get_uniform_block_binding ( block_index );
        if ( bind_index < 0 ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without it's block being bound to an index" };
        auto bound_ubo = ubo::get_bound_ubo_index ( bind_index );
        if ( !bound_ubo ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without a ubo being associated to its block's index" };
        bound_ubo->buffer_sub_data ( offset, sizeof ( ua_v0 ), ua_v0.internal_ptr () );
    }
}
void glh::core::uniform::set_matrix ( const math::fmat3& v0 )
{
    /* check that the type is correct */
    if ( uniform_type != GL_FLOAT_MAT3 ) 
        throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with mat3x3 type on a uniform with name " + name };

    /* apply the uniform */
    if ( block_index == -1 ) 
    {
        prog.bind ();
        glUniformMatrix3fv ( location, 1, GL_FALSE, v0.internal_ptr () );
    } else
    {   
        /* convert to uniform-aligned matrix */
        math::ua_fmat3 ua_v0 { v0 };
        /* get ubo for bind point and throw if anything goes wrong, then set the value */
        const int bind_index = prog.get_uniform_block_binding ( block_index );
        if ( bind_index < 0 ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without it's block being bound to an index" };
        auto bound_ubo = ubo::get_bound_ubo_index ( bind_index );
        if ( !bound_ubo ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without a ubo being associated to its block's index" };
        bound_ubo->buffer_sub_data ( offset, sizeof ( ua_v0 ), ua_v0.internal_ptr () );
    }
}
void glh::core::uniform::set_matrix ( const math::fmat3x4& v0 )
{
    /* check that the type is correct */
    if ( uniform_type != GL_FLOAT_MAT3x4 ) 
        throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with mat4x4 type on a uniform with name " + name };

    /* apply the uniform */
    if ( block_index == -1 ) 
    {
        prog.bind ();
        glUniformMatrix3x4fv ( location, 1, GL_FALSE, v0.internal_ptr () );
    } else
    {   
        /* convert to uniform-aligned matrix */
        math::ua_fmat3x4 ua_v0 { v0 };
        /* get ubo for bind point and throw if anything goes wrong, then set the value */
        const int bind_index = prog.get_uniform_block_binding ( block_index );
        if ( bind_index < 0 ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without it's block being bound to an index" };
        auto bound_ubo = ubo::get_bound_ubo_index ( bind_index );
        if ( !bound_ubo ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without a ubo being associated to its block's index" };
        bound_ubo->buffer_sub_data ( offset, sizeof ( ua_v0 ), ua_v0.internal_ptr () );
    }
}
void glh::core::uniform::set_matrix ( const math::fmat4x2& v0 )
{
    /* check that the type is correct */
    if ( uniform_type != GL_FLOAT_MAT4x2 ) 
        throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with mat4x2 type on a uniform with name " + name };
    
    /* apply the uniform */
    if ( block_index == -1 ) 
    {
        prog.bind ();
        glUniformMatrix4x2fv ( location, 1, GL_FALSE, v0.internal_ptr () );
    } else
    {   
        /* convert to uniform-aligned matrix */
        math::ua_fmat4x2 ua_v0 { v0 };
        /* get ubo for bind point and throw if anything goes wrong, then set the value */
        const int bind_index = prog.get_uniform_block_binding ( block_index );
        if ( bind_index < 0 ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without it's block being bound to an index" };
        auto bound_ubo = ubo::get_bound_ubo_index ( bind_index );
        if ( !bound_ubo ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without a ubo being associated to its block's index" };
        bound_ubo->buffer_sub_data ( offset, sizeof ( ua_v0 ), ua_v0.internal_ptr () );
    }
}
void glh::core::uniform::set_matrix ( const math::fmat4x3& v0 )
{
    /* check that the type is correct */
    if ( uniform_type != GL_FLOAT_MAT4x3 ) 
        throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with mat4x3 type on a uniform with name " + name };

    /* apply the uniform */
    if ( block_index == -1 ) 
    {
        prog.bind ();
        glUniformMatrix4x3fv ( location, 1, GL_FALSE, v0.internal_ptr () );
    } else
    {   
        /* convert to uniform-aligned matrix */
        math::ua_fmat4x3 ua_v0 { v0 };
        /* get ubo for bind point and throw if anything goes wrong, then set the value */
        const int bind_index = prog.get_uniform_block_binding ( block_index );
        if ( bind_index < 0 ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without it's block being bound to an index" };
        auto bound_ubo = ubo::get_bound_ubo_index ( bind_index );
        if ( !bound_ubo ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without a ubo being associated to its block's index" };
        bound_ubo->buffer_sub_data ( offset, sizeof ( ua_v0 ), ua_v0.internal_ptr () );
    }
}
void glh::core::uniform::set_matrix ( const math::fmat4& v0 )
{
    /* check that the type is correct */
    if ( uniform_type != GL_FLOAT_MAT4 ) 
        throw exception::uniform_exception { "attempted to perform invalid uniform setting operation with mat4x4 type on a uniform with name " + name };

    /* apply the uniform */
    if ( block_index == -1 ) 
    {
        prog.bind ();
        glUniformMatrix4fv ( location, 1, GL_FALSE, v0.internal_ptr () );
    } else
    {   
        /* convert to uniform-aligned matrix */
        math::ua_fmat4 ua_v0 { v0 };
        /* get ubo for bind point and throw if anything goes wrong, then set the value */
        const int bind_index = prog.get_uniform_block_binding ( block_index );
        if ( bind_index < 0 ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without it's block being bound to an index" };
        auto bound_ubo = ubo::get_bound_ubo_index ( bind_index );
        if ( !bound_ubo ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without a ubo being associated to its block's index" };
        bound_ubo->buffer_sub_data ( offset, sizeof ( ua_v0 ), ua_v0.internal_ptr () );
    }
}

/* use_program
 *
 * use the associated program
 */
void glh::core::uniform::use_program () const
{
    /* use program */
    prog.bind ();
}

/* is_program_in_use
 *
 * return a boolean for if the associated program is in use
 */
bool glh::core::uniform::is_program_in_use () const
{
    return prog.is_bound ();
}



/* PROGRAM IMPLEMENTATION */

/* three-shader constructor
 *
 * link all three shaders into a program
 * NOTE: the shader program remains valid even when linked shaders are destroyed
 */
glh::core::program::program ( vshader& vs, gshader& gs, fshader& fs )
    : vertex_shader { vs }, geometry_shader { gs }, fragment_shader { fs }
    , has_geometry_shader { true }
    , uniforms { "", * this }, struct_uniforms { "", * this }
    , uniform_array_uniforms { "", * this }, struct_array_uniforms { "", * this }
    , uniform_2d_array_uniforms { "", * this }, struct_2d_array_uniforms { "", * this }
{
    /* generate program */
    id = glCreateProgram ();

    /* check shaders are valid */
    if ( !vs.is_object_valid () || !gs.is_object_valid () || !fs.is_object_valid () ) throw exception::shader_exception { "cannot create shader program from invalid shaders" };
}

/* two-shader constructor
 *
 * link vertex and fragment shaders into a program
 * uses the default geometry shader
 * NOTE: the shader program remains valid even when linked shaders are destroyed
 */
glh::core::program::program ( vshader& vs, fshader& fs )
    : vertex_shader { vs }, fragment_shader { fs }
    , has_geometry_shader { false }
    , uniforms { "", * this }, struct_uniforms { "", * this }
    , uniform_array_uniforms { "", * this }, struct_array_uniforms { "", * this }
    , uniform_2d_array_uniforms { "", * this }, struct_2d_array_uniforms { "", * this }
{
    /* generate program */
    id = glCreateProgram ();

    /* check shaders are valid */
    if ( !vs.is_object_valid () || !fs.is_object_valid () ) throw exception::shader_exception { "cannot create shader program from invalid shaders" };
}

/* destructor */
glh::core::program::~program ()
{
    /* destroy program */
    glDeleteProgram ( id );
}



/* default bind the program */
bool glh::core::program::bind () const
{
    /* if bound, return false, else bind and return true */
    if ( bound_program == this ) return false;
    glUseProgram ( id );
    bound_program = const_cast<program *> ( this );
    return true;
}



/* link
 *
 * link the shader program
 * will throw if any of the shaders are not already compiled
 */
void glh::core::program::link ()
{
    /* check that the shaders are valid and compiled */
    if ( !vertex_shader->is_compiled () || ( has_geometry_shader && !geometry_shader->is_compiled () ) || !fragment_shader->is_compiled () )
        throw exception::shader_exception { "cannot link program with uncompiled shaders" };

    /* attach shaders */
    glAttachShader ( id, vertex_shader->internal_id () );
    if ( has_geometry_shader ) glAttachShader ( id, geometry_shader->internal_id () );
    glAttachShader ( id, fragment_shader->internal_id () );

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



/* compile_and_link
 *
 * compile the shaders then link them
 */
void glh::core::program::compile_and_link ()
{
    /* compile each shader */
    vertex_shader->compile ();
    if ( has_geometry_shader ) geometry_shader->compile ();
    fragment_shader->compile ();

    /* now link the program */
    link ();    
}



/* get_uniform_location
 *
 * get the location of a uniform
 * 
 * name: the name of the uniform
 * 
 * return: location of the uniform
 */
int glh::core::program::get_uniform_location ( const std::string& name ) const
{
    /* try to find the uniform location from the map */
    auto it = uniform_locations.find ( name );
    if ( it != uniform_locations.end () ) return it->second;

    /* otherwise add uniform location to map */
    else
    {
        /* try to get location */
        const int location = glGetUniformLocation ( id, name.c_str () );
        
        /* don't throw, as uniforms not in the default block have a location < 0
         * all uniforms have an index: that can be used to tell if a uniform exists
         * 
         * if ( location < 0 ) throw exception::uniform_exception { "failed to get location of uniform with name " + name };
         */

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
unsigned glh::core::program::get_uniform_index ( const std::string& name ) const
{
    /* try to find the uniform index from the map */
    auto it = uniform_indices.find ( name );
    if ( it != uniform_indices.end () ) return it->second;

    /* otherwise add uniform index to map */
    else
    {
        /* try to get index */
        unsigned index;
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
unsigned glh::core::program::get_uniform_block_index ( const std::string& name ) const
{
    /* try to find the uniform block index from the map */
    auto it = uniform_block_indices.find ( name );
    if ( it != uniform_block_indices.end () ) return it->second;

    /* otherwise add uniform block index to map */
    else
    {
        /* try to get index */
        unsigned index = glGetUniformBlockIndex ( id, name.c_str () );

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
int glh::core::program::get_active_uniform_iv ( const unsigned index, const GLenum target ) const
{
    /* get information and return */
    int param = 0;
    glGetActiveUniformsiv ( id, 1, &index, target, &param );
    return param;
}
int glh::core::program::get_active_uniform_iv ( const std::string& name, const GLenum target ) const
{
    /* get index and call overload */
    return get_active_uniform_iv ( get_uniform_index ( name ), target );
}

/* get_active_uniform_block_iv
 *
 * get integer information about a uniform block
 * 
 * index/name: the index/name of the uniform block
 * target: the target piece of information
 */
int glh::core::program::get_active_uniform_block_iv ( const unsigned index, const GLenum target ) const
{
    /* get information and return */
    int param = 0;
    glGetActiveUniformBlockiv ( id, index, target, &param );
    return param;
}
int glh::core::program::get_active_uniform_block_iv ( const std::string& name, const GLenum target ) const
{
    /* get index and call overload */
    return get_active_uniform_block_iv ( get_uniform_block_index ( name ), target );
}



/* set_uniform_block_binding
 *
 * bind/unbind a uniform block from an index
 *
 * block_index/name: the index/name of the uniform block
 * bp_index: the index of the bind point
 */
bool glh::core::program::set_uniform_block_binding ( const unsigned block_index, const unsigned bp_index ) const
{
    /* resize vector if necessary */
    if ( uniform_block_bindings.size () <= block_index ) uniform_block_bindings.resize ( block_index + 1, -1 );

    /* if already bound to index, return false */
    if ( uniform_block_bindings.at ( block_index ) == bp_index ) return false;

    /* otherwise bind block to index */
    glUniformBlockBinding ( id, block_index, bp_index );

    /* record binding */
    uniform_block_bindings.at ( block_index ) = bp_index;

    /* return true */
    return true;
}
bool glh::core::program::set_uniform_block_binding ( const std::string& block_name, const unsigned bp_index ) const
{
    /* get index and call overload */
    return set_uniform_block_binding ( get_uniform_block_index ( block_name ), bp_index );
}

/* get_uniform_block_binding
 *
 * returns the index of the bind point a uniform block is bound to
 * -1 if not bound
 * 
 * block_index/name: the index/name of the block to get the bind point of
 */
int glh::core::program::get_uniform_block_binding ( const unsigned block_index ) const
{
    /* simply return the value in uniform_block_bindings */
    if ( uniform_block_bindings.size () > block_index ) return uniform_block_bindings.at ( block_index );
    else return -1;
}
int glh::core::program::get_uniform_block_binding ( const std::string& block_name ) const
{
    /* get index and call overload */
    return get_uniform_block_binding ( get_uniform_block_index ( block_name ) );
}



/* the currently bound program */
glh::core::object_pointer<glh::core::program> glh::core::program::bound_program {};