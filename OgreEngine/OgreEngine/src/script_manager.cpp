#include <script_manager.h>
#include <log_manager.h>

OgreEngine::ScriptManager::ScriptManager()
{
	Py_Initialize();
	PyImport_AppendInittab("ogre_module", PyInit_ogre_module);
}

void OgreEngine::ScriptManager::handle_error()
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
