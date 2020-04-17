#include <stdafx.h>
#include <component_input_listener.h>

using namespace OgreEngine;

ComponentInputListener::ComponentInputListener(GameObject* owner, std::string name) : Component(owner)
{

}

ComponentInputListener::~ComponentInputListener()
{
}

void ComponentInputListener::process_key_action(std::string action, bool is_pressed)
{
}

void ComponentInputListener::process_axis_action(std::string axis, float amount)
{
}
