#pragma once
#include <stdafx.h>
#include <component.h>

namespace ssuge
{
	class GameObject;

	/// The MeshComponent component-type is used to load and display a .mesh file instance
	class ComponentMesh : public Component
	{
	// @@@@@ ATTRIBUTES @@@@@
	protected:
		/// The ogre entity.
		Ogre::Entity * mEntity;

		/// The currently playing animation (or NULL) if none is player
		Ogre::AnimationState* mAnimState;

	// @@@@@ CONSTRUCTORS / DESTRUCTORS @@@@@
	public:
		/// The constructor
		ComponentMesh(GameObject* owner, std::string fname);

		/// Destructor
		~ComponentMesh();

	// @@@@@ METHODS @@@@@
	public:
		/// Sets the material for a sub-entity
		void set_material(std::string mat_name, int sub_entity=-1);

		/// Sets whether we case shadows or not (defaults to true)
		void set_cast_shadows(bool cast) { mEntity->setCastShadows(cast); }

	// @@@@@ OVERRIDES from COMPONENT @@@@@
	public:
		/// Returns the type of this component.
		ComponentType get_type() override { return ComponentType::MESH; }
		
		/// Makes this mesh not render (if is_visible is false)
		void set_visible(bool is_visible) override { mEntity->setVisible(is_visible); }

		/// Updates this component
		void update(float dt) override;

	// @@@@@ OTHER METHODS @@@@@
	public:
		/// Starts playing an animation
		void play_animation(std::string anim_name, bool looped);
	};
}
