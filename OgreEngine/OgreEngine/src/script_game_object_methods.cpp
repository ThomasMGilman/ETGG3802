#include <stdafx.h>
#include <script_game_object_methods.h>
#include <game_object_manager.h>
#include <script_functions.h>
#include <log_manager.h>

// NOTE: This file is used to define any methods of our python ssuge.GameObject class.  Put
// the declaration / prototype of any new functions in script_game_object_methods.h.  Then
// if this is a "special" method (e.g. __init__, __new__, __del__, __mul__, etc.), put the
// function pointer to one of these in the "big-giant-structure".  If it's a non-special 
// method (e.g. translate_world), add an entry to the method_def structure at the bottom of this file.

//{"a_new_method", OgreEngine::script::GameObject_???, METH_VARARGS, "its docstring"},
PyMethodDef GameObject_methods[] = {
    {"create_mesh_component", OgreEngine::script::create_mesh_component, METH_VARARGS, "Creates a new mesh component attached to the associated self PyObject."},
    {NULL}  /* Sentinel */
};

void OgreEngine::script::GameObject_dealloc(OgreEngine::script::GameObject* self)
{
    Py_TYPE(self)->tp_free((PyObject*)self);
}

int OgreEngine::script::GameObject_init(OgreEngine::script::GameObject* self, PyObject* args, PyObject* kwds)
{
    // Reset our attributes
	PyObject* exception = nullptr, * curObjLookingAt;
	std::string errorMsg = "";
	int tupleSize = PyTuple_Size(args);
	if (tupleSize >= 3)
	{
		std::string groupName, objName;
		int tag;
		/// Group Name
		if (!PyUnicode_Check(PyTuple_GetItem(args, 0)))
		{
			exception = PyExc_AttributeError;
			errorMsg += "Did not pass valid GroupName!! Needs to be a string value for argument 0!!\n\t";
		}
		else
			groupName = PyUnicode_AsUTF8(PyTuple_GetItem(args, 0));
		/// Object Name
		if (!PyUnicode_Check(PyTuple_GetItem(args, 1)))
		{
			exception = PyExc_AttributeError;
			errorMsg += "Did not pass valid ObjectName!! Needs to be a string value for argument 1!!\n\t";
		}
		else
			objName = PyUnicode_AsUTF8(PyTuple_GetItem(args, 1));
		/// Tag
		if (!PyLong_Check(PyTuple_GetItem(args, 2)))
		{
			exception = PyExc_AttributeError;
			errorMsg += "Did not pass a valid Tag!! Needs to be a integer value for argument 2!!\n\t";
		}
		else
			tag = PyLong_AsLong(PyTuple_GetItem(args, 2));

		Ogre::Vector3 pos = Ogre::Vector3::ZERO;
		Ogre::Quaternion rot = Ogre::Quaternion::IDENTITY;
		std::string pythonClassScript, meshName;
		float singleRot = NULL;
		bool singleRotSet = false, meshNameSet = false, scriptClassSet = false;

		/// Python script class name
		if (tupleSize >= 4)
		{
			curObjLookingAt = PyTuple_GetItem(args, 3);
			if (!PyUnicode_Check(curObjLookingAt))
			{
				exception = PyExc_AttributeError;
				errorMsg += "Did not pass a valid class name!! Needs to be the string name of class currently in scope!!\n\t";
			}
			else
			{
				scriptClassSet = true;
				pythonClassScript = PyUnicode_AsUTF8(curObjLookingAt);
			}
		}
		/// Rotation argument
		if (tupleSize >= 5)
		{
			curObjLookingAt = PyTuple_GetItem(args, 4);
			int rotTupleSize = PyTuple_Size(curObjLookingAt);
			if (!PyTuple_Check(curObjLookingAt) && !(rotTupleSize == 4 || rotTupleSize == 1))
			{
				exception = PyExc_AttributeError;
				errorMsg += "Got Invalid position argument!! need to pass a valid vector3 for the 7th paramater!!\n\t";
			}
			if (rotTupleSize == 4)
			{
				OgreEngine::package<Ogre::Quaternion> rotation = get_quaternion_from_pytuple(curObjLookingAt);
				if (rotation.errSet)
				{
					exception = PyExc_AttributeError;
					errorMsg += rotation.msg;
				}
				else
					rot = rotation.data;
			}
			else if (rotTupleSize == 1)
			{
				OgreEngine::package<float> rotation = get_float_from_pytuple(curObjLookingAt, 0);
				if (rotation.errSet)
				{
					exception = PyExc_AttributeError;
					errorMsg += rotation.msg;
				}
				else
				{
					singleRotSet = true;
					singleRot = rotation.data;
				}
			}

		}
		/// Mesh Name stored in resources
		if (tupleSize >= 6)
		{
			curObjLookingAt = PyTuple_GetItem(args, 5);
			if (!PyUnicode_Check(curObjLookingAt))
			{
				exception = PyExc_AttributeError;
				errorMsg += "Did not pass a valid mesh string name!! Needs to be a valid mesh string name that is located in resources!!\n\t";
			}
			else
			{
				meshNameSet = true;
				pythonClassScript = PyUnicode_AsUTF8(curObjLookingAt);
			}
		}
		/// Position argument
		if (tupleSize >= 7)
		{
			curObjLookingAt = PyTuple_GetItem(args, 6);
			if (!PyTuple_Check(curObjLookingAt) && !(PyTuple_Size(curObjLookingAt) == 3))
			{
				exception = PyExc_AttributeError;
				errorMsg += "Got Invalid position argument!! need to pass a valid vector3 for the 6th paramater!!\n\t";
			}
			else
			{
				OgreEngine::package<Ogre::Vector3> position = get_vector3_from_pytuple(curObjLookingAt);
				if (position.errSet)
				{
					exception = PyExc_AttributeError;
					errorMsg += position.msg;
				}
				else
					pos = position.data;
			}
		}
		if (exception == nullptr)
		{
			self->mTwin = GAME_OBJ_MANAGER->create_game_object(groupName, objName, nullptr, tag, pos, rot);
			//if(scriptClassSet);
			if (singleRotSet)
				self->mTwin->set_orientation(singleRot, 0, 1, 0);
			if (meshNameSet)
				self->mTwin->create_mesh(objName + "_" + meshName, meshName);
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
		return -1;
	}
    return 0;
}

PyObject* OgreEngine::script::GameObject_new(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
    OgreEngine::script::GameObject* newPyObj = (OgreEngine::script::GameObject*)type->tp_alloc(type, 0);
	if (GameObject_init(newPyObj, args, kwds) != 0)
		return NULL;
    return (PyObject*)newPyObj;
}

PyObject* OgreEngine::script::create_mesh_component(PyObject* self, PyObject* args)
{
    return nullptr;
}