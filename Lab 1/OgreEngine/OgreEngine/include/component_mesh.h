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
		Ogre::AnimationState* mAnimationState;

	// @@@@@ CONSTRUCTORS / DESTRUCTORS @@@@@
	public:
		/// The constructor
		MeshComponent(std::string fname, GameObject * owner, std::string name = "");

		/// Destructor
		~MeshComponent();

		void update(float elapsed) override;

	// @@@@@ METHODS @@@@@
	public:
		/// Sets the material for a sub-entity
		void set_material(int subEntity, std::string matName);

	// @@@@@ OVERRIDES from COMPONENT @@@@@
	public:
		/// Returns the type of this component.
		ComponentType get_type() override { return ComponentType::MESH; };
		
		/// Makes this mesh not render (if is_visible is false)
		void set_visible(bool isVisible) override { mEntity->setVisible(isVisible); };
		
		/// Makes this mesh cast shadows
		void set_cast_shadows(bool set) { mEntity->setCastShadows(set); };

		void set_material(std::string fileName) { mEntity->setMaterialName(fileName); };

		void play_animation(std::string animation, bool enabled = true, bool looping = false);

		void set_animation(std::string animation) { mAnimationState = mEntity->getAnimationState(animation); };

		void set_animation_looping(bool set) { mAnimationState->setLoop(set); };

		void enable_animation(bool set) { mAnimationState->setEnabled(set); };
	};
}
