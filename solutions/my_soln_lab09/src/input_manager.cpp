#include <stdafx.h>
#include <input_manager.h>
#include <application.h>

ssuge::InputManager* ssuge::Singleton<ssuge::InputManager>::msSingleton = NULL;



ssuge::InputManager::InputManager(std::string bindings_file)
{
    // Set up the key-name => keycode map
    mKeyNames =
    {
        {"?", SDLK_UNKNOWN},
        {"return", SDLK_RETURN},
        {"escape", SDLK_ESCAPE},
        {"backspace", SDLK_BACKSPACE},
        {"tab", SDLK_TAB},
        {"space", SDLK_SPACE},
        {"!", SDLK_EXCLAIM},
        {"double-quote", SDLK_QUOTEDBL},
        {"#", SDLK_HASH},
        {"%", SDLK_PERCENT},
        {"$", SDLK_DOLLAR},
        {"&", SDLK_AMPERSAND},
        {"single-quote", SDLK_QUOTE},
        {"(", SDLK_LEFTPAREN},
        {")", SDLK_RIGHTPAREN},
        {"*", SDLK_ASTERISK},
        {"+", SDLK_PLUS},
        {",", SDLK_COMMA},
        {"-", SDLK_MINUS},
        {".", SDLK_PERIOD},
        {"/", SDLK_SLASH},
        {"0", SDLK_0},
        {"1", SDLK_1},
        {"2", SDLK_2},
        {"3", SDLK_3},
        {"4", SDLK_4},
        {"5", SDLK_5},
        {"6", SDLK_6},
        {"7", SDLK_7},
        {"8", SDLK_8},
        {"9", SDLK_9},
        {":", SDLK_COLON},
        {";", SDLK_SEMICOLON},
        {"<", SDLK_LESS},
        {", ", SDLK_EQUALS},
        {">", SDLK_GREATER},
        {"?", SDLK_QUESTION},
        {"@", SDLK_AT},

        {"[", SDLK_LEFTBRACKET},
        {"backslash", SDLK_BACKSLASH},
        {"]", SDLK_RIGHTBRACKET},
        {"^", SDLK_CARET},
        {"_", SDLK_UNDERSCORE},
        {"`", SDLK_BACKQUOTE},
        {"a", SDLK_a},
        {"b", SDLK_b},
        {"c", SDLK_c},
        {"d", SDLK_d},
        {"e", SDLK_e},
        {"f", SDLK_f},
        {"g", SDLK_g},
        {"h", SDLK_h},
        {"i", SDLK_i},
        {"j", SDLK_j},
        {"k", SDLK_k},
        {"l", SDLK_l},
        {"m", SDLK_m},
        {"n", SDLK_n},
        {"o", SDLK_o},
        {"p", SDLK_p},
        {"q", SDLK_q},
        {"r", SDLK_r},
        {"s", SDLK_s},
        {"t", SDLK_t},
        {"u", SDLK_u},
        {"v", SDLK_v},
        {"w", SDLK_w},
        {"x", SDLK_x},
        {"y", SDLK_y},
        {"z", SDLK_z},
        {"caps-lock", SDLK_CAPSLOCK},
        {"f1", SDLK_F1},
        {"f2", SDLK_F2},
        {"f3", SDLK_F3},
        {"f4", SDLK_F4},
        {"f5", SDLK_F5},
        {"f6", SDLK_F6},
        {"f7", SDLK_F7},
        {"f8", SDLK_F8},
        {"f9", SDLK_F9},
        {"f10", SDLK_F10},
        {"f11", SDLK_F11},
        {"f12", SDLK_F12},
        {"print-screen", SDLK_PRINTSCREEN},
        {"scroll-lock", SDLK_SCROLLLOCK},
        {"pause", SDLK_PAUSE},
        {"insert", SDLK_INSERT},
        {"home", SDLK_HOME},
        {"page-up", SDLK_PAGEUP},
        {"delete", SDLK_DELETE},
        {"end", SDLK_END},
        {"page-down", SDLK_PAGEDOWN},
        {"right", SDLK_RIGHT},
        {"left", SDLK_LEFT},
        {"down", SDLK_DOWN},
        {"up", SDLK_UP},
        {"numlock-clear", SDLK_NUMLOCKCLEAR},
        {"keypad-divide", SDLK_KP_DIVIDE},
        {"keypad-multiply", SDLK_KP_MULTIPLY},
        {"keypad-minus", SDLK_KP_MINUS},
        {"keypad-plus", SDLK_KP_PLUS},
        {"keypad-enter", SDLK_KP_ENTER},
        {"keypad-1", SDLK_KP_1},
        {"keypad-2", SDLK_KP_2},
        {"keypad-3", SDLK_KP_3},
        {"keypad-4", SDLK_KP_4},
        {"keypad-5", SDLK_KP_5},
        {"keypad-6", SDLK_KP_6},
        {"keypad-7", SDLK_KP_7},
        {"keypad-8", SDLK_KP_8},
        {"keypad-9", SDLK_KP_9},
        {"keypad-0", SDLK_KP_0},
        {"keypad-peiod", SDLK_KP_PERIOD},
        {"application", SDLK_APPLICATION},
        {"power", SDLK_POWER},
        {"keypad-equal", SDLK_KP_EQUALS},
        {"f13", SDLK_F13},
        {"f14", SDLK_F14},
        {"f15", SDLK_F15},
        {"f16", SDLK_F16},
        {"f17", SDLK_F17},
        {"f18", SDLK_F18},
        {"f19", SDLK_F19},
        {"f20", SDLK_F20},
        {"f21", SDLK_F21},
        {"f22", SDLK_F22},
        {"f23", SDLK_F23},
        {"f24", SDLK_F24},
        {"execute", SDLK_EXECUTE},
        {"help", SDLK_HELP},
        {"menu", SDLK_MENU},
        {"select", SDLK_SELECT},
        {"stop", SDLK_STOP},
        {"again", SDLK_AGAIN},
        {"undo", SDLK_UNDO},
        {"cut", SDLK_CUT},
        {"copy", SDLK_COPY},
        {"paste", SDLK_PASTE},
        {"find", SDLK_FIND},
        {"mute", SDLK_MUTE},
        {"volume-up", SDLK_VOLUMEUP},
        {"volume-down", SDLK_VOLUMEDOWN},
        {"comma", SDLK_KP_COMMA},
        {"keypad-(", SDLK_KP_LEFTPAREN},
        {"keypad-)", SDLK_KP_RIGHTPAREN},
        {"keypad-{", SDLK_KP_LEFTBRACE},
        {"keypad-}", SDLK_KP_RIGHTBRACE},
        {"keypad-tab", SDLK_KP_TAB},
        {"keypad-backspace", SDLK_KP_BACKSPACE},
        {"left-ctrl", SDLK_LCTRL},
        {"left-shift", SDLK_LSHIFT},
        {"left-alt", SDLK_LALT},
        {"left-gui", SDLK_LGUI},
        {"right-ctrl", SDLK_RCTRL},
        {"right-shift", SDLK_RSHIFT},
        {"right-alt", SDLK_RALT},
        {"right-gui", SDLK_RGUI}
    };

    mGamepadButtonNames =
    {
        {"a", SDL_CONTROLLER_BUTTON_A},
        {"b", SDL_CONTROLLER_BUTTON_B },
        {"x", SDL_CONTROLLER_BUTTON_X},
        {"y", SDL_CONTROLLER_BUTTON_Y},
        {"back", SDL_CONTROLLER_BUTTON_BACK},
        {"guide", SDL_CONTROLLER_BUTTON_GUIDE},
        {"start", SDL_CONTROLLER_BUTTON_START},
        {"left-stick", SDL_CONTROLLER_BUTTON_LEFTSTICK},
        {"right-stick", SDL_CONTROLLER_BUTTON_RIGHTSTICK},
        {"left-shoulder", SDL_CONTROLLER_BUTTON_LEFTSHOULDER},
        {"right-shoulder", SDL_CONTROLLER_BUTTON_RIGHTSHOULDER},
        {"dpad-up", SDL_CONTROLLER_BUTTON_DPAD_UP},
        {"dpad-down", SDL_CONTROLLER_BUTTON_DPAD_DOWN},
        {"dpad-left", SDL_CONTROLLER_BUTTON_DPAD_LEFT},
        {"dpad-right", SDL_CONTROLLER_BUTTON_DPAD_RIGHT},
        {"max", SDL_CONTROLLER_BUTTON_MAX}
    };

    mGamepadAxisNames =
    {
        {"left-x", SDL_CONTROLLER_AXIS_LEFTX},
        {"left-y", SDL_CONTROLLER_AXIS_LEFTY},
        {"right-x", SDL_CONTROLLER_AXIS_RIGHTX},
        {"right-y", SDL_CONTROLLER_AXIS_RIGHTY},
        {"trigger-left", SDL_CONTROLLER_AXIS_TRIGGERLEFT},
        {"trigger-right", SDL_CONTROLLER_AXIS_TRIGGERRIGHT},
        {"max", SDL_CONTROLLER_AXIS_MAX}
    };

    // Initialize the SDL joystick sub-system (not initialized by Ogre::Bites)
    SDL_InitSubSystem(SDL_INIT_JOYSTICK);
    SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER);

    // Load the initial bindings
    load_bindings(bindings_file);

    // Do our intial scan for gamepads
    scan_for_gamepads();
}


