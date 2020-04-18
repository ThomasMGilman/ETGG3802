#include <stdafx.h>
#include <game_object.h>
#include <application.h>
#include <script_manager.h>
#include <script_game_object.h>

using namespace OgreEngine;

GameObject::GameObject(std::string name, int tag, std::string group_name, GameObject* parent, Ogre::Vector3 pos, Ogre::Quaternion rot) : mName(name), mTag(tag), mGroup(group_name)
{
	if (parent != nullptr)
	{
		mSceneNode = parent->get_scene_node()->createChildSceneNode(pos, rot);
		this->mParent = parent;
		parent->set_child_object(this);
	}
	else
		mSceneNode = APPLICATION->get_scene_manager()->getRootSceneNode()->createChildSceneNode(pos, rot);
}

GameObject::~GameObject()
{
	delete_all_components();
	
	Ogre::SceneNode* parentNode = mSceneNode->getParentSceneNode();
	if (parentNode != nullptr)
		parentNode->removeChild(mSceneNode);

	if (this->mParent != nullptr)
		this->mParent->remove_child_association(this->mName);
	//Ogre::SceneNode* parentNode = mSceneNode->getParentSceneNode();
	APPLICATION->get_scene_manager()->destroySceneNode(mSceneNode);
	
}

void GameObject::update(float elapsed)
{
	// Check if script twin present and pass time to that function
	if (mScriptTwin != nullptr)
	{
		if (PyObject_HasAttrString(mScriptTwin, "update"))
		{
			PyObject* elapsedTuple = PyTuple_New(1);
			PyTuple_SetItem(elapsedTuple, 0, PyFloat_FromDouble(elapsed));
			run_method("update", elapsedTuple);
		}
	}

	// Update Attached Components
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
	this->mParent = parent;
}

void OgreEngine::GameObject::make_script_twin(std::string className)
{
	if (SCRIPT_MANAGER->PyClasses.find(className) != SCRIPT_MANAGER->PyClasses.end())
	{
		//PyObject* args = PyTuple_Pack(3,
		//	PyUnicode_FromStringAndSize(this->mGroup.c_str(), this->mGroup.length()),
		//	PyUnicode_FromStringAndSize(this->mName.c_str(), this->mName.length()),
		//	PyLong_FromLong(this->mTag));
		PyObject* pyClass = SCRIPT_MANAGER->PyClasses[className];
		PyObject* newPyObj = PyObject_Call(pyClass, PyTuple_New(0), 0);
		this->set_script_twin(newPyObj);
		((script::GameObject*)newPyObj)->mTwin = this;

		PyObject* empty = PyTuple_New(0);
		if(PyObject_HasAttrString(pyClass, "create"))
			run_method("create", empty);
	}
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

void OgreEngine::GameObject::set_child_object(GameObject* otherGameObject)
{
	if (!otherGameObject->has_child(this->mName) && this->mChildren.find(otherGameObject->mName) == this->mChildren.end())
	{
		this->mChildren[otherGameObject->mName] = otherGameObject;
		otherGameObject->set_parent(this);
	}
}

GameObject* OgreEngine::GameObject::get_child_object(std::string name)
{
	if (this->mChildren.count > 0 && this->has_child(name))
	{
		if (this->mChildren.count(name))
			return this->mChildren[name];
		else
		{
			std::map<std::string, GameObject*>::iterator child = this->mChildren.begin();
			while (child != this->mChildren.end())
			{
				if (child->second->has_child(name))
					return child->second->get_child_object(name);
				child++;
			}
			throw new std::runtime_error("Child Name: "+name+" Does not exist in Children, yet returned this->has_child(name) returned 'true'!!!");
		}
	}
	return nullptr;
}

bool OgreEngine::GameObject::has_child(std::string name)
{
	if (this->mChildren.count(name))
		return true;
	else if(this->mChildren.size() > 0)
	{
		std::map<std::string, GameObject*>::iterator child = this->mChildren.begin();
		while (child != this->mChildren.end())
		{
			if (child->second->has_child(name))
				return true;
			child++;
		}
	}
	return false;
}

void OgreEngine::GameObject::delete_all_children()
{
	if (this->mChildren.size() > 0)
	{
		std::map<std::string, GameObject*>::iterator child = this->mChildren.begin();
		while (child != this->mChildren.end())
		{
			GAME_OBJ_MANAGER->destroy_game_object(child->second->mGroup, child->second->mName);
			child++;
		}
	}
	LOG_MANAGER->log("GameObject: " + this->mName + " in Group:" + this->mGroup + " has deleted all its children!!");
}

void OgreEngine::GameObject::delete_child(std::string name)
{
	if (this->mChildren.count(name))
	{
		GameObject* child = this->mChildren[name];
		GAME_OBJ_MANAGER->destroy_game_object(child->mGroup, child->mName);
		LOG_MANAGER->log("GameObject: " + this->mName + " in Group:" + this->mGroup + " has deleted its Child: "+name+" !!");
	}
}

void OgreEngine::GameObject::remove_child_association(std::string name)
{
	if (this->mChildren.count(name))
	{
		this->mChildren.erase(name);
		LOG_MANAGER->log("GameObject: " + this->mName + " in Group:" + this->mGroup + " has removed association with its Child: " + name + " !!");
	}
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

bool OgreEngine::GameObject::delete_input_listener()
{
	if (this->mComponents.find(Component::ComponentType::INPUT_LISTENER) != this->mComponents.end())
	{
		mCompIter = this->mComponents[Component::ComponentType::INPUT_LISTENER].begin();
		while (mCompIter != this->mComponents[Component::ComponentType::INPUT_LISTENER].end())
		{
			delete(mCompIter->second);
			mCompIter++;
		}
		this->mComponents.erase(Component::ComponentType::INPUT_LISTENER);
		return true;
	}
	return false;
}

template<class C>
C* GameObject::get_component(std::string objectName, Component::ComponentType type)
{
	if (this->mComponents.size() > 0)
	{
		this->mTypeIter = this->mComponents.find(type);
		if (mTypeIter != this->mComponents.end())
		{
			this->mCompIter = this->mTypeIter->second.find(objectName);
			if (this->mCompIter != this->mTypeIter->second.end())
				return dynamic_cast<C*>(this->mCompIter->second);
		}
	}
	return nullptr;
}

template<class C>
C* GameObject::get_component(Component::ComponentType type)
{
	if (this->mComponents.size() > 0)
	{
		this->mTypeIter = this->mComponents.find(type);
		if (mTypeIter != this->mComponents.end())
		{
			if (this->mTypeIter->second.size() > 0)
				return dynamic_cast<C*>(this->mTypeIter->second.begin()->second);
		}
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

ComponentInputListener* GameObject::create_input_listener(std::string listenerName)
{
	if (get_component<ComponentInputListener>(listenerName, Component::ComponentType::INPUT_LISTENER) != nullptr)
	{
		LOG_MANAGER->log_message("INPUT_LISTENER CREATION ERROR!!! Trying to create camera object with a name that already exists: " + this->get_name() + " _camera" + listenerName);
		return nullptr;
	}
	ComponentInputListener* newInputListener = new ComponentInputListener(this, listenerName);
	mComponents[Component::ComponentType::INPUT_LISTENER][listenerName] = newInputListener;
	return newInputListener;
}

ComponentInputListener* OgreEngine::GameObject::get_input_listener()
{
	return get_component<ComponentInputListener>(Component::ComponentType::INPUT_LISTENER);
}

bool OgreEngine::GameObject::operator==(GameObject* other)
{
	if (other->mGroup == this->mGroup && other->mName == this->mName)
		return true;
	return false;
}
