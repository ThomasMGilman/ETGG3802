#include <game_object.h>
#include <application.h>

using namespace OgreEngine;

GameObject::GameObject(std::string name, int tag, GameObject* parent) : mName(name), mTag(tag)
{
	if (parent != NULL)
		mSceneNode = parent->get_scene_node()->createChildSceneNode();
	else
		mSceneNode = APPLICATION->get_scene_manager()->getRootSceneNode()->createChildSceneNode();
}

GameObject::~GameObject()
{
	// Remove our scene node from the scene
	mSceneNode->getParentSceneNode()->removeChild(mSceneNode);
	APPLICATION->get_scene_manager()->destroySceneNode(mSceneNode);
}


void GameObject::set_parent(GameObject* parent)
{
	// Remove this scene node from it's current parent scene node
	mSceneNode->getParentSceneNode()->removeChild(mSceneNode);

	// Now make it a parent of parent's scene node
	parent->get_scene_node()->addChild(mSceneNode);
}