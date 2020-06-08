/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * src/glhelper/glhelper_core.cpp
 * 
 * implementation of include/glhelper/glhelper_core.hpp
 * 
 */



/* INCLUDES */

/* include glhelper_core.hpp */
#include <glhelper/glhelper_core.hpp>



/* OBJECT DEFINITION */

/* new object constructor
 *
 * construct a new object of the type supplied
 * 
 *  type: the type of the object to generate (minor type)
 */
glh::core::object::object ( const minor_object_type type )
    : id { 0 }
    , unique_id { next_unique_id++ }
    , minor_type { type }
    , major_type { to_major_object_type ( type ) }
    , bind_target { to_object_bind_target ( type ) }
    , opengl_bind_target { to_opengl_bind_target ( bind_target ) }
{
    /* switch on type to generate object and store in id */
    switch ( minor_type )
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
    case minor_object_type::GLH_TEXTURE2D_MULTISAMPLE_TYPE: glGenTextures ( 1, &id ); break;

    default: throw exception::object_exception { "attempted to perform generation operation on unknown object type" };
    }

    /* successfully generated object, so add to object_pointers */

    /* resize the vector for object ids for this type if necessary */
    if ( object_pointers.at ( static_cast<unsigned> ( major_type ) ).size () <= id ) object_pointers.at ( static_cast<unsigned> ( major_type ) ).resize ( id + 1, NULL );

    /* set the pointer to object_pointers */
    object_pointers.at ( static_cast<unsigned> ( major_type ) ).at ( id ) = this;
}

/* move constructor */
glh::core::object::object ( object&& other )
    : id { other.id }
    , unique_id { other.unique_id }
    , minor_type { other.minor_type }
    , major_type { other.major_type }
    , bind_target { other.bind_target }
    , opengl_bind_target { other.opengl_bind_target }
{
    /* set new pointer to object_pointers */
    if ( is_object_valid () ) object_pointers.at ( static_cast<unsigned> ( major_type ) ).at ( id ) = this;

    /* set id of other object to 0 */
    other.id = 0;
}


/* virtual destructor
 *
 * virtual in preparation for polymorphism
 */
glh::core::object::~object ()
{
    /* if not valid, return */
    if ( !is_object_valid () ) return;
    
    /* unbind from all bind points */
    unbind_all ();

    /* switch on type to destroy object */
    switch ( minor_type )
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
    case minor_object_type::GLH_TEXTURE2D_MULTISAMPLE_TYPE: glDeleteTextures ( 1, &id ); break;

    default: break;
        /* throw exception::object_exception { "attempted to perform destroy operation on unknown object type" };
         * it would be nice to throw, but t'is a destructor
         */
    }

    /* set pointer to NULL */
    object_pointers.at ( static_cast<unsigned> ( major_type ) ).at ( id ) = NULL;

    /* set id to 0 */
    id = 0;
}




/* bind/unbind to a target
 *
 * bind/unbind to a given target
 * binding/unbinding is silently ignored if object is already bound/not bound
 * 
 * returns true if a change in binding occured
 */
