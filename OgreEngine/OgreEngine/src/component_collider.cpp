#include <stdafx.h>
#include <utility.h>
#include <game_object.h>
#include <game_object_manager.h>
#include <collision_manager.h>
#include <component_collider.h>

using namespace OgreEngine;

ComponentCollider::ComponentCollider(GameObject* owner, int layer, long long mask, float radius, std::string name) :
	mLayer(layer), mCollisionMask(mask), mColliderType(ColliderType::SPHERE), mData(Ogre::Vector3(radius, radius*radius, 0)), 
	Component(owner, name.empty() ? owner->get_name() + "_collider" : name)
{
	COLLISION_MANAGER->add_collider(this);
}

ComponentCollider::ComponentCollider(GameObject* owner, int layer, long long mask, Ogre::Vector3 extents, std::string name):
	mLayer(layer), mCollisionMask(mask), mColliderType(ColliderType::RECTANGULAR_PRISM), mData(extents), 
	Component(owner, name.empty() ? owner->get_name() + "_collider" : name)
{
	COLLISION_MANAGER->add_collider(this);
}

ComponentCollider::~ComponentCollider()
{
	if (this != nullptr)
	{
		COLLISION_MANAGER->remove_collider(this);
		if (mVolumeVisualizationObject != nullptr)
			GAME_OBJ_MANAGER->destroy_game_object(mVolumeVisualizationObject->get_group_name(), mVolumeVisualizationObject->get_name());
	}
}

void OgreEngine::ComponentCollider::update(float elapsed)
{
}

Ogre::Vector3 OgreEngine::ComponentCollider::get_position()
{
	return this->mParent->get_world_position();
}

std::vector<Ogre::Vector3> OgreEngine::ComponentCollider::get_axis()
{
	std::vector<Ogre::Vector3> axis;
	axis.resize(3);
	this->mParent->get_world_orientation().ToAxes(axis[0], axis[1], axis[2]);
	return axis;
}

bool OgreEngine::ComponentCollider::point_inside(Ogre::Vector3 point)
{
	Ogre::Vector3 offset = this->get_position() - point;
	if (this->mColliderType == ColliderType::SPHERE)
		return magnitude(offset, true) <= this->mData.y;
	else if (this->mColliderType == ColliderType::RECTANGULAR_PRISM)
	{
		std::vector<Ogre::Vector3> axis = this->get_axis();
		for (int i = 0; i < 3; i++)
		{
			float proj = abs(offset.dotProduct(axis[i]));
			if (proj > this->mData[i])
				return false;
		}
		return true;
	}
	return false;
}

Ogre::Vector2 ComponentCollider::get_projection_extents(const Ogre::Vector3& origin, const Ogre::Vector3& direction)
{
	Ogre::Vector2 extents;
	if (this->mColliderType == ColliderType::SPHERE)
	{
		Ogre::Vector3 offset = this->get_position() - origin;
		float proj = offset.dotProduct(direction);
		extents = { proj - this->mData.x, proj + this->mData.x };
	}
	else if (this->mColliderType == ColliderType::RECTANGULAR_PRISM)
	{
		std::vector<Ogre::Vector3> points = this->get_rectangular_prism_points();
		bool firstSet = false;
		for (Ogre::Vector3 p : points)
		{
			Ogre::Vector3 offset = p - origin;
			float result = offset.dotProduct(direction);
			if (!firstSet)
			{
				extents = { result, result };
				firstSet = true;
			}
			else if (result < extents[0])
				extents[0] = result;
			else if (result > extents[1])
				extents[1] = result;
		}
	}
	return extents;
}

std::vector<Ogre::Vector3> OgreEngine::ComponentCollider::get_rectangular_prism_points(const Ogre::Vector3& pos, const Ogre::Vector3& xAxis, const Ogre::Vector3& yAxis, const Ogre::Vector3& zAxis)
{
	return {
		pos + mData[0] * xAxis + mData[1] * yAxis + mData[2] * zAxis,		// PtA pos Z dir
		pos + mData[0] * xAxis + mData[1] * yAxis - mData[2] * zAxis,		// PtB pos Z dir
		pos + mData[0] * xAxis - mData[1] * yAxis + mData[2] * zAxis,		// PtC pos Z dir
		pos + mData[0] * xAxis - mData[1] * yAxis - mData[2] * zAxis,		// PtD pos Z dir
		pos - mData[0] * xAxis + mData[1] * yAxis + mData[2] * zAxis,		// PtA neg Z dir
		pos - mData[0] * xAxis + mData[1] * yAxis - mData[2] * zAxis,		// PtB neg Z dir
		pos - mData[0] * xAxis - mData[1] * yAxis + mData[2] * zAxis,		// PtC neg Z dir
		pos - mData[0] * xAxis - mData[1] * yAxis - mData[2] * zAxis		// PtD neg Z dir
	};
}

