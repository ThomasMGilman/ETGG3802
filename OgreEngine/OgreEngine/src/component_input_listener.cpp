#include <stdafx.h>
#include <component_input_listener.h>
#include <game_object.h>

using namespace OgreEngine;

ComponentInputListener::ComponentInputListener(GameObject* owner, std::string name) : 
	Component(owner, name.empty() ? owner->get_name() + "_listener" : name)
{
}

ComponentInputListener::~ComponentInputListener()
{
}

void ComponentInputListener::process_key_action(std::string action, bool is_pressed)
{
	PyObject* arguments = PyTuple_New(1);
	PyTuple_SetItem(arguments, 0, PyUnicode_FromStringAndSize(action.c_str(), action.length()));
	Component::mParent->run_method(is_pressed ? "action_pressed" : "action_released", arguments);
}

void ComponentInputListener::process_axis_action(std::string axis, float amount)
{
	PyObject* arguments = PyTuple_New(2);
	PyTuple_SetItem(arguments, 0, PyUnicode_FromStringAndSize(axis.c_str(), axis.length()));
	PyTuple_SetItem(arguments, 1, PyFloat_FromDouble(amount));
	Component::mParent->run_method("axis_updated", arguments);
}
