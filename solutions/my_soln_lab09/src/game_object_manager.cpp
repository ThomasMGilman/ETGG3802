#include <stdafx.h>
#include <game_object_manager.h>
#include <game_object.h>
#include <log_manager.h>
#include <application.h>
#include <log_manager.h>
#include <component_mesh.h>
#include <script_functions.h>
#include <utility.h>
#include <script_game_object.h>

// The icky template-specialization / static-initializer for GOM's msSingleton attribute
template<> ssuge::GameObjectManager* ssuge::Singleton<ssuge::GameObjectManager>::msSingleton = NULL;




ssuge::GameObjectManager::GameObjectManager() : mNumGameObjects(0)
{
	// Nothing needed here -- yet.
}

ssuge::GameObjectManager::~GameObjectManager()
{
	// Destroy all game objects
	std::map<std::string, std::map<std::string, GameObject*>>::iterator it = mGameObjects.begin();
	while (it != mGameObjects.end())
	{
		group_destroy(it->first, false);
		++it;
	}
	// We should just have empty groups objects now.  Delete those.
	mGameObjects.clear();
	mNumGameObjects = 0;
}

ssuge::GameObject * ssuge::GameObjectManager::get_game_object(std::string game_object_name)
{
	std::map<std::string, std::map<std::string, GameObject*>>::iterator it = mGameObjects.begin();
	while (it != mGameObjects.end())
	{
		std::map<std::string, GameObject*>::iterator gobj_iter = it->second.find(game_object_name);
		if (gobj_iter != it->second.end())
		{
			return gobj_iter->second;
		}
		++it;
	}
	return NULL;
}


ssuge::GameObject * ssuge::GameObjectManager::get_game_object(std::string game_object_name, std::string group_name)
{
	std::map<std::string, std::map<std::string, GameObject*>>::iterator it = mGameObjects.find(group_name);
	if (it != mGameObjects.end())
	{
		std::map<std::string, GameObject*>::iterator gobj_iter = it->second.find(game_object_name);
		if (gobj_iter != it->second.end())
		{
			return gobj_iter->second;
		}
	}
	return NULL;
}

void ssuge::GameObjectManager::get_game_objects(std::string group_name, std::vector<ssuge::GameObject*>& result)
{
	result.clear();
	std::map<std::string, std::map<std::string, GameObject*>>::iterator it = mGameObjects.find(group_name);
	if (it != mGameObjects.end())
	{
		std::map<std::string, GameObject*>::iterator gobj_iter = it->second.begin();
		while (gobj_iter != it->second.end())
		{
			result.push_back(gobj_iter->second);
			++gobj_iter;
		}
	}
}

bool ssuge::GameObjectManager::has_group(std::string group_name)
{
	std::map<std::string, std::map<std::string, GameObject*>>::iterator it = mGameObjects.find(group_name);
	return it != mGameObjects.end();
}


bool ssuge::GameObjectManager::has_game_object(GameObject* gobj)
{
	std::set<GameObject*>::iterator it = mAllGameObjects.find(gobj);
	return it != mAllGameObjects.end();
}





bool ssuge::GameObjectManager::destroy_game_object(std::string group_name, std::string gobj_name)
{
	std::map<std::string, std::map<std::string, GameObject*>>::iterator it = mGameObjects.find(group_name);
	if (it != mGameObjects.end())
	{
		std::map<std::string, GameObject*>::iterator gobj_iter = it->second.find(gobj_name);
		if (gobj_iter != it->second.end())
		{
			// Delete it from our big set
			std::set<GameObject*>::iterator set_iter = mAllGameObjects.find(gobj_iter->second);
			if (set_iter != mAllGameObjects.end())
				mAllGameObjects.erase(set_iter);
			// And delete it
			delete gobj_iter->second;
			// And remove it from the map of maps
			it->second.erase(gobj_iter);
			mNumGameObjects--;
			return true;
		}
	}
	return false;
}



