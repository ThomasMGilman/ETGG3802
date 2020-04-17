#include <stdafx.h>
#include <application.h>
#include <log_manager.h>
#include <input_manager.h>
#include <SDL_name_mappings.h>

using namespace OgreEngine;

InputManager* InputManager::msSingleton = nullptr;

InputManager::InputManager(std::string bindings_file, bool debugMode)
{
	this->mDebugging = true;
	this->mKeyNames = OgreEngine::InputBindings::mKeyNames;
	this->mGamepadAxisNames = OgreEngine::InputBindings::mGamepadAxisNames;
	this->mGamepadButtonNames = OgreEngine::InputBindings::mGamepadButtonNames;

	/// Load bindings from input action, axis mapping file
	load_bindings(bindings_file);
}

InputManager::~InputManager()
{
}

bool InputManager::has_axis(std::string name)
{
	return mAxisValues.count(name);
}

bool InputManager::has_action(std::string name)
{
	return mActionValues.count(name);
}

float InputManager::get_axis(std::string name)
{
	if (has_axis(name))
		return mAxisValues[name];
	return 0.0f;
}

bool InputManager::is_action_active(std::string name)
{
	return (has_action(name) && mActionValues[name] > 0);
}

void InputManager::update(float dt)
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_MOUSEBUTTONDOWN:
			break;
		case SDL_MOUSEBUTTONUP:
			break;
		case SDL_MOUSEMOTION:
			break;
		case SDL_MOUSEWHEEL:
			break;
		case SDL_KEYDOWN:
			handle_key(event.key.keysym.sym, true);
			break;
		case SDL_KEYUP:
			handle_key(event.key.keysym.sym, false);
			break;
		case SDL_JOYBUTTONDOWN:
			break;
		case SDL_JOYBUTTONUP:
			break;
		case SDL_JOYAXISMOTION:
			handle_joy_axis((SDL_GameControllerAxis)event.jaxis.axis,
				event.jaxis.which,
				event.jaxis.value);
			break;
		case SDL_CONTROLLERBUTTONDOWN:
			handle_joy_button((SDL_GameControllerButton)event.cbutton.button,
				event.cbutton.which,
				event.cbutton.state);
			break;
		case SDL_CONTROLLERBUTTONUP:
			handle_joy_button((SDL_GameControllerButton)event.cbutton.button,
				event.cbutton.which,
				event.cbutton.state);
			break;
		}
	}
}

void InputManager::load_bindings(std::string fname, std::string fpath)
{
	mDoc->LoadFile((fpath + fname).c_str());
	if (mDoc->Error())
	{
		std::string error = mDoc->ErrorStr();
		LOG_MANAGER->log("tinyxml2 Error: " +error);
	}
	else
	{
		tinyxml2::XMLElement* firstElement = mDoc->FirstChildElement();
		if (firstElement != NULL)
		{
			LOG_MANAGER->log_message("Reading in input bindings from: " + fname);
			tinyxml2::XMLElement* bindingNodes = firstElement->FirstChildElement();
			if (bindingNodes != NULL)
				xml_parse_binding_nodes(bindingNodes);
			LOG_MANAGER->log_message(fname + " contains 0 binding input actions!!");
		}
	}
}

void InputManager::xml_parse_binding_nodes(tinyxml2::XMLElement* bindingNode)
{
	std::string nodeVal = bindingNode->Value();
	tinyxml2::XMLElement* childBinding = bindingNode->FirstChildElement();
	if (nodeVal == "actions")
	{
		if (childBinding != NULL)
			xml_parse_action_nodes(childBinding);
	}
	else if (nodeVal == "axes")
	{
		if (childBinding != NULL)
			xml_parse_axis_nodes(childBinding);
	}
	tinyxml2::XMLElement* nextElement = bindingNode->NextSiblingElement();
	if (nextElement != NULL)
		xml_parse_binding_nodes(nextElement);
}

void InputManager::xml_parse_action_nodes(tinyxml2::XMLElement* bindingNode)
{
	std::string action = bindingNode->Attribute("name");
	std::string type = bindingNode->Attribute("type");
	if (type == "key")
	{
		std::string keyName = bindingNode->Attribute("value");
		SDL_Keycode k = mKeyNames[keyName];
		if (mKeyActionBindings.find(k) == mKeyActionBindings.end())
			mKeyActionBindings[k] = action;
		else
			LOG_MANAGER->log_message("Duplicate key action specified!! Line: " + std::to_string(bindingNode->GetLineNum()), ERROR_COLOUR);
	}
	else if (type == "gamepad")
	{
		std::string keyName = bindingNode->Attribute("value");
		int controllerID = atoi(bindingNode->Attribute("id_num"));
		SDL_GameControllerButton k = mGamepadButtonNames[keyName];
		if (mGamepadButtonBindings[controllerID].find(k) == mGamepadButtonBindings[controllerID].end())
			mGamepadButtonBindings[controllerID][k] = action;
		else
			LOG_MANAGER->log_message("Duplicate gamepad action specified!! Line: " + std::to_string(bindingNode->GetLineNum()), ERROR_COLOUR);
	}
	create_action(action);
	tinyxml2::XMLElement* nextElement = bindingNode->NextSiblingElement();
	if (nextElement != NULL)
		xml_parse_action_nodes(nextElement);
}