bool glh::core::object::bind ( const object_bind_target& target ) const
{
    /* if invalid, throw */
    if ( !is_object_valid () ) throw exception::object_exception { "attempted to bind invalid object" };

    /* get index of target */
    const unsigned target_index = static_cast<unsigned> ( target );

    /* get OpenGL enum for target */
    const GLenum opengl_target = ( target == bind_target ? opengl_bind_target : to_opengl_bind_target ( target ) );

    /* if already bound, return false */
    if ( object_bindings.at ( target_index ) == id ) return false;

    /* switch on target and bind to bind point */
    switch ( target )
    {
    case object_bind_target::GLH_VBO_TARGET: glBindBuffer ( opengl_target, id ); break;
    case object_bind_target::GLH_EBO_TARGET: glBindBuffer ( opengl_target, id ); break;
    case object_bind_target::GLH_UBO_TARGET: glBindBuffer ( opengl_target, id ); break;
    case object_bind_target::GLH_COPY_READ_BUFFER_TARGET: glBindBuffer ( opengl_target, id ); break;
    case object_bind_target::GLH_COPY_WRITE_BUFFER_TARGET: glBindBuffer ( opengl_target, id ); break;
    case object_bind_target::GLH_VAO_TARGET: glBindVertexArray ( id ); break;

    case object_bind_target::GLH_RBO_TARGET: glBindRenderbuffer ( opengl_target, id ); break;
    case object_bind_target::GLH_FBO_TARGET: glBindFramebuffer ( opengl_target, id ); break;
    case object_bind_target::GLH_READ_FBO_TARGET: glBindFramebuffer ( opengl_target, id ); break;
    case object_bind_target::GLH_DRAW_FBO_TARGET: glBindFramebuffer ( opengl_target, id ); break;

    case object_bind_target::GLH_PROGRAM_TARGET: glUseProgram ( id ); break;

    case object_bind_target::GLH_UNKNOWN_TARGET: break;

    default:
        if ( is_texture2d_bind_target ( target ) ) 
        {
            const unsigned unit = target_index - static_cast<unsigned> ( object_bind_target::GLH_TEXTURE2D_0_TARGET );
            glActiveTexture ( GL_TEXTURE0 + unit );
            glBindTexture ( opengl_target, id );
        } else
        if ( is_cubemap_bind_target ( target ) )
        {
            const unsigned unit = target_index - static_cast<unsigned> ( object_bind_target::GLH_CUBEMAP_0_TARGET );
            glActiveTexture ( GL_TEXTURE0 + unit );
            glBindTexture ( opengl_target, id );
        } else
        if ( is_texture2d_multisample_bind_target ( target ) )
        {
            const unsigned unit = target_index - static_cast<unsigned> ( object_bind_target::GLH_TEXTURE2D_MULTISAMPLE_0_TARGET );
            glActiveTexture ( GL_TEXTURE0 + unit );
            glBindTexture ( opengl_target, id );
        } else throw exception::object_exception { "attempted to bind unknown bind target" };

    }

    /* record the binding */
    object_bindings.at ( target_index ) = id;

    /* return true */
    return true;
}
bool glh::core::object::unbind ( const object_bind_target& target ) const
{
    /* if invalid, throw */
    if ( !is_object_valid () ) throw exception::object_exception { "attempted to unbind invalid object" };

    /* get index of target */
    const unsigned target_index = static_cast<unsigned> ( target );

    /* get OpenGL enum for target */
    const GLenum opengl_target = ( target == bind_target ? opengl_bind_target : to_opengl_bind_target ( target ) );

    /* if not bound, return false */
    if ( object_bindings.at ( target_index ) != id ) return false;

    /* switch on target and unbind from bind point */
    switch ( target )
    {
    case object_bind_target::GLH_VBO_TARGET: glBindBuffer ( opengl_target, 0 ); break;
    case object_bind_target::GLH_EBO_TARGET: glBindBuffer ( opengl_target, 0 ); break;
    case object_bind_target::GLH_UBO_TARGET: glBindBuffer ( opengl_target, 0 ); break;
    case object_bind_target::GLH_COPY_READ_BUFFER_TARGET: glBindBuffer ( opengl_target, 0 ); break;
    case object_bind_target::GLH_COPY_WRITE_BUFFER_TARGET: glBindBuffer ( opengl_target, 0 ); break;
    case object_bind_target::GLH_VAO_TARGET: glBindVertexArray ( 0 ); break;

    case object_bind_target::GLH_RBO_TARGET: glBindRenderbuffer ( opengl_target, 0 ); break;
    case object_bind_target::GLH_FBO_TARGET: glBindFramebuffer ( opengl_target, 0 ); break;
    case object_bind_target::GLH_READ_FBO_TARGET: glBindFramebuffer ( opengl_target, 0 ); break;
    case object_bind_target::GLH_DRAW_FBO_TARGET: glBindFramebuffer ( opengl_target, 0 ); break;

    case object_bind_target::GLH_PROGRAM_TARGET: glUseProgram ( id ); break;

    case object_bind_target::GLH_UNKNOWN_TARGET: break;

    default:
        if ( is_texture2d_bind_target ( target ) ) 
        {
            const unsigned unit = target_index - static_cast<unsigned> ( object_bind_target::GLH_TEXTURE2D_0_TARGET );
            glActiveTexture ( GL_TEXTURE0 + unit );
            glBindTexture ( opengl_target, 0 );
        } else
        if ( is_cubemap_bind_target ( target ) )
        {
            const unsigned unit = target_index - static_cast<unsigned> ( object_bind_target::GLH_CUBEMAP_0_TARGET );
            glActiveTexture ( GL_TEXTURE0 + unit );
            glBindTexture ( opengl_target, 0 );
        } else
        if ( is_texture2d_multisample_bind_target ( target ) )
        {
            const unsigned unit = target_index - static_cast<unsigned> ( object_bind_target::GLH_TEXTURE2D_MULTISAMPLE_0_TARGET );
            glActiveTexture ( GL_TEXTURE0 + unit );
            glBindTexture ( opengl_target, 0 );
        } else throw exception::object_exception { "attempted to unbind unknown bind target" };

    }

    /* record the unbinding */
    object_bindings.at ( target_index ) = 0;

    /* return true */
    return true;
}



/* bind/unbind unit version
 *
 * bind/unbind the object using a unit integer
 * the base class method will always throw
 * a derived class may wish to override this method, such that unit bindings become possible
 */
