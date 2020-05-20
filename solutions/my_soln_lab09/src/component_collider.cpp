#include <stdafx.h>
#include <component_collider.h>
#include <game_object.h>
#include <application.h>
#include <game_object_manager.h>
#include <collision_manager.h>


ssuge::ComponentCollider::ComponentCollider(GameObject* owner, int layer, long long mask, float radius) : Component(owner),
	mColliderType(ColliderType::SPHERE), mVolumeVisualizationObject(NULL), mCollisionMask(mask), mLayer(layer)
{
	mData.x = radius;
	mData.y = radius * radius;
	mData.z = 0.0f;

	COLLISION_MANAGER->add_collider(this);
}



ssuge::ComponentCollider::ComponentCollider(GameObject* owner, int layer, long long mask, Ogre::Vector3 extents) : Component(owner),
	mColliderType(ColliderType::RECTANGULAR_PRISM), mData(extents), mVolumeVisualizationObject(NULL),
	mCollisionMask(mask), mLayer(layer)
{
	COLLISION_MANAGER->add_collider(this);
}



ssuge::ComponentCollider::~ComponentCollider()
{
	if (COLLISION_MANAGER)
		COLLISION_MANAGER->remove_collider(this);
}



void ssuge::ComponentCollider::set_visible(bool is_visible)
{

}



void ssuge::ComponentCollider::create_collider_visualization()
{
	// Detach any existing visualizations (should be rare)
	if (mVolumeVisualizationObject)
	{
		GAME_OBJECT_MANAGER->destroy_game_object("collider_volumes", mVolumeVisualizationObject->get_name());
		mVolumeVisualizationObject = NULL;
	}

	// These mesh names are hard-coded.  I'm assuming that there will be a "core" media distributed
	// with the engine.  These meshes should go there.
	mVolumeVisualizationObject = GAME_OBJECT_MANAGER->create_game_object("collider_volumes", mOwner->get_name() + "_physics_vol", mOwner, 0xFFFFFF);
	Ogre::Vector3 my_scale = mOwner->get_world_scale();
	if (mColliderType == ColliderType::RECTANGULAR_PRISM)
	{
		Ogre::Vector3 vis_scale(mData.x / my_scale.x, mData.y / my_scale.y, mData.z / my_scale.z);
		mVolumeVisualizationObject->scale(vis_scale);
		ComponentMesh* mptr = mVolumeVisualizationObject->create_mesh_component("ColliderCube.mesh");
		mptr->set_cast_shadows(false);
	}
	else
	{
		Ogre::Vector3 vis_scale(mData.x / my_scale.x, mData.x / my_scale.y, mData.x / my_scale.z);
		mVolumeVisualizationObject->scale(vis_scale);
		ComponentMesh* mptr = mVolumeVisualizationObject->create_mesh_component("ColliderSphere.mesh");
		mptr->set_cast_shadows(false);
	}
}



Ogre::Vector2 ssuge::ComponentCollider::get_projection_extents(const Ogre::Vector3& origin, const Ogre::Vector3& direction)
{
	Ogre::Vector3 obj_origin = mOwner->get_world_position();
	if (mColliderType == ColliderType::SPHERE)
	{
		Ogre::Vector3 offset = obj_origin - origin;
		float proj = offset.dotProduct(direction);
		return Ogre::Vector2(proj - mData.x, proj + mData.x);
	}
	else
	{
		Ogre::Vector3 xaxis, yaxis, zaxis;
		mOwner->get_local_axes(xaxis, yaxis, zaxis);
		std::vector<Ogre::Vector3> corners = get_rectangular_prism_points(obj_origin, xaxis, yaxis, zaxis);

		bool first_assigned = false;
		Ogre::Vector2 result;
		for (Ogre::Vector3 p : corners)
		{
			Ogre::Vector3 offset = p - origin;
			float proj = offset.dotProduct(direction);
			if (!first_assigned)
			{
				result.x = result.y = proj;
				first_assigned = true;
			}
			else
			{
				if (proj < result.x)
					result.x = proj;
				if (proj > result.y)
					result.y = proj;
			}
		}

		return result;
	}
}



std::vector<Ogre::Vector3> ssuge::ComponentCollider::get_rectangular_prism_points(const Ogre::Vector3 pos, 
	const Ogre::Vector3& xaxis, const Ogre::Vector3& yaxis, const Ogre::Vector3& zaxis)
{
	std::vector<Ogre::Vector3> result;

	result.push_back(pos + mData.x * xaxis + mData.y * yaxis + mData.z * zaxis);
	result.push_back(pos + mData.x * xaxis + mData.y * yaxis - mData.z * zaxis);
	result.push_back(pos + mData.x * xaxis - mData.y * yaxis + mData.z * zaxis);
	result.push_back(pos + mData.x * xaxis - mData.y * yaxis - mData.z * zaxis);
	result.push_back(pos - mData.x * xaxis + mData.y * yaxis + mData.z * zaxis);
	result.push_back(pos - mData.x * xaxis + mData.y * yaxis - mData.z * zaxis);
	result.push_back(pos - mData.x * xaxis - mData.y * yaxis + mData.z * zaxis);
	result.push_back(pos - mData.x * xaxis - mData.y * yaxis - mData.z * zaxis);

	return result;
}


bool ssuge::ComponentCollider::has_separation(const Ogre::Vector2& ext1, const Ogre::Vector2& ext2)
{
	return ext1[0] > ext2[1] || ext2[0] > ext1[1];
}