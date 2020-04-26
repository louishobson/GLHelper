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
GLuint glh::core::object_manager::generate_buffer ()
{
    /* assert opengl is loaded */
    asset_opengl_loaded ();

    /* generate and return object */
    GLuint id;
    glGenBuffers ( 1, &id );
    return id;
}

/* destroy_buffer
 *
 * destroy a buffer object, unbinding it from any bindings
 */
void glh::core::object_manager::destroy_buffer ( const GLuint id )
{
    /* if not zero, remove bind entry if bound and then destroy */
    if ( id > 0 )
    {
        if ( id == bound_vbo ) bound_vbo = 0;
        if ( id == bound_vbo ) bound_ebo = 0;
        glDeleteBuffers ( 1, &id );
    }
}



/* bind_vbo
 *
 * bind a buffer object as a vbo
 */
void glh::core::object_manager::bind_vbo ( const GLuint id )
{
    /* throw if invalid object */
    if ( id == 0 ) throw exception::object_management_exception { "attempted bind operation on invalid vbo object" };

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
void glh::core::object_manager::unbind_vbo ( const GLuint id )
{
    /* throw if invalid object */
    if ( id == 0 ) throw exception::object_management_exception { "attempted unbind operation on invalid vbo object" };

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
void glh::core::object_manager::bind_ebo ( const GLuint id )
{
    /* throw if invalid object */
    if ( id == 0 ) throw exception::object_management_exception { "attempted bind operation on invalid ebo object" };

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
void glh::core::object_manager::unbind_ebo ( const GLuint id )
{
    /* throw if invalid object */
    if ( id == 0 ) throw exception::object_management_exception { "attempted unbind operation on invalid ebo object" };

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
GLuint glh::core::object_manager::generate_vao ()
{
    /* assert opengl is loaded */
    asset_opengl_loaded ();

    /* generate and return object */
    GLuint id;
    glGenVertexArrays ( 1, &id );
    return id;
}


/* destroy_vao
 *
 * destroy a vao, unbinding it if is bound
 */
void glh::core::object_manager::destroy_vao ( const GLuint id )
{
    /* if not zero, remove bind entry if bound and then destroy */
    if ( id > 0 )
    {
        if ( id == bound_vao ) bound_vao = 0;
        glDeleteVertexArrays ( 1, &id );
    }
}


/* bind_vao
 *
 * bind a vao
 */
void glh::core::object_manager::bind_vao ( const GLuint id )
{
    /* throw if invalid object */
    if ( id == 0 ) throw exception::object_management_exception { "attempted bind operation on invalid vao object" };

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
void glh::core::object_manager::unbind_vao ( const GLuint id )
{
    /* throw if invalid object */
    if ( id == 0 ) throw exception::object_management_exception { "attempted unbind operation on invalid vao object" };

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
GLuint glh::core::object_manager::generate_shader ( const GLenum type )
{
    /* assert opengl is loaded */
    asset_opengl_loaded ();

    /* generate and return object */
    return glCreateShader ( type );
}

/* destroy_shader
 *
 * destroy a shader object
 */
void glh::core::object_manager::destroy_shader ( const GLuint id )
{
    /* if not zero destroy object */
    if ( id > 0 ) glDeleteShader ( id );
}

/* generate_program
 *
 * generate a program object
 */
GLuint glh::core::object_manager::generate_program ()
{
    /* assert opengl is loaded */
    asset_opengl_loaded ();

    /* generate and return object */
    return glCreateProgram ();
}

/* destroy_program
 * 
 * destroy a program object, making it not in use in the progrss
 */
void glh::core::object_manager::destroy_program ( const GLuint id )
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
void glh::core::object_manager::use_program ( const GLuint id )
{
    /* throw if invalid object */
    if ( id == 0 ) throw exception::object_management_exception { "attempted to use invalid program" };

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
void glh::core::object_manager::unuse_program ( const GLuint id )
{
    /* throw if invalid object */
    if ( id == 0 ) throw exception::object_management_exception { "attempted to unuse invalid program" };

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
GLuint glh::core::object_manager::generate_texture ()
{
    /* assert opengl is loaded */
    asset_opengl_loaded ();

    /* generate texture and return */
    GLuint id;
    glGenTextures ( 1, &id );
    return id;
}

/* destroy_texture
 *
 * destroy a texture object, unbinding it from any texture units
 */
void glh::core::object_manager::destroy_texture ( const GLuint id )
{
    /* unbind the texture from any texture units and return */
    for ( unsigned i = 0; i < GLH_MAX_TEXTURE_UNITS; ++i ) if ( id == bound_texture2ds.at ( i ) ) bound_texture2ds.at ( i ) = 0;
    glDeleteTextures ( 1, &id );
}

/* bind_texture2d
 *
 * bind a texture2d to a texture unit
 * 
 * unit: the texture unit to bind it to
 */
void glh::core::object_manager::bind_texture2d ( const GLuint id, const unsigned unit )
{
    /* throw if invalid object */
    if ( id == 0 ) throw exception::object_management_exception { "attempted bind operation on invalid texture object" };    

    /* if not already bound, bind and record */
    if ( id != bound_texture2ds.at ( unit ) )
    {
        glActiveTexture ( GL_TEXTURE0 + unit );
        glBindTexture ( GL_TEXTURE_2D, id );
        bound_texture2ds.at ( unit ) = id;
    }
}


/* unbind_texture2d
 *
 * unbind a texture2d from a unit, if it is already bound
 * 
 * unit: the texture unit to unbind it from
 */
void glh::core::object_manager::unbind_texture2d ( const GLuint id, const unsigned unit )
{
    /* throw if invalid object */
    if ( id == 0 ) throw exception::object_management_exception { "attempted unbind operation on invalid texture object" };
        
    /* if already bound, unbind and record */
    if ( id == bound_texture2ds.at ( unit ) )
    {
        glActiveTexture ( GL_TEXTURE0 + unit );
        glBindTexture ( GL_TEXTURE_2D, 0 );
        bound_texture2ds.at ( unit ) = 0;
    }
}

/* bind_cubemap
 *
 * bind a cubemap texture to GL_TEXTURE_CUBE_MAP, if not already bounc
 * 
 * unit: the texture unit to bind it to
 */
void glh::core::object_manager::bind_cubemap ( const GLuint id, const unsigned unit )
{
    /* throw if invalid object */
    if ( id == 0 ) throw exception::object_management_exception { "attempted bind operation on invalid cubemap object" };    

    /* if not already bound, bind and record */
    if ( id != bound_cubemaps.at ( unit ) )
    {
        glActiveTexture ( GL_TEXTURE0 + unit );
        glBindTexture ( GL_TEXTURE_CUBE_MAP, id );
        bound_cubemaps.at ( unit ) = id;
    }
}

/* unbind_cubemap
 * 
 * unbind a cubemap texture from GL_TEXTURE_CUBE_MAP, if already bound
 * 
 * unit: the texture unit to unbind it from
 */
void glh::core::object_manager::unbind_cubemap ( const GLuint id, const unsigned unit )
{
    /* throw if invalid object */
    if ( id == 0 ) throw exception::object_management_exception { "attempted unbind operation on invalid cubemap object" };
        
    /* if already bound, unbind and record */
    if ( id == bound_cubemaps.at ( unit ) )
    {
        glActiveTexture ( GL_TEXTURE0 + unit );
        glBindTexture ( GL_TEXTURE_CUBE_MAP, id );
        bound_cubemaps.at ( unit ) = 0;
    }
}



/* RENDERBUFFER OBJECTS */

/* generate_rbo
 *
 * generate a renderbuffer object
 */
GLuint glh::core::object_manager::generate_rbo ()
{
    /* assert opengl is loaded */
    asset_opengl_loaded ();

    /* generate rbo and return */
    GLuint id;
    glGenRenderbuffers ( 1, &id );
    return id;
}

/* destroy_rbo
 *
 * destroy a renderbuffer object, unbindint it if bound
 */
void glh::core::object_manager::destroy_rbo ( const GLuint id )
{
    /* if not zero, remove bind entry if bound and then destroy */
    if ( id > 0 )
    {
        if ( id == bound_rbo ) bound_rbo = 0;
        glDeleteRenderbuffers ( 1, &id );
    }
}

/* bind_rbo
 *
 * bind a renderbuffer object
 */
void glh::core::object_manager::bind_rbo ( const GLuint id )
{
    /* throw if invalid object */
    if ( id == 0 ) throw exception::object_management_exception { "attempted bind operation on invalid renderbuffer object" };    

    /* if not already bound, bind and record */
    if ( id != bound_rbo )
    {
        glBindRenderbuffer ( GL_RENDERBUFFER, id );
        bound_rbo = id;
    }
}

/* unbind_rbo
 *
 * unbind an renderbuffer object, if already bound
 */
void glh::core::object_manager::unbind_rbo ( const GLuint id )
{
    /* throw if invalid object */
    if ( id == 0 ) throw exception::object_management_exception { "attempted unbind operation on invalid renderbuffer object" };    

    /* if already bound, unbind and record */
    if ( id == bound_rbo )
    {
        glBindRenderbuffer ( GL_RENDERBUFFER, 0 );
        bound_rbo = 0;
    } 
}



/* FRAMEBUFFER OBJECTS */

/* generate_fbo
 *
 * generate a framebuffer object
 */
GLuint glh::core::object_manager::generate_fbo ()
{
    /* assert opengl is loaded */
    asset_opengl_loaded ();

    /* generate and return framebuffer */
    GLuint id;
    glGenFramebuffers ( 1, &id );
    return id;
}

/* destroy_fbo
 *
 * destroy a framebuffer object, unbinding it if bound
 */
void glh::core::object_manager::destroy_fbo ( const GLuint id )
{
    /* if not zero, remove bind entry if bound and then destroy */
    if ( id > 0 )
    {
        if ( id == bound_fbo ) bound_fbo = 0;
        glDeleteFramebuffers ( 1, &id );
    }
}

/* bind_fbo
 *
 * bind a framebuffer object
 */
void glh::core::object_manager::bind_fbo ( const GLuint id )
{
    /* throw if invalid object */
    if ( id == 0 ) throw exception::object_management_exception { "attempted bind operation on invalid framebuffer object" };    

    /* if not already bound, bind and record */
    if ( id != bound_fbo )
    {
        glBindFramebuffer ( GL_FRAMEBUFFER, id );
        bound_fbo = id;
    }
}

/* unbind_fbo
 *
 * if the fbo is bound, bind the default fbo instead
 */
void glh::core::object_manager::unbind_fbo ( const GLuint id )
{
    /* throw if invalid object */
    if ( id == 0 ) throw exception::object_management_exception { "attempted unbind operation on invalid framebuffer object" };    

    /* if already bound, unbind and record */
    if ( id == bound_fbo )
    {
        glBindFramebuffer ( GL_FRAMEBUFFER, 0 );
        bound_fbo = 0;
    } 
}

/* bind_default_framebuffer
 *
 * bind the default framebuffer
 */
void glh::core::object_manager::bind_default_fbo ()
{
    /* if not already default, set to default */
    if ( 0 != bound_fbo )
    {
        glBindFramebuffer ( GL_FRAMEBUFFER, 0 );
        bound_fbo = 0;
    }
}



/* STATIC MEMBERS DEFINITIONS */

/* currently bound vbo */
GLuint glh::core::object_manager::bound_vbo { 0 };

/* currently bound ebo */
GLuint glh::core::object_manager::bound_ebo { 0 };

/* currently bound vao */
GLuint glh::core::object_manager::bound_vao { 0 };

/* currently in-use program */
GLuint glh::core::object_manager::in_use_program { 0 };

/* array of texture units and their respectively bound textures */
std::array<GLuint, GLH_MAX_TEXTURE_UNITS> glh::core::object_manager::bound_texture2ds { 0 };

/* array of texture units and their respectively bound cubemaps */
std::array<GLuint, GLH_MAX_TEXTURE_UNITS> glh::core::object_manager::bound_cubemaps { 0 };

/* currently bound renderbuffer */
GLuint glh::core::object_manager::bound_rbo { 0 };

/* currently bound framebuffer */
GLuint glh::core::object_manager::bound_fbo { 0 };