bool glh::core::object::bind ( const unsigned unit ) const
{
    /* throw */
    throw exception::object_exception { "attempted to bind object to a unit, without unit-binding capabilities being defined" };
}
bool glh::core::object::unbind ( const unsigned unit ) const
{
    /* throw */
    throw exception::object_exception { "attempted to unbind object from a unit, without unit-binding capabilities being defined" };
}



/* is_bound to a target
 *
 * returns true if the object is bound to the target supplied
*/
bool glh::core::object::is_bound ( const object_bind_target& target ) const
{
    /* return true if the object is valid and bound */
    return ( is_object_valid () && object_bindings.at ( static_cast<unsigned> ( target ) ) == id );
}



/* force_unbind
 *
 * force the unbinding of any object bound to a bind point
 *
 * returns true if an object was unbound
 */
bool glh::core::object::force_unbind ( const object_bind_target& target )
{
    /* get bound object */
    const auto bound_object = get_bound_object_pointer ( target );

    /* if is not invalid, unbind it */
    if ( bound_object ) bound_object->unbind ();

    /* return truth of bound_object */
    return bound_object;
}



/* create_object_pointers
 *
 * create object pointers array 
 */
std::array<std::vector<glh::core::object *>, static_cast<unsigned> ( glh::core::major_object_type::__COUNT__ )> glh::core::object::create_object_pointers ()
{
    /* initialise to empty vectors */
    std::array<std::vector<object *>, static_cast<unsigned> ( glh::core::major_object_type::__COUNT__ )> _object_pointers;
    _object_pointers.fill ( {} );
    return _object_pointers;
}

/* create_object_bindings
 *
 * create object bindings array
 */
std::array<GLuint, static_cast<unsigned> ( glh::core::object_bind_target::__COUNT__ )> glh::core::object::create_object_bindings ()
{
    /* initialise to 0s */
    std::array<GLuint, static_cast<unsigned> ( glh::core::object_bind_target::__COUNT__ )> _object_bindings;
    _object_bindings.fill ( 0 );
    return _object_bindings;
}



/* to_major_object_type
 * to_object_bind_target
 * to_opengl_bind_target
 *
 * take a minor object type and convert it to its major, gen or bind object type
 */
glh::core::major_object_type glh::core::object::to_major_object_type ( const minor_object_type type )
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
    case minor_object_type::GLH_TEXTURE2D_MULTISAMPLE_TYPE: return major_object_type::GLH_TEXTURE_TYPE;

    case minor_object_type::GLH_UNKNOWN_TYPE: return major_object_type::GLH_UNKNOWN_TYPE;
    
    default: throw exception::object_exception { "attempted to perform minor to major type conversion on unknown object type" };
    }
}
glh::core::object_bind_target glh::core::object::to_object_bind_target ( const minor_object_type type )
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

    case minor_object_type::GLH_VSHADER_TYPE: return object_bind_target::GLH_UNKNOWN_TARGET;
    case minor_object_type::GLH_GSHADER_TYPE: return object_bind_target::GLH_UNKNOWN_TARGET;
    case minor_object_type::GLH_FSHADER_TYPE: return object_bind_target::GLH_UNKNOWN_TARGET;
    case minor_object_type::GLH_PROGRAM_TYPE: return object_bind_target::GLH_PROGRAM_TARGET;

    case minor_object_type::GLH_TEXTURE2D_TYPE: return object_bind_target::GLH_TEXTURE2D_0_TARGET;
    case minor_object_type::GLH_CUBEMAP_TYPE: return object_bind_target::GLH_CUBEMAP_0_TARGET;
    case minor_object_type::GLH_TEXTURE2D_MULTISAMPLE_TYPE: return object_bind_target::GLH_TEXTURE2D_MULTISAMPLE_0_TARGET;

    case minor_object_type::GLH_UNKNOWN_TYPE: return object_bind_target::GLH_UNKNOWN_TARGET;
    
    default: throw exception::object_exception { "attempted to perform minor to bind target conversion on unknown object type" };
    }
}
GLenum glh::core::object::to_opengl_bind_target ( const object_bind_target target )
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
    case object_bind_target::GLH_READ_FBO_TARGET: return GL_READ_FRAMEBUFFER;
    case object_bind_target::GLH_DRAW_FBO_TARGET: return GL_DRAW_FRAMEBUFFER;

    case object_bind_target::GLH_PROGRAM_TARGET: return GL_NONE;

    case object_bind_target::GLH_UNKNOWN_TARGET: return GL_NONE;

    default:
        if ( is_texture2d_bind_target ( target ) ) return GL_TEXTURE_2D; else
        if ( is_cubemap_bind_target ( target ) ) return GL_TEXTURE_CUBE_MAP; else
        if ( is_texture2d_multisample_bind_target ( target ) ) return GL_TEXTURE_2D_MULTISAMPLE; else
        throw exception::object_exception { "attempted to perform glh to opengl bind target conversion on unknown target" };
    }
}
glh::core::major_object_type glh::core::object::to_major_object_type ( const object_bind_target target )
{
    /* switch on target and bind to default bind point */
    switch ( target )
    {
    case object_bind_target::GLH_VBO_TARGET: return major_object_type::GLH_BUFFER_TYPE;
    case object_bind_target::GLH_EBO_TARGET: return major_object_type::GLH_BUFFER_TYPE;
    case object_bind_target::GLH_UBO_TARGET: return major_object_type::GLH_BUFFER_TYPE;
    case object_bind_target::GLH_COPY_READ_BUFFER_TARGET: return major_object_type::GLH_BUFFER_TYPE;
    case object_bind_target::GLH_COPY_WRITE_BUFFER_TARGET: return major_object_type::GLH_BUFFER_TYPE;
    case object_bind_target::GLH_VAO_TARGET: return major_object_type::GLH_VAO_TYPE;

    case object_bind_target::GLH_RBO_TARGET: return major_object_type::GLH_RBO_TYPE;
    case object_bind_target::GLH_FBO_TARGET: return major_object_type::GLH_FBO_TYPE;
    case object_bind_target::GLH_READ_FBO_TARGET: return major_object_type::GLH_FBO_TYPE;
    case object_bind_target::GLH_DRAW_FBO_TARGET: return major_object_type::GLH_FBO_TYPE;

    case object_bind_target::GLH_PROGRAM_TARGET: return major_object_type::GLH_PROGRAM_TYPE;

    case object_bind_target::GLH_UNKNOWN_TARGET: return major_object_type::GLH_UNKNOWN_TYPE;

    default:
        if ( is_texture2d_bind_target ( target ) ) return major_object_type::GLH_TEXTURE_TYPE; else
        if ( is_cubemap_bind_target ( target ) ) return major_object_type::GLH_TEXTURE_TYPE; else
        if ( is_texture2d_multisample_bind_target ( target ) ) return major_object_type::GLH_TEXTURE_TYPE; else
        throw exception::object_exception { "attempted to perform glh to major object type conversion on unknown target" };
    }
}



