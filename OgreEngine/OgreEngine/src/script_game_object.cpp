#include <stdafx.h>
#include <script_game_object.h>
#include <script_game_object_methods.h>

// Note: This file is the definition of the ssuge.GameObject class.  It mainly consists of the 
// "big-giant-structure" that lists all "special" methods (like __init__, __add__, etc.).
// The GameObject_methods (defined in script_game_object_methods.cpp), has any non-special methods
// we want to expose (like translate_world).

extern PyMethodDef GameObject_methods[];                    // Defined in script_game_object_methods.cpp


// The "big-giant-structure".  Any time we need to specify the "type" object for a python ssuge.GameObject,
// pass the address of this thing (which can be safely case as a PyObject*).
PyTypeObject GameObjectType =
{
    PyVarObject_HEAD_INIT(NULL, 0)

    "ssuge.GameObject",                                     // tp_name: For printing, in format "<module>.<name>" 
    sizeof(OgreEngine::script::GameObject),                 // tp_size: size (in bytes) of our Object
    0,                                                      // tp_itemsize

    /* Methods to implement standard operations */
    (destructor)OgreEngine::script::GameObject_dealloc,     // tp_dealloc
    NULL,                                                   // tp_vectorcall_offset
    NULL,                                                   // tp_getattr;
    NULL,                                                   // tp_setattr;
    NULL,                                                   // tp_as_async;                           
    NULL,                                                   // tp_repr;

    /* Method suites for standard classes */
    NULL,                                                   // tp_as_number;
    NULL,                                                   // tp_as_sequence;
    NULL,                                                   // tp_as_mapping;

    /* More standard operations (here for binary compatibility) */
    NULL,                                                   // tp_hash;
    NULL,                                                   // tp_call;
    NULL,                                                   // tp_str;
    NULL,                                                   // tp_getattro;
    NULL,                                                   // tp_setattro;

    /* Functions to access object as input/output buffer */
    NULL,                                                   // tp_as_buffer;

    /* Flags to define presence of optional/expanded features */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,               // tp_flags;

    "GameObject's docstring",                               // tp_doc; 

    /* Assigned meaning in release 2.0 */
    /* call function for all accessible objects */
    NULL,                                                   // tp_traverse;

    /* delete references to contained objects */
    NULL,                                                   // tp_clear;

    /* Assigned meaning in release 2.1 */
    /* rich comparisons */
    NULL,                                                   // tp_richcompare;

    /* weak reference enabler */
    NULL,                                                   // tp_weaklistoffset;

    /* Iterators */
    NULL,                                                   // tp_iter;
    NULL,                                                   // tp_iternext;

    /* Attribute descriptor and subclassing stuff */
    GameObject_methods,                                     // tp_methods;
    NULL,                                                   // tp_members;
    NULL,                                                   // struct PyGetSetDef* tp_getset;
    NULL,                                                   // struct _typeobject* tp_base;
    NULL,                                                   // PyObject* tp_dict;
    NULL,                                                   // descrgetfunc tp_descr_get;
    NULL,                                                   // descrsetfunc tp_descr_set;
    0,                                                      // Py_ssize_t tp_dictoffset;
    (initproc)OgreEngine::script::GameObject_init,          // tp_init;
    NULL,                                                   // tp_alloc;
    OgreEngine::script::GameObject_new,                     // tp_new;
    NULL,                                                   // tp_free; /* Low-level free-memory routine */
    0,                                                      // tp_is_gc; /* For PyObject_IS_GC */
    NULL,                                                   // PyObject* tp_bases;
    NULL,                                                   // tp_mro; /* method resolution order */
    NULL,                                                   // tp_cache;
    NULL,                                                   // tp_subclasses;
    NULL,                                                   // PyObject* tp_weaklist;
    NULL,                                                   // tp_del;
};
    