#pragma once
#include <stdafx.h>

namespace OgreEngine
{
	template<typename T>
	struct package
	{
		T data;
		bool errSet = false;
		std::string msg = "Good\n";
	};

	template<typename T>
	package<T> get_num_from_pytuple(PyObject* tuple, int index)
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

	package<Ogre::ColourValue> get_colour_from_pytuple(PyObject* tuple, int offsetIntoTuple = 0);

	package<Ogre::Vector3> get_vector3_from_pytuple(PyObject* tuple, int offsetIntoTuple = 0);

	package<Ogre::Quaternion> get_quaternion_from_pytuple(PyObject* tuple, int offsetIntoTuple = 0);

	/// Loads the provided script. The provided script must include full path to said script.
	PyObject* load_script(PyObject* self, PyObject* args);

	/// Loads the provided scene. The provided scene must include full path to said scene.
	PyObject* load_scene(PyObject* self, PyObject* args);

	/// Given a string, finds all matches with the given match case.
	PyObject* find_all_string_match_indicies(PyObject* self, PyObject* args);

	/// Logs a message to the screen and log doc.
	PyObject* log(PyObject* self, PyObject* args);

	/// Creates a gameobject, must provide (groupName, objectName, Tag). Can optionally give (scriptClassName, scriptClassArgs).
	PyObject* create_python_game_object(PyObject* self, PyObject* args);

	/// Sets the skybox image to the provided image.
	PyObject* set_skybox(PyObject* self, PyObject* args);

	/// Register the gameobject as a new input listener.
	PyObject* register_input_listener(PyObject* self, PyObject* args);

	/// Deregister the gameobject as a input listener.
	PyObject* deregister_input_listener(PyObject* self, PyObject* args);

	/// Check if gameobject listener has the provided action. Checks by string name.
	PyObject* has_action(PyObject* self, PyObject* args);

	/// Get the action by string name.
	PyObject* get_action(PyObject* self, PyObject* args);

	/// Check if the given object has the specified input axis.
	PyObject* has_axis(PyObject* self, PyObject* args);

	/// Get the requested input axis.
	PyObject* get_axis(PyObject* self, PyObject* args);
}