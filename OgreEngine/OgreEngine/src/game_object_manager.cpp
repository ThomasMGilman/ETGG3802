#include <stdafx.h>
#include <utility.h>
#include <game_object_manager.h>
#include <log_manager.h>
#include <application.h>

using namespace OgreEngine;

GameObjectManager* GameObjectManager::msSingleton = nullptr;

GameObjectManager::GameObjectManager()
{
	mDoc = new tinyxml2::XMLDocument;
}

GameObjectManager::~GameObjectManager()
{
	delete(mDoc);
	this->destroy_all();
	// Delete any Queued objects
	std::unordered_set<GameObject*>::iterator QueIter = mQueuedToCreateObjects.begin();
	while (QueIter != mQueuedToCreateObjects.end())
	{
		if (*QueIter != nullptr)
			delete(*QueIter);
		QueIter++;
	}
}

void GameObjectManager::update(float elapsed)
{
	// Add any queued objects to the map of current scene objects
	std::unordered_set<GameObject*>::iterator QueIter = mQueuedToCreateObjects.begin();
	while (QueIter != mQueuedToCreateObjects.end())
	{
		mObjects[(*QueIter)->mGroup][(*QueIter)->mName] = *QueIter;
		QueIter++;
	}
	mQueuedToCreateObjects.clear();

	// Update all scene objects
	std::map<std::string, std::map<std::string, GameObject*>>::iterator GroupsIter = mObjects.begin();
	while (GroupsIter != mObjects.end())
	{
		std::map<std::string, GameObject* >::iterator ObjectsIter = GroupsIter->second.begin();
		while (ObjectsIter != GroupsIter->second.end())
		{
			ObjectsIter->second->update(elapsed);
			ObjectsIter++;
		}
		GroupsIter++;
	}

	// Finished Updating
	if (this->firstUpdate)
		this->firstUpdate = false;
}

void GameObjectManager::load_scenes(std::list<std::tuple<std::string, std::string>> fileNames, bool printReadValues)
{
	for(std::tuple < std::string, std::string > val : fileNames)
		load_scene(std::get<0>(val), std::get<1>(val), printReadValues);
}

void GameObjectManager::load_scene(std::string file, std::string groupName, bool printReadValues)
{
	mDoc->LoadFile(file.c_str());
	if (mDoc->Error())
	{
		std::string error = mDoc->ErrorStr();
		LOG_MANAGER->log("tinyxml2 Error: " + error);
	}
	tinyxml2::XMLElement* firstElement = mDoc->FirstChildElement();
	if (firstElement != NULL)
	{
		LOG_MANAGER->log_message("Read in Scene by: " + std::string(firstElement->Attribute("author")));
		tinyxml2::XMLElement* nodesElement = firstElement->FirstChildElement();
		if (nodesElement != NULL)
		{
			tinyxml2::XMLElement* firstNode = nodesElement->FirstChildElement();
			if(firstNode != NULL)
				parse_xml_nodes(firstNode, groupName, "", printReadValues);
		}
	}
}

void GameObjectManager::parse_xml_nodes(tinyxml2::XMLElement* element, std::string groupName, std::string path, bool printReadValues, GameObject* parent)
{
	std::string nodeVal = element->Value();
	
	if(printReadValues)
		LOG_MANAGER->log_message("Read in XML Element: " + nodeVal);
	if (nodeVal == "node")
	{
		std::string par = parent != nullptr ? parent->get_name() : "NULL";
		std::string name = element->Attribute("name");
		if (this->get_game_object(name, groupName) == nullptr)
		{
			if (printReadValues)
				LOG_MANAGER->log_message("Creating " + name + " Parent is: " + par);
			GameObject* newObject = create_game_object(groupName, name, parent);
			tinyxml2::XMLElement* firstChild = element->FirstChildElement();
			if (firstChild != NULL)
				parse_xml_gameobject(firstChild, groupName, path, printReadValues, newObject);
		}
	}
	else if (nodeVal == "externals")
	{
		std::string externType = element->Attribute("type");

		tinyxml2::XMLElement* firstElement = element->FirstChildElement();
		if (firstElement != NULL)
			parse_xml_external(firstElement, externType, path, printReadValues);
	}
	else if (nodeVal == "environment")
		parse_xml_environment(element, printReadValues);

	tinyxml2::XMLElement* nextSibling = element->NextSiblingElement();
	if (nextSibling != NULL)
		parse_xml_nodes(nextSibling, groupName, path, printReadValues, parent);
}

