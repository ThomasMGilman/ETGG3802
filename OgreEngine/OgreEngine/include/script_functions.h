#pragma once
#include <stdafx.h>

namespace OgreEngine
{
	template<typename T>
	struct package
	{
		T data;
		bool errSet = false;
		std::string msg = "Good";
	};

	template<typename T>
	OgreEngine::package<T> get_num_from_pytuple(PyObject* tuple, int index)
	{
		OgreEngine::package<T> f;
		if (!PyNumber_Check(PyTuple_GetItem(tuple, index)))
		{
			f.msg = "Item in tuple at index: " + std::to_string(index) + " is not a number\n\t";
			return f;
		}
		PyObject* num = NULL;
		int typeHash = typeid(T).hash_code(), intHash = typeid(int).hash_code(), floatHash = typeid(float).hash_code(), doubleHash = typeid(double).hash_code();
		if(typeHash == intHash)
			num = PyNumber_Long(PyTuple_GetItem(tuple, index));
		else if(typeHash == floatHash || doubleHash)
			num = PyNumber_Float(PyTuple_GetItem(tuple, index));
		if (num == NULL)
		{
			Py_DECREF(num);
			f.msg = "Could not convert object in tuple at index: " + std::to_string(index) + " to " + typeid(T).name() + "!!\n\t";
			f.errSet = true;
			return f;
		}
		if (typeHash == intHash)
			f.data = PyLong_AsLong(num);
		else if (typeHash == floatHash)
			f.data = (float)PyFloat_AsDouble(num);
		else if (typeHash == doubleHash)
			f.data = PyFloat_AsDouble(num);

		Py_DECREF(num);
		return f;
	};

	OgreEngine::package<Ogre::ColourValue> get_colour_from_pytuple(PyObject* tuple);

	OgreEngine::package<Ogre::Vector3> get_vector3_from_pytuple(PyObject* tuple);

	OgreEngine::package<Ogre::Quaternion> get_quaternion_from_pytuple(PyObject* tuple);

	PyObject* load_script(PyObject* self, PyObject* args);

	PyObject* find_string_match_indicies(PyObject* self, PyObject* args);

	PyObject* log(PyObject* self, PyObject* args);

	PyObject* create_python_game_object(PyObject* self, PyObject* args);

	PyObject* set_skybox(PyObject* self, PyObject* args);
}