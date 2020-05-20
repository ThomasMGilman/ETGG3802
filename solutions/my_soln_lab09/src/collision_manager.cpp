#include <stdafx.h>
#include <collision_manager.h>
#include <component_collider.h>
#include <log_manager.h>
#include <game_object.h>
#include <game_object_manager.h>

// Necessary to initialize the static singleton LogManager inherited from Singleton
template<> ssuge::CollisionManager* ssuge::Singleton<ssuge::CollisionManager>::msSingleton = NULL;



ssuge::CollisionManager::CollisionManager()
{
	
}



ssuge::CollisionManager::~CollisionManager()
{

}



void ssuge::CollisionManager::add_collider(ComponentCollider* c)
{
	// Sets don't allow duplicates, so this is safe
	mColliders.insert(c);
}



void ssuge::CollisionManager::remove_collider(ComponentCollider* c)
{
	std::set<ComponentCollider*>::iterator it = mColliders.find(c);
	if (it != mColliders.end())
		mColliders.erase(it);
}



void ssuge::CollisionManager::update(float dt)
{
	std::set<std::pair<GameObject*, GameObject*>> collisions;
	std::set<ComponentCollider*>::iterator itA, itB;

	for (itA = mColliders.begin(); itA != mColliders.end(); itA++)
	{
		ColliderType typeA;
		typeA = (*itA)->get_collider_type();

		for (itB = mColliders.begin(); itB != mColliders.end(); itB++)
		{
			// We're looking for a collision where B collides with A.  There are a few reasons
			// we wouldn't want to even do the check:
			//  1. itA and itB are the same (we're check an object against itself)
			//  2. itA's mask and-ed with itB's layer is 0
			long long shifted_layer = (long long)1 << (*itB)->get_layer();
			if (*itA != *itB && ((*itA)->get_mask() & shifted_layer))
			{
				bool collides = false;
				ColliderType typeB;

				typeB = (*itB)->get_collider_type();

				if (typeA == ColliderType::RECTANGULAR_PRISM && typeB == ColliderType::RECTANGULAR_PRISM)
					collides = rprism_rprism_collide(*itA, *itB);
				else if (typeA == ColliderType::SPHERE && typeB == ColliderType::SPHERE)
					collides = sphere_sphere_collider(*itA, *itB);
				else if (typeA == ColliderType::RECTANGULAR_PRISM && typeB == ColliderType::SPHERE)
					collides = rprism_sphere_collider(*itA, *itB);
				else if (typeA == ColliderType::SPHERE && typeB == ColliderType::RECTANGULAR_PRISM)
					collides = rprism_sphere_collider(*itB, *itA);
				else
				{
					std::string obj_a_name = (*itA)->get_owner()->get_name();
					std::string obj_b_name = (*itB)->get_owner()->get_name();
					LOG_MANAGER->log("Unhandled collision between '" + obj_a_name + "' and '" + obj_b_name + "'", LogPreset::WARNING);
				}

				if (collides)
				{
					GameObject* objA = (*itA)->get_owner();
					GameObject* objB = (*itB)->get_owner();
					collisions.insert(std::pair<GameObject*, GameObject*>(objA, objB));
				}
			}
		}
	}

	// Notify every game object that was colliding
	std::set<std::pair<GameObject*, GameObject*>>::iterator it = collisions.begin();
	while (it != collisions.end())
	{
		PyObject* first = it->first->get_script_twin();
		PyObject* second = it->second->get_script_twin();
		if (first)
		{
			PyObject* args = PyTuple_New(1);
			if (second)
			{
				Py_INCREF(second);
				PyTuple_SetItem(args, 0, second);
			}
			else
			{
				Py_INCREF(Py_None);
				PyTuple_SetItem(args, 0, Py_None);
			}

			// See which collision type we have: collision_start, collison_continue
			std::string method;
			std::set<std::pair<GameObject*, GameObject*>>::iterator existing_it = mOngoingCollisions.find(*it);
			if (existing_it == mOngoingCollisions.end())
			{
				// This is the first time we've seen this collision
				method = "collision_start";
				mOngoingCollisions.insert(*it);
			}
			else
				method = "collision_continue";

			// Call the script method
			it->first->execute_script_method(method, args);

			Py_DECREF(args);
		}

		++it;
	}

	// Go through our ongoing collisions and remove any that aren't active any more, and send
	// that game object a "collision_end" method-call
	std::set<std::pair<GameObject*, GameObject*>>::iterator existing_it = mOngoingCollisions.begin();
	while (existing_it != mOngoingCollisions.end())
	{
		it = collisions.find(*existing_it);
		if (it == collisions.end())
		{
			// This is kind of a costly test, but there's a chance that when an object
			// hit something, it issued a "self-destruct" to destroy itself.  If this happens
			// either this object, or the object that it was hitting no longer exist, so we
			// should make sure both still exist before trying to notify them of a collision_end
			if (GAME_OBJECT_MANAGER->has_game_object(existing_it->first) && 
				GAME_OBJECT_MANAGER->has_game_object(existing_it->second))
			{
				// Notify the game object that it is no longer colliding 
			
				PyObject* args = PyTuple_New(1);
				PyObject* other = existing_it->second->get_script_twin();
				if (other)
				{
					Py_INCREF(other);
					PyTuple_SetItem(args, 0, other);
				}
				else
				{
					Py_INCREF(Py_None);
					PyTuple_SetItem(args, 0, Py_None);
				}

				existing_it->first->execute_script_method("collision_end", args);

				Py_DECREF(args);
			}

			existing_it = mOngoingCollisions.erase(existing_it);
		}
		else
			++existing_it;
	}
}



