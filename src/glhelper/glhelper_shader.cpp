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

/* constructor */
glh::core::shader::shader ( const minor_object_type _type, const std::string& _path )
    : object { _type }
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
        /* throw */
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
void glh::core::uniform::set_int ( const GLint v0 )
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
    if ( block_index == -1 ) 
    {
        assert_is_program_in_use ( "default set uniform" );
        glUniform1i ( location, v0 );
    } else
    {   
        /* get ubo for bind point and throw if anything goes wrong, then set the value */
        const int bind_point = prog.get_uniform_block_binding ( block_index );
        if ( bind_point < 0 ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without it's block being bound to an index" };
        ubo * bound_ubo = ubo::get_index_bound_ubo_pointer ( bind_point );
        if ( !bound_ubo ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without a ubo being associated to its block's index" };
        bound_ubo->buffer_sub_data ( offset, sizeof ( v0 ), &v0 );
    }
}
void glh::core::uniform::set_uint ( const GLuint v0 )
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
    if ( block_index == -1 ) 
    {
        assert_is_program_in_use ( "default set uniform" );
        glUniform1ui ( location, v0 );
    } else
    {   
        /* get ubo for bind point and throw if anything goes wrong, then set the value */
        const int bind_point = prog.get_uniform_block_binding ( block_index );
        if ( bind_point < 0 ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without it's block being bound to an index" };
        ubo * bound_ubo = ubo::get_index_bound_ubo_pointer ( bind_point );
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
        assert_is_program_in_use ( "default set uniform" );
        glUniform1f ( location, v0 );
    } else
    {   
        /* get ubo for bind point and throw if anything goes wrong, then set the value */
        const int bind_point = prog.get_uniform_block_binding ( block_index );
        if ( bind_point < 0 ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without it's block being bound to an index" };
        ubo * bound_ubo = ubo::get_index_bound_ubo_pointer ( bind_point );
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
        assert_is_program_in_use ( "default set uniform" );
        glUniform2f ( location, v0.at ( 0 ), v0.at ( 1 ) );
    } else
    {   
        /* convert to uniform-aligned vector */
        math::ua_fvec2 ua_v0 { v0 };
        /* get ubo for bind point and throw if anything goes wrong, then set the value */
        const int bind_point = prog.get_uniform_block_binding ( block_index );
        if ( bind_point < 0 ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without it's block being bound to an index" };
        ubo * bound_ubo = ubo::get_index_bound_ubo_pointer ( bind_point );
        if ( !bound_ubo ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without a ubo being associated to its block's index" };
        bound_ubo->buffer_sub_data ( offset, sizeof ( ua_v0 ), ua_v0.internal_ptr () );
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
        assert_is_program_in_use ( "default set uniform" );
        glUniform3f ( location, v0.at ( 0 ), v0.at ( 1 ), v0.at ( 2 ) );
    } else
    {   
        /* convert to uniform-aligned vector */
        math::ua_fvec3 ua_v0 { v0 };
        /* get ubo for bind point and throw if anything goes wrong, then set the value */
        const int bind_point = prog.get_uniform_block_binding ( block_index );
        if ( bind_point < 0 ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without it's block being bound to an index" };
        ubo * bound_ubo = ubo::get_index_bound_ubo_pointer ( bind_point );
        if ( !bound_ubo ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without a ubo being associated to its block's index" };
        bound_ubo->buffer_sub_data ( offset, sizeof ( ua_v0 ), ua_v0.internal_ptr () );
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
        assert_is_program_in_use ( "default set uniform" );
        glUniform4f ( location, v0.at ( 0 ), v0.at ( 1 ), v0.at ( 2 ), v0.at ( 3 ) );
    } else
    {   
        /* convert to uniform-aligned vector */
        math::ua_fvec4 ua_v0 { v0 };
        /* get ubo for bind point and throw if anything goes wrong, then set the value */
        const int bind_point = prog.get_uniform_block_binding ( block_index );
        if ( bind_point < 0 ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without it's block being bound to an index" };
        ubo * bound_ubo = ubo::get_index_bound_ubo_pointer ( bind_point );
        if ( !bound_ubo ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without a ubo being associated to its block's index" };
        bound_ubo->buffer_sub_data ( offset, sizeof ( ua_v0 ), ua_v0.internal_ptr () );
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
        assert_is_program_in_use ( "default set uniform" );
        glUniformMatrix2fv ( location, 1, GL_FALSE, v0.internal_ptr () );
    } else
    {   
        /* convert to uniform-aligned matrix */
        math::ua_fmat2 ua_v0 { v0 };
        /* get ubo for bind point and throw if anything goes wrong, then set the value */
        const int bind_point = prog.get_uniform_block_binding ( block_index );
        if ( bind_point < 0 ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without it's block being bound to an index" };
        ubo * bound_ubo = ubo::get_index_bound_ubo_pointer ( bind_point );
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
        assert_is_program_in_use ( "default set uniform" );
        glUniformMatrix2x3fv ( location, 1, GL_FALSE, v0.internal_ptr () );
    } else
    {   
        /* convert to uniform-aligned matrix */
        math::ua_fmat2x3 ua_v0 { v0 };
        /* get ubo for bind point and throw if anything goes wrong, then set the value */
        const int bind_point = prog.get_uniform_block_binding ( block_index );
        if ( bind_point < 0 ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without it's block being bound to an index" };
        ubo * bound_ubo = ubo::get_index_bound_ubo_pointer ( bind_point );
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
        assert_is_program_in_use ( "default set uniform" );
        glUniformMatrix2x4fv ( location, 1, GL_FALSE, v0.internal_ptr () );
    } else
    {   
        /* convert to uniform-aligned matrix */
        math::ua_fmat2x4 ua_v0 { v0 };
        /* get ubo for bind point and throw if anything goes wrong, then set the value */
        const int bind_point = prog.get_uniform_block_binding ( block_index );
        if ( bind_point < 0 ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without it's block being bound to an index" };
        ubo * bound_ubo = ubo::get_index_bound_ubo_pointer ( bind_point );
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
        assert_is_program_in_use ( "default set uniform" );
        glUniformMatrix3x2fv ( location, 1, GL_FALSE, v0.internal_ptr () );
    } else
    {   
        /* convert to uniform-aligned matrix */
        math::ua_fmat3x2 ua_v0 { v0 };
        /* get ubo for bind point and throw if anything goes wrong, then set the value */
        const int bind_point = prog.get_uniform_block_binding ( block_index );
        if ( bind_point < 0 ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without it's block being bound to an index" };
        ubo * bound_ubo = ubo::get_index_bound_ubo_pointer ( bind_point );
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
        assert_is_program_in_use ( "default set uniform" );
        glUniformMatrix3fv ( location, 1, GL_FALSE, v0.internal_ptr () );
    } else
    {   
        /* convert to uniform-aligned matrix */
        math::ua_fmat3 ua_v0 { v0 };
        /* get ubo for bind point and throw if anything goes wrong, then set the value */
        const int bind_point = prog.get_uniform_block_binding ( block_index );
        if ( bind_point < 0 ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without it's block being bound to an index" };
        ubo * bound_ubo = ubo::get_index_bound_ubo_pointer ( bind_point );
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
        assert_is_program_in_use ( "default set uniform" );
        glUniformMatrix3x4fv ( location, 1, GL_FALSE, v0.internal_ptr () );
    } else
    {   
        /* convert to uniform-aligned matrix */
        math::ua_fmat3x4 ua_v0 { v0 };
        /* get ubo for bind point and throw if anything goes wrong, then set the value */
        const int bind_point = prog.get_uniform_block_binding ( block_index );
        if ( bind_point < 0 ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without it's block being bound to an index" };
        ubo * bound_ubo = ubo::get_index_bound_ubo_pointer ( bind_point );
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
        assert_is_program_in_use ( "default set uniform" );
        glUniformMatrix4x2fv ( location, 1, GL_FALSE, v0.internal_ptr () );
    } else
    {   
        /* convert to uniform-aligned matrix */
        math::ua_fmat4x2 ua_v0 { v0 };
        /* get ubo for bind point and throw if anything goes wrong, then set the value */
        const int bind_point = prog.get_uniform_block_binding ( block_index );
        if ( bind_point < 0 ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without it's block being bound to an index" };
        ubo * bound_ubo = ubo::get_index_bound_ubo_pointer ( bind_point );
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
        assert_is_program_in_use ( "default set uniform" );
        glUniformMatrix4x3fv ( location, 1, GL_FALSE, v0.internal_ptr () );
    } else
    {   
        /* convert to uniform-aligned matrix */
        math::ua_fmat4x3 ua_v0 { v0 };
        /* get ubo for bind point and throw if anything goes wrong, then set the value */
        const int bind_point = prog.get_uniform_block_binding ( block_index );
        if ( bind_point < 0 ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without it's block being bound to an index" };
        ubo * bound_ubo = ubo::get_index_bound_ubo_pointer ( bind_point );
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
        assert_is_program_in_use ( "default set uniform" );
        glUniformMatrix4fv ( location, 1, GL_FALSE, v0.internal_ptr () );
    } else
    {   
        /* convert to uniform-aligned matrix */
        math::ua_fmat4 ua_v0 { v0 };
        /* get ubo for bind point and throw if anything goes wrong, then set the value */
        const int bind_point = prog.get_uniform_block_binding ( block_index );
        if ( bind_point < 0 ) throw exception::uniform_exception { "attempted to set value to uniform not in default block without it's block being bound to an index" };
        ubo * bound_ubo = ubo::get_index_bound_ubo_pointer ( bind_point );
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
    : object { minor_object_type::GLH_PROGRAM_TYPE }
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
    : object { minor_object_type::GLH_PROGRAM_TYPE }
    , uniforms { "", * this }, struct_uniforms { "", * this }
    , uniform_array_uniforms { "", * this }, struct_array_uniforms { "", * this }
    , uniform_2d_array_uniforms { "", * this }, struct_2d_array_uniforms { "", * this }
{
    /* check shaders are valid */
    if ( !vs.is_object_valid () || !fs.is_object_valid () ) throw exception::shader_exception { "cannot create shader program from invalid shaders" };

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
        
        if ( location < 0 ) throw exception::uniform_exception { "failed to get location of uniform with name " + name };

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

/* get_active_uniform_block_iv
 *
 * get integer information about a uniform block
 * 
 * index/name: the index/name of the uniform block
 * target: the target piece of information
 */
GLint glh::core::program::get_active_uniform_block_iv ( const GLuint index, const GLenum target ) const
{
    /* get information and return */
    GLint param = 0;
    glGetActiveUniformBlockiv ( id, index, target, &param );
    return param;
}
GLint glh::core::program::get_active_uniform_block_iv ( const std::string& name, const GLenum target ) const
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
bool glh::core::program::set_uniform_block_binding ( const GLuint block_index, const GLuint bp_index ) const
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
bool glh::core::program::set_uniform_block_binding ( const std::string& block_name, const GLuint bp_index ) const
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
GLint glh::core::program::get_uniform_block_binding ( const GLuint block_index ) const
{
    /* simply return the value in uniform_block_bindings */
    if ( uniform_block_bindings.size () > block_index ) return uniform_block_bindings.at ( block_index );
    else return -1;
}
GLint glh::core::program::get_uniform_block_binding ( const std::string& block_name ) const
{
    /* get index and call overload */
    return get_uniform_block_binding ( get_uniform_block_index ( block_name ) );
}
