#pragma once
#include <python.h>
#include <Ogre.h>

namespace OgreEngine
{
	Ogre::ColourValue get_colour_from_pytuple(PyObject* tuple);

	PyObject* find_string_match_indicies(PyObject* self, PyObject* args);

	PyObject* log(PyObject* self, PyObject* args);

	PyObject* set_skybox(PyObject* self, PyObject* args);

	PyMethodDef my_functions[] = {
		{"find_string_match_indicies", find_string_match_indicies, METH_VARARGS, "Takes a two argument tuple of strings.\n\tReturns the indicies of all matches of the 2nd string within the 1st string.\n\tIf no matches, returns None"},
		{"log", log, METH_VARARGS, "Takes a Tuple containing the message to log, optional color value, and optional time to stay on screen for."},
		{"set_skybox", set_skybox, METH_VARARGS, "Takes a material name to set as the background skybox texture and an optinal distance from the viewer."},
		{NULL, NULL, 0, NULL}
	};

	struct PyModuleDef ogre_module = {
		PyModuleDef_HEAD_INIT,			// data to place at beginning
		"ogre functions",
		"set of functions to interract with the Ogre game engine",
		-1,								// -1 = keep module state in globals
		my_functions					// (from last slide)
	};

	PyMODINIT_FUNC PyInit_ogre_module(void)
	{
		printf("in the init function\n");
		PyObject* m = PyModule_Create(&ogre_module);
		return m == NULL ? NULL : m;
	}
}