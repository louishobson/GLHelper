/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * src/glhelper/glhelper_manager.cpp
 * 
 * implementation of include/glhelper/glhelper_manager.hpp
 * 
 */



/* INCLUDES */

/* include glhelper_manager.hpp */
#include <glhelper/glhelper_manager.hpp>



/* OBJECT_MANAGER IMPLEMENTATION */

/* BUFFER OBJECTS */

/* generate_buffer
 *
 * generate a buffer object
 */
GLuint glh::object_manager::generate_buffer ()
{
    /* generate and return object */
    GLuint id;
    glGenBuffers ( 1, &id );
    return id;
}

/* destroy_buffer
 *
 * destroy a buffer object, unbinding it from any bindings
 */
void glh::object_manager::destroy_buffer ( const GLuint id )
{
    /* if not zero, remove bind entry if bound and then destroy */
    if ( id > 0 )
    {
        if ( bound_vbo == id ) bound_vbo = 0;
        if ( bound_ebo == id ) bound_ebo = 0;
        glDeleteBuffers ( 1, &id );
    }
}



/* bind_vbo
 *
 * bind a buffer object as a vbo
 */
void glh::object_manager::bind_vbo ( const GLuint id )
{
    /* throw if invalid object */
    if ( id == 0 ) throw object_management_exception { "attempted bind operation on invalid vbo object" };

    /* if not already bound, bind and record */
    if ( id != bound_vbo )
    {
        glBindBuffer ( GL_ARRAY_BUFFER, id );
        bound_vbo = id;
    }
}

/* unbind_vbo
 *
 * unbind the vbo, only if it is already bound
 */
void glh::object_manager::unbind_vbo ( const GLuint id )
{
    /* throw if invalid object */
    if ( id == 0 ) throw object_management_exception { "attempted unbind operation on invalid vbo object" };

    /* if bound, unbind and record */
    if ( id == bound_vbo )
    {
        glBindBuffer ( GL_ARRAY_BUFFER, 0 );
        bound_vbo = 0;
    }
}

/* bind_ebo
 *
 * bind a buffer object as an ebo
 */
void glh::object_manager::bind_ebo ( const GLuint id )
{
    /* throw if invalid object */
    if ( id == 0 ) throw object_management_exception { "attempted bind operation on invalid ebo object" };

    /* if not already bound, bind and record */
    if ( id != bound_ebo )
    {
        glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, id );
        bound_ebo = id;
    }
}

/* unbind_ebo
 *
 * unbind the ebo, only if it is alreay bound
 */
void glh::object_manager::unbind_ebo ( const GLuint id )
{
    /* throw if invalid object */
    if ( id == 0 ) throw object_management_exception { "attempted unbind operation on invalid ebo object" };

    /* if bound, unbind and record */
    if ( id == bound_ebo )
    {
        glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, 0 );
        bound_ebo = 0;
    }
}



/* VERTEX ARRAY OBJECTS */

/* generate_vao
 *
 * generate a vertex array object
 */
GLuint glh::object_manager::generate_vao ()
{
    /* generate and return object */
    GLuint id;
    glGenVertexArrays ( 1, &id );
    return id;
}


/* destroy_vao
 *
 * destroy a vao, unbinding it if is bound
 */
void glh::object_manager::destroy_vao ( const GLuint id )
{
    /* if not zero, remove bind entry if bound and then destroy */
    if ( id > 0 )
    {
        if ( bound_vao == id ) bound_vao = 0;
        glDeleteBuffers ( 1, &id );
    }
}


/* bind_vao
 *
 * bind a vao
 */
void glh::object_manager::bind_vao ( const GLuint id )
{
    /* throw if invalid object */
    if ( id == 0 ) throw object_management_exception { "attempted bind operation on invalid vao object" };

    /* if not already bound, bind and record */
    if ( id != bound_vao )
    {
        glBindVertexArray ( id );
        bound_vao = id;
    }
}

/* unbind_vao
 *
 * unbind the vao, only if it is already bound
 */
void glh::object_manager::unbind_vao ( const GLuint id )
{
    /* throw if invalid object */
    if ( id == 0 ) throw object_management_exception { "attempted unbind operation on invalid vao object" };

    /* if bound, unbind and record */
    if ( id == bound_vao )
    {
        glBindVertexArray ( 0 );
        bound_vao = 0;
    }
}



