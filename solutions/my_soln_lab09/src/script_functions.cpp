#include <stdafx.h>
#include <script_functions.h>
#include <application.h>
#include <log_manager.h>
#include <script_game_object.h>
#include <input_manager.h>

extern PyTypeObject GameObjectType;

// NOTE: When adding new methods, declare the prototype in script_functions.h, define the body here, then 
// add that method to the "function table of contents" at the bottom of this file.



PyObject* ssuge::script::build_edge_list(PyObject* self, PyObject* args)
{
    if (!PyTuple_Check(args) || PyTuple_Size(args) != 1 || !PyUnicode_Check(PyTuple_GetItem(args, 0)))
    {
        PyErr_SetString(PyExc_ValueError, "You must pass a (string) group name to this function");
        return NULL;
    }

    std::string group_name = SCRIPT_MANAGER->get_string_from_pyobject(PyTuple_GetItem(args, 0));

    // Go through all the meshes in the [group_name] group -- make sure this is where
    // your FileSystem paths are in resources.cfg.
    Ogre::MeshManager& mmgr = Ogre::MeshManager::getSingleton();
    Ogre::ResourceGroupManager& rgmgr = Ogre::ResourceGroupManager::getSingleton();
    Ogre::StringVectorPtr items = rgmgr.findResourceNames(group_name, "mesh", false);
    for (int i = 0; i < items->size(); i++)
    {
        Ogre::String name = items->at(i);
        Ogre::MeshPtr mptr = Ogre::MeshManager::getSingleton().getByName(name);
        if (!mptr)
            mptr->buildEdgeList();
    }

    Py_INCREF(Py_None);
    return Py_None;
}



PyObject* ssuge::script::create_game_object(PyObject* self, PyObject* args)
{
    std::string group_name, object_name;
    int tag;
    std::string base_class_name;

    if (!PyTuple_Check(args) || PyTuple_Size(args) < 3 || !PyUnicode_Check(PyTuple_GetItem(args, 0)) ||
        !PyUnicode_Check(PyTuple_GetItem(args, 1)) || !PyLong_Check(PyTuple_GetItem(args, 2)))
    {
        PyErr_SetString(PyExc_ValueError, "usage: create_game_object(group_name, object_name, tag, parent=None, script_name='')");
        return NULL;
    }

    group_name = SCRIPT_MANAGER->get_string_from_pyobject(PyTuple_GetItem(args, 0));
    object_name = SCRIPT_MANAGER->get_string_from_pyobject(PyTuple_GetItem(args, 1));
    tag = PyLong_AsLong(PyTuple_GetItem(args, 2));

    // Get the class name, if we have one.
    std::string script_name = "";
    if (PyTuple_Size(args) >= 4 && PyUnicode_Check(PyTuple_GetItem(args, 3)))
        script_name = SCRIPT_MANAGER->get_string_from_pyobject(PyTuple_GetItem(args, 3));

    // Create the C++ game object.  This now (as of Lab9) also creates the python object and connects the two.
    ssuge::GameObject* cpp_gobj = GAME_OBJECT_MANAGER->create_game_object(group_name, object_name, NULL, tag, script_name);

    // Get any extra items from our args list and pass them to the "create" function of our game object,
    // if it has one
    PyObject* create_args;
    int args_size = (int)PyTuple_Size(args);
    if (args_size > 4)
    {
        create_args = PyTuple_New(args_size - 4);
        for (int i = 4; i < args_size; i++)
        {
            // We need to incref the element we get from args since this next line will steal a reference
            Py_INCREF(PyTuple_GetItem(args, i));
            int args_index = i - 4;
            PyTuple_SetItem(create_args, args_index, PyTuple_GetItem(args, i));
        }
    }
    else
        create_args = PyTuple_New(0);
    cpp_gobj->execute_script_method("create", create_args);
    Py_DECREF(create_args);
    
    // Return the new instance!  I need to incref here.  The game object manager has one reference, but since
    // we're returning it to the python world, when that caller (a module, a function / method, etc.) goes
    // out of scope, this object will be decref'd.  We don't want it to be destroyed, though.
    Py_INCREF(cpp_gobj->get_script_twin());
    return cpp_gobj->get_script_twin();
}