bool ssuge::GameObjectManager::destroy_game_object(std::string gobj_name)
{
	std::map<std::string, std::map<std::string, GameObject*>>::iterator it = mGameObjects.begin();
	while (it != mGameObjects.end())
	{
		std::map<std::string, GameObject*>::iterator gobj_iter = it->second.find(gobj_name);
		if (gobj_iter != it->second.end())
		{
			// Delete it from the big set
			std::set<GameObject*>::iterator set_iter = mAllGameObjects.find(gobj_iter->second);
			if (set_iter != mAllGameObjects.end())
				mAllGameObjects.erase(set_iter);
			// And delete it
			delete gobj_iter->second;
			// And remove it from the map-of-maps
			it->second.erase(gobj_iter);
			mNumGameObjects--;
			return true;
		}

		++it;
	}
	return false;
}


void ssuge::GameObjectManager::group_destroy(std::string group_name, bool destroy_group)
{
	std::map<std::string, std::map<std::string, GameObject*>>::iterator it = mGameObjects.find(group_name);
	if (it != mGameObjects.end())
	{
		std::map<std::string, GameObject*>::iterator gobj_iter = it->second.begin();
		while (gobj_iter != it->second.end())
		{
			// Remove the current one from the big set
			std::set<GameObject*>::iterator set_iter = mAllGameObjects.find(gobj_iter->second);
			if (set_iter != mAllGameObjects.end())
				mAllGameObjects.erase(set_iter);
			// Delete the object itself
			delete gobj_iter->second;
			++gobj_iter;
		}
		mNumGameObjects -= (int)it->second.size();
		it->second.clear();

		if (destroy_group)
			mGameObjects.erase(it);
	}
}

	
void ssuge::GameObjectManager::set_visibility(std::string group_name, bool is_visible)
{
	std::map<std::string, std::map<std::string, GameObject*>>::iterator it = mGameObjects.find(group_name);
	if (it != mGameObjects.end())
	{
		std::map<std::string, GameObject*>::iterator gobj_iter = it->second.begin();
		while (gobj_iter != it->second.end())
		{
			gobj_iter->second->set_visibility(is_visible);
			++gobj_iter;
		}
	}
}


ssuge::GameObject * ssuge::GameObjectManager::create_game_object(std::string group_name, std::string object_name, GameObject * parent, unsigned int tag, std::string script_name, bool call_create)
{
	// Make sure we have that group.  If not, create it
	std::map<std::string, std::map<std::string, GameObject*>>::iterator it = mGameObjects.find(group_name);
	if (it == mGameObjects.end())
	{
		// Create the group.  Just looking at it creates it.
		mGameObjects[group_name];
	}
	else
	{
		// We already have that group -- make sure we don't already have that game object
		std::map<std::string, GameObject*>::iterator inner_it = it->second.find(object_name);
		if (inner_it != it->second.end())
		{
			// We do have that name already.  Destroy the old one
			// ... remove it from our big set
			std::set<GameObject*>::iterator set_iter = mAllGameObjects.find(inner_it->second);
			if (set_iter != mAllGameObjects.end())
				mAllGameObjects.erase(set_iter);
			// ... and from this group
			LOG_MANAGER->log("Overwriting game object '" + object_name + "' in group '" + group_name + "'", LogPreset::WARNING);
			delete inner_it->second;
			it->second.erase(inner_it);
			mNumGameObjects--;

			
		}
	}

	// Make the C++ game object and add it to the map
	GameObject * gobj = new GameObject(object_name, tag, parent);
	mGameObjects[group_name][object_name] = gobj;
	mAllGameObjects.insert(gobj);
	mNumGameObjects++;

	// See if we can find the class the requested this python game object be an instance of.  If not, the
	// Script manager get_class method returns the "vanilla" game object class (which should never fail)
	PyObject* python_class = SCRIPT_MANAGER->get_class(script_name);

	// Make the python game object and associate it them with each other
	//ssuge::script::GameObject* python_gobj = (ssuge::script::GameObject*)PyObject_New(ssuge::script::GameObject, &GameObjectType);
	PyObject* empty_tup = PyTuple_New(0);
	ssuge::script::GameObject* python_gobj = (ssuge::script::GameObject*)PyObject_Call(python_class, empty_tup, NULL);
	Py_DECREF(empty_tup);
	gobj->set_script_twin((PyObject*)python_gobj);
	python_gobj->mTwin = gobj;

	// Some places (like script's create_game_object) will call this (possibly passing values).  But for
	// game objects created in places like the GOM, just call create, passing no arguments
	if (call_create)
	{
		PyObject* args = PyTuple_New(0);
		gobj->execute_script_method("create", args);
		Py_DECREF(args);
	}

	// The incref'ing here is a bit tricky.  If this python game object were returned to an idle main
	// program, I don't think we'd need to incref it.  But for us, game objects will be created by the GOM
	// and script calls to create_game_object.  
	return gobj;
}


