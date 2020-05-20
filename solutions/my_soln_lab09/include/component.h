#pragma once
#include <stdafx.h>

namespace ssuge
{
	// Forward reference to the GameObject class
	class GameObject;

	/// This is the list of all component types
	enum class ComponentType
	{
		COLLIDER,
		LIGHT,
		MESH,
		CAMERA,
		INPUT_LISTENER
	};

	/// Component is the base class for all "real" components
	/// (e.g. MeshComponent)
	class Component
	{
	protected:
		/// The containing game object
		GameObject* mOwner;
	public:
		/// Main constructor
		Component(GameObject* owner);

		/// The destructor needs to be virtual so the *real* destructor can be called
		virtual ~Component() {}

		/// This method allows any component (even if a Component*) to give back
		/// its true type.
		virtual ComponentType get_type() = 0;

		/// Do any per-frame updates to this component
		virtual void update(float dt) {}

		/// Makes this component visible or not.
		virtual void set_visible(bool is_visible) {}

		/// Gets the game object this component is attached to
		virtual GameObject* get_owner() { return mOwner; }
	};
}
