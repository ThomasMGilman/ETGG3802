#include <stdafx.h>
#include <script_manager.h>
#include <script_functions.h>
#include <script_game_object.h>
#include <log_manager.h>

using namespace OgreEngine;

extern PyTypeObject GameObjectType;

ScriptManager* ScriptManager::msSingleton = nullptr;

PyMethodDef my_functions[] = {
	{"load_script", load_script, METH_VARARGS, "load_script('script-path-and-fname') -> None"},
	{"load_scene", load_scene, METH_VARARGS, "load_scene(scene_fname) -> None"},
	{"load_collider_visualizations", load_collider_visualizations, METH_VARARGS, "load_collider_visualizations() -> None"},
	{"find_all_string_match_indicies", find_all_string_match_indicies, METH_VARARGS, "find_all_string_match_indicies(\"givenString\", \"stringToFind\")"},
	{"log", log, METH_VARARGS, "log('my message', color=None, distance=None) -> None"},
	{"create_game_object", create_python_game_object, METH_VARARGS, "create_game_object('my_group', 'my_name', 42, other_game_object=None, script_fname=None)"},
	{"set_skybox", set_skybox, METH_VARARGS, "set_skybox(\"MySkyboxMaterial\") -> None"},
	{"raycast", raycast, METH_VARARGS, "raycast(origin(x,y,z), direction(x,y,z), mask(int), ray_dist(float)) -> {((x,y,z), PyObject*), ...} | None"},
	{"register_input_listener", register_input_listener, METH_VARARGS, "register_input_listener(game_object) -> None"},
	{"deregister_input_listener", deregister_input_listener, METH_VARARGS, "deregister_input_listener(game_object) -> None"},
	{"has_action", has_action, METH_VARARGS, "has_action(action_name) -> bool"},
	{"get_action", get_action, METH_VARARGS, "get_action(action_name) -> bool"},
	{"has_axis", has_axis, METH_VARARGS, "has_axis(axis_name) -> bool"},
	{"get_axis", get_axis, METH_VARARGS, "get_axis(axis_name) -> float"},
	{"queue_destroy", queue_destroy, METH_VARARGS, "queue_destroy(PyObject*) -> None"},
	{"build_edge_lists", build_edge_list, METH_VARARGS, "build_edge_list(groupName) -> None"},
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
	PyObject* m = PyModule_Create(&ogre_module);
	PyType_Ready((PyTypeObject*)&GameObjectType);
	Py_IncRef((PyObject*)&GameObjectType);
	if (PyModule_AddObject(m, "GameObject", (PyObject*)&GameObjectType) < 0)
	{
		LOG_MANAGER->log_message("Failed to add GameObject Functions to Ogre_Module for Python use!!", ERROR_COLOUR, ERROR_DISPLAY_TIME);
		Py_DECREF((PyObject*)&GameObjectType);
		Py_DECREF(m);
		return NULL;
	}
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
			{
				PyObject* key, * value, * dict = PyModule_GetDict(pyImportExec);
				Py_ssize_t pos = 0;
				std::string modName = PyModule_GetName(pyImportExec);
				while (PyDict_Next(dict, &pos, &key, &value))
				{
					if (PyType_Check(value))
					{
						std::string classString;
						if (PyUnicode_Check(key))
							classString = PyUnicode_AsUTF8(key);

						if (PyClasses.find(classString) == PyClasses.end())
						{
							Py_IncRef(value);
							PyClasses[classString] = value;
							LOG_MANAGER->log_message("Got Class: " + classString, RAND_COLOUR, 10.0f);
						}
					}
				}
				LOG_MANAGER->log("Successfully loaded script: " + fileName);
				
			}
			else
				handle_error();
		}
		else
			handle_error();
		delete(buffer);
	}
	else
		LOG_MANAGER->log_message("FAILED TO OPEN AND READ SCRIPT IN SCRIPT MANAGER\nScript: " + fileName, ERROR_COLOUR, ERROR_DISPLAY_TIME);
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
			LOG_MANAGER->log_message(PyUnicode_AsUTF8(tmpValStr), ERROR_COLOUR, ERROR_DISPLAY_TIME);
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
							LOG_MANAGER->log_message(PyUnicode_AsUTF8(PyList_GetItem(result, i)), ERROR_COLOUR, ERROR_DISPLAY_TIME);
					}
					else
						LOG_MANAGER->log_message("'format_exception_function' ERROR!! returned NULL from result passed!!!", ERROR_COLOUR, ERROR_DISPLAY_TIME);

					Py_XDECREF(result);
					Py_DECREF(args_tuple);
				}
				else
					LOG_MANAGER->log_message("'format_exception_function' ERROR!! could not get format_exception_function!!!", ERROR_COLOUR, ERROR_DISPLAY_TIME);

				Py_XDECREF(format_exception_func);		// make sure its null before decref'ing
				Py_DECREF(traceback_mod);
			}
			else
				LOG_MANAGER->log_message("'traceback_module' ERROR!! Did not get traceback module!!!", ERROR_COLOUR, ERROR_DISPLAY_TIME);
		}
	}
}