ssuge::InputManager::~InputManager()
{

}


bool ssuge::InputManager::has_axis(std::string name)
{
    std::map<std::string, float>::iterator it = mAxisValues.find(name);
    return it != mAxisValues.end();
}



bool ssuge::InputManager::has_action(std::string name)
{
    std::map<std::string, int>::iterator it = mActionValues.find(name);
    return it != mActionValues.end();
}



float ssuge::InputManager::get_axis(std::string name)
{
    std::map<std::string, float>::iterator it = mAxisValues.find(name);
    if (it == mAxisValues.end())
    {
        LOG_MANAGER->log("Attempting to get invalid axis '" + name + "'", LogPreset::WARNING);
        return 0.0f;
    }
    else
        return it->second;
}



bool ssuge::InputManager::is_action_active(std::string name)
{
    std::map<std::string, int>::iterator it = mActionValues.find(name);
    if (it == mActionValues.end())
    {
        LOG_MANAGER->log("Attempting to get invalid action '" + name + "'", LogPreset::WARNING);
        return false;
    }
    else
        return it->second > 0;
}




void ssuge::InputManager::load_bindings(std::string fname)
{
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError err;
	err = doc.LoadFile(fname.c_str());
    if (err != tinyxml2::XMLError::XML_ERROR_FILE_READ_ERROR)
    {
        tinyxml2::XMLElement* elem = doc.RootElement();

        // Find the action bindings
        tinyxml2::XMLElement* action_root = elem->FirstChildElement("actions");
        if (action_root != NULL)
        {
            tinyxml2::XMLElement* cur_action = action_root->FirstChildElement("action");
            while (cur_action != NULL)
            {
                if (cur_action->Attribute("name") && cur_action->Attribute("type") && cur_action->Attribute("value"))
                {
                    std::string name = std::string(cur_action->Attribute("name"));
                    // Create that axis, if we don't already have it
                    create_action(name);

                    std::string type = std::string(cur_action->Attribute("type"));
                    std::string value = std::string(cur_action->Attribute("value"));
                    if (type == "key")
                    {
                        // Actually add a key => action binding
                        std::map<std::string, SDL_Keycode>::iterator it = mKeyNames.find(value);
                        if (it != mKeyNames.end())
                            mKeyActionBindings[it->second] = name;
                    }
                    else if (type == "gamepad" && cur_action->Attribute("id_num"))
                    {
                        int id_num = cur_action->IntAttribute("id_num");
                        // Actually add a gamepad button => action binding
                        std::map<std::string, SDL_GameControllerButton>::iterator it = mGamepadButtonNames.find(value);
                        if (it != mGamepadButtonNames.end())
                            mGamepadButtonBindings[id_num][it->second] = name;
                    }
                    else
                        LOG_MANAGER->log("invalid action binding type in '" + fname + "' Line " + std::to_string(cur_action->GetLineNum()), LogPreset::WARNING);
                }
                else
                    LOG_MANAGER->log("invalid action binding in '" + fname + "' Line " + std::to_string(cur_action->GetLineNum()), LogPreset::WARNING);

                cur_action = cur_action->NextSiblingElement("action");
            }
        }
        else
            LOG_MANAGER->log("Empty action bindings in '" + fname + "'", LogPreset::DIAGNOSTIC);

        // Find the axis bindings
        tinyxml2::XMLElement* axes_root = elem->FirstChildElement("axes");
        if (axes_root != NULL)
        {
            tinyxml2::XMLElement* cur_axis = axes_root->FirstChildElement("axis");
            while (cur_axis != NULL)
            {
                if (cur_axis->Attribute("name") && cur_axis->Attribute("type"))
                {
                    std::string name = std::string(cur_axis->Attribute("name"));
                    std::string type = std::string(cur_axis->Attribute("type"));
                    if (type == "key")
                    {
                        if (cur_axis->Attribute("positive") && cur_axis->Attribute("negative"))
                        {
                            std::string positive_name = std::string(cur_axis->Attribute("positive"));
                            std::string negative_name = std::string(cur_axis->Attribute("negative"));
                            std::map<std::string, SDL_Keycode>::iterator positive_it = mKeyNames.find(positive_name);
                            std::map<std::string, SDL_Keycode>::iterator negative_it = mKeyNames.find(negative_name);

                            if (positive_it != mKeyNames.end() && negative_it != mKeyNames.end())
                            {
                                // Create this axis, if we don't already have it
                                create_axis(name);

                                // Create a key => axis binding
                                mKeyAxisBindings[positive_it->second] = std::pair<std::string, float>(name, 1.0f);
                                mKeyAxisBindings[negative_it->second] = std::pair<std::string, float>(name, -1.0f);
                            }
                            else
                                LOG_MANAGER->log("invalid axis key binding in '" + fname + "' Line " + std::to_string(cur_axis->GetLineNum()), LogPreset::WARNING);
                        }
                        else
                            LOG_MANAGER->log("invalid +/- axis binding in '" + fname + "' Line " + std::to_string(cur_axis->GetLineNum()), LogPreset::WARNING);
                    }
                    else if (type == "gamepad" && cur_axis->Attribute("id_num") && cur_axis->Attribute("value"))
                    {
                        int id_num = cur_axis->IntAttribute("id_num");
                        std::string axis_name = std::string(cur_axis->Attribute("value"));
                        std::map<std::string, SDL_GameControllerAxis>::iterator it = mGamepadAxisNames.find(axis_name);
                        if (it != mGamepadAxisNames.end())
                            mGamepadAxisBindings[id_num][it->second] = name;
                    }
                    else
                        LOG_MANAGER->log("invalid axis binding type in '" + fname + "' Line " + std::to_string(cur_axis->GetLineNum()), LogPreset::WARNING);
                }
                else
                    LOG_MANAGER->log("invalid axis binding '" + fname + "' Line " + std::to_string(cur_axis->GetLineNum()), LogPreset::WARNING);

                cur_axis = cur_axis->NextSiblingElement("axis");
            }
        }
        else
            LOG_MANAGER->log("missing axes bindings in '" + fname + "'", LogPreset::DIAGNOSTIC);
    }
    else
        LOG_MANAGER->log("Can't find input mappings file: '" + fname + "'", LogPreset::ERROR);
}



