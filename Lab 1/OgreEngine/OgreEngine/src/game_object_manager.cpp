#include <game_object_manager.h>
#include <log_manager.h>

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

void GameObjectManager::load_scenes(std::list<std::string> fileNames)
{
	std::list<std::string>::iterator fileIter = fileNames.begin();
	while (fileIter != fileNames.end())
	{
		load_scene(*fileIter);
		fileIter++;
	}
}

void GameObjectManager::load_scene(std::string fileName)
{
	mDoc->LoadFile((mMediaPath + fileName).c_str());
	if (mDoc->Error())
	{
		std::string error = mDoc->ErrorStr();
		LOG_MANAGER->log("tinyxml2 Error: " + error);
	}
	tinyxml2::XMLNode* firstNode = mDoc->FirstChild();
	if (firstNode != NULL)
		parse_xml_nodes(firstNode);
}

void GameObjectManager::parse_xml_nodes(tinyxml2::XMLNode* node)
{
	std::string nodeVal = node->Value();
	LOG_MANAGER->log_message("Readin XML Node: " + nodeVal);
	tinyxml2::XMLNode* firstChild = node->FirstChild();
	if (firstChild != NULL)
		parse_xml_nodes(firstChild);

	tinyxml2::XMLNode* nextSibling = node->NextSibling();
	if (nextSibling != NULL)
		parse_xml_nodes(nextSibling);
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
			mGroupsIter->second[object_name] = newObj; //.emplace(object_name, newObj);
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

