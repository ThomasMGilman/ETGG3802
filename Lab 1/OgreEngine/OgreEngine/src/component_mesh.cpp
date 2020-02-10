#include <component_mesh.h>
#include <game_object.h>
#include <application.h>

using namespace OgreEngine;

MeshComponent::MeshComponent(std::string fname, GameObject * owner, std::string name) : Component(owner)
{
	mEntity = APPLICATION->get_scene_manager()->createEntity(owner->get_name() + "_entity" + name, fname);
	owner->get_scene_node()->attachObject(mEntity);
	mAnimationState = NULL;
}

MeshComponent::~MeshComponent()
{
	if (mEntity)
	{
		if (mEntity->getParentSceneNode())
			mEntity->getParentSceneNode()->detachObject(mEntity);
		APPLICATION->get_scene_manager()->destroyEntity(mEntity);
	}
}

void MeshComponent::set_material(int subEntity, std::string matName)
{
	Ogre::SubEntity * se = mEntity->getSubEntity(subEntity);
	se->setMaterialName(matName);
}

void MeshComponent::play_animation(std::string animation, bool enabled, bool looping)
{
	mAnimationState = mEntity->getAnimationState(animation);
	mAnimationState->setEnabled(enabled);
	mAnimationState->setLoop(looping);
}

void MeshComponent::update(float elapsed)
{
	if (mAnimationState != nullptr)
	{
		mAnimationState->addTime(elapsed);
	}
}