void ssuge::InputManager::create_axis(std::string name)
{
    std::map<std::string, float>::iterator it = mAxisValues.find(name);
    if (it == mAxisValues.end())
        mAxisValues[name] = 0.0f;
}


void ssuge::InputManager::create_action(std::string name)
{
    std::map<std::string, int>::iterator it = mActionValues.find(name);
    if (it == mActionValues.end())
        mActionValues[name] = 0;
}


void ssuge::InputManager::register_listener(ComponentInputListener* comp)
{
    // Note: sets don't allow duplicates, so it's not a problem if they're already in here
    mListeners.insert(comp);
}

/// Deregisters a listener (no effect if that listener is not in our set of listeners)
void ssuge::InputManager::deregister_listener(ComponentInputListener* comp)
{
    std::set<ComponentInputListener*>::iterator it = mListeners.find(comp);
    if (it != mListeners.end())
        mListeners.erase(it);
}


void ssuge::InputManager::broadcast_action(std::string action, bool is_pressed)
{
    std::set<ComponentInputListener*>::iterator it = mListeners.begin();
    while (it != mListeners.end())
    {
        if (is_pressed)
            (*it)->action_begin(action);
        else
            (*it)->action_end(action);
        ++it;
    }
}


void ssuge::InputManager::broadcast_axis_event(std::string axis, float new_value)
{
    std::set<ComponentInputListener*>::iterator it = mListeners.begin();
    while (it != mListeners.end())
    {
        (*it)->axis_changed(axis, new_value);
        ++it;
    }
}