void InputManager::xml_parse_axis_nodes(tinyxml2::XMLElement* bindingNode)
{
	std::string action = bindingNode->Attribute("name");
	std::string type = bindingNode->Attribute("type");
	if (type == "key")
	{
		std::string pos_action = bindingNode->Attribute("positive");
		std::string neg_action = bindingNode->Attribute("negative");
		SDL_Keycode posKey = mKeyNames[pos_action];
		SDL_Keycode negKey = mKeyNames[neg_action];
		if (mKeyAxisBindings.find(posKey) == mKeyAxisBindings.end())
			mKeyAxisBindings[posKey][action] = 1.0f;
		if (mKeyAxisBindings.find(negKey) == mKeyAxisBindings.end())
			mKeyAxisBindings[negKey][action] = -1.0f;
		create_axis(pos_action); 
		create_axis(neg_action);
	}
	else if (type == "gamepad")
	{
		std::string axisName = bindingNode->Attribute("value");
		int controllerID = atoi(bindingNode->Attribute("id_num"));
		SDL_GameControllerAxis k = mGamepadAxisNames[axisName];
		if (mGamepadAxisBindings[controllerID].find(k) == mGamepadAxisBindings[controllerID].end())
			mGamepadAxisBindings[controllerID][k] = action;
		else
			LOG_MANAGER->log_message("Duplicate gamepad axis action specified!! Line: " + std::to_string(bindingNode->GetLineNum()), ERROR_COLOUR);
		create_axis(axisName);
	}
	
	tinyxml2::XMLElement* nextElement = bindingNode->NextSiblingElement();
	if (nextElement != NULL)
		xml_parse_axis_nodes(nextElement);
}

void InputManager::create_axis(std::string name)
{
	if (mAxisValues.find(name) == mAxisValues.end())
		mAxisValues[name] = 0;
	else
		LOG_MANAGER->log_error("Trying to add duplicate Axis mapping!! axis: " + name);
}

void InputManager::create_action(std::string name)
{
	if (mActionValues.find(name) == mActionValues.end())
		mActionValues[name] = 0;
	else
		LOG_MANAGER->log_error("Trying to add duplicate Action mapping!! action: " + name);
}

void InputManager::register_listener(ComponentInputListener* comp)
{
	mListeners.insert(comp);
}

void InputManager::deregister_listener(ComponentInputListener* comp)
{
	mListeners.erase(comp);
}

void InputManager::broadcast_action(std::string action, bool is_pressed)
{
	mActionValues[action] = is_pressed;

	if (this->mDebugging) LOG_MANAGER->log_message((is_pressed ? "pressed: " : "released: ") + action, DEBUG_COLOUR, ERROR_DISPLAY_TIME);
	if (action == "escape")
	{
		APPLICATION->getRoot()->queueEndRendering();
	}
	else
	{
		for (ComponentInputListener* listener : this->mListeners)
			listener->process_key_action(action, is_pressed);
	}
}

void InputManager::broadcast_axis_event(std::string axis, float new_value)
{
	if (this->mDebugging) LOG_MANAGER->log_message(axis + ": " + std::to_string(new_value), DEBUG_COLOUR, ERROR_DISPLAY_TIME);
	for (ComponentInputListener* listener : this->mListeners)
		listener->process_axis_action(axis, new_value);
}

///WORK HERE
void InputManager::scan_for_gamepads()
{
}

///WORK HERE
void InputManager::handle_key(SDL_Keycode k, bool is_pressed)
{
	// Check if action for key_event exists, if it does, update action binding and broadcast event_type
	if (mKeyActionBindings.find(k) != mKeyActionBindings.end())
		broadcast_action(mKeyActionBindings[k], is_pressed);
	else if(mKeyAxisBindings.find(k) != mKeyAxisBindings.end())
	{
		
	}
}

void InputManager::handle_joy_button(SDL_GameControllerButton button, int joy_id, bool is_pressed)
{
	// Find Controller attached
	if (mGamepadButtonBindings.find(joy_id) != mGamepadButtonBindings.end())
	{
		// Find corresponding action
		if (mGamepadButtonBindings[joy_id].find(button) != mGamepadButtonBindings[joy_id].end())
			broadcast_action(mGamepadButtonBindings[joy_id][button], is_pressed);
	}
}

///WORK HERE
void InputManager::handle_joy_axis(SDL_GameControllerAxis axis, int joy_id, Sint16 value)
{
	if (value > mGamepadDeadZone || value < -mGamepadDeadZone)
	{
		// Find Controller attached
		if (mGamepadAxisBindings.find(joy_id) != mGamepadAxisBindings.end())
		{
			// Find corresponding action
			if (mGamepadAxisBindings[joy_id].find(axis) != mGamepadAxisBindings[joy_id].end())
			{
				std::string axisName = mGamepadAxisBindings[joy_id][axis];
				mAxisValues[axisName] += value;
				broadcast_axis_event(axisName, mAxisValues[axisName]);
			}
		}
	}
}