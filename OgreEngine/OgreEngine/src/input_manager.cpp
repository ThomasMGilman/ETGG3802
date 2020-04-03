#include <stdafx.h>
#include <input_manager.h>
#include <SDL_name_mappings.h>
#include <log_manager.h>

OgreEngine::InputManager::InputManager(std::string bindings_file)
{
	this->mKeyNames = OgreEngine::InputBindings::mKeyNames;
	this->mGamepadAxisNames = OgreEngine::InputBindings::mGamepadAxisNames;
	this->mGamepadButtonNames = OgreEngine::InputBindings::mGamepadButtonNames;
	
	auto AxisIter = this->mGamepadAxisNames.begin();
	while(AxisIter != this->mGamepadAxisNames.end())
	{
		mAxisValues[AxisIter->first] = 0;
		AxisIter++;
	}
	auto buttonIter = this->mGamepadButtonNames.begin();
	while (buttonIter != this->mGamepadButtonNames.end)
	{
		mActionValues[buttonIter->first] = 0;
		buttonIter++;
	}
	load_bindings(bindings_file);
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

void OgreEngine::InputManager::load_bindings(std::string fname, std::string fpath)
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

void OgreEngine::InputManager::xml_parse_binding_nodes(tinyxml2::XMLElement* bindingNode)
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
			xml_parse_action_nodes(childBinding);
	}
	tinyxml2::XMLElement* nextElement = bindingNode->NextSiblingElement();
	if (nextElement != NULL)
		xml_parse_binding_nodes(nextElement);
}

void OgreEngine::InputManager::xml_parse_action_nodes(tinyxml2::XMLElement* bindingNode)
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
	tinyxml2::XMLElement* nextElement = bindingNode->NextSiblingElement();
	if (nextElement != NULL)
		xml_parse_action_nodes(nextElement);
}

void OgreEngine::InputManager::xml_parse_axis_nodes(tinyxml2::XMLElement* bindingNode)
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

	}
	else if (type == "gamepad")
	{
		std::string keyName = bindingNode->Attribute("value");
		int controllerID = atoi(bindingNode->Attribute("id_num"));
		SDL_GameControllerAxis k = mGamepadAxisNames[keyName];
		if (mGamepadAxisBindings[controllerID].find(k) == mGamepadAxisBindings[controllerID].end())
			mGamepadAxisBindings[controllerID][k] = action;
		else
			LOG_MANAGER->log_message("Duplicate gamepad axis action specified!! Line: " + std::to_string(bindingNode->GetLineNum()), ERROR_COLOUR);
	}
	tinyxml2::XMLElement* nextElement = bindingNode->NextSiblingElement();
	if (nextElement != NULL)
		xml_parse_axis_nodes(nextElement);
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
