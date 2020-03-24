#include <stdafx.h>
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
	delete_all_components();
	
	Ogre::SceneNode* parentNode = mSceneNode->getParentSceneNode();
	if(parentNode != nullptr)
		parentNode->removeChild(mSceneNode);
	
	//Ogre::SceneNode* parentNode = mSceneNode->getParentSceneNode();
	APPLICATION->get_scene_manager()->destroySceneNode(mSceneNode);
	
}

void GameObject::update(float elapsed)
{
	if (mScriptTwin != nullptr)
	{
		if (PyObject_HasAttrString(mScriptTwin, "update"))
		{
			PyObject* elapsedTuple = PyTuple_New(1);
			PyTuple_SetItem(elapsedTuple, 0, PyFloat_FromDouble(elapsed));
			run_method("update", elapsedTuple);
		}
	}
	mTypeIter = mComponents.begin();
	while (mTypeIter != mComponents.end())
	{
		mCompIter = mTypeIter->second.begin();
		while (mCompIter != mTypeIter->second.end())
		{
			mCompIter->second->update(elapsed);
			mCompIter++;
		}
		mTypeIter++;
	}
}

void GameObject::set_parent(GameObject* parent)
{
	// Remove this scene node from it's current parent scene node
	mSceneNode->getParentSceneNode()->removeChild(mSceneNode);

	// Now make it a parent of parent's scene node
	parent->get_scene_node()->addChild(mSceneNode);
}

void OgreEngine::GameObject::run_method(std::string meth_name, PyObject* args_tuple)
{
	if (mScriptTwin == nullptr)
		throw new std::exception("PyObject* mScriptTwin is NULL!!! Trying to call method on gameobject that does not have an associated python script attached to it!!!");
	if (PyObject_HasAttrString(mScriptTwin, meth_name.c_str()))
	{
		PyObject* method = PyObject_GetAttrString(mScriptTwin, meth_name.c_str());
		PyObject* results = PyObject_Call(method, args_tuple, 0);
		if (results == nullptr)
			throw new std::exception(("PyObject Method failure!!! The method: " + meth_name + " failed! Check arguments passed and method called!!").c_str());
	}
	else
		throw new std::exception(("Invalid Method called!! mScriptTwin does not contain the method called: " + meth_name).c_str());
}

void GameObject::delete_all_components()
{
	mTypeIter = mComponents.begin();
	while (mTypeIter != mComponents.end())
	{
		mCompIter = mTypeIter->second.begin();
		while (mCompIter != mTypeIter->second.end())
		{
			delete(mCompIter->second);
			mTypeIter->second.erase(mCompIter);
		}
		mComponents.erase(mTypeIter);
	}
}

bool GameObject::delete_component(std::string objectName)
{
	bool deleted = false;
	mTypeIter = mComponents.begin();
	while (mTypeIter != mComponents.end())
	{
		mCompIter = mTypeIter->second.find(objectName);
		if (mCompIter != mTypeIter->second.end())
		{
			deleted = true;
			delete(mCompIter->second);
			mTypeIter->second.erase(objectName);
		}
		mTypeIter++;
	}
	return deleted;
}

bool GameObject::delete_component(std::string objectName, Component::ComponentType type)
{
	mTypeIter = mComponents.find(type);
	if (mTypeIter != mComponents.end())
	{
		mCompIter = mTypeIter->second.find(objectName);
		if (mCompIter != mTypeIter->second.end())
		{
			delete(mCompIter->second);
			mTypeIter->second.erase(objectName);
			return true;
		}
	}
	return false;
}

template<class C>
C* GameObject::get_component(std::string objectName, Component::ComponentType type)
{
	mTypeIter = mComponents.find(type);
	if (mTypeIter != mComponents.end())
	{
		mCompIter = mTypeIter->second.find(objectName);
		if (mCompIter != mTypeIter->second.end())
			return dynamic_cast<C*>(mCompIter->second);
	}
	return nullptr;
}

MeshComponent* GameObject::create_mesh(std::string meshName, std::string fname)
{
	if (get_component<MeshComponent>(meshName, Component::ComponentType::MESH) != nullptr)
	{
		LOG_MANAGER->log_message("MESH_COMPONENT CREATION ERROR!!! Trying to create mesh object with a name that already exits: "+ this->get_name() + " _mesh" + meshName);
		return nullptr;
	}
	MeshComponent* newMesh = new MeshComponent(fname, this, meshName);
	mComponents[Component::ComponentType::MESH][meshName] = newMesh;
	return newMesh;
}

LightComponent* GameObject::create_light(std::string lightName, LightType lightType)
{
	if (get_component<LightComponent>(lightName, Component::ComponentType::LIGHT) != nullptr)
	{
		LOG_MANAGER->log_message("LIGHT_COMPONENT CREATION ERROR!!! Trying to create light object with a name that already exists: " + this->get_name() +" _light" + lightName);
		return nullptr;
	}
	LightComponent* newLight = new LightComponent(lightType, this, lightName);
	mComponents[Component::ComponentType::LIGHT][lightName] = newLight;
	return newLight;
}

CameraComponent* GameObject::create_camera(std::string cameraName)
{
	if (get_component<CameraComponent>(cameraName, Component::ComponentType::CAMERA) != nullptr)
	{
		LOG_MANAGER->log_message("CAMERA_COMPONENT CREATION ERROR!!! Trying to create camera object with a name that already exists: " + this->get_name() + " _camera" + cameraName);
		return nullptr;
	}
	CameraComponent* newCamera = new CameraComponent(this, cameraName);
	mComponents[Component::ComponentType::CAMERA][cameraName] = newCamera;
	return newCamera;
}