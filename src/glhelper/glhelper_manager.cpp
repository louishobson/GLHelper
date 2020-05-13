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



/* NON-MEMBER FUNCTIONS IMPLEMENTATIONS */

/* operators+- for bind target enum
 *
 * allows one to step through indexed bind points
 */
glh::core::object_bind_target operator+ ( const glh::core::object_bind_target target, const int scalar )
{
    /* using namespace glh::core to save some effort of namespace management */
    using namespace glh::core;

    /* get new target after adding scalar */
    const int new_target = static_cast<int> ( target ) + scalar;
    
    /* if target is a texture2d indices */
    if ( om::is_texture2d_object_bind_target ( target ) )
    {
        /* if is still a texture2d target, return the new target, otherwise throw */
        if ( new_target > static_cast<int> ( object_bind_target::__TEXTURE2D_START__ ) && new_target < static_cast<int> ( object_bind_target::__TEXTURE2D_END__ ) )
            return static_cast<object_bind_target> ( new_target );
        else throw glh::exception::object_management_exception { "scalar addition to object_bind_target cannot cause the target to no longer refer to a texture2d target" };
    } else

    /* if target is a cubemap indices */
    if ( om::is_cubemap_object_bind_target ( target ) )
    {
        /* if is still a cubemap target, return the new target, otherwise throw */
        if ( new_target > static_cast<int> ( object_bind_target::__CUBEMAP_START__ ) && new_target < static_cast<int> ( object_bind_target::__CUBEMAP_END__ ) )
            return static_cast<object_bind_target> ( new_target );
        else throw glh::exception::object_management_exception { "scalar addition to object_bind_target cannot cause the target to no longer refer to a cubemap target" };
    } else
    
    /* otherwise throw, as cannot add to target */
    throw glh::exception::object_management_exception { "scalar addition to object_bind_target is only valid for indexed targets" };
}
glh::core::object_bind_target operator- ( const glh::core::object_bind_target target, const int scalar ) { return ( target + -scalar ); }
int operator- ( const glh::core::object_bind_target target0, const glh::core::object_bind_target target1 )
{
    /* using namespace glh::core to save some effort of namespace management */
    using namespace glh::core;

    /* get difference */
    const int difference = static_cast<int> ( target0 ) - static_cast<int> ( target1 );

    /* if targets are both texture2d or cubemap indices, return difference, otherwise throw */
    if ( ( om::is_texture2d_object_bind_target ( target0 ) && om::is_texture2d_object_bind_target ( target1 ) ) || ( om::is_cubemap_object_bind_target ( target0 ) && om::is_cubemap_object_bind_target ( target1 ) ) ) 
        return difference;
    else throw glh::exception::object_management_exception { "subtraction between object_bind_target types is only valid if both targets are the same indexed bind target" };
}



/* OBJECT_MANAGER IMPLEMENTATION */

/* generate_object
 *
 * generate an object of a given type
 * 
 * type: the type of the object (minor type)
 * 
 * return: the id of the new object
 */
GLuint glh::core::object_manager::generate_object ( const minor_object_type type )
{
    /* variable to store the id */
    GLuint id = 0;

    /* switch on type to generate object and store in id */
    switch ( type )
    {
    case minor_object_type::GLH_VBO_TYPE: glGenBuffers ( 1, &id ); break;
    case minor_object_type::GLH_EBO_TYPE: glGenBuffers ( 1, &id ); break;
    case minor_object_type::GLH_UBO_TYPE: glGenBuffers ( 1, &id ); break;
    case minor_object_type::GLH_VAO_TYPE: glGenVertexArrays ( 1, &id ); break;

    case minor_object_type::GLH_RBO_TYPE: glGenRenderbuffers ( 1, &id ); break;
    case minor_object_type::GLH_FBO_TYPE: glGenFramebuffers ( 1, &id ); break;

    case minor_object_type::GLH_VSHADER_TYPE: id = glCreateShader ( GL_VERTEX_SHADER ); break;
    case minor_object_type::GLH_GSHADER_TYPE: id = glCreateShader ( GL_GEOMETRY_SHADER ); break;
    case minor_object_type::GLH_FSHADER_TYPE: id = glCreateShader ( GL_FRAGMENT_SHADER ); break;
    case minor_object_type::GLH_PROGRAM_TYPE: id = glCreateProgram (); break;

    case minor_object_type::GLH_TEXTURE2D_TYPE: glGenTextures ( 1, &id ); break;
    case minor_object_type::GLH_CUBEMAP_TYPE: glGenTextures ( 1, &id ); break;

    default: throw exception::object_management_exception { "attempted to perform generation operation on unknown object type" };
    }

    /* return id */
    return id;
}

