#pragma once
#include <stdafx.h>
#include <component.h>
#include <component_camera.h>
#include <component_light.h>
#include <component_mesh.h>
#include <component_input_listener.h>
#include <component_collider.h>

namespace ssuge
{
	/// A THING in our game (bullet, player, trigger-point, etc.) Contains a collection of 
	/// components that give it functionality. By itself, the game object is basically a node 
	/// within a scene graph (and so has a position, orientation, scale, and the ability
	/// to child to another GameObject) and not much else.  GameObject inherits from 
	/// InputObjserver so a particular game object
	/// can be registered as a listener with the InputManager
	class GameObject
	{
	// ***** ATTRIBUTES *****
	protected:
		/// The name of this object 
		std::string mName;

		/// The Ogre Scene Node storing our position / orientation / scale and spot within the hierarchy
		Ogre::SceneNode * mSceneNode;

		/// An integer tag that can be used by the user (e.g. 0 = invaders, 1 = player, ... 
		int mTag;

		/// A map of Components in this GameObject
		std::map<ComponentType, Component*> mComponents;

		/// The Python "twin" of this game object
		PyObject* mScriptTwin;

		/// The parent game object, if we have one.  Potential bug: we don't have a way (currently) to detect
		/// if this game object becomes detached from its parent.  Holds NULL if we don't have a parent.
		GameObject* mParentObject;

	// ***** CONSTRUCTORS / DESTRUCTORS *****
	protected:
		/// Constructor.  Note: Constructors are normally public.  I'm doing this, however, to force the
		/// user to create a GameObject through the GameObjectManager (the "Factory")
		GameObject(std::string name, int tag, GameObject * parent = NULL);

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

		/// Sets the rotational offset (relative to the parent game object)
		void set_orientation(float degrees, float vx, float vy, float vz) { mSceneNode->setOrientation(Ogre::Quaternion(Ogre::Degree(degrees), Ogre::Vector3(vx, vy, vz))); }

		/// Sets the rotational offset (relative to the parent game object)
		void set_orientation(const Ogre::Quaternion& q) { mSceneNode->setOrientation(q); }

		/// Sets the rotational offset (relative to the world axes)
		void set_orientation_world(const Ogre::Quaternion& q) { mSceneNode->setOrientation(Ogre::Quaternion::IDENTITY); mSceneNode->rotate(q, Ogre::Node::TS_WORLD); }

		/// Makes this object's negative z-axis point towards the given world space position
		void look_at(float x, float y, float z) { mSceneNode->lookAt(Ogre::Vector3(x, y, z), Ogre::Node::TS_WORLD); }

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

		/// Gets the world-space positon of a given point (which is expressed in terms of the local space)
		Ogre::Vector3 get_relative_point_position(Ogre::Vector3 rel_pos) { return mSceneNode->convertLocalToWorldPosition(rel_pos); }

		/// Gets the orientation of this object relative to the world axes.
		Ogre::Quaternion get_world_orientation() { return mSceneNode->_getDerivedOrientation(); }

		/// Gets the orientation of this object relative to the parent game object's axes.
		Ogre::Quaternion get_orientation() { return mSceneNode->getOrientation(); }

		/// Gets the scale of this object relative to the world axes.
		Ogre::Vector3 get_world_scale() { return mSceneNode->_getDerivedScale(); }

		/// Gets the scale offset from the parent game object
		Ogre::Vector3 get_scale() { return mSceneNode->getScale(); }

	// @@@@@ COMPONENT-based METHODS @@@@
	public:
		/// Creates a (box) collider component
		ComponentCollider* create_box_collider_component(int layer, long long mask, const Ogre::Vector3& extents);

		/// Creates a (sphere) collider component
		ComponentCollider* create_sphere_collider_component(int layer, long long mask, float radius);
		
		/// Create a mesh component
		ComponentMesh* create_mesh_component(std::string fname);

		/// Create a light component
		ComponentLight* create_light_component(LightType lt);

		/// Create a camera component
		ComponentCamera* create_camera_component();

		/// Create an input listener component
		ComponentInputListener* create_input_listener_component();

		/// Gets the collider component (or NULL if there is none)
		ComponentCollider* get_collider_component();
		
		/// Gets the mesh component (or NULL if there is none)
		ComponentMesh* get_mesh_component();

		/// Gets the light component (or NULL if there is none)
		ComponentLight* get_light_component();

		/// Gets the camera component (or NULL if there is none)
		ComponentCamera* get_camera_component();

		/// Gets the input listener component (or NULL if there is none)
		ComponentInputListener* get_input_listener_component();

		/// Destroys the component of the given type (no effect if we don't have one)
		void destroy_component(ComponentType t);

		/// Updates all components (if they've overrided the update method)
		void update(float dt);

	// ***** OTHER GETTERS / SETTERS *****
	public:
		/// Gets the ogre scene node this object is based upon.
		Ogre::SceneNode* get_scene_node() { return mSceneNode; }

		/// Gets the name of this game object
		std::string get_name() { return mName; }

		/// Gets our parent game object, or NULL if we don't have one.
		GameObject* get_parent() { return mParentObject; }

		/// Gets the group name (as a string) of this game object in the GOM
		int get_tag() { return mTag;}

		/// Makes this game object script-aware.  The PyObject should be a python object derived from
		/// a class which is derived from ssuge.GameObject.  If we already had a twin (unlikely, since this
		/// should get set when we create it), decref it and use the new one
		void set_script_twin(PyObject* twin) { if (mScriptTwin) Py_DECREF(mScriptTwin);  mScriptTwin = twin; }

		/// Returns the PyObject* for the script twin, or NULL if this game object is not script-aware
		PyObject* get_script_twin() { return mScriptTwin; }

		/// Gets our local axes, expressed in world units
		void get_local_axes(Ogre::Vector3& xaxis, Ogre::Vector3& yaxis, Ogre::Vector3& zaxis);


	// ***** OTHER METHODS *****
	public:
		/// Makes all components of this object active / visible (or not, if the parameter is false)
		void set_visibility(bool is_visible);

		/// Executes a method of the script-twin, if it has one, passing a python tuple (which can be
		/// NULL if no arguments are needed).
		void execute_script_method(std::string method_name, PyObject* args_tuple = NULL);
	// ***** FRIENDS *****
		/// This is so the GameObjectManager (GOM) can create instances of GameObject
		friend class GameObjectManager;
	};
}
