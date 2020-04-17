#include <stdafx.h>
#include <script_functions.h>
#include <log_manager.h>
#include <game_object_manager.h>
#include <script_manager.h>
#include <script_game_object_methods.h>
#include <script_game_object.h>

extern PyTypeObject GameObjectType;

OgreEngine::package<Ogre::ColourValue> OgreEngine::get_colour_from_pytuple(PyObject* tuple, int offsetIntoTuple = 0)
{
	OgreEngine::package<Ogre::ColourValue> returnPack;
	if (PyTuple_Size(tuple) != 3)
		returnPack.msg = "Got Tuple of Invalid size!! Tuple needs to be of size 3.\n\t";
	else
	{
		OgreEngine::package<float> r, g, b;
		r = get_num_from_pytuple<float>(tuple, 0 + offsetIntoTuple);
		g = get_num_from_pytuple<float>(tuple, 1 + offsetIntoTuple);
		b = get_num_from_pytuple<float>(tuple, 2 + offsetIntoTuple);
		if (r.errSet || g.errSet || b.errSet)
			returnPack.msg = "Got Invalid Paramater in tuple!!!\n\tr: " + r.msg + "\n\tg: " + g.msg + "\n\tb: " + b.msg+"\n\t";
		if (r.data > 1 || r.data < 0 || g.data > 1 || g.data < 0 || b.data > 1 || b.data < 0)
			returnPack.msg = "Got Invalid Float Value!! Needs to be in the range of 0-1!!\n\tr: "+
				std::to_string(r.data)+"\n\tg: "+std::to_string(g.data)+"\n\tb: "+std::to_string(b.data)+"\n\t";
		else
			returnPack.data = Ogre::ColourValue(r.data, g.data, b.data);
	}
	return returnPack;
}

OgreEngine::package<Ogre::Vector3> OgreEngine::get_vector3_from_pytuple(PyObject* tuple, int offsetIntoTuple)
{
	OgreEngine::package<Ogre::Vector3> returnPack;
	if (PyTuple_Size(tuple) != 3)
		returnPack.msg = "Got Tuple of Invalid size!! Tuple needs to be of size 3.\n\t";
	else
	{
		OgreEngine::package<float> x, y, z;
		x = get_num_from_pytuple<float>(tuple, 0 + offsetIntoTuple);
		y = get_num_from_pytuple<float>(tuple, 1 + offsetIntoTuple);
		z = get_num_from_pytuple<float>(tuple, 2 + offsetIntoTuple);
		if (x.errSet || y.errSet || z.errSet)
		{
			returnPack.errSet = true;
			returnPack.msg = "Got Invalid Paramater in tuple!!!\n\tx: " + x.msg + "\n\ty: " + y.msg + "\n\tz: " + z.msg + "\n\t";
		}
		else
			returnPack.data = Ogre::Vector3(x.data, y.data, z.data);
	}
	return returnPack;
}

OgreEngine::package<Ogre::Quaternion> OgreEngine::get_quaternion_from_pytuple(PyObject* tuple, int offsetIntoTuple)
{
	OgreEngine::package<Ogre::Quaternion> returnPack;
	if (PyTuple_Size(tuple) != 4)
		returnPack.msg = "Got Tuple of Invalid size!! Tuple needs to be of size 4.\n\t";
	else
	{
		OgreEngine::package<float> w, x, y, z;
		w = get_num_from_pytuple<float>(tuple, 0 + offsetIntoTuple);
		x = get_num_from_pytuple<float>(tuple, 1 + offsetIntoTuple);
		y = get_num_from_pytuple<float>(tuple, 2 + offsetIntoTuple);
		z = get_num_from_pytuple<float>(tuple, 3 + offsetIntoTuple);
		if (x.errSet || y.errSet || z.errSet || w.errSet)
		{
			returnPack.errSet = true;
			returnPack.msg = "Got Invalid Paramater in tuple!!!\n\tx: " + x.msg + "\n\ty: " + y.msg + "\n\tz: " + z.msg + "\n\tw: " + w.msg + "\n\t";
		}
		else
			returnPack.data = Ogre::Quaternion(w.data, x.data, y.data, z.data);
	}
	return returnPack;
}

