#include <stdafx.h>
#include <log_manager.h>
#include <script_manager.h>
#include <script_game_object.h>

ssuge::ScriptManager* ssuge::Singleton<ssuge::ScriptManager>::msSingleton = NULL;

extern PyMethodDef ssuge_script_functions[];				// Defined in script_functions.cpp (the bottom)
extern PyTypeObject GameObjectType;							// Defined in script_game_object.cpp


static PyModuleDef ssuge_module = {
	PyModuleDef_HEAD_INIT, 
	"ssuge", 
	NULL, 
	-1, 
	ssuge_script_functions,
	NULL, NULL, NULL, NULL
};

static PyObject* PyInit_ssuge(void)
{
	// This adds methods that we aren't implementing and want to inherit from
	// the base "object" type.  Needs to be called before we try and add it to ssuge
	if (PyType_Ready(&GameObjectType) < 0)
	{
		PyErr_SetString(PyExc_ModuleNotFoundError, "can't finalize GameObject class");
		return NULL;
	}

	// Create the module itself
	PyObject * mod = PyModule_Create(&ssuge_module);
	if (mod == NULL)
	{
		PyErr_SetString(PyExc_ModuleNotFoundError, "can't create ssuge module");
		return NULL;
	}

	// Add GameObject class to ssuge
	Py_INCREF(&GameObjectType);
	if (PyModule_AddObject(mod, "GameObject", (PyObject*)&GameObjectType) < 0) 
	{
		Py_DECREF(&GameObjectType);
		Py_DECREF(mod);
		PyErr_SetString(PyExc_ModuleNotFoundError, "can't attach GameObject to ssuge module");
		return NULL;
	}

	// Success!  Return the functional module
	return mod;
}

ssuge::ScriptManager::ScriptManager()
{
	Py_SetPath(L"./python38.zip");
	PyImport_AppendInittab("ssuge", PyInit_ssuge);
	Py_Initialize();
}

ssuge::ScriptManager::~ScriptManager()
{
	// Probably not necessary since we're about to shut down, but decref all the loaded
	// classes we were hanging on to.
	std::map<std::string, PyObject*>::iterator it = mLoadedClasses.begin();
	while (it != mLoadedClasses.end())
	{
		Py_DECREF(it->second);
		++it;
	}

	// Shut down the interpreter
	Py_Finalize();
}


std::string ssuge::ScriptManager::get_string_from_pyobject(PyObject* py_str)
{
	std::string result;

	if (!PyUnicode_Check(py_str))
		return result;

#if PY_MINOR_VERSION < 3 || defined(Py_LIMITED_API)
	PyObject* bytes_obj = PyUnicode_AsUTF8String(py_str);
	result = std::string(PyBytes_AsString(bytes_obj));
	Py_DECREF(bytes_obj);
#else
	result = std::string(PyUnicode_AsUTF8(py_str));
#endif

	return result;
}


bool ssuge::ScriptManager::extract_vector3(PyObject* obj, Ogre::Vector3& vec, std::string error_message)
{
	if (!PyTuple_Check(obj) || PyTuple_Size(obj) != 3 || !PyNumber_Check(PyTuple_GetItem(obj, 0)) ||
		!PyNumber_Check(PyTuple_GetItem(obj, 1)) || !PyNumber_Check(PyTuple_GetItem(obj, 2)))
	{
		PyErr_SetString(PyExc_ValueError, error_message.c_str());
		return false;
	}
	else
	{
		for (int i = 0; i < 3; i++)
		{
			PyObject* fval = PyNumber_Float(PyTuple_GetItem(obj, i));
			vec[i] = (float)PyFloat_AsDouble(fval);
			Py_DECREF(fval);
		}

		return true;
	}
}



void ssuge::ScriptManager::look_for_classes(PyObject* loaded_module)
{
	PyObject* mod_dict = PyModule_GetDict(loaded_module);

	// Iterate through the dictionary.  Reference: https://docs.python.org/3.8/c-api/dict.html, PyDict_Next function
	PyObject* key, * value;
	Py_ssize_t pos = 0;

	while (PyDict_Next(mod_dict, &pos, &key, &value)) 
	{
		std::string item_name = get_string_from_pyobject(key);
		if (PyType_Check(value) && PyType_IsSubtype((PyTypeObject*)value, &GameObjectType) && PyUnicode_Check(key))
		{
			std::string class_name = get_string_from_pyobject(key);
			// Incref the value since we're going to hang on to it for a while (the module 
			// will get decref'ed at the end of run_script, and it owns the only reference until
			// we do this).  We'll decref this object in the script manager's destructor
			Py_INCREF(value);
			std::map<std::string, PyObject*>::iterator it = mLoadedClasses.find(class_name);
			if (it != mLoadedClasses.end())
			{
				// We're replacing another loaded class.  Let the user know, then do the replacement
				LOG_MANAGER->log("Warning: duplicate class name '" + class_name + "'", LogPreset::WARNING);
				Py_DECREF(it->second);
			}
			mLoadedClasses[class_name] = value;
		}
		
	}
}


