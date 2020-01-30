#pragma once

#include <Ogre.h>

namespace OgreEngine
{
	/// Proto type of friend class
	class GameObjectManager;

	/// A THING in our game (bullet, player, trigger-point, etc.) Contains a collection of 
	/// components that give it functionality. By itself, the game object is basically a node 
	/// within a scene graph (and so has a position, orientation, scale, and the ability
	/// to child to another GameObject) and not much else.  GameObject inherits from 
	/// InputObjserver so a particular game object
	/// can be registered as a listener with the InputManager
	class GameObject
	{
		/// GameObjectManager is the factory for this class GameObject
		friend GameObjectManager;

		// ***** ATTRIBUTES *****
	protected:
		/// The name of this object 
		std::string mName;

		/// The Ogre Scene Node storing our position / orientation / scale and spot within the hierarchy
		Ogre::SceneNode* mSceneNode;

		/// An integer tag that can be used by the user (e.g. 0 = invaders, 1 = player, ... 
		int mTag;

		// ***** CONSTRUCTORS / DESTRUCTORS *****
	protected:
		/// Constructor.  Note: Constructors are normally public.  I'm doing this, however, to force the
		/// user to create a GameObject through the GameObjectManager (the "Factory")
		GameObject(std::string name, int tag, GameObject* parent = NULL, Ogre::Vector3 pos = Ogre::Vector3::ZERO, Ogre::Quaternion rot = Ogre::Quaternion::IDENTITY);

		/// Destructor
		virtual ~GameObject();


		// ***** TRANSFORMATION (ABSOLUTE) SETTERS *****
	public:
		/// Makes this game object a child of the given game object, detaching it from its current parent (if any).
		void set_parent(GameObject* parent);

		/// Set the positional offset (relative to parent game object) 
		void set_position(float x, float y, float z) { mSceneNode->setPosition(Ogre::Vector3(x, y, z)); }

		/// Set the positional offset (relative to parent game object) 
		void set_position(const Ogre::Vector3& v) { mSceneNode->setPosition(v); }

		/// Set the positional offset (relative to world) 
		void set_position_world(float x, float y, float z) { mSceneNode->setPosition(mSceneNode->getParentSceneNode()->convertWorldToLocalPosition(Ogre::Vector3(x, y, z))); }

		/// Set the positional offset (relative to world) 
		void set_position_world(const Ogre::Vector3& v) { mSceneNode->setPosition(mSceneNode->getParentSceneNode()->convertWorldToLocalPosition(v)); }

		/// Sets the scale offset (relative to the parent game object)
		void set_scale(float sx, float sy, float sz) { mSceneNode->setScale(Ogre::Vector3(sx, sy, sz)); }

		/// Sets the scale offset (relative to the parent game object)
		void set_scale(const Ogre::Vector3& s) { mSceneNode->setScale(s); }

		/// Sets the Direction vector of the Object
		void set_direction(float x, float y, float z) { mSceneNode->setDirection(x, y, z); }

		/// Sets the direction vector of the Object
		void set_direction(const Ogre::Vector3& d) { mSceneNode->setDirection(d); }

		/// Sets the rotational offset (relative to the parent game object)
		void set_orientation(float degrees, float vx, float vy, float vz) { mSceneNode->setOrientation(Ogre::Quaternion(Ogre::Degree(degrees), Ogre::Vector3(vx, vy, vz))); }

		/// Sets the rotational offset (relative to the parent game object)
		void set_orientation(const Ogre::Quaternion& q) { mSceneNode->setOrientation(q); }

		/// Makes this object's negative z-axis point towards the given world space position
		void look_at(float x, float y, float z) { mSceneNode->lookAt(Ogre::Vector3(x, y, z), Ogre::Node::TS_WORLD); }

		/// Makes this object's negative z-axis point towards the given world space position
		void look_at(Ogre::Vector3 d = Ogre::Vector3::ZERO) { mSceneNode->lookAt(d, Ogre::Node::TS_WORLD); }

