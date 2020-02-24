// dllmain.cpp : Defines the entry point for the DLL application.
#include <python.h>
#include <iostream>
#include <sstream>
#include <regex>
#include <list>

PyObject* my_func(PyObject* self, PyObject* args)
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
			for(int i = 0; i < indicies->size(); i++)
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

PyMethodDef my_functions[] = {
	{"test", my_func, METH_VARARGS, "test's docstring"},
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