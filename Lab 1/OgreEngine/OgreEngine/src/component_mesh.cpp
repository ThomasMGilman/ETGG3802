#include <component_mesh.h>
#include <game_object.h>
#include <application.h>

using namespace OgreEngine;

MeshComponent::MeshComponent(std::string fname, GameObject * owner, std::string name) : Component(owner)
{
	mEntity = APPLICATION->get_scene_manager()->createEntity(owner->get_name() + "_entity" + name, fname);
	owner->get_scene_node()->attachObject(mEntity);
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