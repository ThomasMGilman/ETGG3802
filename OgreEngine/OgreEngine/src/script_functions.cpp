#include <stdafx.h>
#include <script_functions.h>
#include <log_manager.h>
#include <game_object_manager.h>
#include <script_game_object.h>


OgreEngine::package<float> OgreEngine::get_float_from_pytuple(PyObject* tuple, int index)
{
	OgreEngine::package<float> f;
	if (!PyNumber_Check(PyTuple_GetItem(tuple, index)))
	{
		f.msg = "Item in tuple at index: " + std::to_string(index)+ " is not a number";
		return f;
	}
	PyObject* num = PyNumber_Float(PyTuple_GetItem(tuple, index));
	if (num == NULL)
	{
		Py_DECREF(num);
		f.msg = "Could not convert object in tuple at index: " + std::to_string(index) + " to float!!";
		return f;
	}
	*f.data = PyFloat_AsDouble(num);
	Py_DECREF(num);
	return f;
}

OgreEngine::package<Ogre::ColourValue> OgreEngine::get_colour_from_pytuple(PyObject* tuple)
{
	OgreEngine::package<Ogre::ColourValue> returnPack;
	if (PyTuple_Size(tuple) != 3)
		returnPack.msg = "Got Tuple of Invalid size!! Tuple needs to be of size 3.";
	else
	{
		OgreEngine::package<float> r, g, b;
		r = get_float_from_pytuple(tuple, 0);
		g = get_float_from_pytuple(tuple, 1);
		b = get_float_from_pytuple(tuple, 2);
		if (r.data == nullptr || g.data == nullptr || b.data == nullptr)
			returnPack.msg = "Got Invalid Paramater in tuple!!!\n\tr: " + r.msg + "\n\tg: " + g.msg + "\n\tb: " + b.msg;
		if (*r.data > 1 || *r.data < 0 || *g.data > 1 || *g.data < 0 || *b.data > 1 || *b.data < 0)
			returnPack.msg = "Got Invalid Float Value!! Needs to be in the range of 0-1!!\n\tr: "+
				std::to_string(*r.data)+"\n\tg: "+std::to_string(*g.data)+"\n\tb: "+std::to_string(*b.data);
		else
			returnPack.data = &Ogre::ColourValue(*r.data, *g.data, *b.data);
	}
	return returnPack;
}

OgreEngine::package<Ogre::Vector3> OgreEngine::get_vector3_from_pytuple(PyObject* tuple)
{
	OgreEngine::package<Ogre::Vector3> returnPack;
	if (PyTuple_Size(tuple) != 3)
		returnPack.msg = "Got Tuple of Invalid size!! Tuple needs to be of size 3.";
	else
	{
		OgreEngine::package<float> x, y, z;
		x = get_float_from_pytuple(tuple, 0);
		y = get_float_from_pytuple(tuple, 1);
		z = get_float_from_pytuple(tuple, 2);
		if (x.data == nullptr || y.data == nullptr || z.data == nullptr)
			returnPack.msg = "Got Invalid Paramater in tuple!!!\n\tx: " + x.msg + "\n\ty: " + y.msg + "\n\tz: " + z.msg;
		else
			returnPack.data = &Ogre::Vector3(*x.data, *y.data, *z.data);
	}
	return returnPack;
}

