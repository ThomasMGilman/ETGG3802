#include <stdafx.h>
#include <script_manager.h>
#include <script_functions.h>
#include <log_manager.h>

using namespace OgreEngine;

ScriptManager* ScriptManager::msSingleton = nullptr;

PyMethodDef my_functions[] = {
		{"find_string_match_indicies", find_string_match_indicies, METH_VARARGS, "Takes a two argument tuple of strings.\n\tReturns the indicies of all matches of the 2nd string within the 1st string.\n\tIf no matches, returns None"},
		{"log", log, METH_VARARGS, "Takes a Tuple containing the message to log, optional color value, and optional time to stay on screen for."},
		{"set_skybox", set_skybox, METH_VARARGS, "Takes a material name to set as the background skybox texture and an optinal distance from the viewer."},
		{NULL, NULL, 0, NULL}
};

struct PyModuleDef ogre_module = {
	PyModuleDef_HEAD_INIT,			// data to place at beginning
	"ogre_module",
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

ScriptManager::ScriptManager()
{
	PyImport_AppendInittab("ogre_module", PyInit_ogre_module);
	Py_Initialize();
	
}

ScriptManager::~ScriptManager()
{
	Py_Finalize();
}

void OgreEngine::ScriptManager::run_script(std::string fileName)
{
	std::ifstream is(fileName, std::ifstream::binary);
	if (is)
	{
		is.seekg(0, is.end);
		int len = is.tellg();
		is.seekg(0, is.beg);
		
		char* buffer = new char[len+1];
		is.read(buffer, len);
		buffer[len] = '\0';
		is.close();

		PyObject* pyScript = Py_CompileString(buffer, fileName.c_str(), Py_file_input);
		if (pyScript)
		{
			PyObject* pyImportExec = PyImport_ExecCodeModule(fileName.c_str(), pyScript);
			if (pyImportExec)
				LOG_MANAGER->log("Successfully loaded script: " + fileName);
			else
				handle_error();
		}
		else
			handle_error();
		delete(buffer);
	}
	else
		LOG_MANAGER->log_message("FAILED TO OPEN AND READ SCRIPT IN SCRIPT MANAGER\nScript: " + fileName, ERROR_COLOUR);
}

void ScriptManager::handle_error()
{
	// If there is no error, return!
	if (!PyErr_Occurred())
		return;
	else
	{
		PyObject *errType, *errVal, *errTraceback;
		PyErr_Fetch(&errType, &errVal, &errTraceback);
		PyErr_NormalizeException(&errType, &errVal, &errTraceback);

		// If no stack trace: a syntax error occurred
		if (errTraceback == NULL)
		{
			PyObject* tmpValStr = PyObject_Str(errVal);
			LOG_MANAGER->log_message(PyUnicode_AsUTF8(tmpValStr), ERROR_COLOUR, 10.0f);
			Py_DECREF(tmpValStr);
		}
		else
		{
			// Import the traceback module and call it
			PyObject* traceback_mod = PyImport_ImportModule("traceback");
			if (traceback_mod)
			{
				// Get the format exception function out of the module
				PyObject* format_exception_func = PyObject_GetAttrString(traceback_mod, "format_exception");
				if (format_exception_func && PyCallable_Check(format_exception_func))
				{
					// The format_exception fuction takes 3 parameters
					// We borrowed the reference to the 3 error objects -- when we add them to
					// the argument tuple, it'll steal a reference. ICREF them so they dont get
					// garbage collected
					PyObject* args_tuple = PyTuple_Pack(3, errType, errVal, errTraceback);

					// Actually call the function. Will give us NULL if there was an exception
					// (our error-handling function errored:-(). More likely: we'll get a list of)
					// strings (we own the reference)
					PyObject* result = PyObject_Call(format_exception_func, args_tuple, NULL);
					if (result && PyList_Check(result))
					{
						for (int i = 0; i < PyList_Size(result); i++)
							LOG_MANAGER->log_message(PyUnicode_AsUTF8(PyList_GetItem(result, i)), ERROR_COLOUR, 10.0f);
					}
					else
						LOG_MANAGER->log_message("'format_exception_function' ERROR!! returned NULL from result passed!!!", ERROR_COLOUR, 10.0f);

					Py_XDECREF(result);
					Py_DECREF(args_tuple);
				}
				else
					LOG_MANAGER->log_message("'format_exception_function' ERROR!! could not get format_exception_function!!!", ERROR_COLOUR, 10.0f);

				Py_XDECREF(format_exception_func);		// make sure its null before decref'ing
				Py_DECREF(traceback_mod);
			}
			else
				LOG_MANAGER->log_message("'traceback_module' ERROR!! Did not get traceback module!!!", ERROR_COLOUR, 10.0f);
		}
	}
}
