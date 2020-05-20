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

	/// Retrieves the calling objects tag
	PyObject* get_tag(PyObject* self, PyObject* args);

	/// Retrieves this objects parent if it has one
	PyObject* get_parent(PyObject* self, PyObject* args);

	/// Get the associated gameobjects name
	PyObject* get_name(PyObject* self, PyObject* args);

	/// Gets the point relative to this objects world position and direction
	PyObject* get_relative_point(PyObject* self, PyObject* args);

	/// Gets the world position of the calling object
	PyObject* get_world_position(PyObject* self, PyObject* args);

	/// Sets the world position of the calling object
	PyObject* set_world_position(PyObject* self, PyObject* args);

	/// Creates a mesh component object and attaches it to the associated gameobject
	PyObject* create_mesh_component(PyObject* self, PyObject* args);

	/// Creates a collider component object and attaches it to the associated gameobject
	PyObject* create_collider_component(PyObject* self, PyObject* args);

	/// Creates a mesh component visualization and sets it visible
	PyObject* create_collider_visualization(PyObject* self, PyObject* args);

	/// Rotates the associated gameobject around the world axis
	PyObject* rotate_local(PyObject* self, PyObject* args);

	/// Rotates the associated gameobject around the world axis
	PyObject* rotate_world(PyObject* self, PyObject* args);

	/// Scales the associated gameobject
	PyObject* scale(PyObject* self, PyObject* args);

	/// Translates the associated gameobject along the objects local axis
	PyObject* translate_local(PyObject* self, PyObject* args);

	/// Translates the associated gameobject along the world axis
	PyObject* translate_world(PyObject* self, PyObject* args);
}
