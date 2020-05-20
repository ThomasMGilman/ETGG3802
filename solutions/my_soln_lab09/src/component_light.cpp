#include <stdafx.h>
#include <component_light.h>
#include <game_object.h>
#include <application.h>

ssuge::ComponentLight::ComponentLight(GameObject* owner, LightType type) : Component(owner)
{
	mLight = APPLICATION->get_scene_manager()->createLight(owner->get_name() + "_light");
	owner->get_scene_node()->attachObject(mLight);
	if (type == LightType::DIRECTIONAL)
		mLight->setType(Ogre::Light::LT_DIRECTIONAL);
	else if (type == LightType::POINT)
		mLight->setType(Ogre::Light::LT_POINT);
	else if (type == LightType::SPOT)
		mLight->setType(Ogre::Light::LT_SPOTLIGHT);
	mLight->setDirection(0, 0, 1);

	mLight->setCastShadows(true);
}


ssuge::ComponentLight::~ComponentLight()
{
	if (mLight)
	{
		if (mLight->getParentSceneNode())
			mLight->getParentSceneNode()->detachObject(mLight);
		APPLICATION->get_scene_manager()->destroyLight(mLight);
	}
}