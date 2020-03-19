#include <stdafx.h>
#include <script_game_object_methods.h>
#include <script_manager.h>
#include <log_manager.h>

// NOTE: This file is used to define any methods of our python ssuge.GameObject class.  Put
// the declaration / prototype of any new functions in script_game_object_methods.h.  Then
// if this is a "special" method (e.g. __init__, __new__, __del__, __mul__, etc.), put the
// function pointer to one of these in the "big-giant-structure".  If it's a non-special 
// method (e.g. translate_world), add an entry to the method_def structure at the bottom of this file.


void OgreEngine::script::GameObject_dealloc(OgreEngine::script::GameObject* self)
{
    Py_TYPE(self)->tp_free((PyObject*)self);
}

int OgreEngine::script::GameObject_init(OgreEngine::script::GameObject* self, PyObject* args, PyObject* kwds)
{
    // Reset our attributes
    return 0;
}

PyObject* OgreEngine::script::GameObject_new(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
    return (PyObject*)(GameObject*)type->tp_alloc(type, 0);
}

PyObject* OgreEngine::script::create_game_object(PyObject* self, PyObject* args)
{
    return nullptr;
}

PyObject* OgreEngine::script::create_mesh_component(PyObject* self, PyObject* args)
{
}

PyMethodDef GameObject_methods[] = {
    {"create_game_object", OgreEngine::script::create_game_object, METH_VARARGS, "Creates a new object given a group name, object name, tag, and other parameters."},
    //{"a_new_method", OgreEngine::script::GameObject_???, METH_VARARGS, "its docstring"},
    {NULL}  /* Sentinel */
};