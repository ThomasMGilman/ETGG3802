#pragma once
#include <component.h>
#include <Ogre.h>

namespace OgreEngine
{
	// Forward reference to the game object class (which we include in the .cpp file to avoid a circular dependency)
	class GameObject;

	/// The CameraComponent class is used to create a camera which is attached to a GameObject.  It is a bit unique
	/// in that it can have positional / rotationall offsets (from the parent game object).  This is to facilitate 
	/// first-person, third-person cameras.
	class CameraComponent : public Component
	{
	// @@@@@ ATTRIBUTES @@@@@
	protected:
		/// The Ore Camera this is based on.
		Ogre::Camera * mCamera;

	// @@@@@ CONSTRUCTOR / DESTRUCTORS @@@@@
	public:
		/// Constructor
		CameraComponent(GameObject * owner, std::string name = "");

		/// Destructor
		~CameraComponent();

		void update(float elapsed) override;
		
	// @@@@@ OVERRIDES from Component class @@@@@
	public:
		/// Tells the caller what type we are.
		ComponentType get_type() override { return ComponentType::CAMERA; }
		
		/// Makes this component inactive if is_visible is set to true
		void set_visible(bool isVisible) override { mCamera->setVisible(isVisible); }

	// @@@@@ METHODS @@@@@
	public:
		/// Sets the directional offset of this camera (from game object)
		void set_direction(Ogre::Vector3 d) { mCamera->setDirection(d); }

		/// Sets the positional offset of this camera (from game object)
		void set_positionOffset(Ogre::Vector3 p) { mCamera->setPosition(p); }

		/// Sets the near and far clip values for the camera
		void set_clip_distances(float n, float f) { mCamera->setNearClipDistance(n); mCamera->setFarClipDistance(f); }

		/// Connects this contained camera to a viewport (making it render there)
		void set_main_camera(Ogre::Viewport* v) { v->setCamera(mCamera); this->set_aspect_ratio(v->getActualWidth(), v->getActualHeight()); }

		void set_auto_aspect_ratio(bool set) { mCamera->setAutoAspectRatio(set); }

		void set_aspect_ratio(float width, float height) { mCamera->setAspectRatio(Ogre::Real(width) / Ogre::Real(height)); }

		/// Gets a Ray which goes through the (normalized) screen position
		Ogre::Ray get_screen_ray(Ogre::Vector2 v);
	};
}
