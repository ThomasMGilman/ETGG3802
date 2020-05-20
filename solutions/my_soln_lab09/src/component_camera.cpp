#include <stdafx.h>
#include <component_camera.h>
#include <game_object.h>
#include <application.h>

ssuge::ComponentCamera::ComponentCamera(GameObject * owner) : Component(owner)
{
	mCamera = APPLICATION->get_scene_manager()->createCamera(owner->get_name() + "_camera");
	owner->get_scene_node()->attachObject(mCamera);
	mCamera->setAutoAspectRatio(true);
	mCamera->setNearClipDistance(0.1f);
	mCamera->setFarClipDistance(1000.0f);
}


ssuge::ComponentCamera::~ComponentCamera()
{
	if (mCamera)
	{
		if (mCamera->getParentSceneNode())
			mCamera->getParentSceneNode()->detachObject(mCamera);
		APPLICATION->get_scene_manager()->destroyCamera(mCamera);
	}
}


Ogre::Ray ssuge::ComponentCamera::get_screen_ray(Ogre::Vector2 v)
{
	return mCamera->getCameraToViewportRay(v.x, v.y);
}