PyObject* ssuge::script::deregister_input_listener(PyObject* self, PyObject* args)
{
    if (!PyTuple_Check(args) || !PyObject_IsInstance(PyTuple_GetItem(args, 0), (PyObject*)&GameObjectType))
    {
        PyErr_SetString(PyExc_ValueError, "Argument must be an instance of ssuge.GameObject (or derived class)");
        return NULL;
    }

    ssuge::script::GameObject* python_gobj = (ssuge::script::GameObject*)PyTuple_GetItem(args, 0);
    ssuge::GameObject* cpp_gobj = python_gobj->mTwin;
    ssuge::ComponentInputListener* in_comp = cpp_gobj->get_input_listener_component();
    if (in_comp)
        INPUT_MANAGER->deregister_listener(in_comp);

    Py_INCREF(Py_None);
    return Py_None;
}



PyObject* ssuge::script::get_action(PyObject* self, PyObject* args)
{
    if (!PyTuple_Check(args) || PyTuple_Size(args) != 1 || !PyUnicode_Check(PyTuple_GetItem(args, 0)))
    {
        PyErr_SetString(PyExc_ValueError, "you must pass a string to this function");
        return NULL;
    }

    std::string action_name = SCRIPT_MANAGER->get_string_from_pyobject(PyTuple_GetItem(args, 0));
    if (!INPUT_MANAGER->has_action(action_name))
    {
        std::string temps = "invalid action '" + action_name + "'";
        PyErr_SetString(PyExc_ValueError, temps.c_str());
        return NULL;
    }
    else
    {
        return PyBool_FromLong(INPUT_MANAGER->has_action(action_name) ? 1 : 0);
    }
}


PyObject* ssuge::script::get_axis(PyObject* self, PyObject* args)
{
    if (!PyTuple_Check(args) || PyTuple_Size(args) != 1 || !PyUnicode_Check(PyTuple_GetItem(args, 0)))
    {
        PyErr_SetString(PyExc_ValueError, "you must pass a string to this function");
        return NULL;
    }

    std::string axis_name = SCRIPT_MANAGER->get_string_from_pyobject(PyTuple_GetItem(args, 0));
    if (!INPUT_MANAGER->has_axis(axis_name))
    {
        std::string temps = "invalid axis '" + axis_name + "'";
        PyErr_SetString(PyExc_ValueError, temps.c_str());
        return NULL;
    }
    else
    {
        return PyFloat_FromDouble(INPUT_MANAGER->get_axis(axis_name));
    }
}


PyObject* ssuge::script::has_action(PyObject* self, PyObject* args)
{
    if (!PyTuple_Check(args) || PyTuple_Size(args) != 1 || !PyUnicode_Check(PyTuple_GetItem(args, 0)))
    {
        PyErr_SetString(PyExc_ValueError, "you must pass a string to this function");
        return NULL;
    }

    std::string action_name = SCRIPT_MANAGER->get_string_from_pyobject(PyTuple_GetItem(args, 0));
    return PyBool_FromLong(INPUT_MANAGER->has_action(action_name) ? 1 : 0);
}


PyObject* ssuge::script::has_axis(PyObject* self, PyObject* args)
{
    if (!PyTuple_Check(args) || PyTuple_Size(args) != 1 || !PyUnicode_Check(PyTuple_GetItem(args, 0)))
    {
        PyErr_SetString(PyExc_ValueError, "you must pass a string to this function");
        return NULL;
    }

    std::string axis_name = SCRIPT_MANAGER->get_string_from_pyobject(PyTuple_GetItem(args, 0));
    return PyBool_FromLong(INPUT_MANAGER->has_axis(axis_name) ? 1 : 0);
}



PyObject* ssuge::script::load_collider_visualizations(PyObject* self, PyObject* args)
{
    if (!PyTuple_Check(args) || PyTuple_Size(args) != 0)
    {
        PyErr_SetString(PyExc_ValueError, "This function should be called with no arguments");
        return NULL;
    }

    COLLISION_MANAGER->load_visualizations();

    Py_INCREF(Py_None);
    return Py_None;
}



