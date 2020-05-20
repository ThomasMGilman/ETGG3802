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
    {"tag", OgreEngine::script::get_tag, METH_VARARGS, "tag() -> int"},
    {"name", OgreEngine::script::get_name, METH_VARARGS, "name() -> string"},
    {"parent", OgreEngine::script::get_parent, METH_VARARGS, "parent() -> PyObject* "},
    {"get_relative_point", OgreEngine::script::get_relative_point, METH_VARARGS, "get_relative_point() -> (x,y,z)"},
    {"set_world_position", OgreEngine::script::set_world_position, METH_VARARGS, "set_world_position(x,y,z) -> None"},
    {"get_world_position", OgreEngine::script::get_world_position, METH_VARARGS, "get_world_position() -> (x,y,z)"},
    {"create_mesh_component", OgreEngine::script::create_mesh_component, METH_VARARGS, "create_mesh_component('mesh_fname') -> None"},
    {"create_collider_component", OgreEngine::script::create_collider_component, METH_VARARGS, "create_collider_component('sphere', (float)radius, (int)Layer, (long long)mask) | ('prism', (float)x, (float)y, (float)z, (int)Layer, (long long)mask) -> None"},
    {"create_collider_visualization", OgreEngine::script::create_collider_visualization, METH_VARARGS, "create_collider_visualization() -> None"},
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

PyObject* OgreEngine::script::get_tag(PyObject* self, PyObject* args)
{
    script::GameObject* thisObj = ((script::GameObject*)(PyTypeObject*)self);
    std::string errMsg;
    if (thisObj->mTwin != nullptr)
        return PyLong_FromLong(thisObj->mTwin->get_tag());
    else
        errMsg = "get_tag ERROR!!! object has no twin gameobject attached!!!";
    PyErr_SetString(PyExc_ValueError, errMsg.c_str());
    return NULL;
}

PyObject* OgreEngine::script::get_parent(PyObject* self, PyObject* args)
{
    script::GameObject* thisObj = ((script::GameObject*)(PyTypeObject*)self);
    if (thisObj->mTwin != nullptr)
    {
        OgreEngine::GameObject* parent = thisObj->mTwin->get_parent();
        if (parent != nullptr)
        {
            Py_INCREF(parent->get_script_twin());
            return parent->get_script_twin();
        }
    }
    Py_INCREF(Py_None);
    return Py_None;
}

PyObject* OgreEngine::script::get_name(PyObject* self, PyObject* args)
{
    std::string name = ((script::GameObject*)(PyTypeObject*)self)->mTwin->get_name();
    int size = name.size();
    return PyUnicode_FromStringAndSize(name.c_str(), size);
}

PyObject* OgreEngine::script::get_relative_point(PyObject* self, PyObject* args)
{
    script::GameObject* thisObj = ((script::GameObject*)(PyTypeObject*)self);
    if (thisObj->mTwin != nullptr)
    {
        if (PyTuple_Check(args))
        {
            package<Ogre::Vector3> relative_position = get_vector3_from_pytuple(args);
            if (relative_position.errSet)
            {
                PyErr_SetString(PyExc_AttributeError, ("get_relative_point ERROR!! "+relative_position.msg).c_str());
                return NULL;
            }
            Ogre::Vector3 point = thisObj->mTwin->get_world_position() + relative_position.data;
            return PyTuple_Pack(3, PyFloat_FromDouble(point.x), PyFloat_FromDouble(point.y), PyFloat_FromDouble(point.z));
        }
    }
    Py_IncRef(Py_None);
    return Py_None;
}

PyObject* OgreEngine::script::get_world_position(PyObject* self, PyObject* args)
{
    script::GameObject* thisObj = ((script::GameObject*)(PyTypeObject*)self);
    if (thisObj->mTwin != nullptr)
    {
        Ogre::Vector3 worldPos = thisObj->mTwin->get_world_position();
        return PyTuple_Pack(3, PyFloat_FromDouble(worldPos.x), PyFloat_FromDouble(worldPos.y), PyFloat_FromDouble(worldPos.z));
    }
    Py_INCREF(Py_None);
    return Py_None;
}

