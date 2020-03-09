#include <script_functions.h>
#include <log_manager.h>
#include <game_object_manager.h>
#include <iostream>
#include <sstream>
#include <regex>
#include <list>

float OgreEngine::get_float_from_pytuple(PyObject* tuple, int index)
{

	PyObject* num = PyNumber_Float(PyTuple_GetItem(tuple, index));
	float f = PyFloat_AsDouble(num);
	Py_DECREF(num);
	return f;
}

OgreEngine::package<Ogre::ColourValue> OgreEngine::get_colour_from_pytuple(PyObject* tuple)
{
	OgreEngine::package<Ogre::ColourValue> returnPack;
	if (PyTuple_Size(tuple) != 3)
		returnPack.msg = "Got Tuple of Invalid size!! ColourValue Tuple needs to be of size 3.";
	else
	{
		float r = get_float_from_pytuple(tuple, 0);
		float g = get_float_from_pytuple(tuple, 1);
		float b = get_float_from_pytuple(tuple, 2);
		if (r > 1 || r < 0 || g > 1 || g < 0 || b > 1 || b < 0)
			returnPack.msg = "Got Invalid Float Value!! Needs to be in the range of 0-1";
		else
			returnPack.data = &Ogre::ColourValue(r, g, b);
	}
	return returnPack;
}

PyObject* OgreEngine::find_string_match_indicies(PyObject* self, PyObject* args)
{
	//self is the module we are a part of
	// args is a tuple
	if (PyTuple_Check(args) && PyTuple_Size(args) == 2 && PyUnicode_Check(PyTuple_GetItem(args, 0))
		&& PyUnicode_Check(PyTuple_GetItem(args, 1)))
	{
		std::string lines = PyUnicode_AsUTF8(PyTuple_GetItem(args, 0));
		std::string toCheckFor = PyUnicode_AsUTF8(PyTuple_GetItem(args, 1));

		std::ostringstream oss;
		oss << "(" << toCheckFor << ")";

		std::regex reg(oss.str());
		std::vector<int>* indicies = new std::vector<int>();
		for (std::sregex_iterator it = std::sregex_iterator(lines.begin(), lines.end(), reg); it != std::sregex_iterator(); it++)
			indicies->push_back(it->position());

		if (indicies->size() > 0)
		{
			PyObject* tuplePack = PyTuple_New(indicies->size());
			for (int i = 0; i < indicies->size(); i++)
				PyTuple_SetItem(tuplePack, i, PyLong_FromLong(indicies->at(i)));
			Py_IncRef(tuplePack);
			return tuplePack;
		}
		Py_IncRef(Py_None);
		return Py_None;
	}
	else
		PyErr_SetString(PyExc_ValueError, "Did not get arguement of type Tuple");
	return nullptr;
}

PyObject* OgreEngine::log(PyObject* self, PyObject* args)
{
	if (PyTuple_Check(args) && PyUnicode_Check(PyTuple_GetItem(args, 0)))
	{
		OgreEngine::package<Ogre::ColourValue> c;
		if (PyTuple_Size(args) == 1)
			LOG_MANAGER->log_message(PyUnicode_AsUTF8(PyTuple_GetItem(args, 0)));

		else if (PyTuple_Size(args) == 2 && PyTuple_Check(PyTuple_GetItem(args, 1)))
		{
			c = get_colour_from_pytuple(PyTuple_GetItem(args, 1));
			if (c.data == nullptr)
			{
				PyErr_SetString(PyExc_ValueError, c.msg.c_str());
				return NULL;
			}
			LOG_MANAGER->log_message(PyUnicode_AsUTF8(PyTuple_GetItem(args, 0)),
				*c.data);
		}
		else if (PyTuple_Size(args) == 3 && PyTuple_Check(PyTuple_GetItem(args, 1)) && PyNumber_Check(PyTuple_GetItem(args, 2)))
		{
			c = get_colour_from_pytuple(PyTuple_GetItem(args, 1));
			if (c.data == nullptr)
			{
				PyErr_SetString(PyExc_ValueError, c.msg.c_str());
				return NULL;
			}
			LOG_MANAGER->log_message(PyUnicode_AsUTF8(PyTuple_GetItem(args, 0)),
				*c.data,
				PyFloat_AsDouble(PyTuple_GetItem(args, 2)));
		}
		else
		{
			PyErr_SetString(PyExc_ValueError, "Did not get valid number of tuple arguments for log. Requires 1-3 arguments");
			return NULL;
		}
	}
	else
	{
		PyErr_SetString(PyExc_ValueError, "Did not get arguement of type Tuple");
		return NULL;
	}
		
	Py_IncRef(Py_None);
	return Py_None;
}

PyObject* OgreEngine::set_skybox(PyObject* self, PyObject* args)
{
	if (PyTuple_Check(args) && PyUnicode_Check(PyTuple_GetItem(args, 0)))
	{
		if (PyTuple_Size(args) == 1)
			GAME_OBJ_MANAGER->set_skybox(PyUnicode_AsUTF8(PyTuple_GetItem(args, 0)));
		else if (PyTuple_Size(args) == 2 && PyFloat_Check(PyTuple_GetItem(args, 1)))
			GAME_OBJ_MANAGER->set_skybox(PyUnicode_AsUTF8(PyTuple_GetItem(args, 0)), PyFloat_AsDouble(PyTuple_GetItem(args, 1)));
		else
			PyErr_SetString(PyExc_ValueError, "Did not get valid number of tuple arguments for set_skybox. Requires 1-2 arguments");
	}
	else
		PyErr_SetString(PyExc_ValueError, "Did not get arguement of type Tuple");
	Py_IncRef(Py_None);
	return Py_None;
}