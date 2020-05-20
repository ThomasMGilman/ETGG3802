#include <stdafx.h>
#include <script_game_object_methods.h>
#include <script_manager.h>
#include <application.h>

// NOTE: This file is used to define any methods of our python ssuge.GameObject class.  Put
// the declaration / prototype of any new functions in script_game_object_methods.h.  Then
// if this is a "special" method (e.g. __init__, __new__, __del__, __mul__, etc.), put the
// function pointer to one of these in the "big-giant-structure".  If it's a non-special 
// method (e.g. translate_world), add an entry to the method_def structure at the bottom of this file.


//@@@@@@@@@@@ SPECIAL METHODS (referenced from GameObjectType) @@@@@@@@@@@@@
void ssuge::script::GameObject_dealloc(ssuge::script::GameObject* self)
{
    Py_TYPE(self)->tp_free((PyObject*)self);
}


int ssuge::script::GameObject_init(ssuge::script::GameObject* self, PyObject* args, PyObject* kwds)
{
    // Reset our attributes
    return 0;
}


PyObject* ssuge::script::GameObject_new(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
    GameObject* self;
    self = (GameObject*)type->tp_alloc(type, 0);
    if (self != NULL)
    {
        // Initialize our other attributes
    }
    return (PyObject*)self;
}




//@@@@@@@@@@@ Non-SPECIAL METHODS (referenced from the method table-of-contents) @@@@@@@@@@@@@ 
PyObject* ssuge::script::GameObject_create_mesh_component(PyObject* self, PyObject* args)
{
    if (!PyTuple_Check(args) || PyTuple_Size(args) != 1 || !PyUnicode_Check(PyTuple_GetItem(args, 0)))
    {
        PyErr_SetString(PyExc_ValueError, "usage: create_mesh_component('mesh_fname') -> None");
        return NULL;
    }

    ssuge::script::GameObject* python_gobj = (ssuge::script::GameObject*)self;
    ssuge::GameObject* gobj = python_gobj->mTwin;
    std::string mesh_fname = SCRIPT_MANAGER->get_string_from_pyobject(PyTuple_GetItem(args, 0));

    gobj->create_mesh_component(mesh_fname);

    Py_INCREF(Py_None);
    return Py_None;
}


/// gobj.create_collider_component("sphere", 1.0, 5, 0x07) -> None
        /// gobj.create_collider_component("box", 2.3, 1.5, 4.7, 5, 0x07) -> None
        ///    where 1.0 is the sphere's radius
        ///    5 is the collider layer (0 - 31)
        ///    0x07 is a collider mask.  If the 1 << *other* object's layer anded with our mask is True, we'll
        ///        be notified that there is a collision
        ///    2.3, 1.5, 4.7 are the box extents along the local x, y, and z axes.
PyObject* ssuge::script::GameObject_create_collider_component(PyObject* self, PyObject* args)
{
    ssuge::script::GameObject* python_gobj = (ssuge::script::GameObject*)self;
    ssuge::GameObject* gobj = python_gobj->mTwin;

    if (PyTuple_Check(args) && PyTuple_Size(args) == 4 && PyUnicode_Check(PyTuple_GetItem(args, 0)) &&
        SCRIPT_MANAGER->get_string_from_pyobject(PyTuple_GetItem(args, 0)) == "sphere")
    {
        if (!PyNumber_Check(PyTuple_GetItem(args, 1)) || !PyLong_Check(PyTuple_GetItem(args, 2)) ||
            !PyLong_Check(PyTuple_GetItem(args, 3)))
        {
            PyErr_SetString(PyExc_ValueError, "usage: gobj.create_collider_component('sphere', 1.0, 5, 0x07)");
            return NULL;
        }

        int layer = PyLong_AsLong(PyTuple_GetItem(args, 2));
        long long mask = (long long)PyLong_AsLong(PyTuple_GetItem(args, 3));
        PyObject* float_obj = PyNumber_Float(PyTuple_GetItem(args, 1));
        float radius = (float)PyFloat_AsDouble(float_obj);
        Py_DECREF(float_obj);

        gobj->create_sphere_collider_component(layer, mask, radius);
    }
    else if (PyTuple_Check(args) && PyTuple_Size(args) == 6 && PyUnicode_Check(PyTuple_GetItem(args, 0)) &&
        SCRIPT_MANAGER->get_string_from_pyobject(PyTuple_GetItem(args, 0)) == "box")
    {
        if (!PyNumber_Check(PyTuple_GetItem(args, 1)) || !PyNumber_Check(PyTuple_GetItem(args, 2)) ||
            !PyNumber_Check(PyTuple_GetItem(args, 3)) || !PyLong_Check(PyTuple_GetItem(args, 4)) ||
            !PyLong_Check(PyTuple_GetItem(args, 5)))
        {
            PyErr_SetString(PyExc_ValueError, "usage: gobj.create_collider_component('box', 2.3, 1.5, 4.7, 5, 0x07)");
            return NULL;
        }

        Ogre::Vector3 extents;
        for (int i = 0; i < 3; i++)
        {
            int args_index = i + 1;
            PyObject* float_val = PyNumber_Float(PyTuple_GetItem(args, args_index));
            extents[i] = (float)PyFloat_AsDouble(float_val);
            Py_DECREF(float_val);
        }
        int layer = PyLong_AsLong(PyTuple_GetItem(args, 2));
        long long mask = (long long)PyLong_AsLong(PyTuple_GetItem(args, 3));

        gobj->create_box_collider_component(layer, mask, extents);
    }
    else
    {
        std::string temps = "usage1: gobj.create_collider_component('box', 2.3, 1.5, 4.7, 5, 0x07)\n";
        temps += "usage2: gobj.create_collider_component('sphere', 1.0, 5, 0x07)";
        PyErr_SetString(PyExc_ValueError, temps.c_str());
        return NULL;
    }

    // If we get here, there were no errors
    Py_INCREF(Py_None);
    return Py_None;
}


