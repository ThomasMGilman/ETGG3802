#include <stdafx.h>
#include <collision_manager.h>
#include <game_object.h>
#include <component_collider.h>
#include <utility.h>

using namespace OgreEngine;

CollisionManager* CollisionManager::msSingleton = nullptr;

CollisionManager::CollisionManager()
{
}

CollisionManager::~CollisionManager()
{
}

void CollisionManager::add_collider(ComponentCollider* c)
{
	mColliders.insert(c);
}

void CollisionManager::remove_collider(ComponentCollider* c)
{
	if (mColliders.count(c))
		mColliders.erase(c);
}

void CollisionManager::update(float dt)
{
	// Pairs of objects already checked
	std::set<std::pair<GameObject*, GameObject*>> checked = {};
	std::set<ComponentCollider*>::iterator firstCollider_iter, secondCollider_iter;// = mColliders.begin();
	
	for (firstCollider_iter = mColliders.begin(); firstCollider_iter != mColliders.end(); firstCollider_iter++)
	{
		for (secondCollider_iter = mColliders.begin(); secondCollider_iter != mColliders.end(); secondCollider_iter++)
		{
			std::pair<GameObject*, GameObject*> curPair = { (*firstCollider_iter)->mParent, (*secondCollider_iter)->mParent };
			std::pair<GameObject*, GameObject*> rev_curPair = { (*secondCollider_iter)->mParent, (*firstCollider_iter)->mParent };
			// Check to make sure not same object, as well as the two objects looking at are not already previously checked
			if (firstCollider_iter != secondCollider_iter && (checked.count(curPair) == 0 && checked.count(rev_curPair) == 0))
			{
				checked.insert(curPair);
				// Make sure either object is able to collide with the other
				bool second_collides_with_first = validate_mask_and_layer((*firstCollider_iter)->get_mask(), (*secondCollider_iter)->get_layer());
				bool first_collides_with_second = validate_mask_and_layer((*secondCollider_iter)->get_mask(), (*firstCollider_iter)->get_layer());
				if (second_collides_with_first || first_collides_with_second)
				{
					// Check if colliding, if they do add to ongoing collisions and notify the object that is able to collide with the other.
					// Otherwise check if in ongoing collisions and remove it and tell the collider, collision has ended.
					if (collision_picker(*firstCollider_iter, *secondCollider_iter))
					{
						// Continueing Collision
						if (mOngoingCollisions.count(curPair) || mOngoingCollisions.count(rev_curPair))
						{
							if (second_collides_with_first)
								(*secondCollider_iter)->continue_collision((*firstCollider_iter)->mParent);
							if (first_collides_with_second)
								(*firstCollider_iter)->continue_collision((*secondCollider_iter)->mParent);
						}
						// Starting Collision between objects
						else
						{
							mOngoingCollisions.insert(curPair);
							if (second_collides_with_first)
								(*secondCollider_iter)->start_collision((*firstCollider_iter)->mParent);
							if (first_collides_with_second)
								(*firstCollider_iter)->start_collision((*secondCollider_iter)->mParent);
						}
					}
					// Check if collision has ended if objects were colliding
					else
					{
						bool endedCollision = false;
						if (mOngoingCollisions.count(curPair))
						{
							mOngoingCollisions.erase(curPair);
							endedCollision = true;
						}
						else if (mOngoingCollisions.count(rev_curPair))
						{
							mOngoingCollisions.erase(rev_curPair);
							endedCollision = true;
						}
						if (endedCollision)
						{
							if (second_collides_with_first)
								(*secondCollider_iter)->end_collision((*firstCollider_iter)->mParent);
							if (first_collides_with_second)
								(*firstCollider_iter)->end_collision((*secondCollider_iter)->mParent);
						}
					}
				}
			}
		}
	}
}

void CollisionManager::load_visualizations()
{
	for (ComponentCollider* collider : mColliders)
		collider->set_visible(true);
}

void CollisionManager::set_debug_visibility(bool show)
{
	this->is_visible = show;
	for (ComponentCollider* collider : mColliders)
		collider->set_visible(show);
}

bool CollisionManager::validate_mask_and_layer(long long mask, int layer)
{
	return mask & ((long long)1 << layer);
}

bool OgreEngine::CollisionManager::has_separation(Ogre::Vector2 extA, Ogre::Vector2 extB)
{
	return (extA[0] > extB[1] or extB[0] > extA[1]);
}

