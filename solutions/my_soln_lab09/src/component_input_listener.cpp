#include <stdafx.h>
#include <component_input_listener.h>
#include <input_manager.h>
#include <game_object.h>

ssuge::ComponentInputListener::ComponentInputListener(GameObject* owner) : 
	ssuge::Component(owner)
{
	INPUT_MANAGER->register_listener(this);
}


ssuge::ComponentInputListener::~ComponentInputListener()
{
	INPUT_MANAGER->deregister_listener(this);
}




void ssuge::ComponentInputListener::action_begin(std::string action_name)
{
	PyObject* args = PyTuple_New(1);
	PyTuple_SetItem(args, 0, PyUnicode_FromString(action_name.c_str()));
	mOwner->execute_script_method("action_pressed", args);
	Py_DECREF(args);
}



void ssuge::ComponentInputListener::action_end(std::string action_name)
{
	PyObject* args = PyTuple_New(1);
	PyTuple_SetItem(args, 0, PyUnicode_FromString(action_name.c_str()));
	mOwner->execute_script_method("action_released", args);
	Py_DECREF(args);
}



void ssuge::ComponentInputListener::axis_changed(std::string axis_name, float new_value)
{
	PyObject* args = PyTuple_New(2);
	PyTuple_SetItem(args, 0, PyUnicode_FromString(axis_name.c_str()));
	PyTuple_SetItem(args, 1, PyFloat_FromDouble((double)new_value));
	mOwner->execute_script_method("axis_updated", args);
	Py_DECREF(args);
}