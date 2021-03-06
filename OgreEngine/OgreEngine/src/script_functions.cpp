#include <stdafx.h>
#include <script_functions.h>
#include <log_manager.h>
#include <game_object.h>
#include <game_object_manager.h>
#include <script_manager.h>
#include <script_game_object_methods.h>
#include <script_game_object.h>
#include <input_manager.h>
#include <collision_manager.h>

using namespace OgreEngine;

extern PyTypeObject GameObjectType;

package<Ogre::ColourValue> OgreEngine::get_colour_from_pytuple(PyObject* tuple, int offsetIntoTuple)
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

package<Ogre::Vector3> OgreEngine::get_vector3_from_pytuple(PyObject* tuple, int offsetIntoTuple)
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

package<Ogre::Quaternion> OgreEngine::get_quaternion_from_pytuple(PyObject* tuple, int offsetIntoTuple)
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
	std::string excMsg = (__FILE__);
	bool excSet = false;
	int size = PyTuple_Size(args);

	if (PyTuple_Check(args) && size == 2)
	{
		std::string groupName = PyUnicode_AsUTF8(PyTuple_GetItem(args, 0));
		std::string fileName = PyUnicode_AsUTF8(PyTuple_GetItem(args, 1));
		GAME_OBJ_MANAGER->load_scene(fileName, groupName);
		Py_INCREF(Py_None);
		return Py_None;
	}
	PyErr_SetString(PyExc_AttributeError, (excMsg+" load_scene ERROR!! PyObject argument not tuple or not size 2!!").c_str());
	return nullptr;
}

