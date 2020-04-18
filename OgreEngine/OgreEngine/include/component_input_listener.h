#pragma once
#include <stdafx.h>
#include <component.h>

namespace OgreEngine
{
	class GameObject;

	class ComponentInputListener : public Component
	{
	private:
	protected:

		GameObject* mParent;
		Ogre::Node* mNode;
	public:

		ComponentInputListener(GameObject* owner, std::string name = "");

		~ComponentInputListener();

		void update(float elapsed) override {};

		ComponentType get_type() override { return ComponentType::INPUT_LISTENER; }

		void set_visible(bool isVisible) override {};

		/// process incoming broadcasted input button/key events
		void process_key_action(std::string action, bool is_pressed);

		/// process incoming broadcasted input axis events
		void process_axis_action(std::string axis, float amount);
	};
}