bool ssuge::CollisionManager::rprism_rprism_collide(ComponentCollider* rprismA, ComponentCollider* rprismB)
{
	Ogre::Vector3 a_axes[3], b_axes[3], a_origin, b_origin;
	rprismA->get_owner()->get_local_axes(a_axes[0], a_axes[1], a_axes[2]);
	rprismB->get_owner()->get_local_axes(b_axes[0], b_axes[1], b_axes[2]);
	a_origin = rprismA->get_owner()->get_world_position();
	b_origin = rprismB->get_owner()->get_world_position();

	for (int i = 0; i < 3; i++)
	{
		// Project both prisms onto one of a's axes.  If we have separation, we have no collision
		Ogre::Vector2 a_on_a = rprismA->get_projection_extents(a_origin, a_axes[i]);
		Ogre::Vector2 b_on_a = rprismB->get_projection_extents(a_origin, a_axes[i]);
		if (ComponentCollider::has_separation(a_on_a, b_on_a))
			return false;

		// Project both prisms onto one of b's axes.  If we have separation, we have no collision
		Ogre::Vector2 a_on_b = rprismA->get_projection_extents(a_origin, b_axes[i]);
		Ogre::Vector2 b_on_b = rprismB->get_projection_extents(a_origin, b_axes[i]);
		if (ComponentCollider::has_separation(a_on_b, b_on_b))
			return false;
	}

	// If we get here, the two objects must be colliding!
	return true;
}



bool ssuge::CollisionManager::sphere_sphere_collider(ComponentCollider* sphereA, ComponentCollider* sphereB)
{
	// Just calculate the (squared) distance between the two centers.  If that is less than
	// the radius of both spheres squared, we have a hit.
	Ogre::Vector3 offset = sphereA->get_owner()->get_world_position() - sphereB->get_owner()->get_world_position();
	float threshold = (sphereA->get_data().x + sphereB->get_data().x) * (sphereA->get_data().x + sphereB->get_data().x);
	return offset.dotProduct(offset) < threshold;
}



