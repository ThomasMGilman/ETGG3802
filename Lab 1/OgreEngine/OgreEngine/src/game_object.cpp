#include <game_object.h>
#include <application.h>

using namespace OgreEngine;

GameObject::GameObject(std::string name, int tag, GameObject* parent, Ogre::Vector3 pos, Ogre::Quaternion rot) : mName(name), mTag(tag)
{
	if (parent != nullptr)
		mSceneNode = parent->get_scene_node()->createChildSceneNode(pos, rot);
	else
		mSceneNode = APPLICATION->get_scene_manager()->getRootSceneNode()->createChildSceneNode(pos, rot);
}

GameObject::~GameObject()
{
	// Remove our scene node from the scene
	
	Ogre::SceneNode* parentNode = mSceneNode->getParentSceneNode();		//BREAKS HERE!!! Causes Read Access Violation when trying to get parent of GameObject*
	if(parentNode != nullptr)
		parentNode->removeChild(mSceneNode);
	
	//Ogre::SceneNode* parentNode = mSceneNode->getParentSceneNode();
	APPLICATION->get_scene_manager()->destroySceneNode(mSceneNode);
	
}


void GameObject::set_parent(GameObject* parent)
{
	// Remove this scene node from it's current parent scene node
	mSceneNode->getParentSceneNode()->removeChild(mSceneNode);

	// Now make it a parent of parent's scene node
	parent->get_scene_node()->addChild(mSceneNode);
}