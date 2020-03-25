#pragma once
#include <stdafx.h>
#include <singleton.h>
#include <component_input_listener.h>

#define INPUT_MANAGER OgreEngine::InputManager::get_singleton_ptr()

namespace OgreEngine
{
	enum class InputDevice { NONE, KEYBOARD, GAMEPAD };

	class InputManager : public Singleton<InputManager>
	{
	// ****** ATTRIBUTES ****** //
		/// Axis values
		std::map<std::string, float> mAxisValues;

		/// Action values (if greater than 0, consider that action as active)
		std::map<std::string, int> mActionValues;

		/// The InputListener components that would like to receive updates when
		/// an input device is used
		std::set<ComponentInputListener*> mListeners;

		/// Used to keep track of which gamepad was used last (useful for some axis movement)
		/// Some games use this too -- to display different icons for actions ("Start" with gamepad, "Escape" with keyboard)
		InputDevice mLastDeviceUsed = InputDevice::NONE;



		/// Map of keycode => action bindings
		std::map<SDL_Keycode, std::string> mKeyActionBindings;

		/// Map of human-readable keynames => SDL Keycode
		std::map<std::string, SDL_Keycode> mKeyNames;

		/// Keycode => Axis-value modifications.  When this key goes down, add the float
		/// amount to the axis value.  When it comes up, subtract it.
		std::map<SDL_Keycode, std::pair<std::string, float>> mKeyAxisBindings;

		


		/// A mapping of all plugged in gamepads.  The int is the id# of the gamepda
		std::map<int, SDL_GameController*> mGamepads;

		/// Set of human-readable names for gamepad buttons
		std::map<std::string, SDL_GameControllerButton> mGamepadButtonNames;

		/// Set of human-readable names for gamepad axes
		std::map<std::string, SDL_GameControllerAxis> mGamepadAxisNames;

		/// A map of gamepad buttons to actions (the outer key is the joystrick id (or -1 if we look for it
		/// on ANY gamepad), the inner id is the button
		std::map<int, std::map<SDL_GameControllerButton, std::string>> mGamepadButtonBindings;

		/// A map of gamepad axis to axis names (the outer key is the joystick id (or -1 if we look for it 
		/// on ANY gamepad), the inner_id is the axis id
		std::map<int, std::map<SDL_GameControllerAxis, std::string>> mGamepadAxisBindings;

		/// A dead-zone value for gamepads.  If the absolute value of an axis is less than
		/// this, it's considered to be 0.  If it's between this and -1...+1, the value is scaled up proportionally
		float mGamepadDeadZone = 0.2f;

	// ****** CONSTRUCTORS / DESTUCTOR ******
	public:
		/// Default Constructor
		InputManager(std::string bindings_file);

		/// Destructor
		~InputManager();

		// ***** GETTERS / SETTERS *****
	public:
		/// Do we have this axis?
		bool has_axis(std::string name);

		/// Do we have this action?
		bool has_action(std::string name);

		/// Get the value of this axis (0.0f if it is undefined -- the user should probably call has_axis first!)
		float get_axis(std::string name);

		/// Returns true if this action is pressed (false if not, or if we don't have that axis)
		bool is_action_active(std::string name);

		/// Sets the gamepad deadzone
		void set_gamepad_deadzone(float val) { mGamepadDeadZone = val; }
	

		// ***** OTHER METHODS ******
	public:
		/// update function
		void update(float dt);
		
		/// loads an xml bindings file
		void load_bindings(std::string fname);

		/// Creates the axis with this name, if it doesn't already exist (and sets the axis value to 0.0f)
		void create_axis(std::string name);

		/// Creates the action with this name, if it doesn't already exists (and sets the value to 0)
		void create_action(std::string name);

		/// Registers a listener
		void register_listener(ComponentInputListener* comp);

		/// Deregisters a listener (no effect if that listener is not in our set of listeners)
		void deregister_listener(ComponentInputListener* comp);

		/// Broadcasts an action_pressed/released event to all listeners
		void broadcast_action(std::string action, bool is_pressed);

		/// Broadcasta an axis event to all listeners
		void broadcast_axis_event(std::string axis, float new_value);

		/// Scans for gamepads, updating the mGamepads map
		void scan_for_gamepads();

	protected:
		/// Our input event processing will call this when a key goes down (passing true) or up (passing false)
		void handle_key(SDL_Keycode k, bool is_pressed);

		/// Our input event processing will call this when a joy button goes down (passing true) or up (passing false)
		void handle_joy_button(SDL_GameControllerButton button, int joy_id, bool is_pressed);

		/// Our input event processing will call this when a joy axis changes (which will be almost every frame)
		void handle_joy_axis(SDL_GameControllerAxis axis, int joy_id, Sint16 value);
	};
}