/* destroy_object
 *
 * destroy an object of a given type
 * 
 * id: the id of the object to destroy
 * type: the type of the object (major type)
 */
void glh::core::object_manager::destroy_object ( const GLuint id, const minor_object_type type )
{
    /* unbind from all bind points */
    unbind_object_all ( id, type );

    /* switch on type to destroy object */
    switch ( type )
    {
    case minor_object_type::GLH_VBO_TYPE: glDeleteBuffers ( 1, &id ); break;
    case minor_object_type::GLH_EBO_TYPE: glDeleteBuffers ( 1, &id ); break;
    case minor_object_type::GLH_UBO_TYPE: glDeleteBuffers ( 1, &id ); break;
    case minor_object_type::GLH_VAO_TYPE: glDeleteVertexArrays ( 1, &id ); break;

    case minor_object_type::GLH_RBO_TYPE: glDeleteRenderbuffers ( 1, &id ); break;
    case minor_object_type::GLH_FBO_TYPE: glDeleteFramebuffers ( 1, &id ); break;

    case minor_object_type::GLH_VSHADER_TYPE: glDeleteShader ( id ); break;
    case minor_object_type::GLH_GSHADER_TYPE: glDeleteShader ( id ); break;
    case minor_object_type::GLH_FSHADER_TYPE: glDeleteShader ( id ); break;
    case minor_object_type::GLH_PROGRAM_TYPE: glDeleteProgram ( id ); break;

    case minor_object_type::GLH_TEXTURE2D_TYPE: glDeleteTextures ( 1, &id ); break;
    case minor_object_type::GLH_CUBEMAP_TYPE: glDeleteTextures ( 1, &id ); break;

    default: throw exception::object_management_exception { "attempted to perform destroy operation on unknown object type" };
    }
}

/* unbind_object_all
 *
 * unbinds an object from all possible bind points
 * includes all indexed bindings, where applicable
 * 
 * id: the id of the object to unbind
 * type: the type of the object (minor type)
 */
void glh::core::object_manager::unbind_object_all ( const GLuint id, const minor_object_type type )
{
    /* get the bind target */
    const object_bind_target target = to_object_bind_target ( type );

    /* if target is a texture bind point, ensure is unbound from all units, otherwise just unbind */
    if ( target == object_bind_target::GLH_TEXTURE2D_0_TARGET || target == object_bind_target::GLH_CUBEMAP_0_TARGET )
    {
        for ( unsigned i = 0; i < 32; ++i ) unbind_object ( id, target + i );
    } else unbind_object ( id, target );
}

/* bind_object
 *
 * bind an object to a target
 * if already bound, it will not be bound twice
 * 
 * id: the id of the object
 * type: the bind target of the object (bind target)
 */
