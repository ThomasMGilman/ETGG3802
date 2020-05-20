#pragma once
#include <stdafx.h>
#include <game_object.h>

// This file contains the GameObject structure used when python instantiates
// a game object.  The only attribute stored by the structure is a pointer
// to the corresponding c++ game object.  The create_game_object top-level
// function should be used to make instances of the python game-object class (it
// will set the corresponding pointers)

namespace ssuge
{
    namespace script
    {
        typedef struct 
        {
            PyObject_HEAD
            ssuge::GameObject* mTwin;
        } GameObject;
    }
}