void ssuge::GameObjectManager::update(float dt)
{
	std::map<std::string, std::map<std::string, GameObject*>>::iterator outer_it;
	outer_it = mGameObjects.begin();
	while (outer_it != mGameObjects.end())
	{
		std::map<std::string, GameObject*>::iterator inner_it;
		inner_it = outer_it->second.begin();
		while (inner_it != outer_it->second.end())
		{
			inner_it->second->update(dt);
			++inner_it;
		}
		++outer_it;
	}

	// Attempt to remove any game objects that were queued to be destroyed
	for (int i = 0; i < mUngroupedObjectsToDestroy.size(); i++)
		destroy_game_object(mUngroupedObjectsToDestroy[i]);
	mUngroupedObjectsToDestroy.clear();
	for (int i = 0; i < mObjectsToDestroy.size(); i++)
		destroy_game_object(mObjectsToDestroy[i].first, mObjectsToDestroy[i].second);
	mObjectsToDestroy.clear();
}


void ssuge::GameObjectManager::load_scene_file(std::string group_name, std::string fname)
{
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError err;
	err = doc.LoadFile(fname.c_str());
	if (err != tinyxml2::XMLError::XML_ERROR_FILE_READ_ERROR)
	{
		tinyxml2::XMLElement* elem = doc.RootElement();
		if (elem != NULL)
			process_XML_node(group_name, elem, NULL);
	}
	else
		LOG_MANAGER->log("Error opening scene file '" + fname + "'", Ogre::ColourValue::Red, 5.0f);
}

Ogre::Vector3 ssuge::GameObjectManager::process_XML_vector_node(tinyxml2::XMLElement* e)
{
	// <position x="0.211461" y="2.753002" z="1.305722" />
	Ogre::Vector3 v;
	if (e->Attribute("x"))
		v.x = e->FloatAttribute("x");
	if (e->Attribute("y"))
		v.y = e->FloatAttribute("y");
	if (e->Attribute("z"))
		v.z = e->FloatAttribute("z");
	return v;
}

Ogre::Quaternion ssuge::GameObjectManager::process_XML_quaternion_node(tinyxml2::XMLElement* e)
{
	// <rotation qw = "1.000000" qx = "0.000000" qy = "0.000000" qz = "0.000000" / >
	float qw = 0.0f, qx = 0.0f, qy = 0.0f, qz = 0.0f;
	if (e->Attribute("qw"))
		qw = e->FloatAttribute("qw");
	if (e->Attribute("qx"))
		qx = e->FloatAttribute("qx");
	if (e->Attribute("qy"))
		qy = e->FloatAttribute("qy");
	if (e->Attribute("qz"))
		qz = e->FloatAttribute("qz");
	return Ogre::Quaternion(qw, qx, qy, qz);
}

Ogre::ColourValue ssuge::GameObjectManager::process_XML_color_node(tinyxml2::XMLElement* e)
{
	float r = 0.0f, g = 0.0f, b = 0.0f;
	if (e->Attribute("r"))
		r = e->FloatAttribute("r");
	if (e->Attribute("g"))
		g = e->FloatAttribute("g");
	if (e->Attribute("b"))
		b = e->FloatAttribute("b");
	return Ogre::ColourValue(r, g, b);
}