bool CollisionManager::raycast_unbounded(const Ogre::Vector3& origin, const Ogre::Vector3& direction, std::vector<std::pair<Ogre::Vector3, GameObject*>>& hits, long long mask)
{
	return this->raycast_bounded(origin, direction, hits, -1.0f, mask);
}

bool CollisionManager::raycast_bounded(const Ogre::Vector3& origin, const Ogre::Vector3& direction, std::vector<std::pair<Ogre::Vector3, GameObject*>>& hits, float max_dist, long long mask)
{
	hits = {};
	for (ComponentCollider* c : this->mColliders)
	{
		// Check to see if collision is possible between object casting ray (mask) and other component (other objects layer)
		if (validate_mask_and_layer(mask, c->get_layer()))
		{
			switch (c->get_collider_type())
			{
			case ColliderType::SPHERE:
				sphere_ray_collide(c, origin, direction, hits, max_dist);
				break;
			case ColliderType::RECTANGULAR_PRISM:
				rprism_ray_collide(c, origin, direction, hits, max_dist);
				break;
			default:
				break;
			}
		}
	}
	return (hits.size() > 0);
}

bool OgreEngine::CollisionManager::collision_picker(ComponentCollider* firstCollider, ComponentCollider* secondCollider)
{
	switch (firstCollider->get_collider_type())
	{
	case ColliderType::SPHERE:
		switch (secondCollider->get_collider_type())
		{
		case ColliderType::SPHERE:
			return this->sphere_sphere_collider(firstCollider, secondCollider);
		case ColliderType::RECTANGULAR_PRISM:
			return this->rprism_sphere_collider(secondCollider, firstCollider);
		default:
			return false;
		}
	case ColliderType::RECTANGULAR_PRISM:
		switch (secondCollider->get_collider_type())
		{
		case ColliderType::SPHERE:
			return this->rprism_sphere_collider(firstCollider, secondCollider);
		case ColliderType::RECTANGULAR_PRISM:
			return this->rprism_rprism_collide(firstCollider, secondCollider);
		default:
			return false;
		}
	default:
		return false;
	}
}

bool CollisionManager::rprism_rprism_collide(ComponentCollider* rprismA, ComponentCollider* rprismB)
{
	std::vector<Ogre::Vector3> bAxis = rprismB->get_axis();
	std::vector<Ogre::Vector3> aAxis = rprismA->get_axis();
	Ogre::Vector3 aPos = rprismA->get_position();
	// Project both prisms on B's axes, as well as project both prisms on A's axes.
	for (int i = 0; i < 3; i++)
	{
		Ogre::Vector2 A_on_Aaxis_ext = rprismA->get_projection_extents(aPos, aAxis[i]);
		Ogre::Vector2 B_on_Aaxis_ext = rprismB->get_projection_extents(aPos, aAxis[i]);
		if (this->has_separation(A_on_Aaxis_ext, B_on_Aaxis_ext))
			return false;

		Ogre::Vector2 A_on_Baxis_ext = rprismA->get_projection_extents(aPos, bAxis[i]);
		Ogre::Vector2 B_on_Baxis_ext = rprismB->get_projection_extents(aPos, bAxis[i]);
		if (this->has_separation(A_on_Baxis_ext, B_on_Baxis_ext))
			return false;
	}
	//Colliding
	return true;
}

bool CollisionManager::sphere_sphere_collider(ComponentCollider* sphereA, ComponentCollider* sphereB)
{
	Ogre::Vector3 offset = sphereA->get_position() - sphereB->get_position();
	return (offset.dotProduct(offset) < pow(sphereA->get_data().x + sphereB->get_data().x, 2));
}

bool CollisionManager::rprism_sphere_collider(ComponentCollider* rprism, ComponentCollider* sphere)
{
	std::vector<Ogre::Vector3> prismAxis = rprism->get_axis();
	Ogre::Vector3 prismPos = rprism->get_position();
	// Check for seperation on prism axis.
	for (int i = 0; i < 3; i++)
	{
		Ogre::Vector2 S_on_Prism_ext = sphere->get_projection_extents(prismPos, prismAxis[i]);
		Ogre::Vector2 Prism_on_Prism_ext = rprism->get_projection_extents(prismPos, prismAxis[i]);
		if (this->has_separation(S_on_Prism_ext, Prism_on_Prism_ext))
			return false;
	}

	// project both objects onto vector from sphere center to prism.
	Ogre::Vector3 dir = (prismPos - sphere->get_position()).normalisedCopy();
	Ogre::Vector2 S_on_offset = sphere->get_projection_extents(prismPos, dir);
	Ogre::Vector2 Prism_on_offset = rprism->get_projection_extents(prismPos, dir);
	if(this->has_separation(S_on_offset, Prism_on_offset))
		return false;

	// No seperation. Objects are colliding.
	return true;
}