Ogre::ColourValue GameObjectManager::parse_xml_color_data(tinyxml2::XMLElement* element)
{
	float alpha = element->FloatAttribute("a");
	return Ogre::ColourValue(element->FloatAttribute("r"), element->FloatAttribute("g"), element->FloatAttribute("b"), alpha == NULL ? 1.0f : alpha);
}

Ogre::Vector3 GameObjectManager::parse_xml_position_data(tinyxml2::XMLElement* element)
{
	Ogre::Vector3 newVec;
	if (element->Attribute("x"))
		newVec.x = element->FloatAttribute("x");
	if (element->Attribute("y"))
		newVec.y = element->FloatAttribute("y");
	if (element->Attribute("z"))
		newVec.z = element->FloatAttribute("z");
	return newVec;
}

Ogre::Quaternion GameObjectManager::parse_xml_quaternion_data(tinyxml2::XMLElement* element)
{
	Ogre::Quaternion newQuat;
	if (element->Attribute("qx"))
		newQuat.x = element->FloatAttribute("qx");
	if (element->Attribute("qy"))
		newQuat.y = element->FloatAttribute("qy");
	if (element->Attribute("qz"))
		newQuat.z = element->FloatAttribute("qz");
	if (element->Attribute("qw"))
		newQuat.w = element->FloatAttribute("qw");
	return newQuat;
}

void GameObjectManager::parse_xml_external(tinyxml2::XMLElement* element, std::string resourceType, std::string path, bool printReadValues)
{
	mExternals[resourceType].push_back({ element->Attribute("name"), path });
	if (resourceType == "material")
		set_skybox(element->Attribute("name"));//create_ground_plane(element->Attribute("name"));

	tinyxml2::XMLElement* nextSibling = element->NextSiblingElement();
	if (nextSibling != NULL)
		parse_xml_external(nextSibling, resourceType, path, printReadValues);
}

void GameObjectManager::parse_xml_environment(tinyxml2::XMLElement* element, bool printReadValues)
{
	std::string nodeVal = element->Value();
	if (nodeVal == "colourAmbient")
		APPLICATION->get_scene_manager()->setAmbientLight(parse_xml_color_data(element));
	else if (nodeVal == "colourBackground")
		APPLICATION->getRenderWindow()->getViewport(0)->setBackgroundColour(parse_xml_color_data(element));

	tinyxml2::XMLElement* nextSibling = element->NextSiblingElement();
	if (nextSibling != NULL)
		parse_xml_environment(nextSibling, printReadValues);
}