OgreEngine::package<Ogre::Quaternion> OgreEngine::get_quaternion_from_pytuple(PyObject* tuple)
{
	OgreEngine::package<Ogre::Quaternion> returnPack;
	if (PyTuple_Size(tuple) != 4)
		returnPack.msg = "Got Tuple of Invalid size!! Tuple needs to be of size 4.";
	else
	{
		OgreEngine::package<float> x, y, z, w;
		x = get_float_from_pytuple(tuple, 0);
		y = get_float_from_pytuple(tuple, 1);
		z = get_float_from_pytuple(tuple, 2);
		w = get_float_from_pytuple(tuple, 3);
		if (x.data == nullptr || y.data == nullptr || z.data == nullptr || w.data == nullptr)
			returnPack.msg = "Got Invalid Paramater in tuple!!!\n\tx: " + x.msg + "\n\ty: " + y.msg + "\n\tz: " + z.msg + "\n\tw: " + w.msg;
		else
			returnPack.data = &Ogre::Quaternion(*w.data, *x.data, *y.data, *z.data);
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

PyObject* OgreEngine::create_python_game_object(PyObject* self, PyObject* args)
{
	int tupleSize = PyTuple_Size(args);
	OgreEngine::LOG_MANAGER->log_message("Got Tuple of size: " + std::to_string(tupleSize));
	PyObject* exception = nullptr, *newPyGameObject;
	const char* errorMsg;
	if (tupleSize >= 3)
	{
		if (!PyUnicode_Check(PyTuple_GetItem(args, 0)))
		{
			exception = PyExc_AttributeError;
			errorMsg = "Did not pass valid GroupName!! Needs to be a string value for argument 0!!";
		}
		if (!PyUnicode_Check(PyTuple_GetItem(args, 1)))
		{
			exception = PyExc_AttributeError;
			errorMsg = "Did not pass valid ObjectName!! Needs to be a string value for argument 1!!";
		}
		if (!PyNumber_Check(PyTuple_GetItem(args, 2)))
		{
			exception = PyExc_AttributeError;
			errorMsg = "Did not pass a valid Tag!! Needs to be a number value for argument 2!!";
		}
		else
		{
			Ogre::Vector3 pos = Ogre::Vector3::ZERO; Ogre::Quaternion rot = Ogre::Quaternion::IDENTITY;
			if (tupleSize >= 4)
			{
				if (!PyTuple_Check(PyTuple_GetItem(args, 3)) && !(PyTuple_Size(PyTuple_GetItem(args,3)) == 3))
				{
					exception = PyExc_AttributeError;
					errorMsg = "Got Invalid position argument!! need to pass a valid vector3 for the 4th paramater!!";
				}
				else
				{
					OgreEngine::package<Ogre::Vector3> position = get_vector3_from_pytuple(PyTuple_GetItem(args, 3));
					if (position.data == nullptr)
					{
						exception = PyExc_AttributeError;
						errorMsg = position.msg.c_str();
					}
					else
						pos = *position.data;
				}
			}
			if (tupleSize >= 5)
			{
				if (!PyTuple_Check(PyTuple_GetItem(args, 4)) && !(PyTuple_Size(PyTuple_GetItem(args, 4)) == 4))
				{
					exception = PyExc_AttributeError;
					errorMsg = "Got Invalid position argument!! need to pass a valid vector3 for the 4th paramater!!";
				}
				OgreEngine::package<Ogre::Quaternion> rotation = get_quaternion_from_pytuple(PyTuple_GetItem(args, 4));
				if (rotation.data == nullptr)
				{
					exception = PyExc_AttributeError;
					errorMsg = rotation.msg.c_str();
				}
				else
					rot = *rotation.data;
			}
			if (exception == nullptr)
			{
				// CREAT NEW SCRIPT GAME OBJECT HERE WITH PARAMETERS!!!
			}
		}
	}
	else
	{
		exception = PyExc_AttributeError;
		errorMsg = "Got Invalid number of attributes passed!! need to pass a GroupName, ObjectName, and Tag atleast to create a valid gameObject!!";
	}
	if (exception != NULL)
	{
		PyErr_SetString(exception, errorMsg);
		return NULL;
	}
	
	
	PyObject* newPyGameObject = (PyObject*)new OgreEngine::script::GameObject;
	
	//GameObject* pyGameObj = (GameObject*)self;
	return newPyGameObject;
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
		{
			PyErr_SetString(PyExc_ValueError, "Did not get valid number of tuple arguments for set_skybox. Requires 1-2 arguments");
			return NULL;
		}
	}
	else
		PyErr_SetString(PyExc_ValueError, "Did not get arguement of type Tuple");
	Py_IncRef(Py_None);
	return Py_None;
}