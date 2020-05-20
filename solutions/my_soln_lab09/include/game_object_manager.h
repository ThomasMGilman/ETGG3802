#ifndef _GAME_OBJECT_MANAGER_H_
#define _GAME_OBJECT_MANAGER_H_
#include <stdafx.h>
#include <singleton.h>
#include <game_object.h>
#include <tinyxml2.h>

#define GAME_OBJECT_MANAGER ssuge::GameObjectManager::get_singleton_ptr()

namespace ssuge
{
	/// The GameObjectManager (or GOM for short) is a collection of GameObjects.  This class
	/// is a "Factory" for GameObjects (which means it is the only way to create / destroy
	/// GameObjects, plus there are ways to access a GameObject via the GOM).  The organization
	/// is pretty simple for now -- we have any number of uniquely-named groups.  Within that group
	/// is a collection of uniquely named GameObjects (a map-of-maps) -- in theory a GameObject name
	/// only has to be unique within the group, but when finding a GameObject solely by name, it is 
	/// non-deterministic which one will be returned.
	class GameObjectManager : public Singleton<GameObjectManager>
	{
	// ***** ATTRIBUTES *****
	protected:
		/// The master list of game objects
		std::map<std::string, std::map<std::string, GameObject*>> mGameObjects;

		/// The total number of game objects (I store it so I don't have to recalculate)
		int mNumGameObjects;

		/// A collection of game objects (that we don't know the group) that the user wants to destroy asap
		std::vector<std::string> mUngroupedObjectsToDestroy;

		/// A collection of game objects (group / name) that the user wants to destroy asap
		std::vector<std::pair<std::string, std::string>> mObjectsToDestroy;

		/// A set of all GameObject pointers.  Why? This is a little wasteful on memory, but it 
		/// greatly speeds up the "has_game_object" method.  The user is really meant to use
		/// the map-of-maps most of the time.  This is *just* for the membership test
		std::set<GameObject*> mAllGameObjects;

	// ***** CONSTRUCTOR / DESTRUCTOR *****
	public:
		/// Constructor
		GameObjectManager();

		/// Destructor
		~GameObjectManager();

	// ***** GETTERS / SETTERS *****
	public:
		/// Gets a game object (NULL if it doesn't exist) -- more costly: searches through all groups
		GameObject * get_game_object(std::string game_object_name);

		/// Gets a game object within a group (NULL if it doesn't exist)
		GameObject * get_game_object(std::string game_object_name, std::string group_name);

		/// Gets all game objects within a group.  result is an output parameter (and is cleared upon calling this)
		void get_game_objects(std::string group_name, std::vector<ssuge::GameObject*>& result);

		/// Returns the number of game objects (in total) the GOM is storing
		int get_num_game_objects() { return mNumGameObjects; }

		/// Returns true if there is a group by this name
		bool has_group(std::string group_name);

		/// Returns true if this game object is being stored somewhere by the GOM
		bool has_game_object(GameObject* gobj);

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
		GameObject * create_game_object(std::string group_name, std::string object_name, GameObject * parent = NULL, unsigned int tag = 0, std::string script_name = "", bool call_create = false);

		/// Updates all game objects (which will cause their components to update)
		void update(float dt);

		/// Loads a scene file
		void load_scene_file(std::string group_name, std::string fname);

		/// Queues destruction of a game object with this name (where we don't know the group)
		void queue_destroy(std::string object_name) { mUngroupedObjectsToDestroy.push_back(object_name); }

		/// Queues destruction of a game object with this name (where we DO know the group -- prefer this one!)
		void queue_destroy(std::string group_name, std::string object_name) { mObjectsToDestroy.push_back(std::pair<std::string, std::string>(group_name, object_name)); }

	protected:

		/// Processes and returns a Vector3 node
		Ogre::Vector3 process_XML_vector_node(tinyxml2::XMLElement* e);

		/// Process and returns a Quaternione node
		Ogre::Quaternion process_XML_quaternion_node(tinyxml2::XMLElement* e);

		/// Process and returns a Colour value from the given node
		Ogre::ColourValue process_XML_color_node(tinyxml2::XMLElement* e);

		/// Processes a node (GameObject) branch of the tree, including its descendents
		GameObject * process_XML_game_object(std::string group_name, tinyxml2::XMLElement* e, GameObject * cur_parent);

		/// Process the light branch of an xml tree
		void process_XML_light_node(tinyxml2::XMLElement* elem, GameObject* gobj);

		/// Process the camera branch of an xml tree
		void process_XML_camera_node(tinyxml2::XMLElement* elem, GameObject* gobj);

		/// Process the collider information of an xml tree
		void process_XML_collider_data(tinyxml2::XMLElement* elem, GameObject* gobj);

		/// Processes one node (in a recursive fashion) of the xml tree.  cur_parent will be NULL unless
		/// we're processing descendent nodes of another node (GameObject) node [too many 'nodes' in that sentence:-)]
		void process_XML_node(std::string group_name, tinyxml2::XMLElement* elem, GameObject * cur_parent);
	};
}

#endif