void GameObjectManager::parse_xml_gameobject(tinyxml2::XMLElement* element, std::string groupName, std::string path, bool printReadValues, GameObject* parent)
{
	std::string nodeVal = element->Value();
	if(printReadValues)
		LOG_MANAGER->log_message("Read in objectProperty: " + nodeVal);

	// Object Type
	if (nodeVal == "node")
		parse_xml_nodes(element, groupName, path, printReadValues, parent);
	else if (nodeVal == "entity")
		parse_xml_mesh(element, groupName, path, printReadValues, parent);
	else if (nodeVal == "camera")
		parse_xml_camera(element, groupName, path, printReadValues, parent);
	else if (nodeVal == "light")
		parse_xml_light(element, groupName, path, printReadValues, parent);
	else if (nodeVal == "userData")
	{
		tinyxml2::XMLElement* propertyElement = element->FirstChildElement();
		if (propertyElement != NULL)
			parse_xml_gameobject(propertyElement, groupName, path, printReadValues, parent);
	}
	else if (nodeVal == "property")
	{
		std::string nodeName = element->Attribute("name");
		if (nodeName == "script")
			parent->make_script_twin(element->Attribute("data"));
		else if (nodeName == "tag")
			set_game_object_tag(element->IntAttribute("data"), parent);
		else if (nodeName == "collider")
		{
			// break up line into colliderType, ColliderType::RECTANGULAR_PRISM ? extentsX, extentsY, extentsZ : radius
			std::vector<std::string> dataLine;
			split_str(element->Attribute("data"), dataLine, ' ');
			if (dataLine.size() > 0)
			{
				if (dataLine[0] == "box" && dataLine.size() == 4)
					parent->create_collider("main_collider", ColliderType::RECTANGULAR_PRISM,
						Ogre::Vector3(atof(dataLine[1].c_str()), atof(dataLine[2].c_str()), atof(dataLine[3].c_str())));
				else if (dataLine[0] == "sphere")
					parent->create_collider("main_collider", ColliderType::SPHERE, Ogre::Vector3(atof(dataLine[1].c_str())));
			}
			else
				throw new std::runtime_error("Scene Parse ERROR!!! Improperly formatted XML. Does not contain collider type and extents info!! data: " + (std::string)element->Attribute("data"));
		}
		else if (nodeName == "collider_layer" || nodeName == "collider_mask")
		{
			if(nodeName == "collider_layer")
				parent->get_collider()->set_layer(element->IntAttribute("data"));
			else
				parent->get_collider()->set_mask(element->IntAttribute("data"));
		}
	}
	else
	{
		// Object Positional data
		if (nodeVal == "position")
			parent->set_position(parse_xml_position_data(element));//Breaks at Camera Node
		else if (nodeVal == "rotation")
			parent->set_orientation(parse_xml_quaternion_data(element));
		else if (nodeVal == "scale")
			parent->set_scale(parse_xml_position_data(element));
	}
	
	tinyxml2::XMLElement* nextSibling = element->NextSiblingElement();
	if (nextSibling != NULL)
		parse_xml_gameobject(nextSibling, groupName, path, printReadValues, parent);
}

void GameObjectManager::parse_xml_mesh(tinyxml2::XMLElement* element, std::string groupName, std::string path, bool printReadValues, GameObject* parent)
{
	std::string name = element->Attribute("name");
	MeshComponent* mesh = parent->create_mesh(name.empty() ? parent->get_name() : name, std::string(element->Attribute("meshFile")));

	// Handle check for userData
	tinyxml2::XMLElement* firstElement = element->FirstChildElement();
	if (firstElement != NULL)
		parse_xml_mesh_data(firstElement, groupName, path, printReadValues, mesh);
}

void GameObjectManager::parse_xml_mesh_data(tinyxml2::XMLElement* element, std::string groupName, std::string path, bool printReadValues, MeshComponent* parent)
{
	std::string nodeVal = element->Value();
	if(printReadValues)
		LOG_MANAGER->log_message("Read in meshProperty: " + nodeVal);

	// Mesh Object Data
	if (nodeVal == "userData")
	{
		tinyxml2::XMLElement* firstElement = element->FirstChildElement();
		if (firstElement != NULL)
			parse_xml_properties(firstElement, groupName, path, printReadValues, parent);
	}

	tinyxml2::XMLElement* nextSibling = element->NextSiblingElement();
	if (nextSibling != NULL)
		parse_xml_mesh_data(nextSibling, groupName, path, printReadValues, parent);
}

