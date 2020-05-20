#pragma once
#include <stdafx.h>

// This file contains declarations for any "top-leve" functions (that are callable through
// the ssuge module)

namespace ssuge
{
    namespace script
    {
        /// builds the edge-list for all meshes in an ogre resource group by the given name.
        /// This is silly, but my ogre blender => ogre exporter doesn't seem to build this, and 
        /// without it, shadows won't work on our models.
        PyObject* build_edge_list(PyObject* self, PyObject* args);

        /// create_game_object(group_name, object_name, tag_num) -> game object
        /// create_game_object(group_name, object_name, tag_num, class_name) -> class_name instance
        PyObject* create_game_object(PyObject* self, PyObject* args);

        /// deregister_input_lisener(ssuge.GameObject-derived-class-instance) -> None
        PyObject* deregister_input_listener(PyObject* self, PyObject* args);

        /// get_action(action_name) -> bool [True if that action is pressed]
        PyObject* get_action(PyObject* self, PyObject* args);

        /// get_axis(axis_name) -> float (-1...+1)
        PyObject* get_axis(PyObject* self, PyObject* args);

        /// has_action(action_name) -> bool [True if that action exists]
        PyObject* has_action(PyObject* self, PyObject* args);

        /// has_axis(axis_name) -> bool [True if that action exists]
        PyObject* has_axis(PyObject* self, PyObject* args);

        /// load_collider_visualizations() -> None
        PyObject* load_collider_visualizations(PyObject* self, PyObject* args);

        /// load_script(file_name) -> None
        PyObject* load_scene(PyObject* self, PyObject* args);

        /// load_script(file_name) -> None
        PyObject* load_script(PyObject* self, PyObject* args);

        /// log(msg) -> None
        /// log(msg, (1, 0.2, 0), 5) -> None
        PyObject* log(PyObject* self, PyObject* args);

        /// queue_destroy(gobj) -> None
        /// queue_destroy(gobj, "group_name") -> None
        PyObject* queue_destroy(PyObject* self, PyObject* args);

        /// raycast(ray_origin, ray_direction, mask, max_dist = None) -> ((hit_pt1, hit_obj1), ...)
        PyObject* raycast(PyObject* self, PyObject* args);

        /// register_input_lisener(ssuge.GameObject-derived-class-instance) -> None
        PyObject* register_input_listener(PyObject* self, PyObject* args);

        /// set_skybox(material_name, dist=300) -> None
        PyObject* set_skybox(PyObject* self, PyObject* args);

    }
}