ssuge::GameObject * ssuge::GameObjectManager::process_XML_game_object(std::string group_name, tinyxml2::XMLElement* e, GameObject* cur_parent)
{
	// Collect attributes used to create the game object
	tinyxml2::XMLElement* temp;
	std::string name;
	std::string script_name = "";
	std::string collider_type = "";
	int collider_layer = -1, collider_mask = -1;
	int tag = 0;
	
	if (e->Attribute("name"))
		name = std::string(e->Attribute("name"));

	if ((temp = e->FirstChildElement("userData")) != NULL)
	{
		tinyxml2::XMLElement* temp2 = temp->FirstChildElement("property");
		while (temp2 != NULL)
		{
			if (temp2->Attribute("name"))
			{
				if (!strcmp(temp2->Attribute("name"), "tag"))
					tag = temp2->IntAttribute("data");
				if (!strcmp(temp2->Attribute("name"), "script"))
					script_name = std::string(temp2->Attribute("data"));
				if (!strcmp(temp2->Attribute("name"), "collider"))
					collider_type = std::string(temp2->Attribute("data"));
				if (!strcmp(temp2->Attribute("name"), "collider_layer"))
					collider_layer = temp2->IntAttribute("data");
				if (!strcmp(temp2->Attribute("name"), "collider_mask"))
					collider_mask = temp2->IntAttribute("data");

			}
			temp2 = temp2->NextSiblingElement("property");
		}
	}

	GameObject* gobj = create_game_object(group_name, name, cur_parent, tag, script_name, script_name != "");

	if (collider_type != "")
	{
		std::vector<std::string> elements;
		split_str(collider_type, elements, ' ');
		if (elements.size() == 4 && elements[0] == "box")
		{
			bool error = false;
			float vals[3];
			for (int i = 0; i < 3; i++)
			{
				try
				{
					int higher_i = i + 1;
					vals[i] = std::stof(elements[higher_i]);
				}
				catch (std::exception)
				{
					error = true;
				}
			}
			if (!error)
				gobj->create_box_collider_component(collider_layer, collider_mask, Ogre::Vector3(vals[0], vals[1], vals[2]));
		}
		else if (elements.size() == 2 && elements[0] == "sphere")
		{
			bool error = false;
			float rad;
			try
			{
				rad = std::stof(elements[1]);
			}
			catch (std::exception)
			{
				error = true;
			}
			
			if (!error)
				gobj->create_sphere_collider_component(collider_layer, collider_mask, rad);
		}
	}

	// Look for properties that adjust the new game object
	if ((temp = e->FirstChildElement("position")) != NULL)
	{
		//gobj->set_position(process_XML_vector_node(temp));
		Ogre::Vector3 p = process_XML_vector_node(temp);
		gobj->set_position(p);
	}
	if ((temp = e->FirstChildElement("rotation")) != NULL)
	{
		//gobj->set_orientation(process_XML_quaternion_node(temp));
		Ogre::Quaternion q = process_XML_quaternion_node(temp);
		Ogre::Vector3 qangles(q.getPitch().valueDegrees(), q.getYaw().valueDegrees(), q.getRoll().valueDegrees());		// For testing only
		gobj->set_orientation(q);
	}
	if ((temp = e->FirstChildElement("scale")) != NULL)
	{
		Ogre::Vector3 s = process_XML_vector_node(temp);
		gobj->set_scale(s);
	}
	if ((temp = e->FirstChildElement("entity")) != NULL)
	{
		// <entity meshFile="ShieldA.mesh" name="ShieldA.002" >
		if (temp->Attribute("meshFile") != NULL)
		{
			ssuge::ComponentMesh* comp = gobj->create_mesh_component(temp->Attribute("meshFile"));
			comp->set_cast_shadows(true);
		}
	}
	if ((temp = e->FirstChildElement("light")) != NULL)
		process_XML_light_node(temp, gobj);
	if ((temp = e->FirstChildElement("camera")) != NULL)
		process_XML_camera_node(temp, gobj);

	return gobj;
}


