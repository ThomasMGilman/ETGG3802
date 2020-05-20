#include <stdafx.h>
#include <component_camera.h>
#include <game_object.h>
#include <application.h>

using namespace OgreEngine;

CameraComponent::CameraComponent(GameObject * owner, std::string name) : Component(owner, name.empty() ? owner->get_name() + "_camera" : name)
{
	mCamera = APPLICATION->get_scene_manager()->createCamera(this->mComponentName);
	owner->get_scene_node()->attachObject(mCamera);
	mCamera->setAutoAspectRatio(true);
	mCamera->setNearClipDistance(0.1f);
	mCamera->setFarClipDistance(1000.0f);
	mCamera->lookAt(Ogre::Vector3::ZERO);;
}


CameraComponent::~CameraComponent()
{
	if (mCamera)
	{
		if (mCamera->getParentSceneNode())
			mCamera->getParentSceneNode()->detachObject(mCamera);
		APPLICATION->get_scene_manager()->destroyCamera(mCamera);
	}
}

void CameraComponent::update(float elapsed)
{

}

Ogre::Ray CameraComponent::get_screen_ray(Ogre::Vector2 v)
{
	return mCamera->getCameraToViewportRay(v.x, v.y);
}