void GameObjectManager::parse_xml_camera(tinyxml2::XMLElement* element, std::string groupName, std::string path, bool printReadValues, GameObject* parent)
{
	std::string name = element->Attribute("name");
	CameraComponent* camera = parent->create_camera(name.empty() ? parent->get_name() : name);
	camera->set_fov(element->FloatAttribute("fov"));
	//if (element->Attribute("projectionType") != "perspective")
	//	camera->set_projection_type(Ogre::ProjectionType::PT_ORTHOGRAPHIC);
	camera->set_main_camera(APPLICATION->getRenderWindow()->getViewport(0));

	tinyxml2::XMLElement* firstElement = element->FirstChildElement();
	if (firstElement != NULL)
		parse_xml_camera_data(firstElement, groupName, path, printReadValues, camera);
}

void GameObjectManager::parse_xml_camera_data(tinyxml2::XMLElement* element, std::string groupName, std::string path, bool printReadValues, CameraComponent* parent)
{
	std::string nodeVal = element->Value();
	if(printReadValues)
		LOG_MANAGER->log_message("Read in cameraProperty: " + nodeVal);

	// Camera Object Data
	if (nodeVal == "clipping")
		parent->set_clip_distances(element->FloatAttribute("near"), element->FloatAttribute("far"));

	tinyxml2::XMLElement* nextSibling = element->NextSiblingElement();
	if (nextSibling != NULL)
		parse_xml_camera_data(nextSibling, groupName, path, printReadValues, parent);
}

void GameObjectManager::parse_xml_light(tinyxml2::XMLElement* element, std::string groupName, std::string path, bool printReadValues, GameObject* parent)
{
	std::string lightType = element->Attribute("type");
	OgreEngine::LightType type;
	if (lightType == "point")
		type = OgreEngine::LightType::POINT;
	else if (lightType == "spot")
		type = OgreEngine::LightType::SPOT;
	else
		type = OgreEngine::LightType::DIRECTIONAL;

	std::string name = element->Attribute("name");
	LightComponent* light = parent->create_light(name.empty() ? parent->get_name() : name, type);
	if (type == OgreEngine::LightType::DIRECTIONAL)
		light->set_direction(Ogre::Vector3(0, 0, -1));

	tinyxml2::XMLElement* firstElement = element->FirstChildElement();
	if (firstElement != NULL)
		parse_xml_light_data(firstElement, groupName, path, printReadValues, light);
}

void GameObjectManager::parse_xml_light_data(tinyxml2::XMLElement* element, std::string groupName, std::string path, bool printReadValues, LightComponent* parent)
{
	std::string nodeVal = element->Value();
	if(printReadValues)
		LOG_MANAGER->log_message("Read in lightProperty: " + nodeVal);

	// Light Object data
	if (nodeVal == "colorDiffuse")
		parent->set_diffuse_color(parse_xml_color_data(element));
	else if (nodeVal == "colorSpecular")
		parent->set_specular_color(parse_xml_color_data(element));
	else if (nodeVal == "lightAttenuation")
		parent->set_attenuation(element->FloatAttribute("constant"), element->FloatAttribute("linear"), element->FloatAttribute("quadratic"), element->FloatAttribute("range"));

	tinyxml2::XMLElement* nextSibling = element->NextSiblingElement();
	if (nextSibling != NULL)
		parse_xml_light_data(nextSibling, groupName, path, printReadValues, parent);
}

void GameObjectManager::parse_xml_properties(tinyxml2::XMLElement* element, std::string groupName, std::string path, bool printReadValues, Component* parent)
{
	std::string nodeVal = element->Attribute("name");
	std::string type = element->Attribute("type");
	if (type == "str")
		parent->add_xml_property(nodeVal, std::string(element->Attribute("data")));
	else if (type == "bool")
		parent->add_xml_property(nodeVal, element->BoolAttribute("data"));
	else if (type == "int")
		parent->add_xml_property(nodeVal, element->IntAttribute("data"));
	else if (type == "float")
		parent->add_xml_property(nodeVal, element->FloatAttribute("data"));
	else if (type == "double")
		parent->add_xml_property(nodeVal, element->DoubleAttribute("data"));
	else
		throw new std::exception(("TYPE ERROR!!! Did not provide valid data type given to attrieve!!\n\tGot Type: " + type).c_str());

	tinyxml2::XMLElement* nextSibling = element->NextSiblingElement();
	if (nextSibling != NULL)
		parse_xml_properties(nextSibling, groupName, path, printReadValues, parent);
}

