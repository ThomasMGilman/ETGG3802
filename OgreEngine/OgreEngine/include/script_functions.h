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

	float get_float_from_pytuple(PyObject* tuple, int index);

	OgreEngine::package<Ogre::ColourValue> get_colour_from_pytuple(PyObject* tuple);

	PyObject* find_string_match_indicies(PyObject* self, PyObject* args);

	PyObject* log(PyObject* self, PyObject* args);

	PyObject* set_skybox(PyObject* self, PyObject* args);
}