#pragma once
#include <component.h>
#include <Ogre.h>

namespace OgreEngine
{
	// Forward reference to game object class (used to prevent a circular dependency loop)
	class GameObject;

	/// The types of lights we support.
	enum class LightType { POINT, DIRECTIONAL, SPOT };

	/// The LightComponent component-specilization is used to create a standard light (spot, point, or directional) on the 
	/// attached game object.
	class LightComponent : public Component
	{
	// @@@@@ ATTRIBUTES @@@@@
	protected:
		/// The ogre light we're based upon.
		Ogre::Light * mLight;

	// @@@@@ CONSTRUCTORS / DESTRUCTORS @@@@@
	public:
		/// Constructor
		LightComponent(LightType type, GameObject * owner, std::string name = "");

		/// Destructor
		~LightComponent();

		void update(float elapsed) override;

	// @@@@@ COMPONENT OVERRIDES
	public:
		/// Return the type of this component
		ComponentType get_type() override { return ComponentType::LIGHT; }

		/// Deactivates this component if is_visible is false.
		void set_visible(bool isVisible) override { mLight->setVisible(isVisible); }
		
	// @@@@@ SETTERS @@@@@
	public:
		/// Sets the directional offset of this light (ignored if this is a point light)
		void set_direction(Ogre::Vector3 d) { mLight->setDirection(d); }

		void set_attenuation(float constant, float linear, float quadratic, float range) { mLight->setAttenuation(range, constant, linear, quadratic); };

		void set_direction(float x, float y, float z) { mLight->setDirection(x, y, z); }

		/// Sets the diffuse light color of this light
		void set_diffuse_color(Ogre::ColourValue c) { mLight->setDiffuseColour(c); }

		/// Sets the diffuse light color of this light
		void set_diffuse_color(float r, float g, float b) { mLight->setDiffuseColour(Ogre::ColourValue(r, g, b)); }

		/// Sets the specular light color of this light.
		void set_specular_color(Ogre::ColourValue c) { mLight->setSpecularColour(c); }

		/// Sets the specular light color of this light.
		void set_specular_color(float r, float g, float b) { mLight->setSpecularColour(Ogre::ColourValue(r, g, b)); }

		/// Sets the position of this light (relative to the parent game object) -- ignored if this is a directional light.
		void set_position_offset(Ogre::Vector3 p) { mLight->setPosition(p); }
		
		/// Only affects spotlights.
		void set_spotlight_params(float innerAngle, float outerAngle) { mLight->setSpotlightRange(Ogre::Degree(innerAngle), Ogre::Degree(outerAngle)); }
	};
}
