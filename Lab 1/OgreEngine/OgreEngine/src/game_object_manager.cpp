#include <game_object_manager.h>

using namespace OgreEngine;

GameObjectManager* GameObjectManager::msSingleton = nullptr;

GameObject* OgreEngine::GameObjectManager::get_game_object(std::string game_object_name)
{
	GameObject* reqObject = nullptr;
	mGroupsIter = this->mObjects.begin();
	mGroupsRevIter = this->mObjects.rbegin();
	while (mGroupsIter != this->mObjects.end() && mGroupsRevIter != this->mObjects.rend() && *mGroupsIter != *mGroupsRevIter)
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

GameObject* OgreEngine::GameObjectManager::get_game_object(std::string game_object_name, std::string group_name)
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

	return nullptr;
}

void OgreEngine::GameObjectManager::get_game_objects(std::string group_name, std::vector<GameObject*>& result)
{
	mGroupsIter = this->mObjects.find(group_name);
	if (mGroupsIter != this->mObjects.end())
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

bool OgreEngine::GameObjectManager::has_group(std::string group_name)
{
	mGroupsIter = this->mObjects.find(group_name);
	if (mGroupsIter != this->mObjects.end())
		return true;
	return false;
}

bool OgreEngine::GameObjectManager::destroy_game_object(std::string group_name, std::string gobj_name, bool ignoreLog = true)
{
	mGroupsIter = this->mObjects.find(group_name);
	if (mGroupsIter != this->mObjects.end())
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

bool OgreEngine::GameObjectManager::destroy_game_object(std::string gobj_name)
{
	bool deletedObj = false;
	mGroupsIter = this->mObjects.begin();
	mGroupsRevIter = this->mObjects.rbegin();
	while (mGroupsIter != this->mObjects.end() && mGroupsRevIter != this->mObjects.rend())
	{
		if (*mGroupsIter == *mGroupsRevIter)
		{
			this->destroy_game_object(mGroupsIter->first, gobj_name, false);
			break;
		}

		this->destroy_game_object(mGroupsIter->first, gobj_name, false);
		this->destroy_game_object(mGroupsRevIter->first, gobj_name, false);

		mGroupsIter++;
		mGroupsRevIter++;
	}
	return deletedObj;
}

void OgreEngine::GameObjectManager::group_destroy(std::string group_name, bool destroy_group)
{
	mGroupsIter = this->mObjects.find(group_name);
	if (mGroupsIter != this->mObjects.end())
	{
		mObjIter = mGroupsIter->second.begin();
		while(mObjIter != mGroupsIter->second.end())
		{
			delete(mObjIter->second);
			mGroupsIter->second.erase(mObjIter);
			mObjIter++;
		}
	}
	else
		LOG_MANAGER->log("User trying to clear or delete nonexistent group: " + group_name);
}

void OgreEngine::GameObjectManager::set_visibility(std::string group_name, bool is_visible)
{
	mGroupsIter = this->mObjects.find(group_name);
	if (mGroupsIter != this->mObjects.end())
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

GameObject* OgreEngine::GameObjectManager::create_game_object(std::string group_name, std::string object_name, GameObject* parent, unsigned int tag)
{
	GameObject* newObj = new GameObject(object_name, tag, parent);
	mGroupsIter = this->mObjects.find(group_name);
	if (mGroupsIter != this->mObjects.end())
	{

	}
	else
	{

	}
	return newObj;
}