PyObject* OgreEngine::script::set_world_position(PyObject* self, PyObject* args)
{
    script::GameObject* thisObj = ((script::GameObject*)(PyTypeObject*)self);
    if (thisObj->mTwin != nullptr)
    {
        std::string errMsg;
        if (PyTuple_Check(args))
        {
            int tupleSize = PyTuple_Size(args);
            if (tupleSize == 3)
            {
                package<Ogre::Vector3> new_pos = get_vector3_from_pytuple(args);
                if (new_pos.errSet)
                    errMsg = "set_world_position ERROR!! Data Error!! Error: " + new_pos.msg;
                else
                    thisObj->mTwin->set_position_world(new_pos.data);
            }
            else
                errMsg = "set_world_position ERROR!! Tuple needs to be of size 3!!! Got size: "+std::to_string(tupleSize);
        }
        else
            errMsg = "set_world_position ERROR!! Did not recieve a tuple as an argument!!! Tuple needs to be of size 3!!!";
        if (!errMsg.empty())
        {
            PyErr_SetString(PyExc_ValueError, errMsg.c_str());
            return NULL;
        }
    }
    Py_INCREF(Py_None);
    return Py_None;
}

PyObject* OgreEngine::script::create_mesh_component(PyObject* self, PyObject* args)
{
    script::GameObject* thisObj = ((script::GameObject*)(PyTypeObject*)self);
    if (thisObj->mTwin != nullptr)
    {
        std::string errMsg;
        if (PyTuple_Check(args) && PyTuple_Size(args) == 1)
        {
            if (PyUnicode_Check(PyTuple_GetItem(args, 0)))
            {
                std::string fileName = PyUnicode_AsUTF8(PyTuple_GetItem(args, 0));
                thisObj->mTwin->create_mesh(thisObj->mTwin->get_name() + "_Mesh", fileName);
            }
            else
                errMsg = "create_mesh_component ERROR!! Value Is not of type string!!";
        }
        else
            errMsg = "create_mesh_component ERROR!! Did not get a valid Tuple!! Need to recieve a tuple of size 1 containing a string!!";
        if (!errMsg.empty())
        {
            PyErr_SetString(PyExc_ValueError, errMsg.c_str());
            return NULL;
        }
    }
    Py_INCREF(Py_None);
    return Py_None;
}

PyObject* OgreEngine::script::create_collider_component(PyObject* self, PyObject* args)
{
    script::GameObject* thisObj = ((script::GameObject*)(PyTypeObject*)self);
    if (thisObj->mTwin != nullptr)
    {
        std::string errMsg;
        if (PyTuple_Check(args))
        {
            int tupleSize = PyTuple_Size(args);
            if (tupleSize == 4 || tupleSize == 6) // Sphere | Prism
            {
                std::string colliderType = PyUnicode_AsUTF8(PyTuple_GetItem(args, 0));
                if (colliderType == "sphere")
                {
                    OgreEngine::package<float> radius = get_num_from_pytuple<float>(args, 1);
                    OgreEngine::package<int> layer = get_num_from_pytuple<int>(args, 2);
                    OgreEngine::package<int> mask = get_num_from_pytuple<int>(args, 3);
                    if (radius.errSet || layer.errSet || mask.errSet)
                        errMsg = "create_collider_component ERROR!! Sphere collider data Error!!!\n\tRadius: " + radius.msg + "\n\tLayer: " + layer.msg + "\n\tMask: " + mask.msg;
                    else
                        thisObj->mTwin->create_collider("", ColliderType::SPHERE, Ogre::Vector3(radius.data), layer.data, mask.data);

                }
                else if (colliderType == "box")
                {
                    OgreEngine::package<Ogre::Vector3> extents = get_vector3_from_pytuple(args, 1);
                    OgreEngine::package<int> layer = get_num_from_pytuple<int>(args, 4);
                    OgreEngine::package<int> mask = get_num_from_pytuple<int>(args, 5);
                    if (extents.errSet || layer.errSet || mask.errSet)
                        errMsg = "create_collider_component ERROR!! Box collider data Error!!!\n\tExtents: " + extents.msg + "\n\tLayer: " + layer.msg + "\n\tMask: " + mask.msg;
                    else
                        thisObj->mTwin->create_collider("", ColliderType::RECTANGULAR_PRISM, extents.data, layer.data, mask.data);
                }
                else
                    errMsg = "create_collider_component ERROR!! Type Error!!!\n\t Need to specify type 'sphere' if passing 4 arguments, and type 'box' if passing 6 arguments!!! Got type: " + colliderType;
            }
            else
                errMsg = "create_collider_component ERROR!! Recieved Tuple of invalid size!! Required size 4 or 6, got: " + std::to_string(tupleSize);
        }
        else
            errMsg = "create_collider_component ERROR!! Did not get a valid Tuple!!! Need to recieve a tuple of size 4 or 6!!!";
        if (!errMsg.empty())
        {
            PyErr_SetString(PyExc_ValueError, errMsg.c_str());
            return NULL;
        }
    }
    Py_INCREF(Py_None);
    return Py_None;
}