void ssuge::ScriptManager::run_script(std::string fname)
{
	// Part I: parse the file itself
	std::ifstream fp(fname, std::ios::binary);
	char* buffer = NULL;
	std::streamsize buffer_size = 0;
	if (fp.is_open())
	{
		fp.seekg(0, std::ios::end);
		buffer_size = fp.tellg();
		fp.seekg(0, std::ios::beg);
		buffer = new char[buffer_size + 1L];
		if (buffer)
		{
			fp.read(buffer, buffer_size);
			buffer[buffer_size] = '\0';
		}
		else
			LOG_MANAGER->log("Error reading script data from '" + fname + "' buffer_size=" + std::to_string(buffer_size), Ogre::ColourValue::Red, 10.0f);
	}
	else
		LOG_MANAGER->log("Error opening script file '" + fname + "'", ssuge::LogPreset::ERROR);

	if (!buffer)
		return;

	// Part II: Compile the char-buffer into a byte-code object.
	PyObject* byteCode = Py_CompileString(buffer, fname.c_str(), Py_file_input);
	if (byteCode == NULL)
		handle_error();
	else
	{
		// Part III: Execute the byte-code object
		PyObject* module = PyImport_ExecCodeModule((char*)fname.c_str(), byteCode);
		Py_DECREF(byteCode);
		if (!module)
			handle_error();
		else
		{
			// Part IV: Look for classes in that loaded module
			look_for_classes(module);

			Py_DECREF(module);
		}
	}

	// Part V: Cleanup
	if (buffer)
		delete[] buffer;
	LOG_MANAGER->log("Successfully executed script '" + fname + "'", LogPreset::DIAGNOSTIC);
}



PyObject* ssuge::ScriptManager::get_class(std::string name)
{
	std::map<std::string, PyObject*>::iterator it = mLoadedClasses.find(name);
	if (it != mLoadedClasses.end())
		return it->second;
	else
	{
		// The above check will fail if the class was not in a module that was *already*
		// loaded.  In some cases, the user might define a class in a python file and in that
		// same python file, try and call create_game_object.  In this case, that class 
		// should be in the globals dictionary.  Try and find it.
		PyObject* class_name = PyUnicode_FromString(name.c_str());
		PyObject* globs = PyEval_GetGlobals();
		if (globs != NULL)
		{
			PyObject* val = PyDict_GetItem(globs, class_name);
			if (val != NULL)
			{
				Py_DECREF(class_name);
				return val;
			}
		}
		Py_DECREF(class_name);

		// Still couldn't find it -- fall back to the "vanilla" ssuge.GameObject class
		// and log a warning (as long as the script name is not '' -- that's used some places in our
		// code to indicate we *want* to use the vanilla class)
		if (name != "")
			LOG_MANAGER->log("Error finding python class '" + name + "' -- using basic ssuge.GameObject class instead", LogPreset::WARNING);
		return (PyObject*)&GameObjectType;;
	}
}



void ssuge::ScriptManager::handle_error()
{
	std::string errString;
	PyObject* pErr;
	PyObject* pErrType = NULL, * pErrValue = NULL, * pErrTraceback = NULL;
	
	// Make sure we really have an error.  If not, return.
	pErr = PyErr_Occurred();
	if (!pErr)
		return;

	PyErr_Fetch(&pErrType, &pErrValue, &pErrTraceback);
	// Sometimes the errors fetched on the line above aren't fully-formed.  This
	// fleshes out the exception, if necessary.
	PyErr_NormalizeException(&pErrType, &pErrValue, &pErrTraceback);
	if (pErrTraceback == NULL)
	{
		// We don't have a traceback -- just convert the error value to a string and use that.  This
		// is useful for syntax errors
		PyObject* temp_str = PyObject_Str(pErrValue);
		errString = std::string(PyUnicode_AsUTF8(temp_str));
		Py_DECREF(temp_str);
	}
	else 
	{
		// In case we fail along the way, generate *some* kind of error message
		errString = "Can't generate stack trace...";

		// In python, you can generate a stack trace (like Idle does), by doing this:
		// import traceback
		// import sys
		// ... (when an error has occurred)
		//    exception_type, exception_value, exception_traceback = sys.exc_info()  # Like PyErr_Fetch above
		//    s = traceback.format_exception(exception_type, exception_value, exception_traceback)
		//    print(s)
		// We want to do essentially the same thing here.
		PyObject* traceback_module = PyImport_ImportModule("traceback");
		if (traceback_module)
		{
			PyObject* format_exception_func = PyObject_GetAttrString(traceback_module, "format_exception");
			if (format_exception_func && PyCallable_Check(format_exception_func))
			{
				// Construct a tuple to pass some arguments to format_exception
				PyObject* args_tuple = PyTuple_New(3);
				// We don't own these objects -- incref them b/c the PyTuple_SetItem steals a reference
				Py_INCREF(pErrType);
				Py_INCREF(pErrValue);
				Py_INCREF(pErrTraceback);// new reference
				PyTuple_SetItem(args_tuple, 0, pErrType);
				PyTuple_SetItem(args_tuple, 1, pErrValue);
				PyTuple_SetItem(args_tuple, 2, pErrTraceback);
				PyObject* result = PyObject_Call(format_exception_func, args_tuple, NULL);
				if (result != NULL && PyList_Check(result))
				{
					errString = "";
					for (int i = 0; i < PyList_Size(result); i++)
						errString += std::string(PyUnicode_AsUTF8(PyList_GetItem(result, i)));

					Py_XDECREF(result);
				}

				Py_XDECREF(args_tuple);
			}

			Py_XDECREF(format_exception_func);
			Py_DECREF(traceback_module);
		}
	}

	LOG_MANAGER->log(errString, Ogre::ColourValue(1.0f, 0.8f, 0.2f), 30.0f);
}