void GameObjectManager::set_game_object_tag(int newTag, GameObject* object)
{
	if (mTaggedObjects.find(object->get_tag()) != mTaggedObjects.end())
	{
		std::vector<GameObject*>::iterator taggedObjects = mTaggedObjects[object->get_tag()].begin();
		while (*taggedObjects != object && taggedObjects != mTaggedObjects[object->get_tag()].end())
			taggedObjects++;

		if (taggedObjects == mTaggedObjects[object->get_tag()].end())
			throw new std::exception("Did not find the object in the tagged list!! Fix comparison");

		mTaggedObjects[object->get_tag()].erase(taggedObjects);
	}
	mTaggedObjects[newTag].push_back(object);
	
	object->set_tag(newTag);
}

GameObject* GameObjectManager::get_game_object(std::string game_object_name)
{
	GameObject* reqObject = nullptr;
	std::map<std::string, std::map<std::string, GameObject*>>::iterator GroupsIter = mObjects.begin();
	std::map<std::string, std::map<std::string, GameObject*>>::reverse_iterator GroupsRevIter = mObjects.rbegin();
	while (GroupsIter != mObjects.end() && GroupsRevIter != mObjects.rend() && *GroupsIter != *GroupsRevIter)
	{
		std::map<std::string, GameObject* >::iterator ObjIter = GroupsIter->second.find(game_object_name);
		if (ObjIter != GroupsIter->second.end())
		{
			reqObject = ObjIter->second;
			break;
		}
		ObjIter = GroupsRevIter->second.find(game_object_name);
		if (ObjIter != GroupsIter->second.end())
		{
			reqObject = ObjIter->second;
			break;
		}
		GroupsIter++;
		GroupsRevIter++;
	}

	return reqObject;
}

GameObject* GameObjectManager::get_game_object(std::string game_object_name, std::string group_name)
{
	GameObject* reqObject = nullptr;
	
	std::map<std::string, std::map<std::string, GameObject*>>::iterator GroupsIter = mObjects.find(group_name);
	if (GroupsIter != mObjects.end())
	{
		std::map<std::string, GameObject* >::iterator ObjIter = GroupsIter->second.find(game_object_name);
		if (ObjIter != GroupsIter->second.end())
			reqObject = ObjIter->second;
		else
			LOG_MANAGER->log("User trying to access nonexistent gameObject: " + game_object_name + " in group: " + group_name);
	}
	else
		LOG_MANAGER->log("User trying to access nonexistent group: "+group_name+" to find gameObject: "+game_object_name);

	return reqObject;
}

void GameObjectManager::get_game_objects(std::string group_name, std::vector<GameObject*>& result)
{
	std::map<std::string, std::map<std::string, GameObject*>>::iterator GroupsIter = mObjects.find(group_name);
	if (GroupsIter != mObjects.end())
	{
		std::map<std::string, GameObject* >::iterator ObjIter = GroupsIter->second.begin();
		while (ObjIter != GroupsIter->second.end())
		{
			result.push_back(ObjIter->second);
			ObjIter++;
		}
	}
	else
		LOG_MANAGER->log("User trying to get gameObjects from nonexistent group: " + group_name);
}

bool GameObjectManager::has_group(std::string group_name)
{
	std::map<std::string, std::map<std::string, GameObject*>>::iterator GroupsIter = mObjects.find(group_name);
	if (GroupsIter != mObjects.end())
		return true;
	return false;
}

void OgreEngine::GameObjectManager::queue_deletion_of_game_object(GameObject* obj)
{
	if (mQueuedToDestroyObjects.count(obj) == 0) 
		mQueuedToDestroyObjects.insert(obj);
}

