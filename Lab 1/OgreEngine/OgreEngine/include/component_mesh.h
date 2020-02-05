#pragma once
#include <component.h>

namespace OgreEngine
{
	class GameObject;

	/// The MeshComponent component-type is used to load and display a .mesh file instance
	class MeshComponent : public Component
	{
	// @@@@@ ATTRIBUTES @@@@@
	protected:
		/// The ogre entity.
		Ogre::Entity * mEntity;

	// @@@@@ CONSTRUCTORS / DESTRUCTORS @@@@@
	public:
		/// The constructor
		MeshComponent(std::string fname, GameObject * owner, std::string name = "");

		/// Destructor
		~MeshComponent();

	// @@@@@ METHODS @@@@@
	public:
		/// Sets the material for a sub-entity
		void set_material(int subEntity, std::string matName);

	// @@@@@ OVERRIDES from COMPONENT @@@@@
	public:
		/// Returns the type of this component.
		ComponentType get_type() override { return ComponentType::MESH; }
		
		/// Makes this mesh not render (if is_visible is false)
		void set_visible(bool isVisible) override { mEntity->setVisible(isVisible); }
	};
}
