/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * src/glhelper/glhelper_render.cpp
 * 
 * implementation of include/glhelper/glhelper_render.hpp
 * 
 */



/* INCLUDES */

/* include glhelper_render.hpp */
#include <glhelper/glhelper_render.hpp>



/* STATIC_RENDERABLE IMPLEMENTATION */

/* prepare
 *
 * perform all binds etc. ready for rendering
 */
void glh::static_renderable::prepare ()
{
    /* make the window current */
    _window.make_current ();

    /* bind the vao and use the program */
    _vao.bind ();
    _program.use ();

    /* loop through the textures and bind them all */
    for ( auto tex: _texture2d ) tex->bind ();
}

/* unprepare
 *
 * perform all unbinds etc.
 */
void glh::static_renderable::unprepare ()
{
    /* unbind the vao */
    _vao.unbind ();

    /* loop through the textures and unbind them all */
    for ( auto tex: _texture2d ) tex->unbind ();
}