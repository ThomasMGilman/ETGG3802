#pragma once
#include <stdafx.h>
#include <component.h>

namespace ssuge
{
	// Forward references
	class GameObject;
	
	/// This component is a little different, structurally, from our others.
	/// It doesn't really have any attributes (other than those inherited).  Its
	/// purpose -- when the game object creaates one of these, it is signalling
	/// that it wants to receive input messages (when a key is pressed, for example)
	class ComponentInputListener : public Component
	{
	protected:
		// @@@@@ ATTRIBUTES @@@@@
		// None!

		// @@@@@ CONSTRUCTORS / DESTRUCTORS @@@@@
	public:
		/// The constructor
		ComponentInputListener(GameObject* owner);

		/// Destructor
		~ComponentInputListener();


		// @@@@@ OVERRIDES from COMPONENT @@@@@
	public:
		/// Returns the type of this component.
		ComponentType get_type() override { return ComponentType::INPUT_LISTENER; }

		// @@@@@ METHODS the INPUT MANAGER will call
		/// Called when an action button is pressed (e.g. "jump")
		void action_begin(std::string action_name);

		/// Called when an action button is released (e.g. "jump")
		void action_end(std::string action_name);


		/// Called when an axis changes value (e.g. "horizontal")
		void axis_changed(std::string axis_name, float new_value);

	};
}