void glh::core::object_manager::bind_object ( const GLuint id, const object_bind_target target )
{   
    /* if already bound, return */
    if ( object_bindings.at ( static_cast<unsigned> ( target ) ) == id ) return;

    /* otherwise record the binding */
    object_bindings.at ( static_cast<unsigned> ( target ) ) = id;

    /* switch on target and bind to default bind point */
    switch ( target )
    {
    case object_bind_target::GLH_VBO_TARGET: glBindBuffer ( GL_ARRAY_BUFFER, id ); break;
    case object_bind_target::GLH_EBO_TARGET: glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, id ); break;
    case object_bind_target::GLH_UBO_TARGET: glBindBuffer ( GL_UNIFORM_BUFFER, id ); break;
    case object_bind_target::GLH_COPY_READ_BUFFER_TARGET: glBindBuffer ( GL_COPY_READ_BUFFER, id ); break;
    case object_bind_target::GLH_COPY_WRITE_BUFFER_TARGET: glBindBuffer ( GL_COPY_WRITE_BUFFER, id ); break;
    case object_bind_target::GLH_VAO_TARGET: glBindVertexArray ( id ); break;

    case object_bind_target::GLH_RBO_TARGET: glBindRenderbuffer ( GL_RENDERBUFFER, id ); break;
    case object_bind_target::GLH_FBO_TARGET: glBindFramebuffer ( GL_FRAMEBUFFER, id ); break;

    case object_bind_target::GLH_PROGRAM_TARGET: glUseProgram ( id ); break;

    case object_bind_target::GLH_NO_TARGET: break;

    default:
        if ( is_texture2d_object_bind_target ( target ) )
        {
            glActiveTexture ( GL_TEXTURE0 + ( target - object_bind_target::GLH_TEXTURE2D_0_TARGET ) );
            glBindTexture ( GL_TEXTURE_2D, id );
        } else
        if ( is_cubemap_object_bind_target ( target ) )
        {
            glActiveTexture ( GL_TEXTURE0 + ( target - object_bind_target::GLH_CUBEMAP_0_TARGET ) );
            glBindTexture ( GL_TEXTURE_CUBE_MAP, id );
        } else
        throw exception::object_management_exception { "attempted to perform bind operation to unknown target" };
    }
}

/* unbind_object
 *
 * unbind an object from a target
 * if not bound, the function returns having done nothing
 * 
 * id: the id of the object
 * target: the bind target of the object (bind target)
 */
void glh::core::object_manager::unbind_object ( const GLuint id, const object_bind_target target )
{
    /* if not bound, return */
    if ( object_bindings.at ( static_cast<unsigned> ( target ) ) != id ) return;

    /* otherwise record the unbinding */
    object_bindings.at ( static_cast<unsigned> ( target ) ) = 0;

    /* switch on target and bind to default bind point */
    switch ( target )
    {
    case object_bind_target::GLH_VBO_TARGET: glBindBuffer ( GL_ARRAY_BUFFER, 0 ); break;
    case object_bind_target::GLH_EBO_TARGET: glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, 0 ); break;
    case object_bind_target::GLH_UBO_TARGET: glBindBuffer ( GL_UNIFORM_BUFFER, 0 ); break;
    case object_bind_target::GLH_COPY_READ_BUFFER_TARGET: glBindBuffer ( GL_COPY_READ_BUFFER, 0 ); break;
    case object_bind_target::GLH_COPY_WRITE_BUFFER_TARGET: glBindBuffer ( GL_COPY_WRITE_BUFFER, 0 ); break;
    case object_bind_target::GLH_VAO_TARGET: glBindVertexArray ( 0 ); break;

    case object_bind_target::GLH_RBO_TARGET: glBindRenderbuffer ( GL_RENDERBUFFER, 0 ); break;
    case object_bind_target::GLH_FBO_TARGET: glBindFramebuffer ( GL_FRAMEBUFFER, 0 ); break;

    case object_bind_target::GLH_PROGRAM_TARGET: glUseProgram ( 0 ); break;

    case object_bind_target::GLH_NO_TARGET: break;

    default:
        if ( is_texture2d_object_bind_target ( target ) )
        {
            glActiveTexture ( GL_TEXTURE0 + ( target - object_bind_target::GLH_TEXTURE2D_0_TARGET ) );
            glBindTexture ( GL_TEXTURE_2D, 0 );
        } else
        if ( is_cubemap_object_bind_target ( target ) )
        {
            glActiveTexture ( GL_TEXTURE0 + ( target - object_bind_target::GLH_CUBEMAP_0_TARGET ) );
            glBindTexture ( GL_TEXTURE_CUBE_MAP, 0 );
        } else
        throw exception::object_management_exception { "attempted to perform unbind operation to unknown target" };
    }
}

