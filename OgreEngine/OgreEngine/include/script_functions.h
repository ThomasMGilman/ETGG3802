#pragma once
#include <python.h>
#include <Ogre.h>

namespace OgreEngine
{
	Ogre::ColourValue get_colour_from_pytuple(PyObject* tuple);

	PyObject* find_string_match_indicies(PyObject* self, PyObject* args);

	PyObject* log(PyObject* self, PyObject* args);

	PyObject* set_skybox(PyObject* self, PyObject* args);
}