PyObject* OgreEngine::load_script(PyObject* self, PyObject* args)
{
	std::string excMsg = (__FILE__);
	bool excSet = false;
	if (PyTuple_Check(args) && PyTuple_Size(args) == 1)
	{
		if (PyUnicode_Check(PyTuple_GetItem(args, 0)))
		{
			std::string file = PyUnicode_AsUTF8(PyTuple_GetItem(args, 0));
			SCRIPT_MANAGER->run_script(file);
		}
		else
		{
			excMsg += "\n\tLine: " + (__LINE__);
			excMsg += " Argument passed is not string!! Need to pass a valid script file to parse!!";
			excSet = true;
		}
	}
	else
	{
		excMsg += "\n\tLine: " + (__LINE__);
		excMsg += " Argument passed is not tuple, or not size 1!!Need to pass a valid script file to parse!!";
		excSet = true;
	}
	if (excSet)
	{
		PyErr_SetString(PyExc_AttributeError, excMsg.c_str());
		return NULL;
	}
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject* OgreEngine::load_scene(PyObject* self, PyObject* args)
{
	return nullptr;
}

PyObject* OgreEngine::find_all_string_match_indicies(PyObject* self, PyObject* args)
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
			if (c.errSet)
			{
				PyErr_SetString(PyExc_ValueError, c.msg.c_str());
				return NULL;
			}
			LOG_MANAGER->log_message(PyUnicode_AsUTF8(PyTuple_GetItem(args, 0)),
				c.data);
		}
		else if (PyTuple_Size(args) == 3 && PyTuple_Check(PyTuple_GetItem(args, 1)) && PyNumber_Check(PyTuple_GetItem(args, 2)))
		{
			c = get_colour_from_pytuple(PyTuple_GetItem(args, 1));
			if (c.errSet)
			{
				PyErr_SetString(PyExc_ValueError, c.msg.c_str());
				return NULL;
			}
			LOG_MANAGER->log_message(PyUnicode_AsUTF8(PyTuple_GetItem(args, 0)),
				c.data,
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
	PyObject* exception = nullptr, * curObjLookingAt, *pythonClassScript = NULL, *newPyObj = NULL, *extraArgs = NULL;
	std::string errorMsg = (__FILE__);
	int tupleSize = PyTuple_Size(args);
	if (tupleSize >= 3)
	{
		std::string groupName, objName;
		int tag;
		/// Group Name
		if (!PyUnicode_Check(PyTuple_GetItem(args, 0)))
		{
			exception = PyExc_AttributeError;
			errorMsg += " Line: " + std::to_string(__LINE__) + "Did not pass valid GroupName!! Needs to be a string value for argument 0!!\n\t";
		}
		else
			groupName = PyUnicode_AsUTF8(PyTuple_GetItem(args, 0));
		/// Object Name
		if (!PyUnicode_Check(PyTuple_GetItem(args, 1)))
		{
			exception = PyExc_AttributeError;
			errorMsg += "Line: " + std::to_string(__LINE__) + "Did not pass valid ObjectName!! Needs to be a string value for argument 1!!\n\t";
		}
		else
			objName = PyUnicode_AsUTF8(PyTuple_GetItem(args, 1));
		/// Tag
		if (!PyLong_Check(PyTuple_GetItem(args, 2)))
		{
			exception = PyExc_AttributeError;
			errorMsg += "Line: " + std::to_string(__LINE__) + "Did not pass a valid Tag!! Needs to be a integer value for argument 2!!\n\t";
		}
		else
			tag = PyLong_AsLong(PyTuple_GetItem(args, 2));

		bool scriptClassSet = false;

		/// Python script class name
		if (tupleSize >= 4)
		{
			curObjLookingAt = PyTuple_GetItem(args, 3);
			if (!PyUnicode_Check(curObjLookingAt))
			{
				exception = PyExc_AttributeError;
				errorMsg += "Line: " + std::to_string(__LINE__) + "Did not pass a valid class name!! Needs to be the string name of class currently in scope!!\n\t";
			}
			else
			{
				scriptClassSet = true;
				pythonClassScript = curObjLookingAt;
			}
		}
		if (tupleSize >= 5) // Any additional arguments
		{
			int numArgs = tupleSize - 4;
			extraArgs = PyTuple_New(numArgs);
			for (int i = 4; i < tupleSize; i++)
			{
				int index = numArgs - (tupleSize - i);
				PyObject* objToMove = PyTuple_GetItem(args, i);
				Py_INCREF(objToMove);
				if (PyTuple_SetItem(extraArgs, index, objToMove) == -1)
				{
					exception = PyExc_IndexError;
					errorMsg += "Line: "+std::to_string(__LINE__)+" Invalid index of tuple!! Trying to place PyObject outside of range of tuple size specified!!\n\t";
				}
			}
		}
		if (exception == nullptr)
		{
			//LOG_MANAGER->log_message("Making OBJ!!",RAND_COLOUR,10.0f);
			if (!scriptClassSet)
			{
				newPyObj = PyObject_Call((PyObject*)&GameObjectType, args, 0);
				((script::GameObject*)newPyObj)->mTwin = GAME_OBJ_MANAGER->create_game_object(groupName, objName, nullptr, tag);
			}
			else
			{
				PyObject* globs = PyEval_GetGlobals(); // dictionary of all globals
				PyObject* pyClass = PyDict_GetItem(globs, pythonClassScript);
				if (pyClass == NULL)
				{
					if (SCRIPT_MANAGER->PyClasses.find(PyUnicode_AsUTF8(pythonClassScript)) != SCRIPT_MANAGER->PyClasses.end())
						pyClass = SCRIPT_MANAGER->PyClasses.at(PyUnicode_AsUTF8(pythonClassScript));
				}
				if (pyClass != NULL)
				{
					newPyObj = PyObject_Call(pyClass, args, 0);
					((script::GameObject*)newPyObj)->mTwin = GAME_OBJ_MANAGER->create_game_object(groupName, objName, nullptr, tag);
					((script::GameObject*)newPyObj)->mTwin->set_script_twin(newPyObj);
					if (extraArgs == NULL)
						extraArgs = PyTuple_New(0);
					((script::GameObject*)newPyObj)->mTwin->run_method("create", extraArgs);
				}
				else
				{
					exception = PyExc_AttributeError;
					errorMsg += "Line: " + std::to_string(__LINE__) + " Invalid class passed!! class is not contained within scope!!";
				}
			}	
		}
	}
	else
	{
		exception = PyExc_AttributeError;
		errorMsg += "Got Invalid number of attributes passed!! need to pass a GroupName, ObjectName, and Tag atleast to create a valid gameObject!!";
	}
	if (exception != NULL)
	{
		PyErr_SetString(exception, errorMsg.c_str());
		newPyObj = NULL;
	}
	return newPyObj;
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

PyObject* OgreEngine::register_input_listener(PyObject* self, PyObject* args)
{
	return nullptr;
}

PyObject* OgreEngine::deregister_input_listener(PyObject* self, PyObject* args)
{
	return nullptr;
}

PyObject* OgreEngine::has_action(PyObject* self, PyObject* args)
{
	return nullptr;
}

PyObject* OgreEngine::get_action(PyObject* self, PyObject* args)
{
	return nullptr;
}

PyObject* OgreEngine::has_axis(PyObject* self, PyObject* args)
{
	return nullptr;
}

PyObject* OgreEngine::get_axis(PyObject* self, PyObject* args)
{
	return nullptr;
}