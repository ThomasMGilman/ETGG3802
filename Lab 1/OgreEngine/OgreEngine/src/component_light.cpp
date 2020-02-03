#include <stdafx.h>
#include <light_component.h>
#include <game_object.h>
#include <application.h>

ssuge::LightComponent::LightComponent(LightType type, GameObject * owner) : Component(owner)
{
	mLight = APPLICATION->getSceneManager()->createLight(owner->getName() + "_light");
	owner->getSceneNode()->attachObject(mLight);
	if (type == LightType::DIRECTIONAL)
		mLight->setType(Ogre::Light::LT_DIRECTIONAL);
	else if (type == LightType::POINT)
		mLight->setType(Ogre::Light::LT_POINT);
	else if (type == LightType::SPOT)
		mLight->setType(Ogre::Light::LT_SPOTLIGHT);
	mLight->setDirection(0, 0, 1);
}


ssuge::LightComponent::~LightComponent()
{
	if (mLight)
	{
		if (mLight->getParentSceneNode())
			mLight->getParentSceneNode()->detachObject(mLight);
		APPLICATION->getSceneManager()->destroyLight(mLight);
	}
}