/* SHADER/PROGRAM OBJECTS */

/* generate_shader
 *
 * generate a shader object
 * 
 * type: the type of the shader
 */
GLuint glh::object_manager::generate_shader ( const GLenum type )
{
    /* generate and return object */
    return glCreateShader ( type );
}

/* destroy_shader
 *
 * destroy a shader object
 */
void glh::object_manager::destroy_shader ( const GLuint id )
{
    /* if not zero destroy object */
    if ( id > 0 ) glDeleteShader ( id );
}

/* generate_program
 *
 * generate a program object
 */
GLuint glh::object_manager::generate_program ()
{
    /* generate and return object */
    return glCreateProgram ();
}

/* destroy_program
 * 
 * destroy a program object, making it not in use in the progrss
 */
void glh::object_manager::destroy_program ( const GLuint id )
{
    /* if not zero, remove in use entry and then destroy */
    if ( id > 0 )
    {
        if ( id == in_use_program ) in_use_program = 0;
        glDeleteProgram ( id );
    }
}

/* use_program
 *
 * make a program currently in use
 */
void glh::object_manager::use_program ( const GLuint id )
{
    /* throw if invalid object */
    if ( id == 0 ) throw object_management_exception { "attempted to use invalid program" };

    /* if not alreay in use, use and record */
    if ( id != in_use_program )
    {
        glUseProgram ( id );
        in_use_program = id;
    }
}

/* unuse_program
 *
 * unuse the program, only if it is already in use
 */
void glh::object_manager::unuse_program ( const GLuint id )
{
    /* throw if invalid object */
    if ( id == 0 ) throw object_management_exception { "attempted to unuse invalid program" };

    /* if in use, unuse and record */
    if ( id == in_use_program )
    {
        glUseProgram ( 0 );
        in_use_program = 0;
    }
}



/* TEXTURE OBJECTS */

/* generate_texture
 * 
 * generate a texture object
 */
GLuint glh::object_manager::generate_texture ()
{
    /* generate texture and return */
    GLuint id;
    glGenTextures ( 1, &id );
    return id;
}

/* destroy_texture
 *
 * destroy a texture object, unbindint it from its texture unit
 */
void glh::object_manager::destroy_texture ( const GLuint id )
{
    /* unbind the texture from any texture units and return */
    for ( unsigned i = 0; i < GLH_MAX_TEXTURE_UNITS; ++i ) if ( id == bound_textures.at ( i ) ) bound_textures.at ( i ) = 0;
    glDeleteTextures ( 1, &id );
}

/* bind_texture
 *
 * bind a texture to a texture unit
 * 
 * unit: the texture unit to bind it to
 */
void glh::object_manager::bind_texture ( const GLuint id, const unsigned unit )
{
    /* throw if invalid object */
    if ( id == 0 ) throw object_management_exception { "attempted bind operation on invalid texture object" };    

    /* if not already bound, bind and record */
    if ( id != bound_textures.at ( unit ) )
    {
        glActiveTexture ( GL_TEXTURE0 + unit );
        glBindTexture ( GL_TEXTURE_2D, id );
        bound_textures.at ( unit ) = id;
    }
}


/* unbind_texture
 *
 * unbind a texture from a unit, if it is already bound
 * 
 * unit: the texture unit to unbind it from
 */
void glh::object_manager::unbind_texture ( const GLuint id, const unsigned unit )
{
    /* throw if invalid object */
    if ( id == 0 ) throw object_management_exception { "attempted unbind operation on invalid texture object" };
        
    /* if already bound, unbind and record */
    if ( id == bound_textures.at ( unit ) )
    {
        glActiveTexture ( GL_TEXTURE0 + unit );
        glBindTexture ( GL_TEXTURE_2D, 0 );
        bound_textures.at ( unit ) = 0;
    }
}



/* STATIC MEMBERS DEFINITIONS */

/* currently bound vbo */
GLuint glh::object_manager::bound_vbo { 0 };

/* currently bound ebo */
GLuint glh::object_manager::bound_ebo { 0 };

/* currently bound vao */
GLuint glh::object_manager::bound_vao { 0 };

/* currently in-use program */
GLuint glh::object_manager::in_use_program { 0 };

/* array of texture units and their respectively bound textures */
std::array<GLuint, GLH_MAX_TEXTURE_UNITS> glh::object_manager::bound_textures { 0 };