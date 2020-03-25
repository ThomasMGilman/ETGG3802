#include <stdafx.h>
#include <input_manager.h>

OgreEngine::InputManager::InputManager(std::string bindings_file)
{
}

OgreEngine::InputManager::~InputManager()
{
}

bool OgreEngine::InputManager::has_axis(std::string name)
{
	return false;
}

bool OgreEngine::InputManager::has_action(std::string name)
{
	return false;
}

float OgreEngine::InputManager::get_axis(std::string name)
{
	return 0.0f;
}

bool OgreEngine::InputManager::is_action_active(std::string name)
{
	return false;
}

void OgreEngine::InputManager::update(float dt)
{
}

void OgreEngine::InputManager::load_bindings(std::string fname)
{
}

void OgreEngine::InputManager::create_axis(std::string name)
{
}

void OgreEngine::InputManager::create_action(std::string name)
{
}

void OgreEngine::InputManager::register_listener(ComponentInputListener* comp)
{
	mListeners.insert(comp);
}

void OgreEngine::InputManager::deregister_listener(ComponentInputListener* comp)
{
	mListeners.erase(comp);
}

void OgreEngine::InputManager::broadcast_action(std::string action, bool is_pressed)
{
}

void OgreEngine::InputManager::broadcast_axis_event(std::string axis, float new_value)
{
}

void OgreEngine::InputManager::scan_for_gamepads()
{
}

void OgreEngine::InputManager::handle_key(SDL_Keycode k, bool is_pressed)
{
}

void OgreEngine::InputManager::handle_joy_button(SDL_GameControllerButton button, int joy_id, bool is_pressed)
{
}

void OgreEngine::InputManager::handle_joy_axis(SDL_GameControllerAxis axis, int joy_id, Sint16 value)
{
}
