#pragma once
#include <stdafx.h>

namespace OgreEngine
{
	template<typename T>
	struct package
	{
		T* data = nullptr;
		std::string msg = "Good";
	};

	OgreEngine::package<float> get_float_from_pytuple(PyObject* tuple, int index);

	OgreEngine::package<Ogre::ColourValue> get_colour_from_pytuple(PyObject* tuple);

	OgreEngine::package<Ogre::Vector3> get_vector3_from_pytuple(PyObject* tuple);

	OgreEngine::package<Ogre::Quaternion> get_quaternion_from_pytuple(PyObject* tuple);

	PyObject* find_string_match_indicies(PyObject* self, PyObject* args);

	PyObject* log(PyObject* self, PyObject* args);

	PyObject* create_python_game_object(PyObject* self, PyObject* args);

	PyObject* set_skybox(PyObject* self, PyObject* args);
}