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

/* full constructor
 *
 * _id: the id of the object
 */
glh::core::object::object ( const minor_object_type _minor_type )
    : id { om::generate_object ( _minor_type ) }
    , minor_type { _minor_type }
    , major_type { om::to_major_object_type ( _minor_type ) }
    , bind_target { om::to_object_bind_target ( _minor_type ) }
    , gl_target { om::to_opengl_bind_target ( bind_target ) }
{}

/* bind/unbind
 *
 * bind/unbind the object
 */
void glh::core::object::bind () const { om::bind_object ( id, bind_target ); }
void glh::core::object::unbind () const { om::unbind_object ( id, bind_target ); }

/* is_bound
 *
 * returns true if the object is bound
 */
bool glh::core::object::is_bound () const { return om::is_object_bound ( id, bind_target ); }

/* destroy
 *
 * destroys the object, setting its id to 0
 */
void glh::core::object::destroy () { om::destroy_object ( id, minor_type ); id = 0; }



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