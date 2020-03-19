/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * test.cpp
 * 
 * test parts of the library
 * 
 */



/* INCLUDES */

/* include core headers */
#include <iostream>
#include <chrono>
#include <thread>

/* include glhelper.hpp */
#include <glhelper/glhelper.hpp>



/* MAIN */

int main ()
{
    glh::window win1
    { 
        []()
        {
            glh::window win2 { "Test window 1", 600, 500 };
            glh::window win3 { "Test window 2", 600, 500 };
            return win3;
        } ()
    };

    unsigned vdata []
    {
        0.0f, 0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f
    };
    
    glh::vbo vbo;
    vbo.buffer_data ( sizeof ( vdata ), vdata, GL_STATIC_DRAW );


    std::this_thread::sleep_for ( std::chrono::seconds ( 5 ) );

    return 0;
}