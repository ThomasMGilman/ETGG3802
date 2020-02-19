#pragma once

#ifndef _GAME_OBJECT_MANAGER_H_
#define _GAME_OBJECT_MANAGER_H_

#include <game_object.h>
#include <singleton.h>
#include <Ogre.h>
#include <tinyxml2.h>
#include <map>

#define GAME_OBJ_MANAGER GameObjectManager::getSingletonPtr()

namespace OgreEngine
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
		// ***** structs *****
	public:
		struct ExternalResource
		{
			std::string name;
			std::string path;
		};

		// ***** ATTRIBUTES *****
	protected:
		// GameObject map for creating and keeping track of GameObjects
		std::map<std::string, std::map<std::string, GameObject*>> mObjects;
		std::map<int, std::vector<GameObject*>> mTaggedObjects;

		//map of Externals parsed by scene xml, each external is a list of the externalResource and its Path
		std::map<std::string, std::list<ExternalResource>> mExternals;

		// Iterators
		std::map<std::string, GameObject*>::iterator mObjIter;
		std::map<std::string, GameObject*>::reverse_iterator mObjRevIter;
		std::map<std::string, std::map<std::string, GameObject*>>::iterator mGroupsIter;
		std::map<std::string, std::map<std::string, GameObject*>>::reverse_iterator mGroupsRevIter;

		std::string mMediaPath = "../Media/my_media/";

		//tinyXML2 document reader
		tinyxml2::XMLDocument* mDoc;

		// ***** CONSTRUCTOR / DESTRUCTOR *****
	public:
		/// Constructor
		GameObjectManager();

		/// Destructor
		~GameObjectManager();
		
		// ***** Update Function *****
	public:
		void update(float elapsed);
		
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

		// ***** OBJECT / XML READER FUNCTIONS *****
	public:
		/// Given a list of scenes or object xmls. This loads in the scene/objects
		void load_scenes(std::list<std::tuple<std::string, std::string>> fileNames);

		/// Load in the given scene/objects from the file given using tinyxml2
		void load_scene(std::string fileName, std::string path);

		/// Create new GameObject Component from xml parameters
		void parse_xml_nodes(tinyxml2::XMLElement* element, std::string groupName, std::string path, GameObject* parent = nullptr);
		
		/// Get Colour back from XML
		Ogre::ColourValue parse_xml_color_data(tinyxml2::XMLElement* element);

		/// Get Vec3 back from XML
		Ogre::Vector3 parse_xml_position_data(tinyxml2::XMLElement* element);

		/// Get Vec4/Quaternion back from XML
		Ogre::Quaternion parse_xml_quaternion_data(tinyxml2::XMLElement* element);

		/// Parse External Resource file and add it to map of its type
		void parse_xml_external(tinyxml2::XMLElement* element, std::string resourceType, std::string path);

		/// Parse Envrionment variables
		void parse_xml_environment(tinyxml2::XMLElement* element);

		/// Templated class appends node elements from xml file to specified parent passed, both GameObject or Component.
		void parse_xml_gameobject(tinyxml2::XMLElement* element, std::string groupName, std::string path, GameObject* parent);

		/// Create new Mesh Component from xml parameters
		void parse_xml_mesh(tinyxml2::XMLElement* element, std::string groupName, std::string path, GameObject* parent);

		void parse_xml_mesh_data(tinyxml2::XMLElement* element, std::string groupName, std::string path, MeshComponent* parent);

		/// Create new Camera Component from xml parameters
		void parse_xml_camera(tinyxml2::XMLElement* element, std::string groupName, std::string path, GameObject* parent);

		void parse_xml_camera_data(tinyxml2::XMLElement* element, std::string groupName, std::string path, CameraComponent* parent);

		/// Create new Light Component from xml parameters
		void parse_xml_light(tinyxml2::XMLElement* element, std::string groupName, std::string path, GameObject* parent);

		void parse_xml_light_data(tinyxml2::XMLElement* element, std::string groupName, std::string path, LightComponent* parent);

		/// Append Properties from the userData section of the xml being parsed to the parent object
		void parse_xml_properties(tinyxml2::XMLElement* element, std::string groupName, std::string path, Component* parent);

		// ***** DELETION METHODS ***** 
	public:
		/// Destroys a game object (faster version) if you know the group name
		bool destroy_game_object(std::string group_name, std::string gobj_name, bool ignoreLog = true);
		
		/// Destroy a game object (slower version) if you don't know the group name.  Returns true if that game object was destroyed
		/// (i.e. was it found)
		bool destroy_game_object(std::string gobj_name);

		/// Destroys all game objects within the given group.  If destroy_group is true, also remove the group.  
		/// Returns true if that game object was destroyed (i.e. was it found).
		void group_destroy(std::string group_name, bool destroy_group);

		
		void group_destroy(std::map<std::string, std::map<std::string, GameObject*>>::iterator& group, bool destroy_group);

		/// Destroys all game objects and groups entirely
		void destroy_all();

		void set_game_object_tag(int newTag, GameObject* object);

		/// Sets the visibility of all game objects within the given group.
		void set_visibility(std::string group_name, bool is_visible);

		/// Creates a new game object.  This creates a group as well, if the given one doesn't exist.
		GameObject* create_game_object(std::string group_name, std::string object_name, GameObject* parent = nullptr, unsigned int tag = 0, Ogre::Vector3 pos = Ogre::Vector3::ZERO, Ogre::Quaternion rot = Ogre::Quaternion::IDENTITY);
		
		GameObject* create_ground_plane(std::string materialName);

		/// Creates the default scene
		void set_default_scene();
	};
}

#endif