/* get_bound_object
 *
 * get the object thats bound to a target
 *
 * target: the bind target of the object (bind target)
 *
 * return: the id of the bound object
 */
GLuint glh::core::object_manager::get_bound_object ( const object_bind_target target )
{   
    /* return object from bind point */
    return object_bindings.at ( static_cast<unsigned> ( target ) );
}

/* is_object_bound
 *
 * return true if an object is bound to its target
 * 
 * id: the id of the object to check
 * target: the bind target of the object (bind target)
 *
 * return: true if bound, false if not
 */
bool glh::core::object_manager::is_object_bound ( const GLuint id, const object_bind_target target )
{
    /* return comparison between supplied id and bound id */
    return ( object_bindings.at ( static_cast<unsigned> ( target ) ) == id );
}

/* to_major/gen/object_bind_target
 *
 * take a minor object type and convert it to its major, gen or bind object type
 */
glh::core::major_object_type glh::core::object_manager::to_major_object_type ( const minor_object_type type )
{
    /* switch for different object types */
    switch ( type )
    {
    case minor_object_type::GLH_VBO_TYPE: return major_object_type::GLH_BUFFER_TYPE;
    case minor_object_type::GLH_EBO_TYPE: return major_object_type::GLH_BUFFER_TYPE;
    case minor_object_type::GLH_UBO_TYPE: return major_object_type::GLH_BUFFER_TYPE;
    case minor_object_type::GLH_VAO_TYPE: return major_object_type::GLH_VAO_TYPE;
    
    case minor_object_type::GLH_RBO_TYPE: return major_object_type::GLH_RBO_TYPE;
    case minor_object_type::GLH_FBO_TYPE: return major_object_type::GLH_FBO_TYPE;

    case minor_object_type::GLH_VSHADER_TYPE: return major_object_type::GLH_SHADER_TYPE;
    case minor_object_type::GLH_GSHADER_TYPE: return major_object_type::GLH_SHADER_TYPE;
    case minor_object_type::GLH_FSHADER_TYPE: return major_object_type::GLH_SHADER_TYPE;
    case minor_object_type::GLH_PROGRAM_TYPE: return major_object_type::GLH_PROGRAM_TYPE;

    case minor_object_type::GLH_TEXTURE2D_TYPE: return major_object_type::GLH_TEXTURE_TYPE;
    case minor_object_type::GLH_CUBEMAP_TYPE: return major_object_type::GLH_TEXTURE_TYPE;
    
    default: throw exception::object_management_exception { "attempted to perform minor to major type conversion on unknown object type" };
    }
}
glh::core::object_bind_target glh::core::object_manager::to_object_bind_target ( const minor_object_type type )
{
    /* switch for different object types */
    switch ( type )
    {
    case minor_object_type::GLH_VBO_TYPE: return object_bind_target::GLH_VBO_TARGET;
    case minor_object_type::GLH_EBO_TYPE: return object_bind_target::GLH_EBO_TARGET;
    case minor_object_type::GLH_UBO_TYPE: return object_bind_target::GLH_UBO_TARGET;
    case minor_object_type::GLH_VAO_TYPE: return object_bind_target::GLH_VAO_TARGET;
    
    case minor_object_type::GLH_RBO_TYPE: return object_bind_target::GLH_RBO_TARGET;
    case minor_object_type::GLH_FBO_TYPE: return object_bind_target::GLH_FBO_TARGET;

    case minor_object_type::GLH_VSHADER_TYPE: return object_bind_target::GLH_NO_TARGET;
    case minor_object_type::GLH_GSHADER_TYPE: return object_bind_target::GLH_NO_TARGET;
    case minor_object_type::GLH_FSHADER_TYPE: return object_bind_target::GLH_NO_TARGET;
    case minor_object_type::GLH_PROGRAM_TYPE: return object_bind_target::GLH_PROGRAM_TARGET;

    case minor_object_type::GLH_TEXTURE2D_TYPE: return object_bind_target::GLH_TEXTURE2D_0_TARGET;
    case minor_object_type::GLH_CUBEMAP_TYPE: return object_bind_target::GLH_CUBEMAP_0_TARGET;
    
    default: throw exception::object_management_exception { "attempted to perform minor to bind target conversion on unknown object type" };
    }
}
GLenum glh::core::object_manager::to_opengl_bind_target ( const object_bind_target target )
{
    /* switch on target and bind to default bind point */
    switch ( target )
    {
    case object_bind_target::GLH_VBO_TARGET: return GL_ARRAY_BUFFER;
    case object_bind_target::GLH_EBO_TARGET: return GL_ELEMENT_ARRAY_BUFFER;
    case object_bind_target::GLH_UBO_TARGET: return GL_UNIFORM_BUFFER;
    case object_bind_target::GLH_COPY_READ_BUFFER_TARGET: return GL_COPY_READ_BUFFER;
    case object_bind_target::GLH_COPY_WRITE_BUFFER_TARGET: return GL_COPY_WRITE_BUFFER;
    case object_bind_target::GLH_VAO_TARGET: return GL_NONE;

    case object_bind_target::GLH_RBO_TARGET: return GL_RENDERBUFFER;
    case object_bind_target::GLH_FBO_TARGET: return GL_FRAMEBUFFER;

    case object_bind_target::GLH_PROGRAM_TARGET: return GL_NONE;

    case object_bind_target::GLH_NO_TARGET: return GL_NONE;

    default:
        if ( is_texture2d_object_bind_target ( target ) ) return GL_TEXTURE_2D; else
        if ( is_cubemap_object_bind_target ( target ) ) return GL_TEXTURE_CUBE_MAP; else
        throw exception::object_management_exception { "attempted to perform glh to opengl bind target conversion on unknown target" };
    }
}

