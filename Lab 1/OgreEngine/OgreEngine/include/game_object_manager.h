#ifndef _GAME_OBJECT_MANAGER_H_
#define _GAME_OBJECT_MANAGER_H_

#include <game_object.h>


namespace OgreEngine
{
	/// The GameObjectManager (or GOM for short) is a collection of GameObjects.  This class
	/// is a "Factory" for GameObjects (which means it is the only way to create / destroy
	/// GameObjects, plus there are ways to access a GameObject via the GOM).  The organization
	/// is pretty simple for now -- we have any number of uniquely-named groups.  Within that group
	/// is a collection of uniquely named GameObjects (a map-of-maps) -- in theory a GameObject name
	/// only has to be unique within the group, but when finding a GameObject solely by name, it is 
	/// non-deterministic which one will be returned.
	class GameObjectManager
	{
		// ***** ATTRIBUTES *****
	protected:


		// ***** CONSTRUCTOR / DESTRUCTOR *****
	public:
		/// Constructor
		GameObjectManager();

		/// Destructor
		~GameObjectManager();

		// ***** GETTERS / SETTERS *****
	public:
		/// Gets a game object (NULL if it doesn't exist) -- more costly: searches through all groups
		GameObject* get_game_object(std::string game_object_name);

		/// Gets a game object within a group (NULL if it doesn't exist)
		GameObject* get_game_object(std::string game_object_name, std::string group_name);

		/// Gets all game objects within a group.  result is an output parameter (and is cleared upon calling this)
		void get_game_objects(std::string group_name, std::vector<GameObject*>& result);

		/// Returns true if there is a group by this name
		bool has_group(std::string group_name);

		// ***** METHODS ***** 
	public:
		/// Destroys a game object (faster version) if you know the group name
		bool destroy_game_object(std::string group_name, std::string gobj_name);

		/// Destroy a game object (slower version) if you don't know the group name.  Returns true if that game object was destroyed
		/// (i.e. was it found)
		bool destroy_game_object(std::string gobj_name);

		/// Destroys all game objects within the given group.  If destroy_group is true, also remove the group.  
		/// Returns true if that game object was destroyed (i.e. was it found).
		void group_destroy(std::string group_name, bool destroy_group);

		/// Sets the visibility of all game objects within the given group.
		void set_visibility(std::string group_name, bool is_visible);

		/// Creates a new game object.  This creates a group as well, if the given one doesn't exist.
		GameObject* create_game_object(std::string group_name, std::string object_name, GameObject* parent = NULL, unsigned int tag = 0);
	};
}

#endif