void OgreEngine::GameObjectManager::destroy_queued_objects()
{
	// Destroy Queued objects
	std::unordered_set<GameObject*>::iterator QueIter = mQueuedToDestroyObjects.begin();
	while (QueIter != mQueuedToDestroyObjects.end())
	{
		this->destroy_game_object((*QueIter)->mGroup, (*QueIter)->mName);
		QueIter++;
	}
	mQueuedToDestroyObjects.clear();
}

bool GameObjectManager::destroy_game_object(std::string group_name, std::string gobj_name, bool ignoreLog)
{
	std::map<std::string, std::map<std::string, GameObject*>>::iterator GroupsIter = mObjects.find(group_name);
	if (GroupsIter != mObjects.end())
	{
		std::map<std::string, GameObject* >::iterator ObjIter = GroupsIter->second.find(gobj_name);
		if (ObjIter != GroupsIter->second.end())
		{
			for (int i = 0; i < mTaggedObjects[ObjIter->second->get_tag()].size(); i++)
			{
				if (mTaggedObjects[ObjIter->second->get_tag()][i] == ObjIter->second)
				{
					mTaggedObjects[ObjIter->second->get_tag()].erase(mTaggedObjects[ObjIter->second->get_tag()].begin() + i);
					break;
				}
			}
			delete(ObjIter->second);
			GroupsIter->second.erase(ObjIter);
			return true;
		}
		else if(!ignoreLog)
			LOG_MANAGER->log("User trying to delete nonexistent gameObject: " + gobj_name + " from group: " + group_name);
	}
	else
		LOG_MANAGER->log("User trying to delete gameObject: "+gobj_name+" from nonexistent group: "+group_name);
	return false;
}

bool GameObjectManager::destroy_game_object(std::string gobj_name)
{
	bool deletedObj = false;
	std::map<std::string, std::map<std::string, GameObject*>>::iterator GroupsIter = mObjects.begin();
	std::map<std::string, std::map<std::string, GameObject*>>::reverse_iterator GroupsRevIter = mObjects.rbegin();
	while (GroupsIter != mObjects.end() && GroupsRevIter != mObjects.rend())
	{
		if (*GroupsIter == *GroupsRevIter)
		{
			this->destroy_game_object(GroupsIter->first, gobj_name, false);
			break;
		}

		destroy_game_object(GroupsIter->first, gobj_name, false);
		destroy_game_object(GroupsRevIter->first, gobj_name, false);

		GroupsIter++;
		GroupsRevIter++;
	}
	return deletedObj;
}

void GameObjectManager::group_destroy(std::string group_name, bool destroy_group)
{
	std::map<std::string, std::map<std::string, GameObject*>>::iterator GroupsIter = mObjects.find(group_name);
	if (GroupsIter != mObjects.end())
	{
		std::map<std::string, GameObject* >::iterator ObjIter = GroupsIter->second.begin();
		while(ObjIter != GroupsIter->second.end())
		{
			delete(ObjIter->second);
			GroupsIter->second.erase(ObjIter);
			ObjIter++;
		}
		if(destroy_group)
			mObjects.erase(GroupsIter->first);
	}
	else
		LOG_MANAGER->log("User trying to clear or delete nonexistent group: " + group_name);
}

void GameObjectManager::group_destroy(std::map<std::string, std::map<std::string, GameObject*>>::iterator& group, bool destroy_group)
{
	std::map<std::string, GameObject* >::iterator ObjIter = group->second.begin();
	while (ObjIter != group->second.end())
	{
		if (updating)
			mQueuedToDestroyObjects.insert(ObjIter->second);
		else
			delete(ObjIter->second);
		ObjIter++;
	}
	if (!updating)
	{
		group->second.clear();
		if (destroy_group)
			mObjects.erase(group->first);
	}
}

void GameObjectManager::destroy_all()
{
	std::map<std::string, std::map<std::string, GameObject*>>::iterator GroupsIter;
	while (mObjects.size() > 0)
	{
		GroupsIter = mObjects.begin();
		group_destroy(GroupsIter, true);
	}
}

