// dllmain.cpp : Defines the entry point for the DLL application.
#include <python.h>

PyObject* my_func(PyObject* self, PyObject* args)
{
	//self is the module we are a part of
	// args is a tuple
	if (PyTuple_Check(args) && PyTuple_Size(args) == 2)
	{
		PyObject* inString = PyTuple_GetItem(args, 0);
		PyObject* charsToCheckFor = PyTuple_GetItem(args, 1);
		PyObject* listOfIndicies = PyList_New()
	}
	else
		PyErr_SetString(PyExc_ValueError, "Did not get arguement of type Tuple");
	return nullptr;
}

PyObject* get_val(PyObject* self, PyObject* args)
{
	return (PyObject*)42;
}

PyMethodDef my_functions[] = {
	{"test", my_func, METH_VARARGS, "test's docstring"},
	{"mystery_val", get_val, 0, "get not random value"},
	{NULL, NULL, 0, NULL}
};

struct PyModuleDef test_module = {
	PyModuleDef_HEAD_INIT,			// data to place at beginning
	"test functions",
	"Set of test functions",
	-1,								// -1 = keep module state in globals
	my_functions					// (from last slide)
};

PyMODINIT_FUNC PyInit_test_module(void)
{
	printf("in the init function\n");
	PyObject* m = PyModule_Create(&test_module);
	PyModule_AddIntConstant(m, "mystery_val", 42);
	return m == NULL ? NULL : m;
}