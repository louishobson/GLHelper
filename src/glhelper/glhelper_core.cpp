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
 * _id: the id of the new object (defaults to 0)
 */
glh::core::object::object ( const unsigned _id )
    : id { _id }
    , unique_id { next_unique_id++ }
{
    /* add object pointer to map */
    object_pointers.insert ( std::make_pair ( unique_id, this ) );
}

/* move constructor */
glh::core::object::object ( object&& other )
    : id { other.id }
    , unique_id { other.unique_id }
{
    /* if id is not 0, update object pointers */
    if ( id != 0 )
    {
       object_pointers.at ( unique_id ) = this;
       other.id = 0;
    }
}

/* virtual destructor */
glh::core::object::~object ()
{
    /* remove object pointer from map, if id does not already equal 0 */
    if ( id != 0 )
    {
        object_pointers.erase ( unique_id );
        id = 0;
    }
}



/* STATIC MEMBERS OF OBJECT DEFINITIONS */

/* next_unique_id is one at start */
GLuint glh::core::object::next_unique_id { 1 };

/* object_pointers
 *
 * map between unique ids and pointers to their objects
 */
std::map<unsigned, glh::core::object *> glh::core::object::object_pointers {};