#include <stdafx.h>
#include <component_light.h>
#include <game_object.h>
#include <application.h>

using namespace OgreEngine;

LightComponent::LightComponent(LightType type, GameObject * owner, std::string name) : 
	Component(owner, name.empty() ? owner->get_name() + "_light" : name)
{
	mLight = APPLICATION->get_scene_manager()->createLight(this->mComponentName);
	owner->get_scene_node()->attachObject(mLight);
	if (type == LightType::DIRECTIONAL)
		mLight->setType(Ogre::Light::LT_DIRECTIONAL);
	else if (type == LightType::POINT)
		mLight->setType(Ogre::Light::LT_POINT);
	else if (type == LightType::SPOT)
		mLight->setType(Ogre::Light::LT_SPOTLIGHT);
	mLight->setDirection(0, 0, 1);
}


LightComponent::~LightComponent()
{
	if (mLight)
	{
		if (mLight->getParentSceneNode())
			mLight->getParentSceneNode()->detachObject(mLight);
		APPLICATION->get_scene_manager()->destroyLight(mLight);
	}
}

void LightComponent::update(float elapsed)
{

}