PyObject* ssuge::script::GameObject_create_collider_visualization(PyObject* self, PyObject* args)
{
    if (!PyTuple_Check(args) || PyTuple_Size(args) != 0)
    {
        PyErr_SetString(PyExc_ValueError, "call this method with no arguments");
        return NULL;
    }

    ssuge::script::GameObject* python_gobj = (ssuge::script::GameObject*)self;
    ssuge::GameObject* gobj = python_gobj->mTwin;
    ssuge::ComponentCollider* coll_comp = gobj->get_collider_component();

    if (coll_comp != NULL)
        coll_comp->create_collider_visualization();

    Py_INCREF(Py_None);
    return Py_None;
}



PyObject* ssuge::script::GameObject_get_world_position(PyObject* self, PyObject* args)
{
    ssuge::script::GameObject* python_gobj = (ssuge::script::GameObject*)self;
    ssuge::GameObject* gobj = python_gobj->mTwin;

    Ogre::Vector3 pos = gobj->get_world_position();

    PyObject* rval = PyTuple_New(3);
    PyTuple_SetItem(rval, 0, PyFloat_FromDouble((double)pos.x));
    PyTuple_SetItem(rval, 1, PyFloat_FromDouble((double)pos.y));
    PyTuple_SetItem(rval, 2, PyFloat_FromDouble((double)pos.z));
    return rval;
}



PyObject* ssuge::script::GameObject_name(PyObject* self, PyObject* args)
{
    ssuge::script::GameObject* python_gobj = (ssuge::script::GameObject*)self;
    ssuge::GameObject* gobj = python_gobj->mTwin;

    return PyUnicode_FromString(gobj->get_name().c_str());
}



PyObject* ssuge::script::GameObject_tag(PyObject* self, PyObject* args)
{
    ssuge::script::GameObject* python_gobj = (ssuge::script::GameObject*)self;
    ssuge::GameObject* gobj = python_gobj->mTwin;

    return PyLong_FromLong(gobj->get_tag());
}





PyObject* ssuge::script::GameObject_parent(PyObject* self, PyObject* args)
{
    if (!PyTuple_Check(args) || PyTuple_Size(args) != 0)
    {
        PyErr_SetString(PyExc_ValueError, "pass no arguments to this method");
        return NULL;
    }

    ssuge::script::GameObject* python_gobj = (ssuge::script::GameObject*)self;
    ssuge::GameObject* gobj = python_gobj->mTwin;
    ssuge::GameObject* parent_gobj = gobj->get_parent();

    if (parent_gobj == NULL)
    {
        Py_INCREF(Py_None);
        return Py_None;
    }
    else
    {
        PyObject* parent_python_obj = parent_gobj->get_script_twin();
        Py_INCREF(parent_python_obj);
        return parent_python_obj;
    }
}