/* is_texture2d_bind_target
 * is_cubemap_bind_target
 * 
 * returns true if the target supplied is one of the above bind targets
 */
bool glh::core::object::is_texture2d_bind_target ( const object_bind_target target )
{
    /* if the target is between __TEXTURE2D_START__ and __TEXTURE2D_END__ return true, else false */
    return ( target > object_bind_target::__TEXTURE2D_START__ && target < object_bind_target::__TEXTURE2D_END__ );
}
bool glh::core::object::is_cubemap_bind_target ( const object_bind_target target )
{
    /* if the target is between __CUBEMAP_START__ and __CUBEMAP_END__ return true, else false */
    return ( target >= object_bind_target::__CUBEMAP_START__ && target <= object_bind_target::__CUBEMAP_END__ );
}
bool glh::core::object::is_texture2d_multisample_bind_target ( const object_bind_target target )
{
    /* if the target is between __TEXTURE2D_MULTISAMPLE_START__ and __TEXTURE2D_MULTISAMPLE_END__ return true, else false */
    return ( target >= object_bind_target::__TEXTURE2D_MULTISAMPLE_START__ && target <= object_bind_target::__TEXTURE2D_MULTISAMPLE_END__ );
}



/* STATIC MEMBERS OF OBJECT DEFINITIONS */

/* next_unique_id is zero at start */
GLuint glh::core::object::next_unique_id { 0 };

/* number of existing object ids per major type */
std::array<std::vector<glh::core::object *>, static_cast<unsigned> ( glh::core::major_object_type::__COUNT__ )> glh::core::object::object_pointers
{ create_object_pointers () };

/* object bindings per bind target */
std::array<GLuint, static_cast<unsigned> ( glh::core::object_bind_target::__COUNT__ )> glh::core::object::object_bindings
{ create_object_bindings () };



/* NON-MEMBER FUNCTION IMPLEMENTATIONS */

/* comparison operators
 *
 * determines if two objects are equal by comparing ids
 * 
 * return: boolean representing equality
 */
bool operator== ( const glh::core::object& lhs, const glh::core::object& rhs ) 
{ 
    return ( lhs.get_minor_object_type () == rhs.get_minor_object_type () && lhs.internal_id () == rhs.internal_id () );
}
bool operator!= ( const glh::core::object& lhs, const glh::core::object& rhs ) 
{
     return ( lhs.get_minor_object_type () == rhs.get_minor_object_type () && lhs.internal_id () != rhs.internal_id () );
}