PyObject* OgreEngine::script::create_collider_visualization(PyObject* self, PyObject* args)
{
    script::GameObject* thisObj = ((script::GameObject*)(PyTypeObject*)self);
    if (thisObj->mTwin != nullptr)
    {
        ComponentCollider* c = thisObj->mTwin->get_component<ComponentCollider>(ComponentCollider::ComponentType::COLLIDER);
        if (c == nullptr)
        {
            PyErr_SetString(PyExc_ValueError, "create_collider_visualization ERROR!! GameObject does not have a collider component!! Cant create a visualization of something that does not exist!!");
            return NULL;
        }
        c->create_collider_visualization();
    }
    Py_INCREF(Py_None);
    return Py_None;
}

PyObject* OgreEngine::script::rotate_local(PyObject* self, PyObject* args)
{
    script::GameObject* thisObj = ((script::GameObject*)(PyTypeObject*)self);
    if (thisObj->mTwin != nullptr)
    {
        OgreEngine::package<float> degrees = get_num_from_pytuple<float>(args, 0);
        OgreEngine::package<Ogre::Vector3> axisPackage = get_vector3_from_pytuple(args);
        if (degrees.errSet || axisPackage.errSet)
        {
            PyErr_SetString(PyExc_ValueError, ("rotate_world ERROR!! Degrees: " + degrees.msg + "Axis: " + axisPackage.msg).c_str());
            return NULL;
        }
        thisObj->mTwin->rotate_local(degrees.data, axisPackage.data);
    }
    Py_INCREF(Py_None);
    return Py_None;
}

PyObject* OgreEngine::script::rotate_world(PyObject* self, PyObject* args)
{
    script::GameObject* thisObj = ((script::GameObject*)(PyTypeObject*)self);
    if (thisObj->mTwin != nullptr)
    {
        OgreEngine::package<float> degrees = get_num_from_pytuple<float>(args, 0);
        OgreEngine::package<Ogre::Vector3> axisPackage = get_vector3_from_pytuple(args);
        if (degrees.errSet || axisPackage.errSet)
        {
            PyErr_SetString(PyExc_ValueError, ("rotate_world ERROR!! Degrees: " + degrees.msg + "Axis: " + axisPackage.msg).c_str());
            return NULL;
        }
        thisObj->mTwin->rotate_world(degrees.data, axisPackage.data);
    }
    Py_INCREF(Py_None);
    return Py_None;
}

PyObject* OgreEngine::script::scale(PyObject* self, PyObject* args)
{
    script::GameObject* thisObj = ((script::GameObject*)(PyTypeObject*)self);
    if (thisObj->mTwin != nullptr)
    {
        OgreEngine::package<Ogre::Vector3> translationPackage = get_vector3_from_pytuple(args);
        if (translationPackage.errSet)
        {
            PyErr_SetString(PyExc_ValueError, ("scale ERROR!! " + translationPackage.msg).c_str());
            return NULL;
        }
        thisObj->mTwin->scale(translationPackage.data);
    }
    Py_INCREF(Py_None);
    return Py_None;
}

PyObject* OgreEngine::script::translate_local(PyObject* self, PyObject* args)
{
    script::GameObject* thisObj = ((script::GameObject*)(PyTypeObject*)self);
    if (thisObj->mTwin != nullptr)
    {
        OgreEngine::package<Ogre::Vector3> translationPackage = get_vector3_from_pytuple(args);
        if (translationPackage.errSet)
        {
            PyErr_SetString(PyExc_ValueError, ("tanslate_local ERROR!! " + translationPackage.msg).c_str());
            return NULL;
        }
        thisObj->mTwin->translate_local(translationPackage.data);
    }
    Py_INCREF(Py_None);
    return Py_None;
}

PyObject* OgreEngine::script::translate_world(PyObject* self, PyObject* args)
{
    script::GameObject* thisObj = ((script::GameObject*)(PyTypeObject*)self);
    if (thisObj->mTwin != nullptr)
    {
        OgreEngine::package<Ogre::Vector3> translationPackage = get_vector3_from_pytuple(args);
        if (translationPackage.errSet)
        {
            PyErr_SetString(PyExc_ValueError, ("tanslate_world ERROR!! " + translationPackage.msg).c_str());
            return NULL;
        }
        thisObj->mTwin->translate_world(translationPackage.data);
    }
    Py_INCREF(Py_None);
    return Py_None;
}