PyObject* ssuge::script::GameObject_relative_point(PyObject* self, PyObject* args)
{
    if (!PyTuple_Check(args) || PyTuple_Size(args) != 3)
    {
        PyErr_SetString(PyExc_ValueError, "You must 3 numbers: x, y, z");
        return NULL;
    }

    Ogre::Vector3 nums;
    for (int i = 0; i < 3; i++)
    {
        if (PyNumber_Check(PyTuple_GetItem(args, i)))
        {
            PyObject* fval = PyNumber_Float(PyTuple_GetItem(args, i));
            nums[i] = (float)PyFloat_AsDouble(fval);
            Py_DECREF(fval);
        }
        else
        {
            PyErr_SetString(PyExc_ValueError, "all 3 arguments must be numbers");
            return NULL;
        }
    }

    ssuge::script::GameObject* python_gobj = (ssuge::script::GameObject*)self;
    Ogre::Vector3 wpos = python_gobj->mTwin->get_relative_point_position(nums);
    PyObject* result = PyTuple_New(3);
    for (int i = 0; i < 3; i++)
        PyTuple_SetItem(result, i, PyFloat_FromDouble((double)wpos[i]));

    return result;
}


PyObject* ssuge::script::GameObject_rotate_world(PyObject* self, PyObject* args)
{
    if (!PyTuple_Check(args) || PyTuple_Size(args) != 4 || !PyNumber_Check(PyTuple_GetItem(args, 0)) ||
        !PyNumber_Check(PyTuple_GetItem(args, 1)) || !PyNumber_Check(PyTuple_GetItem(args, 2)) ||
        !PyNumber_Check(PyTuple_GetItem(args, 3)))
    {
        PyErr_SetString(PyExc_ValueError, "usage: rotate_world(degrees, axis-x, axis-y, axis-z)");
        return NULL;
    }

    float degrees;
    Ogre::Vector3 axis;
    for (int i = 0; i < 4; i++)
    {
        PyObject* fobj = PyNumber_Float(PyTuple_GetItem(args, i));
        float val = (float)PyFloat_AsDouble(fobj);
        Py_DECREF(fobj);
        switch (i)
        {
        case 0: degrees = val;  break;
        case 1: axis.x = val; break;
        case 2: axis.y = val; break;
        case 3: axis.z = val; break;
        }
    }

    ssuge::script::GameObject* python_gobj = (ssuge::script::GameObject*)self;
    ssuge::GameObject* gobj = python_gobj->mTwin;

    gobj->rotate_world(degrees, axis.x, axis.y, axis.z);

    Py_INCREF(Py_None);
    return Py_None;
}


PyObject* ssuge::script::GameObject_scale(PyObject* self, PyObject* args)
{
    if (!PyTuple_Check(args) || PyTuple_Size(args) != 3 || !PyNumber_Check(PyTuple_GetItem(args, 0)) ||
        !PyNumber_Check(PyTuple_GetItem(args, 1)) || !PyNumber_Check(PyTuple_GetItem(args, 2)))
    {
        PyErr_SetString(PyExc_ValueError, "usage: scale(sx, sy, sz) -> None");
        return NULL;
    }

    float sfactor[3];
    for (int i = 0; i < 3; i++)
    {
        PyObject* fobj = PyNumber_Float(PyTuple_GetItem(args, i));
        sfactor[i] = (float)PyFloat_AsDouble(fobj);
        Py_DECREF(fobj);
    }

    ssuge::script::GameObject* python_gobj = (ssuge::script::GameObject*)self;
    ssuge::GameObject* gobj = python_gobj->mTwin;

    gobj->scale(sfactor[0], sfactor[1], sfactor[2]);

    Py_INCREF(Py_None);
    return Py_None;
}


PyObject* ssuge::script::GameObject_set_world_position(PyObject* self, PyObject* args)
{
    if (!PyTuple_Check(args) || PyTuple_Size(args) != 3 || !PyNumber_Check(PyTuple_GetItem(args, 0)) ||
        !PyNumber_Check(PyTuple_GetItem(args, 1)) || !PyNumber_Check(PyTuple_GetItem(args, 2)))
    {
        PyErr_SetString(PyExc_ValueError, "usage: set_world_position(x, y, z) -> None");
        return NULL;
    }

    float position[3];
    for (int i = 0; i < 3; i++)
    {
        PyObject* fobj = PyNumber_Float(PyTuple_GetItem(args, i));
        position[i] = (float)PyFloat_AsDouble(fobj);
        Py_DECREF(fobj);
    }

    ssuge::script::GameObject* python_gobj = (ssuge::script::GameObject*)self;
    ssuge::GameObject* gobj = python_gobj->mTwin;

    gobj->set_position_world(position[0], position[1], position[2]);

    Py_INCREF(Py_None);
    return Py_None;
}


