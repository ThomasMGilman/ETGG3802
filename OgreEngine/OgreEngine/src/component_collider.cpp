#include <stdafx.h>
#include <utility.h>
#include <game_object.h>
#include <collision_manager.h>
#include <component_collider.h>

using namespace OgreEngine;

ComponentCollider::ComponentCollider(GameObject* owner, int layer, long long mask, float radius) :
	mLayer(layer), mCollisionMask(mask), mColliderType(ColliderType::SPHERE), mData(Ogre::Vector3(radius, radius*radius, 0)), Component(owner)
{
	COLLISION_MANAGER->add_collider(this);
}

ComponentCollider::ComponentCollider(GameObject* owner, int layer, long long mask, Ogre::Vector3 extents):
	mLayer(layer), mCollisionMask(mask), mColliderType(ColliderType::SPHERE), mData(extents/2), Component(owner)
{
	COLLISION_MANAGER->add_collider(this);
}

ComponentCollider::~ComponentCollider()
{
	COLLISION_MANAGER->remove_collider(this);
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
		bool firstSet = true;
		for (Ogre::Vector3 p : points)
		{
			Ogre::Vector3 offset = p - origin;
			float result = offset.dotProduct(direction);
			if (firstSet)
			{
				extents = { result, result };
				firstSet = false;
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
		pos - mData[0] * xAxis + mData[1] * yAxis + mData[2] * zAxis,		// PtB pos Z dir
		pos - mData[0] * xAxis - mData[1] * yAxis + mData[2] * zAxis,		// PtC pos Z dir
		pos + mData[0] * xAxis - mData[1] * yAxis + mData[2] * zAxis,		// PtD pos Z dir
		pos + mData[0] * xAxis + mData[1] * yAxis + mData[2] * zAxis,		// PtA neg Z dir
		pos - mData[0] * xAxis + mData[1] * yAxis + mData[2] * zAxis,		// PtB neg Z dir
		pos - mData[0] * xAxis - mData[1] * yAxis + mData[2] * zAxis,		// PtC neg Z dir
		pos + mData[0] * xAxis - mData[1] * yAxis + mData[2] * zAxis		// PtD neg Z dir
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
}

void ComponentCollider::create_collider_visualization()
{
}

bool ComponentCollider::has_separation(const Ogre::Vector2& ext1, const Ogre::Vector2& ext2)
{
	return false;
}
