#pragma once
#include <stdafx.h>
#include <singleton.h>

#define COLLISION_MANAGER ssuge::CollisionManager::get_singleton_ptr()

namespace ssuge
{
	class ComponentCollider;
	class GameObject;

	/// The collision manager is responsible for storing pointers to all collider
	/// components (that are managed by the GameObject).  When update it called, it
	/// does a brute-force (O(n^2)) test to see if collider A collides with any collider B.
	/// If it finds one, both are notified, if they are script-aware.
	class CollisionManager : public Singleton<CollisionManager>
	{
	protected:
		/// All colliders that are active in the scene right now
		std::set<ComponentCollider*> mColliders;

		/// A cached set of all pairs of collisions we have seen up to this point.  This
		/// is necessary because we want to notify the game objects when the start / stop / keep
		/// colliding
		std::set<std::pair<GameObject*, GameObject*>> mOngoingCollisions;

	public:
		/// Default constructor
		CollisionManager();

		/// Destructor
		virtual ~CollisionManager();

		/// Adds a collider to our master list.  This is done by the ComponentCollider
		void add_collider(ComponentCollider* c);


		/// This is called by the ComponentCollider destructor -- removes a collider form our master list
		void remove_collider(ComponentCollider* c);

		/// Looks for any collisions
		void update(float dt);

		/// Loads volume visualizations for all current colliders
		void load_visualizations();

		/// Shows / hides debug visualization (no effect if they haven't been created)
		void set_debug_visibility(bool show);

		/// Returns true if the given mask and layer are valid, false if either is not
		bool validate_mask_and_layer(long long mask, int layer);

		/// Clears and filles in the passed Vector with GameObjects that are
		/// hit by the given ray.  Returns true if one thing or more is hit.  This variant
		/// will consider the ray to be infinite.
		bool raycast_unbounded(const Ogre::Vector3& origin, const Ogre::Vector3& direction, std::vector<std::pair<Ogre::Vector3, GameObject*>>& hits, long long mask = 0xFFFFFFFFFFFFFFFF);

		/// Similar to raycast_unbounded, but this version will only count hits that are less along
		/// the ray than max_dist. 
		bool raycast_bounded(const Ogre::Vector3& origin, const Ogre::Vector3& direction, std::vector<std::pair<Ogre::Vector3, GameObject*>>& hits, float max_dist, long long mask = 0xFFFFFFFFFFFFFFFF);

	protected:
		/// Helper function that looks for collision between a rectangular prism and another rectangular prism
		bool rprism_rprism_collide(ComponentCollider* rprismA, ComponentCollider* rprismB);

		/// Helper function that looks for collision between two spheres
		bool sphere_sphere_collider(ComponentCollider* sphereA, ComponentCollider* sphereB);

		/// Helper function that looks for collision between a rectangular prism and a sphere
		bool rprism_sphere_collider(ComponentCollider* rprism, ComponentCollider* sphere);	

		/// Appends any hits with this ray and this prism to the hit_objects list
		void rprism_ray_collide(ComponentCollider* rprism, const Ogre::Vector3& ray_origin, const Ogre::Vector3& ray_direction, std::vector<std::pair<Ogre::Vector3, GameObject*>>& hits, float max_dist = -1.0f);

		///  Appends any hits with this ray and this sphere to the hit_objects list
		void sphere_ray_collide(ComponentCollider* sphere, const Ogre::Vector3& ray_origin, const Ogre::Vector3& ray_direction, std::vector<std::pair<Ogre::Vector3, GameObject*>>& hits, float max_dist = -1.0f);
	};
}