void ssuge::InputManager::scan_for_gamepads()
{
    std::map<int, SDL_GameController*>::iterator it = mGamepads.begin();
    while (it != mGamepads.end())
    {
        SDL_GameControllerClose(it->second);
        ++it;
    }
    mGamepads.clear();
    for (int i = 0; i < SDL_NumJoysticks(); ++i) 
    {
        if (SDL_IsGameController(i)) 
        {
            SDL_GameController* gcon = SDL_GameControllerOpen(i);
            if (gcon == NULL)
                LOG_MANAGER->log("Error initializing gamepad " + std::to_string(i), LogPreset::WARNING);
            else
                mGamepads[i] = gcon;
        }
    }
}






void ssuge::InputManager::update(float dt)
{
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
        {
            if (e.key.keysym.sym == SDLK_ESCAPE)
                APPLICATION->quit();
            else if (e.key.keysym.sym == SDLK_BACKQUOTE)
                APPLICATION->toggle_debug_stats();
            else
                handle_key(e.key.keysym.sym, true);
        }
        else if (e.type == SDL_KEYUP && e.key.repeat == 0)
            handle_key(e.key.keysym.sym, false);
        else if (e.type == SDL_JOYBUTTONDOWN || e.type == SDL_JOYBUTTONUP)
            handle_joy_button((SDL_GameControllerButton)e.jbutton.button, e.jbutton.which, e.type == SDL_JOYBUTTONDOWN);
        else if (e.type == SDL_JOYAXISMOTION)
            handle_joy_axis((SDL_GameControllerAxis)e.jaxis.axis, e.jaxis.which, e.jaxis.value);
    }
}



