#include <stdafx.h>
#include <component_mesh.h>
#include <game_object.h>
#include <application.h>

ssuge::ComponentMesh::ComponentMesh(GameObject* owner, std::string fname) : Component(owner),
	mEntity(NULL), mAnimState(NULL)
{
	mEntity = APPLICATION->get_scene_manager()->createEntity(owner->get_name() + "_entity", fname);
	owner->get_scene_node()->attachObject(mEntity);
	mEntity->setCastShadows(true);

	Ogre::MeshPtr mptr = Ogre::MeshManager::getSingleton().getByName(fname);
	mptr->buildEdgeList();
}


ssuge::ComponentMesh::~ComponentMesh()
{
	if (mEntity)
	{
		if (mEntity->getParentSceneNode())
			mEntity->getParentSceneNode()->detachObject(mEntity);
		APPLICATION->get_scene_manager()->destroyEntity(mEntity);
	}
}


void ssuge::ComponentMesh::set_material(std::string mat_name, int sub_entity)
{
	if (sub_entity >= 0)
	{
		Ogre::SubEntity* se = mEntity->getSubEntity(sub_entity);
		se->setMaterialName(mat_name);
	}
	else
		mEntity->setMaterialName(mat_name);
}

void ssuge::ComponentMesh::update(float dt)
{
	if (mAnimState)
	{
		mAnimState->addTime(dt);
	}
}

void ssuge::ComponentMesh::play_animation(std::string anim_name, bool looped)
{
	Ogre::AnimationState* new_anim = mEntity->getAnimationState(anim_name);
	new_anim->setEnabled(true);
	if (new_anim != NULL)
	{
		// See if we have a currently playing animation.  If so, stop it
		if (mAnimState)
			mAnimState->setEnabled(false);

		// Set the looping state for the new animation and make it our
		// active animation
		new_anim->setLoop(looped);
		mAnimState = new_anim;
	}
}
