#pragma once
#include <stdafx.h>
#include <script_game_object.h>

// This file contains declarations for all methods of the python GameObject class.

namespace ssuge
{
	namespace script
	{
		//@@@@@@@@@@@ SPECIAL METHODS (referenced from GameObjectType) @@@@@@@@@@@@@
		/// Called internally by the python interpreter when a python GO ref-count reaches 0
		void GameObject_dealloc(ssuge::script::GameObject* self);

		/// The __init__ function for python GameObjects.  Returns 0 on success.  Can be called more than
		/// once to re-initialize the game object.  Currently doesn't do much because all methods of the 
		/// python game object call c++ methods of the c++ game object to do their work.
		int GameObject_init(ssuge::script::GameObject* self, PyObject* args, PyObject* kwds);

		/// Called when python needs to allocate memory for a new python game object.  Currently doesn't 
		/// do much, for the same reason as GameObject_init
		PyObject* GameObject_new(PyTypeObject* type, PyObject* args, PyObject* kwds);


		//@@@@@@@@@@@ Non-SPECIAL METHODS (referenced from the method table-of-contents) @@@@@@@@@@@@@ 
		/// gobj.create_mesh_component(mesh_fname) -> None
		PyObject* GameObject_create_mesh_component(PyObject* self, PyObject* args);

		/// gobj.create_collider_component("sphere", 1.0, 5, 0x07) -> None
		/// gobj.create_collider_component("box", 2.3, 1.5, 4.7, 5, 0x07) -> None
		///    where 1.0 is the sphere's radius
		///    5 is the collider layer (0 - 31)
		///    0x07 is a collider mask.  If the 1 << *other* object's layer anded with our mask is True, we'll
		///        be notified that there is a collision
		///    2.3, 1.5, 4.7 are the box extents along the local x, y, and z axes.
		PyObject* GameObject_create_collider_component(PyObject* self, PyObject* args);

		/// gobj.create_collider_visualization() -> None
		PyObject* GameObject_create_collider_visualization(PyObject* self, PyObject* args);

		/// gobj.get_world_world() -> (x, y, z)
		PyObject* GameObject_get_world_position(PyObject* self, PyObject* args);

		/// gobj.name() -> string (the c++ game object's name)
		PyObject* GameObject_name(PyObject* self, PyObject* args);

		/// gobj.tag() -> integer (the c++ game object's tag)
		PyObject* GameObject_tag(PyObject* self, PyObject* args);

		/// Gets the (python) parent game object or None if this doesn't have a parent.
		PyObject* GameObject_parent(PyObject* self, PyObject* args);

		/// Gets a point (3-tuple) in world space from the given point (in local space)
		PyObject* GameObject_relative_point(PyObject* self, PyObject* args);

		/// gobj.rotate_world(degrees, axis-x, axis-y, axis-z) -> None
		PyObject* GameObject_rotate_world(PyObject* self, PyObject* args);

		/// gobj.scale(sx, sy, sz) -> None
		PyObject* GameObject_scale(PyObject* self, PyObject* args);

		/// gobj.set_world_position(x, y, z) -> None
		PyObject* GameObject_set_world_position(PyObject* self, PyObject* args);

		/// gobj.translate_local(tx, ty, tz) -> None
		PyObject* GameObject_translate_local(PyObject* self, PyObject* args);

		/// gobj.translate_world(tx, ty, tz) -> None
		PyObject* GameObject_translate_world(PyObject* self, PyObject* args);
	}
}