void ssuge::GameObjectManager::process_XML_light_node(tinyxml2::XMLElement* elem, GameObject* gobj)
{
	// <light name="Lamp" powerScale="1.0" type="point" >
		//    <colourDiffuse b = "1.0" g = "1.0" r = "1.0" />
		//    <colourSpecular b = "1.0" g = "1.0" r = "1.0" />
		//    <lightAttenuation constant = "1.0" linear = "0.03333335240683057" quadratic = "0.0" range = "5000" / >
		// </light>
	ComponentLight* light_ptr = NULL;
	const char* light_type_name = elem->Attribute("type");
	if (light_type_name != NULL)
	{
		if (!strcmp(light_type_name, "point"))
			light_ptr = gobj->create_light_component(LightType::POINT);
		if (!strcmp(light_type_name, "directional"))
		{
			light_ptr = gobj->create_light_component(LightType::DIRECTIONAL);
			// Blender lights seem to have their "forward" direction in Ogre's (0, 0, -1) direction.
			light_ptr->set_direction(Ogre::Vector3(0, 0, -1));
		}
		// TO-DO: Process other light types...(just didn't have an example of them...yet:-))
	}
	if (light_ptr != NULL)
	{
		tinyxml2::XMLElement* temp2 = elem->FirstChildElement("colourDiffuse");
		if (temp2 != NULL)
			light_ptr->set_diffuse_color(process_XML_color_node(temp2));
		temp2 = elem->FirstChildElement("colourSpecular");
		if (temp2 != NULL)
		{
			light_ptr->set_specular_color(process_XML_color_node(temp2));
		}
		temp2 = elem->FirstChildElement("lightAttenuation");
		if (temp2 != NULL)
		{
			float constant = 1.0f, linear = 0.0f, quadratic = 0.0f, range = 1000.0f;
			if (temp2->Attribute("constant"))
				constant = temp2->FloatAttribute("constant");
			if (temp2->Attribute("linear"))
				linear = temp2->FloatAttribute("linear");
			if (temp2->Attribute("quadratic"))
				quadratic = temp2->FloatAttribute("quadratic");
			if (temp2->Attribute("range"))
				range = temp2->FloatAttribute("range");
			light_ptr->set_attenuation_params(range, constant, linear, quadratic);
		}
	}
}

void ssuge::GameObjectManager::process_XML_camera_node(tinyxml2::XMLElement* elem, GameObject* gobj)
{
	// <camera fov="0.5033799435163735" name="Camera" projectionType="perspective" >
	//   <clipping far = "100.0" near = "0.10000000149011612" / >
	//	< / camera>
	ComponentCamera* cam_comp = gobj->create_camera_component();
	float near = 1.0f, far = 1000.0f;
	if (elem->Attribute("fov"))
	{
		float fov = 57.3f * elem->FloatAttribute("fov");
		cam_comp->set_field_of_view(fov);
	}
	tinyxml2::XMLElement* temp = elem->FirstChildElement("clipping");
	if (temp != NULL)
	{
		if (temp->Attribute("far"))
			far = temp->FloatAttribute("far");
		if (temp->Attribute("near"))
			near = temp->FloatAttribute("near");
		cam_comp->set_clip_distances(near, far);
	}

	// I might need to revisit this part if we have multiple scene files being loaded.  We probably
	// need a way to indicate (maybe in script?) if a given camera should be made the "active" camera or not
	if (cam_comp)
		APPLICATION->set_active_camera(cam_comp);
}



void ssuge::GameObjectManager::process_XML_collider_data(tinyxml2::XMLElement* elem, GameObject* gobj)
{

}



void ssuge::GameObjectManager::process_XML_node(std::string group_name, tinyxml2::XMLElement* elem, GameObject* cur_parent)
{
	std::string msg(elem->Name());

	// If this is a game-object creating node, process it (this will look at / process that
	// node's immediate children -- we'll still visit those in the following loop, but we
	// won't do anything with them)
	if (!strcmp(elem->Name(), "node"))
		cur_parent = process_XML_game_object(group_name, elem, cur_parent);

	// Process any child node elements -- this loop (and the associated recursion) will
	// visit every xml node in the xml tree.
	tinyxml2::XMLElement* childElem = elem->FirstChildElement();
	while (childElem != NULL)
	{
		process_XML_node(group_name, childElem, cur_parent);
		childElem = childElem->NextSiblingElement();
	}
}

