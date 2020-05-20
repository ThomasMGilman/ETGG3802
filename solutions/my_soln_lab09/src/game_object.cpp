#include <stdafx.h>
#include <game_object.h>
#include <application.h>

ssuge::GameObject::GameObject(std::string name, int tag, GameObject * parent) : mName(name), mTag(tag),
	mScriptTwin(NULL), mParentObject(parent)
{
	if (parent != NULL)
		mSceneNode = parent->get_scene_node()->createChildSceneNode();
	else
		mSceneNode = APPLICATION->get_scene_manager()->getRootSceneNode()->createChildSceneNode();
}

ssuge::GameObject::~GameObject()
{
	// If we have a script twin, decref it (this *hopefully* is the only reference
	// to it, so it should then be garbage-collected)
	if (mScriptTwin)
		Py_DECREF(mScriptTwin);

	// Destroy our components
	std::map<ComponentType, Component*>::iterator it = mComponents.begin();
	while (it != mComponents.end())
	{
		delete it->second;
		++it;
	}
	mComponents.clear();

	// Remove our scene node from the scene
	if (mSceneNode->getParentSceneNode())
	{
		mSceneNode->getParentSceneNode()->removeChild(mSceneNode);
	}
	APPLICATION->get_scene_manager()->destroySceneNode(mSceneNode);
}


void ssuge::GameObject::set_parent(GameObject * parent)
{
	// Remove this scene node from it's current parent scene node
	mSceneNode->getParentSceneNode()->removeChild(mSceneNode);

	// Now make it a parent of parent's scene node
	parent->get_scene_node()->addChild(mSceneNode);
}



ssuge::ComponentCollider* ssuge::GameObject::create_box_collider_component(int layer, long long mask, const Ogre::Vector3& extents)
{
	if (COLLISION_MANAGER->validate_mask_and_layer(mask, layer))
	{
		// Create the new component
		ComponentCollider* comp = new ComponentCollider(this, layer, mask, extents);

		// If we have an existing component of this type, delete it before we replace it
		std::map<ComponentType, Component*>::iterator it = mComponents.find(ComponentType::COLLIDER);
		if (it != mComponents.end())
			delete it->second;

		// Set our new component to that slot and return it
		mComponents[ComponentType::COLLIDER] = comp;
		return comp;
	}
	else
	{
		LOG_MANAGER->log("invalid collision layer (" + std::to_string(layer) + ") or mask (" + std::to_string(mask) + ")", LogPreset::WARNING);
		return NULL;
	}
}



ssuge::ComponentCollider* ssuge::GameObject::create_sphere_collider_component(int layer, long long mask, float radius)
{
	if (COLLISION_MANAGER->validate_mask_and_layer(mask, layer))
	{
		// Create the new component
		ComponentCollider* comp = new ComponentCollider(this, layer, mask, radius);

		// If we have an existing component of this type, delete it before we replace it
		std::map<ComponentType, Component*>::iterator it = mComponents.find(ComponentType::COLLIDER);
		if (it != mComponents.end())
			delete it->second;

		// Set our new component to that slot and return it
		mComponents[ComponentType::COLLIDER] = comp;
		return comp;
	}
	else
	{
		LOG_MANAGER->log("invalid collision layer (" + std::to_string(layer) + ") or mask (" + std::to_string(mask) + ")", LogPreset::WARNING);
		return NULL;
	}
}



ssuge::ComponentMesh* ssuge::GameObject::create_mesh_component(std::string fname)
{
	// Create the new component
	ComponentMesh* comp = new ComponentMesh(this, fname);

	// If we have an existing component of this type, delete it before we replace it
	std::map<ComponentType, Component*>::iterator it = mComponents.find(ComponentType::MESH);
	if (it != mComponents.end())
		delete it->second;

	// Set our new component to that slot and return it
	mComponents[ComponentType::MESH] = comp;
	return comp;
}

ssuge::ComponentLight* ssuge::GameObject::create_light_component(LightType lt)
{
	// Create the new component
	ComponentLight* comp = new ComponentLight(this, lt);

	// If we have an existing component of this type, delete it before we replace it
	std::map<ComponentType, Component*>::iterator it = mComponents.find(ComponentType::LIGHT);
	if (it != mComponents.end())
		delete it->second;

	// Set our new component to that slot and return it
	mComponents[ComponentType::LIGHT] = comp;
	return comp;
}

