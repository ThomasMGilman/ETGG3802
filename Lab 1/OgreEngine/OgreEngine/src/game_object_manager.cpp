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
	destroy_all();
}

void GameObjectManager::update(float elapsed)
{
	mGroupsIter = mObjects.begin();
	while (mGroupsIter != mObjects.end())
	{
		mObjIter = mGroupsIter->second.begin();
		while (mObjIter != mGroupsIter->second.end())
		{
			mObjIter->second->update(elapsed);
			mObjIter++;
		}
		mGroupsIter++;
	}
}

void GameObjectManager::load_scenes(std::list<std::tuple<std::string, std::string>> fileNames)
{
	for(std::tuple < std::string, std::string > val : fileNames)
		load_scene(std::get<0>(val), std::get<1>(val));
}

void GameObjectManager::load_scene(std::string fileName, std::string path)
{
	mDoc->LoadFile((path + fileName).c_str());
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
				parse_xml_nodes(firstNode, fileName, path);
		}
	}
}

void GameObjectManager::parse_xml_nodes(tinyxml2::XMLElement* element, std::string groupName, std::string path, GameObject* parent)
{
	std::string nodeVal = element->Value();
	LOG_MANAGER->log_message("Read in XML Element: " + nodeVal);
	if (nodeVal == "node")
	{
		GameObject* newObject = create_game_object(groupName, element->Attribute("name"), parent);
		tinyxml2::XMLElement* firstChild = element->FirstChildElement();
		if (firstChild != NULL)
			parse_xml_gameobject(firstChild, groupName, path, newObject);
	}
	else if (nodeVal == "externals")
	{
		std::string externType = element->Attribute("type");

		tinyxml2::XMLElement* firstElement = element->FirstChildElement();
		if (firstElement != NULL)
			parse_xml_external(firstElement, externType, path);
	}
	else if (nodeVal == "environment")
		parse_xml_environment(element);
	else if (parent != NULL)
	{
		tinyxml2::XMLElement* firstElement = element->FirstChildElement();
		if (firstElement != NULL)
			parse_xml_nodes(firstElement, groupName, path, parent);
	}
	tinyxml2::XMLElement* nextSibling = element->NextSiblingElement();
	if (nextSibling != NULL)
		parse_xml_nodes(nextSibling, groupName, path, parent);
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

void GameObjectManager::parse_xml_external(tinyxml2::XMLElement* element, std::string resourceType, std::string path)
{
	mExternals[resourceType].push_back({ element->Attribute("name"), path });
	if (resourceType == "material")
		set_skybox(element->Attribute("name"));//create_ground_plane(element->Attribute("name"));

	tinyxml2::XMLElement* nextSibling = element->NextSiblingElement();
	if (nextSibling != NULL)
		parse_xml_external(nextSibling, resourceType, path);
}

void GameObjectManager::parse_xml_environment(tinyxml2::XMLElement* element)
{
	std::string nodeVal = element->Value();
	if (nodeVal == "colourAmbient")
		APPLICATION->get_scene_manager()->setAmbientLight(parse_xml_color_data(element));
	else if (nodeVal == "colourBackground")
		APPLICATION->getRenderWindow()->getViewport(0)->setBackgroundColour(parse_xml_color_data(element));

	tinyxml2::XMLElement* nextSibling = element->NextSiblingElement();
	if (nextSibling != NULL)
		parse_xml_environment(nextSibling);
}

void GameObjectManager::parse_xml_gameobject(tinyxml2::XMLElement* element, std::string groupName, std::string path, GameObject* parent)
{
	std::string nodeVal = element->Value();
	LOG_MANAGER->log_message("Read in objectProperty: " + nodeVal);

	// Object Type
	if (nodeVal == "node")
		parse_xml_nodes(element, groupName, path, parent);
	else if (nodeVal == "entity")
		parse_xml_mesh(element, groupName, path, parent);
	else if (nodeVal == "camera")
		parse_xml_camera(element, groupName, path, parent);
	else if (nodeVal == "light")
		parse_xml_light(element, groupName, path, parent);
	else if (nodeVal == "userData")
	{
		tinyxml2::XMLElement* propertyElement = element->FirstChildElement();
		if (propertyElement != NULL)
			parse_xml_gameobject(propertyElement, groupName, path, parent);
	}
	else if (nodeVal == "property")
	{
		std::string nodeName = element->Attribute("name");
		if (nodeName == "tag")
			set_game_object_tag(element->IntAttribute("data"), parent);
		//else if (nodeName == "script")
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
		parse_xml_gameobject(nextSibling, groupName, path, parent);
}

void GameObjectManager::parse_xml_mesh(tinyxml2::XMLElement* element, std::string groupName, std::string path, GameObject* parent)
{
	std::string name = element->Attribute("name");
	MeshComponent* mesh = parent->create_mesh(name.empty() ? parent->get_name() : name, std::string(element->Attribute("meshFile")));

	// Handle check for userData
	tinyxml2::XMLElement* firstElement = element->FirstChildElement();
	if (firstElement != NULL)
		parse_xml_mesh_data(firstElement, groupName, path, mesh);
}

void GameObjectManager::parse_xml_mesh_data(tinyxml2::XMLElement* element, std::string groupName, std::string path, MeshComponent* parent)
{
	std::string nodeVal = element->Value();
	LOG_MANAGER->log_message("Read in meshProperty: " + nodeVal);

	// Mesh Object Data
	if (nodeVal == "userData")
	{
		tinyxml2::XMLElement* firstElement = element->FirstChildElement();
		if (firstElement != NULL)
			parse_xml_properties(firstElement, groupName, path, parent);
	}

	tinyxml2::XMLElement* nextSibling = element->NextSiblingElement();
	if (nextSibling != NULL)
		parse_xml_mesh_data(nextSibling, groupName, path, parent);
}

void GameObjectManager::parse_xml_camera(tinyxml2::XMLElement* element, std::string groupName, std::string path, GameObject* parent)
{
	std::string name = element->Attribute("name");
	CameraComponent* camera = parent->create_camera(name.empty() ? parent->get_name() : name);
	camera->set_fov(element->FloatAttribute("fov"));
	//if (element->Attribute("projectionType") != "perspective")
	//	camera->set_projection_type(Ogre::ProjectionType::PT_ORTHOGRAPHIC);
	camera->set_main_camera(APPLICATION->getRenderWindow()->getViewport(0));

	tinyxml2::XMLElement* firstElement = element->FirstChildElement();
	if (firstElement != NULL)
		parse_xml_camera_data(firstElement, groupName, path, camera);
}

void GameObjectManager::parse_xml_camera_data(tinyxml2::XMLElement* element, std::string groupName, std::string path, CameraComponent* parent)
{
	std::string nodeVal = element->Value();
	LOG_MANAGER->log_message("Read in cameraProperty: " + nodeVal);

	// Camera Object Data
	if (nodeVal == "clipping")
		parent->set_clip_distances(element->FloatAttribute("near"), element->FloatAttribute("far"));

	tinyxml2::XMLElement* nextSibling = element->NextSiblingElement();
	if (nextSibling != NULL)
		parse_xml_camera_data(nextSibling, groupName, path, parent);
}

void GameObjectManager::parse_xml_light(tinyxml2::XMLElement* element, std::string groupName, std::string path, GameObject* parent)
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

	tinyxml2::XMLElement* firstElement = element->FirstChildElement();
	if (firstElement != NULL)
		parse_xml_light_data(firstElement, groupName, path, light);
}

void GameObjectManager::parse_xml_light_data(tinyxml2::XMLElement* element, std::string groupName, std::string path, LightComponent* parent)
{
	std::string nodeVal = element->Value();
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
		parse_xml_light_data(nextSibling, groupName, path, parent);
}

void GameObjectManager::parse_xml_properties(tinyxml2::XMLElement* element, std::string groupName, std::string path, Component* parent)
{
	std::string nodeVal = element->Attribute("name");
	std::string type = element->Attribute("type");
	if (type == "str")
		parent->add_xml_string_property(nodeVal, std::string(element->Attribute("data")));
	else if (type == "bool")
		parent->add_xml_bool_property(nodeVal, element->BoolAttribute("data"));
	else if (type == "int")
		parent->add_xml_int_property(nodeVal, element->IntAttribute("data"));
	else if (type == "float")
		parent->add_xml_float_property(nodeVal, element->FloatAttribute("data"));
	else if (type == "double")
		parent->add_xml_double_property(nodeVal, element->DoubleAttribute("data"));
	else
		throw new std::exception(("TYPE ERROR!!! Did not provide valid data type given to attrieve!!\n\tGot Type: " + type).c_str());

	tinyxml2::XMLElement* nextSibling = element->NextSiblingElement();
	if (nextSibling != NULL)
		parse_xml_properties(nextSibling, groupName, path, parent);
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
	mGroupsIter = mObjects.begin();
	mGroupsRevIter = mObjects.rbegin();
	while (mGroupsIter != mObjects.end() && mGroupsRevIter != mObjects.rend() && *mGroupsIter != *mGroupsRevIter)
	{
		mObjIter = mGroupsIter->second.find(game_object_name);
		if (mObjIter != mGroupsIter->second.end())
		{
			reqObject = mObjIter->second;
			break;
		}
		mObjIter = mGroupsRevIter->second.find(game_object_name);
		if (mObjIter != mGroupsIter->second.end())
		{
			reqObject = mObjIter->second;
			break;
		}
		mGroupsIter++;
		mGroupsRevIter++;
	}

	return reqObject;
}

GameObject* GameObjectManager::get_game_object(std::string game_object_name, std::string group_name)
{
	GameObject* reqObject = nullptr;
	
	mGroupsIter = mObjects.find(group_name);
	if (mGroupsIter != mObjects.end())
	{
		mObjIter = mGroupsIter->second.find(game_object_name);
		if (mObjIter != mGroupsIter->second.end())
			reqObject = mObjIter->second;
		else
			LOG_MANAGER->log("User trying to access nonexistent gameObject: " + game_object_name + " in group: " + group_name);
	}
	else
		LOG_MANAGER->log("User trying to access nonexistent group: "+group_name+" to find gameObject: "+game_object_name);

	return reqObject;
}

void GameObjectManager::get_game_objects(std::string group_name, std::vector<GameObject*>& result)
{
	mGroupsIter = mObjects.find(group_name);
	if (mGroupsIter != mObjects.end())
	{
		mObjIter = mGroupsIter->second.begin();
		while (mObjIter != mGroupsIter->second.end())
		{
			result.push_back(mObjIter->second);
			mObjIter++;
		}
	}
	else
		LOG_MANAGER->log("User trying to get gameObjects from nonexistent group: " + group_name);
}

bool GameObjectManager::has_group(std::string group_name)
{
	mGroupsIter = mObjects.find(group_name);
	if (mGroupsIter != mObjects.end())
		return true;
	return false;
}

bool GameObjectManager::destroy_game_object(std::string group_name, std::string gobj_name, bool ignoreLog)
{
	mGroupsIter = mObjects.find(group_name);
	if (mGroupsIter != mObjects.end())
	{
		mObjIter = mGroupsIter->second.find(gobj_name);
		if (mObjIter != mGroupsIter->second.end())
		{
			delete(mObjIter->second);
			mGroupsIter->second.erase(mObjIter);
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
	mGroupsIter = mObjects.begin();
	mGroupsRevIter = mObjects.rbegin();
	while (mGroupsIter != mObjects.end() && mGroupsRevIter != mObjects.rend())
	{
		if (*mGroupsIter == *mGroupsRevIter)
		{
			this->destroy_game_object(mGroupsIter->first, gobj_name, false);
			break;
		}

		destroy_game_object(mGroupsIter->first, gobj_name, false);
		destroy_game_object(mGroupsRevIter->first, gobj_name, false);

		mGroupsIter++;
		mGroupsRevIter++;
	}
	return deletedObj;
}

void GameObjectManager::group_destroy(std::string group_name, bool destroy_group)
{
	mGroupsIter = mObjects.find(group_name);
	if (mGroupsIter != mObjects.end())
	{
		mObjIter = mGroupsIter->second.begin();
		while(mObjIter != mGroupsIter->second.end())
		{
			delete(mObjIter->second);
			mGroupsIter->second.erase(mObjIter);
			mObjIter++;
		}
		if(destroy_group)
			mObjects.erase(mGroupsIter->first);
	}
	else
		LOG_MANAGER->log("User trying to clear or delete nonexistent group: " + group_name);
}

void GameObjectManager::group_destroy(std::map<std::string, std::map<std::string, GameObject*>>::iterator& group, bool destroy_group)
{
	mObjIter = group->second.begin();
	while (mObjIter != group->second.end())
	{
		delete(mObjIter->second);
		mObjIter++;
	}
	group->second.clear();
	if (destroy_group)
		mObjects.erase(mGroupsIter->first);
}

void GameObjectManager::destroy_all()
{
	mGroupsIter = mObjects.begin();
	while (mObjects.size() > 0)
	{
		group_destroy(mGroupsIter, true);
	}
}

void GameObjectManager::set_visibility(std::string group_name, bool is_visible)
{
	mGroupsIter = mObjects.find(group_name);
	if (mGroupsIter != mObjects.end())
	{
		mObjIter = mGroupsIter->second.begin();
		while (mObjIter != mGroupsIter->second.end())
		{
			mObjIter->second->set_visibility(is_visible);
			mObjIter++;
		}
	}
	else
		LOG_MANAGER->log("User trying to set object visibility for a nonexistent group: "+group_name);
}

GameObject* GameObjectManager::create_game_object(std::string group_name, std::string object_name, GameObject* parent, unsigned int tag, Ogre::Vector3 pos, Ogre::Quaternion rot)
{
	GameObject* newObj = new GameObject(object_name, tag, parent, pos, rot);
	
	mGroupsIter = mObjects.find(group_name);
	if (mGroupsIter != mObjects.end())
	{
		// Check if Object is already in the group
		mObjIter = mGroupsIter->second.find(object_name);
		if (mObjIter == mGroupsIter->second.end()) // GameObject not in group, create it
		{
			mGroupsIter->second[object_name] = newObj; //.emplace(object_name, newObj);
			mTaggedObjects[tag].push_back(newObj);
		}
			
		else // GameObject is already apart of the dictionary Log Error and return nullptr as GameObject
		{
			LOG_MANAGER->log_message("GameObject_CreationError: Object: " + object_name + " already exists in group: " + group_name, Ogre::ColourValue(1, 0, 0));
			delete(newObj);
			return nullptr;
		}
	}
	else // Object Group does not exist, create group and place new object in group
		mObjects[group_name][object_name] = newObj;
		
	return newObj;
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
	Ogre::Viewport* mVp = APPLICATION->getRenderWindow()->addViewport(nullptr);
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