void CollisionManager::rprism_ray_collide(ComponentCollider* rprism, const Ogre::Vector3& ray_origin, const Ogre::Vector3& ray_direction, std::vector<std::pair<Ogre::Vector3, GameObject*>>& hits, float max_dist)
{
	std::vector<std::pair<Ogre::Vector3, float>> planes = rprism->get_rectangular_prism_planes();
	std::vector<Ogre::Vector3> axes; 
	axes.resize(3);
	rprism->mParent->get_world_orientation().ToAxes(axes[0], axes[1], axes[2]);

	for (int i = 0; i < planes.size(); i++)
	{
		std::pair<Ogre::Vector3, float> plane = planes[i];
		float denominator = plane.first.dotProduct(ray_direction);

		// Check if plane is parallel to ray_direction
		if (denominator == 0)
			continue;

		// Get point where we hit the plane
		float dist = (plane.second - plane.first.dotProduct(ray_origin)) / denominator;
		if (dist >= 0 && (max_dist < 0 || dist < max_dist))
		{
			Ogre::Vector3 hit_point = ray_origin + dist * ray_direction;
			bool in_bounds = true;
			int axis_to_ingore = i / 2;
			for (int j = 0; j < 3; j++)
			{
				if (j == axis_to_ingore)
					continue;

				float projection = axes[j].dotProduct(hit_point - rprism->mParent->get_world_position());
				if (fabs(projection) > rprism->get_data()[j])
				{
					in_bounds = false;
					break;
				}
			}

			if (in_bounds)
				hits.push_back(std::pair(hit_point, rprism->mParent));
		}
	}
}

void CollisionManager::sphere_ray_collide(ComponentCollider* sphere, const Ogre::Vector3& ray_origin, const Ogre::Vector3& ray_direction, std::vector<std::pair<Ogre::Vector3, GameObject*>>& hits, float max_dist)
{
	Ogre::Vector3 offset = sphere->mParent->get_world_position() - ray_origin;
	float offset_mag_sq = offset.dotProduct(offset);
	float parallel_dist = offset.dotProduct(ray_direction);

	// check if ray originates outside sphere and is facing away. If so return.
	if (offset_mag_sq > sphere->get_data().y && parallel_dist < 0)
		return;

	float parallel_dist_sqrd = parallel_dist * parallel_dist;
	float perpandicular_dist_sqrd = offset_mag_sq - parallel_dist_sqrd;

	// check if ray intersects sphere.
	if (perpandicular_dist_sqrd <= sphere->get_data().y)
	{
		Ogre::Vector3 closest_point = ray_origin + parallel_dist * ray_direction;

		// Intersection at tip of sphere
		if (perpandicular_dist_sqrd == sphere->get_data().y)
		{
			float dist = magnitude(closest_point - ray_origin);
			if (max_dist < 0 || dist < max_dist)
				hits.push_back(std::pair(closest_point, sphere->mParent));
		}
		else
		{
			float closest_offset = sqrt(sphere->get_data().y - perpandicular_dist_sqrd);

			// Ray originates in sphere
			if (offset_mag_sq < sphere->get_data().y)
			{
				Ogre::Vector3 hit_point = closest_point + closest_offset * ray_direction;
				float dist = magnitude(hit_point - ray_origin);
				if (max_dist < 0 || dist < max_dist)
					hits.push_back(std::pair(hit_point, sphere->mParent));
			}

			// Ray originates outside of sphere
			else
			{
				std::vector<Ogre::Vector3> hit_points = 
					{ closest_point - closest_offset * ray_direction, closest_point + closest_offset * ray_direction };
				for (Ogre::Vector3 p : hit_points)
				{
					float dist = magnitude(p - ray_origin);
					if (max_dist >= 0 and dist > max_dist)
						continue;
					hits.push_back(std::pair(p, sphere->mParent));
				}
			}
		}
	}
}