void GameObjectManager::set_visibility(std::string group_name, bool is_visible)
{
	std::map<std::string, std::map<std::string, GameObject*>>::iterator GroupsIter = mObjects.find(group_name);
	if (GroupsIter != mObjects.end())
	{
		std::map<std::string, GameObject* >::iterator ObjIter = GroupsIter->second.begin();
		while (ObjIter != GroupsIter->second.end())
		{
			ObjIter->second->set_visibility(is_visible);
			ObjIter++;
		}
	}
	else
		LOG_MANAGER->log("User trying to set object visibility for a nonexistent group: "+group_name);
}

GameObject* GameObjectManager::create_game_object(std::string group_name, std::string object_name, GameObject* parent, unsigned int tag, Ogre::Vector3 pos, Ogre::Quaternion rot)
{
	GameObject* newObj = new GameObject(object_name, tag, group_name, parent, pos, rot);
	
	std::map<std::string, std::map<std::string, GameObject*>>::iterator GroupsIter = mObjects.find(group_name);
	if (GroupsIter != mObjects.end())
	{
		// Check if Object is already in the group
		std::map<std::string, GameObject* >::iterator ObjectsIter = GroupsIter->second.find(object_name);
		// GameObject not in group, create it
		if (ObjectsIter == GroupsIter->second.end() && mQueuedToCreateObjects.count(newObj) == 0)
		{
			if (!updating)
				GroupsIter->second[object_name] = newObj; //.emplace(object_name, newObj);
			else
				mQueuedToCreateObjects.insert(newObj);
		}
		// GameObject is already apart of the dictionary Log Error and return nullptr as GameObject
		else 
		{
			LOG_MANAGER->log_message("GameObject_CreationError: Object: " + object_name + " already exists in group: " + group_name, Ogre::ColourValue(1, 0, 0));
			delete(newObj);
			return nullptr;
		}
	}
	else // Object Group does not exist, create group and place new object in group
		mObjects[group_name][object_name] = newObj;
	mTaggedObjects[tag].push_back(newObj);

	return newObj;
}

void OgreEngine::GameObjectManager::add_queued_objects()
{
	for (std::unordered_set<GameObject*>::iterator objIter = mQueuedToCreateObjects.begin(); objIter != mQueuedToCreateObjects.end(); objIter++)
	{
		if (mObjects[(*objIter)->get_group_name()].count((*objIter)->get_name()) == 0)
			mObjects[(*objIter)->get_group_name()][(*objIter)->get_name()] = *objIter;
		else
			throw new std::runtime_error("Trying to create another instance of an already exsisting game object, this is not allowed!!");
	}
	mQueuedToCreateObjects.clear();
}

GameObject* GameObjectManager::create_ground_plane(std::string materialName)
{
	//Create Ground Plane
	Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);
	Ogre::MeshManager::getSingleton().createPlane(
		"ground",				//Name
		Ogre::RGN_DEFAULT,		//GroupName
		plane,					//Plane object
		100,					//Width
		100,					//Height
		20,						//xSegments
		20,						//ySegments
		true,					//normals?
		1,						//numTexCoordSets
		5,						//uTile floats
		5,						//vTile floats
		Ogre::Vector3::UNIT_Z);	//upVector

	GameObject* groundNode = GAME_OBJ_MANAGER->create_game_object("temporary", "GroundNode");
	MeshComponent* groundEnt = groundNode->create_mesh("groundMesh", "ground");
	groundEnt->set_cast_shadows(true);
	groundEnt->set_material(materialName);

	return groundNode;
}

void GameObjectManager::set_skybox(std::string materialName, float distance, bool drawFirst)
{
	APPLICATION->get_scene_manager()->setSkyBox(true, materialName, distance, drawFirst);
}