PyObject* OgreEngine::load_collider_visualizations(PyObject* self, PyObject* args)
{
	COLLISION_MANAGER->set_debug_visibility(true);
	Py_IncRef(Py_None);
	return Py_None;
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

PyObject* OgreEngine::raycast(PyObject* self, PyObject* args)
{
	std::string errMsg;
	if (PyTuple_Check(args))
	{
		int tupleSize = PyTuple_Size(args);
		if (tupleSize == 4)
		{
			package<Ogre::Vector3> origin = get_vector3_from_pytuple(PyTuple_GetItem(args, 0));
			package<Ogre::Vector3> direction = get_vector3_from_pytuple(PyTuple_GetItem(args, 1));
			package<int> mask = get_num_from_pytuple<int>(args, 2);
			package<int> dist = get_num_from_pytuple<int>(args, 3);
			if (origin.errSet || direction.errSet || mask.errSet || dist.errSet)
				errMsg = "raycast ERROR!! args Data ERROR!!\n\tOrigin: " + origin.msg + "\n\tDirection: " + direction.msg + "\n\tMask: " + mask.msg + "\n\tDistance: " + dist.msg;
			else
			{
				std::vector<std::pair<Ogre::Vector3, OgreEngine::GameObject*>> hits;
				COLLISION_MANAGER->raycast_bounded(origin.data, direction.data, hits, dist.data, mask.data);

				int hits_size = hits.size();
				PyObject* returnTuple = PyTuple_New(hits_size);
				if (hits_size > 0)
				{
					for (int i = 0; i < hits_size; i++)
					{
						PyObject* hit_point = PyTuple_Pack(3, PyFloat_FromDouble(hits[i].first.x), PyFloat_FromDouble(hits[i].first.y), PyFloat_FromDouble(hits[i].first.z));
						Py_INCREF(hits[i].second->get_script_twin());
						PyObject* pair = PyTuple_Pack(2, hit_point, hits[i].second->get_script_twin());
						PyTuple_SetItem(returnTuple, i, pair);
					}
				}
				return returnTuple;
			}
		}
		else
			errMsg = "raycast ERROR!! Invalid Tuple size!!! Need to pass a Tuple of size 4, got: " + std::to_string(tupleSize);
	}
	else
		errMsg = "raycast ERROR!! Did not recieve argument of type Tuple!!! Need to pass Tuple of size 4!!";
	if (!errMsg.empty())
	{
		PyErr_SetString(PyExc_AttributeError, errMsg.c_str());
		return NULL;
	}
	Py_IncRef(Py_None);
	return Py_None;
}

PyObject* OgreEngine::register_input_listener(PyObject* self, PyObject* args)
{
	int size = PyTuple_Size(args);
	if (size == 1)
	{
		PyObject* gameObject = PyTuple_GetItem(args, 0);
		if (((script::GameObject*)gameObject)->mTwin != nullptr)
		{
			INPUT_MANAGER->register_listener(((script::GameObject*)gameObject)->mTwin->create_input_listener("listener"));
		}
		Py_IncRef(Py_None);
		return Py_None;
	}
	PyErr_SetString(PyExc_ValueError, "register_input_listener ERROR!! Recieved to many arguments");
	return nullptr;
}

PyObject* OgreEngine::deregister_input_listener(PyObject* self, PyObject* args)
{
	int size = PyTuple_Size(args);
	if (size == 1)
	{
		GameObject* gameObject = ((script::GameObject*)PyTuple_GetItem(args, 0))->mTwin;
		INPUT_MANAGER->deregister_listener(gameObject->get_input_listener());
		gameObject->delete_input_listener();
		Py_IncRef(Py_None);
		return Py_None;
	}
	PyErr_SetString(PyExc_ValueError, "register_input_listener ERROR!! Recieved to many arguments");
	return nullptr;
}

PyObject* OgreEngine::has_action(PyObject* self, PyObject* args)
{
	int size = PyTuple_Size(args);
	if (size == 1)
		return PyBool_FromLong(INPUT_MANAGER->has_action(PyUnicode_AsUTF8(PyTuple_GetItem(args, 0))));
	PyErr_SetString(PyExc_ValueError, "has_action ERROR!! Recieved to many arguments");
	return nullptr;
}

PyObject* OgreEngine::get_action(PyObject* self, PyObject* args)
{
	int size = PyTuple_Size(args);
	if (size == 1)
		return PyBool_FromLong(INPUT_MANAGER->is_action_active(PyUnicode_AsUTF8(PyTuple_GetItem(args, 0))));
	PyErr_SetString(PyExc_ValueError, "get_action ERROR!! Recieved to many arguments");
	return nullptr;
}

PyObject* OgreEngine::has_axis(PyObject* self, PyObject* args)
{
	int size = PyTuple_Size(args);
	if (size == 1)
		return PyBool_FromLong(INPUT_MANAGER->has_axis(PyUnicode_AsUTF8(PyTuple_GetItem(args, 0))));
	PyErr_SetString(PyExc_ValueError, "has_axis ERROR!! Recieved to many arguments");
	return nullptr;
}

PyObject* OgreEngine::get_axis(PyObject* self, PyObject* args)
{
	int size = PyTuple_Size(args);
	if (size == 1)
		return PyFloat_FromDouble(INPUT_MANAGER->get_axis(PyUnicode_AsUTF8(PyTuple_GetItem(args, 0))));
	PyErr_SetString(PyExc_ValueError, "get_axis ERROR!! Recieved to many arguments");
	return nullptr;
}

PyObject* OgreEngine::queue_destroy(PyObject* self, PyObject* args)
{
	std::string errMsg;
	if (PyTuple_Check(args))
	{
		int tupleSize = PyTuple_Size(args);
		if (tupleSize == 1)
		{
			script::GameObject* pyObj = (script::GameObject*)(PyTypeObject*)PyTuple_GetItem(args, 0);
			GAME_OBJ_MANAGER->queue_deletion_of_game_object(pyObj->mTwin);
		}
		else
			errMsg = "queue_destroy ERROR!! Tuple passed needs to be size 1!! got size: "+std::to_string(tupleSize);
	}
	if (!errMsg.empty())
	{
		PyErr_SetString(PyExc_AttributeError, errMsg.c_str());
		return nullptr;
	}
	Py_IncRef(Py_None);
	return Py_None;
}

PyObject* OgreEngine::build_edge_list(PyObject* self, PyObject* args)
{
	if (!PyTuple_Check(args) || PyTuple_Size(args) != 1 || !PyUnicode_Check(PyTuple_GetItem(args, 0)))
	{
		PyErr_SetString(PyExc_ValueError, "You must pass a (string) group name to this function");
		return NULL;
	}

	std::string group_name = PyUnicode_AsUTF8(PyTuple_GetItem(args, 0));

	// Go through all the meshes in the [group_name] group -- make sure this is where
	// your FileSystem paths are in resources.cfg.
	Ogre::MeshManager& mmgr = Ogre::MeshManager::getSingleton();
	Ogre::ResourceGroupManager& rgmgr = Ogre::ResourceGroupManager::getSingleton();
	Ogre::StringVectorPtr items = rgmgr.findResourceNames(group_name, "mesh", false);
	for (int i = 0; i < items->size(); i++)
	{
		Ogre::String name = items->at(i);
		Ogre::MeshPtr mptr = Ogre::MeshManager::getSingleton().getByName(name);
		if (!mptr)
			mptr->buildEdgeList();
	}

	Py_INCREF(Py_None);
	return Py_None;
}
