// table of contents from script_functions.cpp
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


// table of contents from script_game_object_methods.cpp
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