void ssuge::InputManager::handle_key(SDL_Keycode k, bool is_pressed)
{
    // See if this key is bound to an action
    std::map<SDL_Keycode, std::string>::iterator action_it = mKeyActionBindings.find(k);
    if (action_it != mKeyActionBindings.end())
    {
        mActionValues[action_it->second] += is_pressed ? 1 : -1;
        mLastDeviceUsed = InputDevice::KEYBOARD;
        broadcast_action(action_it->second, is_pressed);
    }

    // See if this key is bound to an axis
    std::map<SDL_Keycode, std::pair<std::string, float>>::iterator axis_it = mKeyAxisBindings.find(k);
    if (axis_it != mKeyAxisBindings.end())
    {
        mLastDeviceUsed = InputDevice::KEYBOARD;
        mAxisValues[axis_it->second.first] = is_pressed ? axis_it->second.second : 0;
        broadcast_axis_event(axis_it->second.first, axis_it->second.second);
    }
}



void ssuge::InputManager::handle_joy_button(SDL_GameControllerButton button, int joy_id, bool is_pressed)
{
    std::map<int, std::map<SDL_GameControllerButton, std::string>>::iterator it = mGamepadButtonBindings.find(joy_id);
    if (it == mGamepadButtonBindings.end())
    {
        // If we fail finding a *specific* binding for this gamepad, see if we have an "any" binding
        // (that applies to all gamepads) 
        it = mGamepadButtonBindings.find(-1);
    }

    if (it != mGamepadButtonBindings.end())
    {
        std::map<SDL_GameControllerButton, std::string>::iterator it2 = it->second.find(button);
        if (it2 != it->second.end())
        {
            std::map<std::string, int>::iterator action_it = mActionValues.find(it2->second);
            if (action_it != mActionValues.end())
            {
                broadcast_action(it2->second, is_pressed);
                action_it->second += is_pressed ? 1 : -1;
                mLastDeviceUsed = InputDevice::GAMEPAD;
            }
        }
    }
}



void ssuge::InputManager::handle_joy_axis(SDL_GameControllerAxis axis, int joy_id, Sint16 value)
{
    std::map<int, std::map<SDL_GameControllerAxis, std::string>>::iterator it = mGamepadAxisBindings.find(joy_id);
    if (it == mGamepadAxisBindings.end())
    {
        // If we fail finding a *specific* binding for this gamepad, see if we have an "any" binding
        // (that applies to all gamepads) 
        it = mGamepadAxisBindings.find(-1);
    }

    if (it != mGamepadAxisBindings.end())
    {
        std::map<SDL_GameControllerAxis, std::string>::iterator it2 = it->second.find(axis);
        if (it2 != it->second.end())
        {
            std::map<std::string, float>::iterator axis_it = mAxisValues.find(it2->second);
            if (axis_it != mAxisValues.end())
            {
                float normalized_val = value / 32768.0f;
                
                // Compensate for the deadzone
                if (fabs(normalized_val) < mGamepadDeadZone)
                    normalized_val = 0.0f;
                else
                {
                    if (normalized_val < 0)
                        normalized_val = (normalized_val + mGamepadDeadZone) / (1.0f - mGamepadDeadZone);
                    else
                        normalized_val = (normalized_val - mGamepadDeadZone) / (1.0f - mGamepadDeadZone);

                    // We saw significant movement on a gamepad axis
                    mLastDeviceUsed = InputDevice::GAMEPAD;
                }

                // If we either saw significant movement on a gamepad axis (or no movement, but the
                // last input device used was a gamepad, update the actual axis values)
                if (normalized_val > 0.0f || mLastDeviceUsed == InputDevice::GAMEPAD)
                {
                    axis_it->second = normalized_val;
                    broadcast_axis_event(axis_it->first, normalized_val);
                }
                
            }
        }
    }
}