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
    {"name", OgreEngine::script::get_name, METH_VARARGS, "name() -> string"},
    {"create_mesh_component", OgreEngine::script::create_mesh_component, METH_VARARGS, "create_mesh_component('mesh_fname') -> None"},
    {"rotate_local", OgreEngine::script::rotate_world, METH_VARARGS, "rotate_local(degrees, axisx, axisy, axisz) -> None"},
    {"rotate_world", OgreEngine::script::rotate_world, METH_VARARGS, "rotate_world(degrees, axisx, axisy, axisz) -> None"},
    {"scale", OgreEngine::script::scale, METH_VARARGS, "scale(sx, sy, sz) -> None"},
    {"translate_local", OgreEngine::script::translate_local, METH_VARARGS, "translate_local(tx, ty, tz) -> None"},
    {"translate_world", OgreEngine::script::translate_world, METH_VARARGS, "translate_world(tx, ty, tz) -> None"},
    {NULL}  /* Sentinel */
};

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
    return (PyObject*)(OgreEngine::script::GameObject*)type->tp_alloc(type, 0);
}

PyObject* OgreEngine::script::get_name(PyObject* self, PyObject* args)
{
    std::string name = ((script::GameObject*)(PyTypeObject*)self)->mTwin->get_name();
    int size = name.size();
    return PyUnicode_FromStringAndSize(name.c_str(), size);
}

PyObject* OgreEngine::script::create_mesh_component(PyObject* self, PyObject* args)
{
    std::string errMsg;
    if (PyTuple_Check(args) && PyTuple_Size(args) == 1)
    {
        if (PyUnicode_Check(PyTuple_GetItem(args, 0)))
        {
            std::string fileName = PyUnicode_AsUTF8(PyTuple_GetItem(args, 0));
            script::GameObject* thisObj = ((script::GameObject*)(PyTypeObject*)self);
            thisObj->mTwin->create_mesh(thisObj->mTwin->get_name() + "_Mesh", fileName);
            Py_INCREF(Py_None);
            return Py_None;
        }
        else
            errMsg = "create_mesh_component ERROR!! Value Is not of type string!!";
    }
    else
        errMsg = "create_mesh_component ERROR!! Did not get a valid Tuple!! Need to recieve a tuple of size 1 containing a string!!";
    PyErr_SetString(PyExc_ValueError, errMsg.c_str());
    return NULL;
}

PyObject* OgreEngine::script::rotate_local(PyObject* self, PyObject* args)
{
    OgreEngine::package<float> degrees = get_num_from_pytuple<float>(args, 0);
    OgreEngine::package<Ogre::Vector3> axisPackage = get_vector3_from_pytuple(args);
    if (degrees.errSet || axisPackage.errSet)
    {
        PyErr_SetString(PyExc_ValueError, ("rotate_world ERROR!! Degrees: " + degrees.msg + "Axis: " + axisPackage.msg).c_str());
        return NULL;
    }
    script::GameObject* thisObj = ((script::GameObject*)(PyTypeObject*)self);
    thisObj->mTwin->rotate_local(degrees.data, axisPackage.data);
    Py_INCREF(Py_None);
    return Py_None;
}

PyObject* OgreEngine::script::rotate_world(PyObject* self, PyObject* args)
{
    OgreEngine::package<float> degrees = get_num_from_pytuple<float>(args, 0);
    OgreEngine::package<Ogre::Vector3> axisPackage = get_vector3_from_pytuple(args);
    if (degrees.errSet || axisPackage.errSet)
    {
        PyErr_SetString(PyExc_ValueError, ("rotate_world ERROR!! Degrees: "+degrees.msg+"Axis: "+axisPackage.msg).c_str());
        return NULL;
    }
    script::GameObject* thisObj = ((script::GameObject*)(PyTypeObject*)self);
    thisObj->mTwin->rotate_world(degrees.data, axisPackage.data);
    Py_INCREF(Py_None);
    return Py_None;
}

PyObject* OgreEngine::script::scale(PyObject* self, PyObject* args)
{
    OgreEngine::package<Ogre::Vector3> translationPackage = get_vector3_from_pytuple(args);
    if (translationPackage.errSet)
    {
        PyErr_SetString(PyExc_ValueError, ("scale ERROR!! "+translationPackage.msg).c_str());
        return NULL;
    }
    script::GameObject* thisObj = ((script::GameObject*)(PyTypeObject*)self);
    thisObj->mTwin->scale(translationPackage.data);
    Py_INCREF(Py_None);
    return Py_None;
}

PyObject* OgreEngine::script::translate_local(PyObject* self, PyObject* args)
{
    OgreEngine::package<Ogre::Vector3> translationPackage = get_vector3_from_pytuple(args);
    if (translationPackage.errSet)
    {
        PyErr_SetString(PyExc_ValueError, ("tanslate_local ERROR!! "+translationPackage.msg).c_str());
        return NULL;
    }
    script::GameObject* thisObj = ((script::GameObject*)(PyTypeObject*)self);
    thisObj->mTwin->translate_local(translationPackage.data);
    Py_INCREF(Py_None);
    return Py_None;
}

PyObject* OgreEngine::script::translate_world(PyObject* self, PyObject* args)
{
    OgreEngine::package<Ogre::Vector3> translationPackage = get_vector3_from_pytuple(args);
    if (translationPackage.errSet)
    {
        PyErr_SetString(PyExc_ValueError, ("tanslate_world ERROR!! "+translationPackage.msg).c_str());
        return NULL;
    }
    script::GameObject* thisObj = ((script::GameObject*)(PyTypeObject*)self);
    thisObj->mTwin->translate_world(translationPackage.data);
    Py_INCREF(Py_None);
    return Py_None;
}