void GameObjectManager::set_default_scene()
{
	//add lights to the scene
	GameObject* lightNode = GAME_OBJ_MANAGER->create_game_object("temporary", "PointLight0", nullptr, 0, Ogre::Vector3(20, 80, 50));
	lightNode->create_light("MainLight", OgreEngine::LightType::POINT);

	//attach spotlight to new node and set direction and position
	GameObject* spotLightNode = GAME_OBJ_MANAGER->create_game_object("temporary", "SpotLights", nullptr, 0, Ogre::Vector3(200, 200, 0));

	//create spotlight in scene
	LightComponent* spotLight = spotLightNode->create_light("SpotLight", OgreEngine::LightType::SPOT);
	spotLight->set_diffuse_color(.1, .1, .1);
	spotLight->set_specular_color(.1, .1, .1);
	spotLight->set_spotlight_params(35, 50);
	spotLight->set_direction(-1, -1, 0);

	//add Directional Light
	GameObject* dirLightNode = GAME_OBJ_MANAGER->create_game_object("temporary", "DirectionLights");
	LightComponent* dirLight = dirLightNode->create_light("DirectionLight", OgreEngine::LightType::DIRECTIONAL);//mScnMgr->createLight("DirectionalLight");
	dirLight->set_diffuse_color(.2, .2, .2);
	dirLight->set_specular_color(.2, .2, .2);
	dirLight->set_direction(Ogre::Vector3(0, -1, 1));

	//add Point Light
	GameObject* pointLightNode = GAME_OBJ_MANAGER->create_game_object("temporary", "PointLight1", nullptr, 0, Ogre::Vector3(0, 150, 250));
	LightComponent* pointLight = pointLightNode->create_light("PointLight", OgreEngine::LightType::POINT);
	pointLight->set_diffuse_color(.3, .3, .3);
	pointLight->set_specular_color(.3, .3, .3);

	///////////////////////////////////////////////////////////////////////////////////////////// Add Camera and ViewPort
	// Create Viewport
	Ogre::Viewport* mVp = APPLICATION->getRenderWindow()->getViewport(0);
	mVp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));

	// Create the camera
	// Create CameraNode and attach camera to be put into the scene
	GameObject* camNode = GAME_OBJ_MANAGER->create_game_object("temporary", "CameraNode", nullptr, 0, Ogre::Vector3(0, 15, 30));
	camNode->look_at(Ogre::Vector3::ZERO);

	CameraComponent* cam = camNode->create_camera("MainCamera");
	cam->set_clip_distances(5, 100);
	cam->set_auto_aspect_ratio(true);
	cam->set_main_camera(mVp);

	///////////////////////////////////////////////////////////////////////////////////////////// Add Entitys and plane
	//create Ogre Entity to render and set to cast shadows

	GameObject* ogreNode = GAME_OBJ_MANAGER->create_game_object("temporary", "OgreEnt", nullptr, 0, Ogre::Vector3(0, 5, 0));
	ogreNode->look_at();
	ogreNode->rotate_world(90, 1, 0, 0);

	MeshComponent* ogreEnt = ogreNode->create_mesh("OgreMesh", "Sinbad.mesh");
	ogreEnt->set_cast_shadows(true);

	//create Peguin Entity to render that is a child of Ogre
	GameObject* penguinNode = GAME_OBJ_MANAGER->create_game_object("temporary", "PenguinEnt", ogreNode, 0, Ogre::Vector3(1.75, 3.5, 0));
	penguinNode->look_at();
	penguinNode->set_scale(Ogre::Vector3(.05));
	penguinNode->rotate_world(90, 1, 0, 0);
	penguinNode->rotate_world(-25, 0, 0, 1);

	MeshComponent* penguinEnt = penguinNode->create_mesh("PenguinMesh", "penguin.mesh");
	penguinEnt->set_cast_shadows(true);
	penguinEnt->play_animation("amuse", true, true);

	//Create Ground Plane
	create_ground_plane("Examples/Rockwall");

	///////////////////////////////////////////////////////////////////////////////////////////// Enable and Add SkyBox
	set_skybox();
}