ssuge::ComponentCamera* ssuge::GameObject::create_camera_component()
{
	// Create the new component
	ComponentCamera* comp = new ComponentCamera(this);

	// If we have an existing component of this type, delete it before we replace it
	std::map<ComponentType, Component*>::iterator it = mComponents.find(ComponentType::CAMERA);
	if (it != mComponents.end())
		delete it->second;

	// Set our new component to that slot and return it
	mComponents[ComponentType::CAMERA] = comp;
	return comp;
}



ssuge::ComponentInputListener* ssuge::GameObject::create_input_listener_component()
{
	// Create the new component
	ComponentInputListener* comp = new ComponentInputListener(this);

	// If we have an existing component of this type, delete it before we replace it
	std::map<ComponentType, Component*>::iterator it = mComponents.find(ComponentType::INPUT_LISTENER);
	if (it != mComponents.end())
		delete it->second;

	// Set our new component to that slot and return it
	mComponents[ComponentType::INPUT_LISTENER] = comp;
	return comp;
}



ssuge::ComponentCollider* ssuge::GameObject::get_collider_component()
{
	std::map<ComponentType, Component*>::iterator it;
	it = mComponents.find(ComponentType::COLLIDER);
	if (it != mComponents.end())
		return dynamic_cast<ComponentCollider*>(it->second);
	else
		return NULL;
}


ssuge::ComponentMesh* ssuge::GameObject::get_mesh_component()
{
	std::map<ComponentType, Component*>::iterator it;
	it = mComponents.find(ComponentType::MESH);
	if (it != mComponents.end())
		return dynamic_cast<ComponentMesh*>(it->second);
	else
		return NULL;
}

ssuge::ComponentLight* ssuge::GameObject::get_light_component()
{
	std::map<ComponentType, Component*>::iterator it;
	it = mComponents.find(ComponentType::LIGHT);
	if (it != mComponents.end())
		return dynamic_cast<ComponentLight*>(it->second);
	else
		return NULL;
}

ssuge::ComponentCamera* ssuge::GameObject::get_camera_component()
{
	std::map<ComponentType, Component*>::iterator it;
	it = mComponents.find(ComponentType::CAMERA);
	if (it != mComponents.end())
		return dynamic_cast<ComponentCamera*>(it->second);
	else
		return NULL;
}


ssuge::ComponentInputListener* ssuge::GameObject::get_input_listener_component()
{
	std::map<ComponentType, Component*>::iterator it;
	it = mComponents.find(ComponentType::INPUT_LISTENER);
	if (it != mComponents.end())
		return dynamic_cast<ComponentInputListener*>(it->second);
	else
		return NULL;
}



void ssuge::GameObject::destroy_component(ComponentType t)
{
	std::map<ComponentType, Component*>::iterator it = mComponents.find(t);
	if (it != mComponents.end())
	{
		delete it->second;
		mComponents.erase(it);
	}
}


void ssuge::GameObject::update(float dt)
{
	// Call the script-twin's update method, if we have one
	if (mScriptTwin)
	{
		PyObject* args = PyTuple_New(1);
		PyTuple_SetItem(args, 0, PyFloat_FromDouble((double)dt));
		execute_script_method("update", args);
		Py_DECREF(args);
	}

	// Update our components
	std::map<ComponentType, Component*>::iterator it = mComponents.begin();
	while (it != mComponents.end())
	{
		it->second->update(dt);
		++it;
	}
}



void ssuge::GameObject::get_local_axes(Ogre::Vector3& xaxis, Ogre::Vector3& yaxis, Ogre::Vector3& zaxis)
{
	Ogre::Quaternion Q = mSceneNode->_getDerivedOrientation();
	Q.ToAxes(xaxis, yaxis, zaxis);
}



void ssuge::GameObject::set_visibility(bool is_visible)
{
	mSceneNode->setVisible(is_visible);		// Doesn't seem to do anything...
	for (std::pair<ComponentType, Component*> c : mComponents)
		c.second->set_visible(is_visible);
}



void ssuge::GameObject::execute_script_method(std::string method_name, PyObject* args_tuple)
{
	PyObject* meth;
	if (PyObject_HasAttrString(mScriptTwin, method_name.c_str()))
		meth = PyObject_GetAttrString(mScriptTwin, method_name.c_str());
	else
		meth = NULL;
	
	if (meth != NULL && PyCallable_Check(meth))
	{
		PyObject* result = PyObject_Call(meth, args_tuple, NULL);
		if (result == NULL)
			SCRIPT_MANAGER->handle_error();
		else
		{
			// Eventually, we might want to do something with the result...
			Py_DECREF(result);
		}
	}
	
	Py_XDECREF(meth);
}

