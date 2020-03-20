#pragma once
#include <stdafx.h>
#include <script_game_object.h>

// This file contains declarations for all methods of the python GameObject class.

namespace OgreEngine::script
{
	/// Called internally by the python interpreter when a python GO ref-count reaches 0
	void GameObject_dealloc(OgreEngine::script::GameObject* self);

	/// The __init__ function for python GameObjects.  Returns 0 on success.  Can be called more than
	/// once to re-initialize the game object.  Currently doesn't do much because all methods of the 
	/// python game object call c++ methods of the c++ game object to do their work.
	int GameObject_init(OgreEngine::script::GameObject* self, PyObject* args, PyObject* kwds);

	/// Called when python needs to allocate memory for a new python game object.  Currently doesn't 
	/// do much, for the same reason as GameObject_init
	PyObject* GameObject_new(PyTypeObject* type, PyObject* args, PyObject* kwds);

	PyObject* create_mesh_component(PyObject* self, PyObject* args);
}
