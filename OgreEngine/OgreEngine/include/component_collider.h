#pragma once
#include <stdafx.h>
#include <component.h>

namespace ssuge
{
	// Forward references
	enum class CollisionLayer;

	/// Collilder types
	enum class ColliderType { RECTANGULAR_PRISM, SPHERE};

	/// When a ComponentCollider is attached to an object, it becomes "collision-aware".  It has the 
	/// potential to collide with another collider.  Warning: the major thing that is currently not implemented
	/// is the ability to react to changes in the parent game object's scale.  The boundaries given here
	/// are in world-space terms.  If the object is scaled, some components (like Mesh) will reflect this, but
	/// others (like hit detection) will not.
	class ComponentCollider : public Component
	{
	// ***** ATTRIBUTES *****
	protected:
		/// What type of collider is this?
		ColliderType mColliderType;

		/// For rectangular prisms, this is the "half-extents" centered around the game object's
		/// origin.  For sphere's, the .x field is the radius, the .y field is the radius^2, the .z is unused
		Ogre::Vector3 mData;

		/// If create_collider_vis is called, this will be initialized.  This game object will be scaled to match
		/// this collider's dimensions and a mesh component will be loaded to show the volume
		GameObject* mVolumeVisualizationObject;

		/// The layer *this* object lies on.  Should be a number between 0 - 31 (this is enforced in GameObject)
		int mLayer;

		/// A mask that indicates what collision layers this object will collide with
		/// (one bit = a layer, 1 = we can collide with it, 0 = we ignore it)
		long long mCollisionMask;

	// ***** CONSTRUCTORS / DESTRUCTORS *****
	public:
		/// Creates a sphere-collider
		ComponentCollider(GameObject* owner, int layer, long long mask, float radius);

		/// Creates a rectangular-prism-collider
		ComponentCollider(GameObject* owner, int layer, long long mask, Ogre::Vector3 extents);

		/// Destructor
		virtual ~ComponentCollider();

	// ***** GETTERS / SETTERS *****
	public:
		/// Returns the type of collider
		ColliderType get_collider_type() { return mColliderType;  }

		/// Gets the extents data for this collider
		Ogre::Vector3 get_data() { return mData; }

		/// Gets the layer this object exists on (should be 0 - 31)
		int get_layer() { return mLayer; }

		/// Gets the mask.  If 1 bit-left-shifted the other object's layer anded with this mask is non-zero,
		/// we potentially have a collision
		long long get_mask() { return mCollisionMask; }

		/// Projects this object onto the given Ray.  The first number is <= the second
		Ogre::Vector2 get_projection_extents(const Ogre::Vector3& origin, const Ogre::Vector3& direction);

		/// Gets the corner points (only makes sense if this is a box collider, but the call always returns something)
		std::vector<Ogre::Vector3> get_rectangular_prism_points(const Ogre::Vector3 pos, const Ogre::Vector3& xaxis, const Ogre::Vector3& yaxis, const Ogre::Vector3& zaxis);

		/// Return the type of this component
		ComponentType get_type() override { return ComponentType::COLLIDER; }

		/// Deactivates this component if is_visible is false.
		void set_visible(bool is_visible) override;

	// ***** OTHER METHODS *****
	public:
		/// Create a volume visualization model
		void create_collider_visualization();

		/// Returns true if two extents (from calling get_projection_extents) have "daylight" between them
		static bool has_separation(const Ogre::Vector2& ext1, const Ogre::Vector2& ext2);
	};
}