PyObject* ssuge::script::GameObject_translate_local(PyObject* self, PyObject* args)
{
    if (!PyTuple_Check(args) || PyTuple_Size(args) != 3 || !PyNumber_Check(PyTuple_GetItem(args, 0)) ||
        !PyNumber_Check(PyTuple_GetItem(args, 1)) || !PyNumber_Check(PyTuple_GetItem(args, 2)))
    {
        PyErr_SetString(PyExc_ValueError, "usage: translate_local(tx, ty, tz) -> None");
        return NULL;
    }

    float translation[3];
    for (int i = 0; i < 3; i++)
    {
        PyObject* fobj = PyNumber_Float(PyTuple_GetItem(args, i));
        translation[i] = (float)PyFloat_AsDouble(fobj);
        Py_DECREF(fobj);
    }

    ssuge::script::GameObject* python_gobj = (ssuge::script::GameObject*)self;
    ssuge::GameObject* gobj = python_gobj->mTwin;

    gobj->translate_local(translation[0], translation[1], translation[2]);

    Py_INCREF(Py_None);
    return Py_None;
}


PyObject* ssuge::script::GameObject_translate_world(PyObject* self, PyObject* args)
{
    if (!PyTuple_Check(args) || PyTuple_Size(args) != 3 || !PyNumber_Check(PyTuple_GetItem(args, 0)) ||
        !PyNumber_Check(PyTuple_GetItem(args, 1)) || !PyNumber_Check(PyTuple_GetItem(args, 2)))
    {
        PyErr_SetString(PyExc_ValueError, "usage: translate_world(tx, ty, tz) -> None");
        return NULL;
    }

    float translation[3];
    for (int i = 0; i < 3; i++)
    {
        PyObject* fobj = PyNumber_Float(PyTuple_GetItem(args, i));
        translation[i] = (float)PyFloat_AsDouble(fobj);
        Py_DECREF(fobj);
    }

    ssuge::script::GameObject* python_gobj = (ssuge::script::GameObject*)self;
    ssuge::GameObject* gobj = python_gobj->mTwin;

    gobj->translate_world(translation[0], translation[1], translation[2]);

    Py_INCREF(Py_None);
    return Py_None;
}


PyMethodDef GameObject_methods[] = 
{
    {"create_mesh_component", ssuge::script::GameObject_create_mesh_component, METH_VARARGS, "create_mesh_component('mesh_fname') -> None"},
    {"create_collider_component", ssuge::script::GameObject_create_collider_component, METH_VARARGS, "create_collider_component('collider_type', ...arguments..., collider_layer_num, collider_mask) -> None"},
    {"create_collider_visualization", ssuge::script::GameObject_create_collider_visualization, METH_VARARGS, "create_collider_visualization() -> None"},
    {"get_relative_point", ssuge::script::GameObject_relative_point, METH_VARARGS, "gets the world space position of a passed local-space point"},
    {"get_world_position", ssuge::script::GameObject_get_world_position, METH_VARARGS, "get_world_position() -> (x, y, z)"},
    {"name", ssuge::script::GameObject_name, METH_VARARGS, "name() -> string"},
    {"parent", ssuge::script::GameObject_parent, METH_VARARGS, "parent() -> ssuge.GameObject (or None)"},
    {"rotate_world", ssuge::script::GameObject_rotate_world, METH_VARARGS, "rotate_world(degrees, axisx, axisy, axisz) -> None"},
    {"scale", ssuge::script::GameObject_scale, METH_VARARGS, "scale(sx, sy, sz) -> None"},
    {"set_world_position", ssuge::script::GameObject_set_world_position, METH_VARARGS, "set_world_position(x, y, z) -> None"},
    {"tag", ssuge::script::GameObject_tag, METH_VARARGS, "tag() -> int"},
    {"translate_local", ssuge::script::GameObject_translate_world, METH_VARARGS, "translate_local(tx, ty, tz) -> None"},
    {"translate_world", ssuge::script::GameObject_translate_world, METH_VARARGS, "translate_world(tx, ty, tz) -> None"},
    {NULL}  /* Sentinel */
};