		// ***** TRANSFORMATION (RELATIVE) SETTERS *****
	public:
		/// Adjusts the rotational offset (relative to the parent game object) -- the passed rotation information is relative to the world axes
		void rotate_world(float degrees, float vx, float vy, float vz) { mSceneNode->rotate(Ogre::Quaternion(Ogre::Degree(degrees), Ogre::Vector3(vx, vy, vz)), Ogre::Node::TS_WORLD); }

		/// Adjusts the rotational offset (relative to the parent game object) -- the passed rotation information is relative to the world axes
		void rotate_world(const Ogre::Quaternion& q) { mSceneNode->rotate(q, Ogre::Node::TS_WORLD); }

		/// Adjusts the rotational offset (relative to the parent game object) -- the passed rotation information is relative to the local axes
		void rotate_local(float degrees, float vx, float vy, float vz) { mSceneNode->rotate(Ogre::Quaternion(Ogre::Degree(degrees), Ogre::Vector3(vx, vy, vz)), Ogre::Node::TS_LOCAL); }

		/// Adjusts the rotational offset (relative to the parent game object) -- the passed rotation information is relative to the local axes
		void rotate_local(const Ogre::Quaternion& q) { mSceneNode->rotate(q, Ogre::Node::TS_LOCAL); }

		/// Adjusts the positional offset (relative to the parent game object) -- the passed translation information is relative to the world axes
		void translate_world(float tx, float ty, float tz) { mSceneNode->translate(Ogre::Vector3(tx, ty, tz), Ogre::Node::TS_WORLD); }

		/// Adjusts the positional offset (relative to the parent game object) -- the passed translation information is relative to the world axes
		void translate_world(const Ogre::Vector3& v) { mSceneNode->translate(v, Ogre::Node::TS_WORLD); }

		/// Adjusts the positional offset (relative to the parent game object) -- the passed translation information is relative to the local axes
		void translate_local(float tx, float ty, float tz) { mSceneNode->translate(Ogre::Vector3(tx, ty, tz), Ogre::Node::TS_LOCAL); }

		/// Adjusts the positional offset (relative to the parent game object) -- the passed translation information is relative to the local axes
		void translate_local(const Ogre::Vector3& v) { mSceneNode->translate(v, Ogre::Node::TS_LOCAL); }

		/// Adjusts the scale offset (relative to the parent game object)
		void scale(float sx, float sy, float sz) { mSceneNode->scale(Ogre::Vector3(sx, sy, sz)); }

		/// Adjusts the scale offset (relative to the parent game object)
		void scale(const Ogre::Vector3& s) { mSceneNode->scale(s); }

		// ***** TRANSFORMATION GETTERS *****
	public:
		/// Gets the position of this object relative to the world axes.
		Ogre::Vector3 get_world_position() { return mSceneNode->_getDerivedPosition(); }

		/// Gets the position of this object relative to the parent game object's axes.
		Ogre::Vector3 get_position() { return mSceneNode->getPosition(); }

		/// Gets the orientation of this object relative to the world axes.
		Ogre::Quaternion get_world_orientation() { return mSceneNode->_getDerivedOrientation(); }

		/// Gets the orientation of this object relative to the parent game object's axes.
		Ogre::Quaternion get_orientation() { return mSceneNode->getOrientation(); }

		/// Gets the scale of this object relative to the world axes.
		Ogre::Vector3 get_world_scale() { return mSceneNode->_getDerivedScale(); }

		/// Gets the scale offset from the parent game object
		Ogre::Vector3 get_scale() { return mSceneNode->getScale(); }

		// ***** OTHER GETTERS *****
	public:
		/// Gets the ogre scene node this object is based upon.
		Ogre::SceneNode* get_scene_node() { return mSceneNode; }

		/// [TEMPORARY] Adds a Moveable object (Entity, Light, etc.) to the GameObject's scene node
		/// Once we have our Component model, this will likely go away.
		void attach_object(Ogre::MovableObject* obj) { mSceneNode->attachObject(obj); }

		/// Gets the name of this game object
		std::string get_name() { return mName; }

		/// Gets the group name (as a string) of this game object in the GOM
		int get_tag() { return mTag; }


		// ***** OTHER METHODS *****
	public:
		/// Makes all components of this object active / visible (or not, if the parameter is false)
		void set_visibility(bool is_visible) { mSceneNode->setVisible(is_visible); }
	};
}