bool ssuge::CollisionManager::rprism_sphere_collider(ComponentCollider* rprism, ComponentCollider* sphere)
{
	Ogre::Vector3 prism_axes[3], prism_origin, sphere_origin;
	prism_origin = rprism->get_owner()->get_world_position();
	sphere_origin = sphere->get_owner()->get_world_position();
	rprism->get_owner()->get_local_axes(prism_axes[0], prism_axes[1], prism_axes[2]);
	
	// Project both objects onto each axis of the prism
	for (int i = 0; i < 3; i++)
	{
		Ogre::Vector2 s_on_r_axis = sphere->get_projection_extents(prism_origin, prism_axes[i]);
		Ogre::Vector2 r_on_r_axis = rprism->get_projection_extents(prism_origin, prism_axes[i]);
		if (ComponentCollider::has_separation(s_on_r_axis, r_on_r_axis))
			return false;
	}

	// See if there's separation on the axis between the sphere and prism
	Ogre::Vector3 s_axis = (prism_origin - sphere_origin).normalisedCopy();
	Ogre::Vector2 s_on_saxis = sphere->get_projection_extents(prism_origin, s_axis);
	Ogre::Vector2 r_on_saxis = rprism->get_projection_extents(prism_origin, s_axis);
	if (ComponentCollider::has_separation(s_on_saxis, r_on_saxis))
		return false;

	// If we get here, the two objects must be colliding!
	return true;
}

void ssuge::CollisionManager::rprism_ray_collide(ComponentCollider* rprism, const Ogre::Vector3& ray_origin, const Ogre::Vector3& ray_direction, std::vector<std::pair<Ogre::Vector3, GameObject*>>& hit_objects, float max_dist)
{
	Ogre::Vector3 axes[3];
	Ogre::Vector3 extents = rprism->get_data();
	Ogre::Vector3 pos = rprism->get_owner()->get_world_position();
	rprism->get_owner()->get_world_orientation().ToAxes(axes);
	Ogre::Vector3 plane_normals[6];
	float plane_d[6];
	for (int i = 0; i < 3; i++)
	{
		plane_normals[i << 1] = axes[i];
		plane_d[i << 1] = axes[i].dotProduct(pos + axes[i] * extents[i]);
		plane_normals[(i << 1) + 1] = -axes[i];
		plane_d[(i << 1) + 1] = (-axes[i]).dotProduct(pos - axes[i] * extents[i]);
	}
	
	for (int i = 0; i < 6; i++)
	{
		Ogre::Vector3 cur_plane_normal = plane_normals[i];
		float cur_plane_d = plane_d[i];

		// Do a ray plane intersection to see if we hit that plane
		float denominator = cur_plane_normal.dotProduct(ray_direction);
		if (denominator == 0)
		{
			// Plane is parallel to the ray direction -- no hits
			continue;
		}
		else
		{
			// Get the point where we hit the plane
			float t_value = (cur_plane_d - cur_plane_normal.dotProduct(ray_origin)) / denominator;
			if (t_value >= 0 && (max_dist < 0 || t_value < max_dist))
			{
				Ogre::Vector3 hit_point = ray_origin + t_value * ray_direction;

				// We might have a hit.  Check to see if it's in-bounds.
				// The idea (in 2d) is we check against the opposing extents (so if this
				// is a plane perpendicular to the x-axis, we check the y-extents).  If 3d,
				// this will be like this: for a plane perpendicular to the x-axis we check for the
				// point being withing the y and z boundaries.  For a y-axis plane, we check against the x-z bounds, and so on.

				int axis_to_ignore = i / 2;
				bool in_bounds = true;
				float epsilon = 0.0001f;           // A small "fudge-factor" for float - roundoff
				for (int j = 0; j < 3; j++)
				{
					if (j == axis_to_ignore)
						continue;
					else
					{
						// Project that point onto that axis
						float proj = axes[j].dotProduct(hit_point - pos);
						if (fabs(proj) > extents[j])
						{
							// It hit the plane but is outside the bounds.  No hit!
							in_bounds = false;
							break;
						}
					}
				}
				if (in_bounds)
					hit_objects.push_back(std::make_pair(hit_point, rprism->get_owner()));
			}
		}
	}
}

