#pragma once
#include <stdafx.h>
#include <component.h>

namespace ssuge
{
	// Forward reference to the game object class (which we include in the .cpp file to avoid a circular dependency)
	class GameObject;

	/// The CameraComponent class is used to create a camera which is attached to a GameObject.  It is a bit unique
	/// in that it can have positional / rotationall offsets (from the parent game object).  This is to facilitate 
	/// first-person, third-person cameras.
	class ComponentCamera : public Component
	{
	// @@@@@ ATTRIBUTES @@@@@
	protected:
		/// The Ore Camera this is based on.
		Ogre::Camera * mCamera;

	// @@@@@ CONSTRUCTOR / DESTRUCTORS @@@@@
	public:
		/// Constructor
		ComponentCamera(GameObject * owner);

		/// Destructor
		~ComponentCamera();
		
	// @@@@@ OVERRIDES from Component class @@@@@
	public:
		/// Tells the caller what type we are.
		ComponentType get_type() override { return ComponentType::CAMERA; }
		
		/// Makes this component inactive if is_visible is set to true
		void set_visible(bool is_visible) override { mCamera->setVisible(is_visible); }

	// @@@@@ METHODS @@@@@
	public:
		/// Sets the directional offset of this camera (from game object)
		void set_direction(Ogre::Vector3 d) { mCamera->setDirection(d); }

		/// Sets the positional offset of this camera (from game object)
		void set_position_offset(Ogre::Vector3 p) { mCamera->setPosition(p); }

		/// Sets the near and far clip values for the camera
		void set_clip_distances(float n, float f) { mCamera->setNearClipDistance(n); mCamera->setFarClipDistance(f); }

		/// Connects this contained camera to a viewport (making it render there)
		void connect_to_viewport(Ogre::Viewport * v) { v->setCamera(mCamera); }

		/// Gets a Ray which goes through the (normalized) screen position
		Ogre::Ray get_screen_ray(Ogre::Vector2 v);

		/// Sets the field of view of the camera.  The passed value should be the angle made by the camera's look direction and either
		/// the top (or bottom) of the view plane.
		void set_field_of_view(float degrees) { mCamera->setFOVy(Ogre::Degree(degrees)); }

	// @@@@@ FRIENDS @@@@@
		friend class Application;
	};
}