PyObject* ssuge::script::load_scene(PyObject* self, PyObject* args)
{
    if (!PyTuple_Check(args) || PyTuple_Size(args) != 2 || !PyUnicode_Check(PyTuple_GetItem(args, 0)) ||
        !PyUnicode_Check(PyTuple_GetItem(args, 1)))
    {
        PyErr_SetString(PyExc_ValueError, "usage: load_scene('group_name', 'path_and_fname') -> None");
        return NULL;
    }

    std::string group_name = SCRIPT_MANAGER->get_string_from_pyobject(PyTuple_GetItem(args, 0));
    std::string scene_fname = SCRIPT_MANAGER->get_string_from_pyobject(PyTuple_GetItem(args, 1));
    GAME_OBJECT_MANAGER->load_scene_file(group_name, scene_fname);

    Py_INCREF(Py_None);
    return Py_None;
}


PyObject* ssuge::script::load_script(PyObject* self, PyObject* args)
{
    if (!PyTuple_Check(args) || PyTuple_Size(args) != 1 || !PyUnicode_Check(PyTuple_GetItem(args, 0)))
    {
        PyErr_SetString(PyExc_ValueError, "usage: load_script('script_path_and_fname') -> None");
        return NULL;
    }

    std::string script_fname = SCRIPT_MANAGER->get_string_from_pyobject(PyTuple_GetItem(args, 0));
    SCRIPT_MANAGER->run_script(script_fname);

    Py_INCREF(Py_None);
    return Py_None;
}


PyObject* ssuge::script::log(PyObject* self, PyObject* args)
{
    bool error = false;
    std::string error_msg;
    Ogre::ColourValue col;
    float time_to_show = 0.0f;

    if (!PyTuple_Check(args))
    {
        error = true;
        error_msg = "Corrupt argument tuple";
    }
    if (PyTuple_Size(args) == 1 && PyUnicode_Check(PyTuple_GetItem(args, 0)))
    {
        LOG_MANAGER->log(SCRIPT_MANAGER->get_string_from_pyobject(PyTuple_GetItem(args, 0)));
        Py_INCREF(Py_None);
        return Py_None;
    }
    else if (PyTuple_Size(args) == 3)
    {
        PyObject* color_tuple = NULL;
        if (!PyUnicode_Check(PyTuple_GetItem(args, 0)))
        {
            error = true;
            error_msg = "argument 1 must be a string";
        }
        else if (!PyTuple_Check(PyTuple_GetItem(args, 1)) || PyTuple_Size(PyTuple_GetItem(args, 1)) != 3)
        {
            error = true;
            error_msg = "argument 2 must be a color tuple";
        }
        else
        {
            color_tuple = PyTuple_GetItem(args, 1);
            for (int i = 0; i < 3; i++)
            {
                if (!PyNumber_Check(PyTuple_GetItem(color_tuple, i)))
                {
                    error = true;
                    error_msg = "Invalid color value (must be a number [0.0...1.0])";
                }
                else
                {
                    PyObject* num = PyNumber_Float(PyTuple_GetItem(color_tuple, i));
                    float numf = (float)PyFloat_AsDouble(num);
                    Py_DECREF(num);
                    if (numf < 0.0 || numf > 1.0)
                    {
                        error = true;
                        error_msg = "All color values must be in the 0.0 - 1.0 range";
                    }
                    else
                    {
                        if (i == 0)         col.r = numf;
                        else if (i == 1)    col.g = numf;
                        else if (i == 2)    col.b = numf;
                    }

                }
            }

            if (!PyNumber_Check(PyTuple_GetItem(args, 2)))
            {
                error = true;
                error_msg = "Argument 2 must be a number";
            }
            else
            {
                PyObject* num = PyNumber_Float(PyTuple_GetItem(args, 2));
                time_to_show = (float)PyFloat_AsDouble(num);
                Py_DECREF(num);

            }
        }
    }
    else
    {
        error = true;
        error_msg = "incorrect number of arguments (must be 1 or 3)";
    }
    
    if (error)
    {
        PyErr_SetString(PyExc_ValueError, error_msg.c_str());
        return NULL;
    }
    else
    {
        LOG_MANAGER->log(SCRIPT_MANAGER->get_string_from_pyobject(PyTuple_GetItem(args, 0)), col, time_to_show);
        Py_INCREF(Py_None);
        return Py_None;
    }
}