/* is_texture2d_object_bind_target
 * is_cubemap_object_bind_target
 * 
 * returns true if the target supplied is a texture/cubemap bind target
 */
bool glh::core::object_manager::is_texture2d_object_bind_target ( const object_bind_target target )
{
    /* if the target is between __TEXTURE2D_START__ and __TEXTURE2D_END__ return true, else false */
    return ( target > object_bind_target::__TEXTURE2D_START__ && target < object_bind_target::__TEXTURE2D_END__ );
}
bool glh::core::object_manager::is_cubemap_object_bind_target ( const object_bind_target target )
{
    /* if the target is between __CUBEMAP_START__ and __CUBEMAP_END__ return true, else false */
    return ( target >= object_bind_target::__CUBEMAP_START__ && target <= object_bind_target::__CUBEMAP_END__ );
}

/* assert_is_object_valid
 *
 * throws if an object has an id of 0
 *
 * id: the id of the object to test
 * operation: a description of the operation
 */
void glh::core::object_manager::assert_is_object_valid ( const GLuint id, const std::string& operation )
{
    /* throw if id == 0 */
    if ( id == 0 )
    {
        if ( operation.size () > 0 ) throw exception::object_management_exception { "attempted to perform " + operation + " operation on an invalid object" };
        else throw exception::object_management_exception { "attempted to perform operation on an invalid object" };
    }
}



/* STATIC MEMBERS DEFINITIONS */

/* object bindings */
std::array<GLuint, static_cast<unsigned> ( glh::core::object_bind_target::__COUNT__ )> glh::core::object_manager::object_bindings
{
    0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0
};