void ssuge::CollisionManager::sphere_ray_collide(ComponentCollider* sphere, const Ogre::Vector3& ray_origin, const Ogre::Vector3& ray_direction, std::vector<std::pair<Ogre::Vector3, GameObject*>>& hit_objects, float max_dist)
{
	Ogre::Vector3 pos = sphere->get_owner()->get_world_position();
	Ogre::Vector3 offset = pos - ray_origin;
	float offset_mag_sq = offset.dotProduct(offset);
	std::vector<Ogre::Vector3> hits;

	// If this ray originates outside the circle(first clause) and is facing away from the sphere(second clause),
	// there can't be a hit -- no need to go farther
	if (offset_mag_sq > sphere->get_data().y && offset.dotProduct(ray_direction) < 0)
	{
		return;
	}

	float para_dist = offset.dotProduct(ray_direction);
	float para_dist_sq = para_dist * para_dist;
	float perp_dist_sq = offset_mag_sq - para_dist_sq;
	float radius_sq = sphere->get_data().y;
	if (perp_dist_sq <= radius_sq)
	{
		// The ray goes through the sphere.Start to calculate the intersection points
		Ogre::Vector3 closest = ray_origin + para_dist * ray_direction;
		if (perp_dist_sq == radius_sq)
		{
			// Incredibly rare, but oh well.Just one intersection at the tip of the sphere
			hits.push_back(closest);
		}
		else
		{
			// A non-glancing hit
			float closest_offset = sqrtf(radius_sq - perp_dist_sq);
			if (offset_mag_sq < radius_sq)
			{
				// The ray originates inside the sphere.There will just be one collision
				hits.push_back(closest + closest_offset * ray_direction);
			}
			else
			{
				// More common case: the ray originates outside, there're two collisions
				hits.push_back(closest + closest_offset * ray_direction);
				hits.push_back(closest - closest_offset * ray_direction);
			}
		}
	}

	// If we have a maximum hit distance, cull those that are beyond it
	if (max_dist >= 0)
	{
		for (int i = (int)hits.size() - 1; i >= 0; i--)
		{
			float dist = (hits[i] - ray_origin).length();
			if (dist > max_dist)
				hits.erase(hits.begin() + i);
		}
	}

	// Add any remaining hits to the hits list
	for (int i = 0; i < hits.size(); i++)
		hit_objects.push_back(std::make_pair(hits[i], sphere->get_owner()));
}



void ssuge::CollisionManager::load_visualizations()
{
	std::set<ComponentCollider*>::iterator it = mColliders.begin();
	while (it != mColliders.end())
	{
		(*it)->create_collider_visualization();
		++it;
	}
}


void ssuge::CollisionManager::set_debug_visibility(bool show)
{
	std::set<ComponentCollider*>::iterator it = mColliders.begin();
	while (it != mColliders.end())
	{
		(*it)->set_visible(false);
		++it;
	}
}


bool ssuge::CollisionManager::validate_mask_and_layer(long long mask, int layer)
{
	return mask >= 0 && mask <= 0xFFFFFFFF && layer >= 0 && layer < 32;
}


bool ssuge::CollisionManager::raycast_unbounded(const Ogre::Vector3& origin, const Ogre::Vector3& direction, std::vector<std::pair<Ogre::Vector3, GameObject*>>& hits, long long mask)
{
	return raycast_bounded(origin, direction, hits, -1.0f, mask);
}



bool ssuge::CollisionManager::raycast_bounded(const Ogre::Vector3& origin, const Ogre::Vector3& direction, std::vector<std::pair<Ogre::Vector3, GameObject*>>& hits, float max_dist, long long mask)
{
	hits.clear();
	Ogre::Vector3 norm_direction = direction.normalisedCopy();

	std::set<ComponentCollider*>::iterator it = mColliders.begin();
	while (it != mColliders.end())
	{
		long long shifted_layer = (long long)1 << (*it)->get_layer();
		if (mask & shifted_layer)
		{
			if ((*it)->get_collider_type() == ColliderType::SPHERE)
				sphere_ray_collide(*it, origin, norm_direction, hits, max_dist);
			else if ((*it)->get_collider_type() == ColliderType::RECTANGULAR_PRISM)
				rprism_ray_collide(*it, origin, norm_direction, hits, max_dist);
		}

		++it;
	}

	return hits.size() > 0;
}