PyObject* ssuge::script::queue_destroy(PyObject* self, PyObject* args)
{
    if (!PyTuple_Check(args) || PyTuple_Size(args) < 1 || PyTuple_Size(args) > 2 ||
        !PyObject_IsInstance(PyTuple_GetItem(args, 0), (PyObject*)&GameObjectType))
    {
        PyErr_SetString(PyExc_ValueError, "You must pass a ssuge.GameObject instance and optionally a group_name");
        return NULL;
    }

    ssuge::script::GameObject* python_gobj = (ssuge::script::GameObject*)PyTuple_GetItem(args, 0);
    ssuge::GameObject* cpp_gobj = python_gobj->mTwin;
    std::string group_name = "";

    if (PyTuple_Size(args) == 2 && PyUnicode_Check(PyTuple_GetItem(args, 1)))
        group_name = SCRIPT_MANAGER->get_string_from_pyobject(PyTuple_GetItem(args, 1));

    if (group_name != "")
        GAME_OBJECT_MANAGER->queue_destroy(group_name, cpp_gobj->get_name());
    else
        GAME_OBJECT_MANAGER->queue_destroy(cpp_gobj->get_name());

    Py_INCREF(Py_None);
    return Py_None;
}


PyObject* ssuge::script::raycast(PyObject* self, PyObject* args)
{
    // ray_origin = self.get_relative_point((0, -0.55, 0))
    // ray_direction = (0, -1, 0)
    // ray_dist = 10 (-1 if you want it to be infinite)
    // ray_mask = 0x01                 # Only look for hits with layer0
    // ray_result = ssuge.raycast(ray_origin, ray_direction, ray_mask, ray_dist)
    //    ray_result should be a tuple of pairs (hit-pt, hit-obj) of objects hit, if any.
    if (!PyTuple_Check(args) || PyTuple_Size(args) != 4 || !PyTuple_Check(PyTuple_GetItem(args, 0)) ||
        PyTuple_Size(PyTuple_GetItem(args, 0)) != 3 || !PyTuple_Check(PyTuple_GetItem(args, 1)) ||
        PyTuple_Size(PyTuple_GetItem(args, 1)) != 3 || !PyLong_Check(PyTuple_GetItem(args, 2)) ||
        !PyNumber_Check(PyTuple_GetItem(args, 3)))
    {
        PyErr_SetString(PyExc_ValueError, "usage: raycast(ray_origin, ray_direction, ray_mask, ray_dist)");
        return NULL;
    }

    Ogre::Vector3 ray_origin, ray_direction;
    if (!SCRIPT_MANAGER->extract_vector3(PyTuple_GetItem(args, 0), ray_origin, "parameter1 must be 3d position"))
        return NULL;
    if (!SCRIPT_MANAGER->extract_vector3(PyTuple_GetItem(args, 1), ray_direction, "parameter2 must be a 3d vector"))
        return NULL;
    PyObject* fval = PyNumber_Float(PyTuple_GetItem(args, 3));
    float ray_dist = (float)PyFloat_AsDouble(fval);
    Py_DECREF(fval);

    long long ray_mask = (long long)PyLong_AsLong(PyTuple_GetItem(args, 2));

    std::vector<std::pair<Ogre::Vector3, ssuge::GameObject*>> ray_result;

    if (ray_dist < 0)
        COLLISION_MANAGER->raycast_unbounded(ray_origin, ray_direction, ray_result, ray_mask);
    else
        COLLISION_MANAGER->raycast_bounded(ray_origin, ray_direction, ray_result, ray_dist, ray_mask);

    PyObject* result = PyTuple_New(ray_result.size());
    int index = 0;
    for (std::pair<Ogre::Vector3, ssuge::GameObject*> cur_pair : ray_result)
    {
        PyObject* sub_tup = PyTuple_New(2);
        PyObject* pos = PyTuple_New(3);
        for (int i = 0; i < 3; i++)
            PyTuple_SetItem(pos, i, PyFloat_FromDouble((double)cur_pair.first[i]));
        PyTuple_SetItem(sub_tup, 0, pos);
        PyObject* hit_obj = cur_pair.second->get_script_twin();
        Py_INCREF(hit_obj);
        PyTuple_SetItem(sub_tup, 1, hit_obj);
        PyTuple_SetItem(result, index, sub_tup);
        ++index;
    }

    return result;
}


