#pragma once
#include <Ogre.h>

namespace OgreEngine
{
	class GameObject;
	class Component
	{
	protected:
		GameObject* Parent;
	private:

	public:

		enum class ComponentType { CAMERA, LIGHT, MESH };

		Component(GameObject* owner) { this->Parent = owner; };

		~Component() {};

		virtual ComponentType get_type() = NULL;

		virtual void set_visible(bool isVisible) = NULL;
	};
}