std::vector<Ogre::Vector3> OgreEngine::ComponentCollider::get_rectangular_prism_points()
{
	std::vector<Ogre::Vector3> axis = this->get_axis();
	return this->get_rectangular_prism_points(this->get_position(), axis[0], axis[1], axis[2]);
}

std::vector<std::pair<Ogre::Vector3, float>> OgreEngine::ComponentCollider::get_rectangular_prism_planes(const Ogre::Vector3& pos, const Ogre::Vector3& xAxis, const Ogre::Vector3& yAxis, const Ogre::Vector3& zAxis)
{
	return {
		std::pair(xAxis, (pos + xAxis * mData[0]).dotProduct(xAxis)),	// Pos X plane
		std::pair(-xAxis, (pos - xAxis * mData[0]).dotProduct(-xAxis)),	// Neg X plane
		std::pair(yAxis, (pos + yAxis * mData[1]).dotProduct(yAxis)),	// Pos Y plane
		std::pair(-yAxis, (pos - yAxis * mData[1]).dotProduct(-yAxis)),	// Neg Y plane
		std::pair(zAxis, (pos + zAxis * mData[2]).dotProduct(zAxis)),	// Pos Z plane
		std::pair(-zAxis, (pos - zAxis * mData[2]).dotProduct(-zAxis))	// Neg Z plane
	};
}

std::vector<std::pair<Ogre::Vector3, float>> OgreEngine::ComponentCollider::get_rectangular_prism_planes()
{
	std::vector<Ogre::Vector3> axis = this->get_axis();
	return this->get_rectangular_prism_planes(this->get_position(), axis[0], axis[1], axis[2]);
}

void ComponentCollider::set_visible(bool is_visible)
{
	if(this->mVolumeVisualizationObject != nullptr)
		this->mVolumeVisualizationObject->set_visibility(is_visible);
}

void ComponentCollider::create_collider_visualization()
{
	if (this->mVolumeVisualizationObject == nullptr)
	{
		this->mVolumeVisualizationObject = GAME_OBJ_MANAGER->create_game_object(mParent->get_group_name(), mParent->get_name() + "_collisionVisualizer",
			mParent, mParent->get_tag(), mParent->get_position(), mParent->get_orientation());
		Ogre::Vector3 my_scale = this->mVolumeVisualizationObject->get_world_scale();
		OgreEngine::MeshComponent* m = nullptr;
		switch (this->mColliderType)
		{
		case ColliderType::SPHERE:
			this->mVolumeVisualizationObject->set_scale(this->mData.x / my_scale.x, this->mData.y / my_scale.y, this->mData.z / my_scale.z);
			m = this->mVolumeVisualizationObject->create_mesh("", "ColliderSphere.mesh");
			
			break;
		case ColliderType::RECTANGULAR_PRISM:
			this->mVolumeVisualizationObject->set_scale(this->mData.x / my_scale.x, this->mData.y / my_scale.y, this->mData.z / my_scale.z);
			m = this->mVolumeVisualizationObject->create_mesh("", "ColliderCube.mesh");
			break;
		default:
			break;
		}
		if(m != nullptr)
			m->set_cast_shadows(false);
	}
	if (COLLISION_MANAGER->is_visible)
		this->mVolumeVisualizationObject->set_visibility(true);
}

void OgreEngine::ComponentCollider::start_collision(GameObject* otherObject)
{
	this->mParent->run_method("collision_start", PyTuple_Pack(1, otherObject->get_script_twin()));
}

void OgreEngine::ComponentCollider::continue_collision(GameObject* otherObject)
{
	this->mParent->run_method("collision_continue", PyTuple_Pack(1, otherObject->get_script_twin()));
}

void OgreEngine::ComponentCollider::end_collision(GameObject* otherObject)
{
	this->mParent->run_method("collision_end", PyTuple_Pack(1, otherObject->get_script_twin()));
}