PyObject* ssuge::script::register_input_listener(PyObject* self, PyObject* args)
{
    if (!PyTuple_Check(args) || !PyObject_IsInstance(PyTuple_GetItem(args, 0), (PyObject*)&GameObjectType))
    {
        PyErr_SetString(PyExc_ValueError, "Argument must be an instance of ssuge.GameObject (or derived class)");
        return NULL;
    }

    ssuge::script::GameObject* python_gobj = (ssuge::script::GameObject*)PyTuple_GetItem(args, 0);
    ssuge::GameObject* cpp_gobj = python_gobj->mTwin;
    ssuge::ComponentInputListener* in_comp = cpp_gobj->get_input_listener_component();
    if (!in_comp)
        in_comp = cpp_gobj->create_input_listener_component();

    INPUT_MANAGER->register_listener(in_comp);

    Py_INCREF(Py_None);
    return Py_None;
}



PyObject* ssuge::script::set_skybox(PyObject* self, PyObject* args)
{
    float distance = 300.0f;

    if (!PyTuple_Check(args) || PyTuple_Size(args) < 1 || !PyUnicode_Check(PyTuple_GetItem(args, 0)))
    {
        PyErr_SetString(PyExc_ValueError, "usage: ssuge.set_skybox('mat_name', dist=300.) -> None");
        return NULL;
    }
    if (PyTuple_Check(args) && PyTuple_Size(args) == 2 && PyNumber_Check(PyTuple_GetItem(args, 1)))
    {
        PyObject* num = PyNumber_Float(PyTuple_GetItem(args, 1));
        distance = (float)PyFloat_AsDouble(num);
        Py_DECREF(num);
    }

    APPLICATION->set_skybox(SCRIPT_MANAGER->get_string_from_pyobject(PyTuple_GetItem(args, 0)), distance);
    Py_INCREF(Py_None);
    return Py_None;
}


PyMethodDef ssuge_script_functions[] = 
{
    {"build_edge_lists", ssuge::script::build_edge_list, METH_VARARGS, "build_edge_lists(group_name) -> None"},
    {"create_game_object", ssuge::script::create_game_object, METH_VARARGS, "create_game_object('my_group', 'my_name', 42, other_game_object=None, script_fname=None)"},
    {"deregister_input_listener", ssuge::script::deregister_input_listener, METH_VARARGS, "deregister_input_listener(game_object) -> None"},
    {"get_action", ssuge::script::get_action, METH_VARARGS, "get_action(action_name) -> bool"},
    {"get_axis", ssuge::script::get_axis, METH_VARARGS, "get_axis(axis_name) -> float"},
    {"has_action", ssuge::script::has_action, METH_VARARGS, "has_action(action_name) -> bool"},
    {"has_axis", ssuge::script::has_axis, METH_VARARGS, "has_axis(axis_name) -> bool"},
    {"load_collider_visualizations", ssuge::script::load_collider_visualizations, METH_VARARGS, "load_collider_visualizations() -> None"},
    {"load_scene", ssuge::script::load_scene, METH_VARARGS, "load_scene(scene_fname) -> None"},
    {"load_script", ssuge::script::load_script, METH_VARARGS, "load_script('script-path-and-fname') -> None"},
    {"log", ssuge::script::log, METH_VARARGS, "log('my message', color=None, distance=None) -> None"},
    {"queue_destroy", ssuge::script::queue_destroy, METH_VARARGS, "queue_destroy(gobj, group_name='') -> None"},
    {"raycast", ssuge::script::raycast, METH_VARARGS, "ray_cast(ray_origin, ray_direction, mask, max_dist=None) -> ((hit_pt1, hit_obj1), ...)"},
    {"register_input_listener", ssuge::script::register_input_listener, METH_VARARGS, "register_input_listener(game_object) -> None"},
    {"set_skybox", ssuge::script::set_skybox, METH_VARARGS,"set_skybox(\"MySkyboxMaterial\") -> None"},
    {NULL, NULL, 0, NULL}
};
