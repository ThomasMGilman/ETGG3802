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
    {"translate_world", OgreEngine::script::translate_world, METH_VARARGS, "Translates the object in world space by x, y, z amount specified."},
    {"rotate_world", OgreEngine::script::rotate_world, METH_VARARGS, "Rotates the object in world space by the degrees specified along the x, y, and z axis amount specified."},
    {"scale", OgreEngine::script::scale, METH_VARARGS, "Scales the object by the x, y, and z amount specified."},
    {"name", OgreEngine::script::get_name, METH_VARARGS, "returns the name of the object."},
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
    //LOG_MANAGER->log_message("creating mesh!!", RAND_COLOUR, 10.0f);
    if (PyTuple_Check(args) && PyTuple_Size(args) == 1)
    {
        if(PyUnicode_Check(PyTuple_GetItem(args, 0)))
        {
            std::string fileName = PyUnicode_AsUTF8(PyTuple_GetItem(args, 0));
            script::GameObject* thisObj = ((script::GameObject*)(PyTypeObject*)self);
            thisObj->mTwin->create_mesh(thisObj->mTwin->get_name() + "_Mesh", fileName);
        }
        else
        {
            PyErr_SetString(PyExc_ValueError, "Value Is not of type string!!");
            return NULL;
        }
    }
    else
    {
        PyErr_SetString(PyExc_ValueError, "Did not get a valid Tuple!! Need to recieve a tuple of size 1 containing a string!!");
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}

PyObject* OgreEngine::script::rotate_world(PyObject* self, PyObject* args)
{
    //LOG_MANAGER->log_message("Rotating world!!", RAND_COLOUR, 10.0f);
    if (PyTuple_Check(args) && PyTuple_Size(args) == 4)
    {
        OgreEngine::package<float> degrees = get_num_from_pytuple<float>(args, 0);
        OgreEngine::package<float> xAmount = get_num_from_pytuple<float>(args, 1);
        OgreEngine::package<float> yAmount = get_num_from_pytuple<float>(args, 2);
        OgreEngine::package<float> zAmount = get_num_from_pytuple<float>(args, 3);
        if (!degrees.errSet && !xAmount.errSet && !yAmount.errSet && !zAmount.errSet)
        {
            //LOG_MANAGER->log_message("Rotating!! "+ std::to_string(degrees.data) + "," + std::to_string(xAmount.data) + "," + std::to_string(yAmount.data) + "," + std::to_string(zAmount.data), RAND_COLOUR, 10.0f);
            script::GameObject* thisObj = ((script::GameObject*)(PyTypeObject*)self);
            thisObj->mTwin->rotate_world(degrees.data, xAmount.data, yAmount.data, zAmount.data);
            Py_INCREF(Py_None);
            return Py_None;
        }
        std::string excMsg = "Rotate_world ERROR!! Internal Tuple Value Error!!\n\tRotationVal: " + degrees.msg +
            "\n\txVal: " + xAmount.msg + "\n\tyVal: " + yAmount.msg + "\n\tzVal: " + zAmount.msg;
        PyErr_SetString(PyExc_ValueError, excMsg.c_str());
        return NULL;
    }
    PyErr_SetString(PyExc_ValueError, "Rotate_world ERROR!! Did not get a valid Tuple!! Need to recieve a tuple of size 4 containing Floats!!");
    return NULL;
}

PyObject* OgreEngine::script::scale(PyObject* self, PyObject* args)
{
    //LOG_MANAGER->log_message("Scaling!!", RAND_COLOUR, 10.0f);
    if (PyTuple_Check(args) && PyTuple_Size(args) == 3)
    {
        OgreEngine::package<float> xAmount = get_num_from_pytuple<float>(args, 0);
        OgreEngine::package<float> yAmount = get_num_from_pytuple<float>(args, 1);
        OgreEngine::package<float> zAmount = get_num_from_pytuple<float>(args, 2);
        if (!xAmount.errSet && !yAmount.errSet && !zAmount.errSet)
        {
            //LOG_MANAGER->log_message("Scaling!! " + std::to_string(xAmount.data) + "," + std::to_string(yAmount.data) + "," + std::to_string(zAmount.data), RAND_COLOUR, 10.0f);
            script::GameObject* thisObj = ((script::GameObject*)(PyTypeObject*)self);
            thisObj->mTwin->scale(xAmount.data, yAmount.data, zAmount.data);
            Py_INCREF(Py_None);
            return Py_None;
        }
        std::string excMsg = "Scale ERROR!! Internal Tuple Value Error!!\n\txVal" + 
            xAmount.msg + "\n\tyVal: " + yAmount.msg + "\n\tzVal: " + zAmount.msg;
        PyErr_SetString(PyExc_ValueError, excMsg.c_str());
        return NULL;
    }
    PyErr_SetString(PyExc_ValueError, "Scale ERROR!! Did not get a valid Tuple!! Need to recieve a tuple of size 3 containing Floats!!");
    return NULL;
}

PyObject* OgreEngine::script::translate_world(PyObject* self, PyObject* args)
{
    //LOG_MANAGER->log_message("Translating!!", RAND_COLOUR, 10.0f);
    if (PyTuple_Check(args) && PyTuple_Size(args) == 3)
    {
        OgreEngine::package<float> xAmount = get_num_from_pytuple<float>(args, 0);
        OgreEngine::package<float> yAmount = get_num_from_pytuple<float>(args, 1);
        OgreEngine::package<float> zAmount = get_num_from_pytuple<float>(args, 2);
        if (!xAmount.errSet && !yAmount.errSet && !zAmount.errSet)
        {
            //LOG_MANAGER->log_message("Translating!! "+std::to_string(xAmount.data)+","+ std::to_string(yAmount.data) + ","+ std::to_string(zAmount.data), RAND_COLOUR, 10.0f);
            script::GameObject* thisObj = ((script::GameObject*)(PyTypeObject*)self);
            thisObj->mTwin->translate_world(xAmount.data, yAmount.data, zAmount.data);
            Py_INCREF(Py_None);
            return Py_None;
        }
        std::string excMsg = "Translate_world ERROR!! Internal Tuple Value Error!!\n\txVal" +
            xAmount.msg + "\n\tyVal: " + yAmount.msg + "\n\tzVal: " + zAmount.msg;
        PyErr_SetString(PyExc_ValueError, excMsg.c_str());
        return NULL;
    }
    PyErr_SetString(PyExc_ValueError, "Translate_world ERROR!! Did not get a valid Tuple!! Need to recieve a tuple of size 3 containing Floats!!");
    return NULL;
}
