import vector
import pygame
import math


# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# @ BASE-COLLIDER class              @
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
class BaseCollider:
    """ The Base of all other collider types.  Stores attributes common to all"""
    selected_color = (64, 64, 64)
    colliding_color = (255, 255, 255)

    def __init__(self, position, color):
        """
        position: a VectorN of the *center* of this object
        color: the color to draw it in pygame
        """
        self.mPosition = position.copy()                                # The center of the object
        self.mColor = color                                             # The color to draw it in pygame
        self.mAxes = [vector.Vector2(1, 0), vector.Vector2(0, 1)]       # A set of 2 (in 2d) or 3 (in 3d)
                                                                        #   unit-length axes.  If the object
                                                                        #   is rotated, this is what changes

    def rotate(self, degrees):
        """
        in 2d, this rotates around the z-axis.
        """
        radians = math.radians(degrees)
        c_rad = math.cos(radians)
        s_rad = math.sin(radians)
        cur_x = self.mAxes[0]
        cur_y = self.mAxes[1]
        self.mAxes[0] = vector.Vector2(cur_x.x * c_rad + cur_x.y * s_rad, -cur_x.x * s_rad + cur_x.y * c_rad)
        self.mAxes[1] = vector.Vector2(cur_y.x * c_rad + cur_y.y * s_rad, -cur_y.x * s_rad + cur_y.y * c_rad)

    def move(self, xoffset, yoffset):
        """
        translates this object
        """
        self.mPosition += vector.Vector2(xoffset, yoffset)

    def draw(self, surf, draw_highlighted, draw_colliding):
        """
        draws this object in pygame.  For the base class, this is just the origin and axes.  The derived
        classes will normally augment this with additional draw routines.
        """
        pygame.draw.line(surf, (255, 0, 0), self.mPosition.i, (self.mPosition + 10 * self.mAxes[0]).i)
        pygame.draw.line(surf, (0, 255, 0), self.mPosition.i, (self.mPosition + 10 * self.mAxes[1]).i)

    def get_projection(self, axis, reference_pt):
        """
        The heart of our hit-detection routine.  The axis must be unit-length and the reference point
        is one point to "anchor" the axis (together, they essentially make a ray).  The job of this
        function is to produce 2 values (a small and then a possibly bigger value) for the position of the "shadow"
        of a give object along the ray.  For example:

                           /------\
                     /----+      /
                     |          /
                     +-----     \
                           \     \
                           \----/


           *---------==============--------------------
                    3.2           4.7

           The object this is called through is the blobby thing at top.   The axis is at the bottom with the
           reference point indicated by *.  The ='s show the projection, and the (3.2, 4.7) are the distances
           along the ray to the projection shadow.  The function would return (3.2, 4.7).  Important, the
           ray in generaly will not be axis-aligned like this (it's just hard to draw a slanted line in ASCII)

            The derived classes will provide their own implementation of this
        """
        raise NotImplementedError("must be implemented in derived class")

    def point_inside(self, pt):
        """
        Returns true if the given point is inside the object, False if not.  The derived class will implement this.
        """
        raise NotImplementedError("must be implemented in derived class")

    def ray_collision(self, ray_origin, ray_direction, max_dist = -1):
        """ returns a list of pairs (object, point) where the ray hits this object, or an empty list if no hits.
        max_dist is negative if we want to consider the ray as infinite.  If it is a positive number, hits farther away
        than that distance are not counted"""
        raise NotImplementedError("must be implemented in derived class")

    @staticmethod
    def has_separation(extA, extB):
        """
        This is another key part of the hit-detection logic.  If I have two objects projected to the same
        axis (and using the same reference point), using our get_projection method, we'll have two pairs of
        numbers.  If there is a separation of the two extents, we have no collision.  If not, we do (at least
        along this axis -- there could be other axes that do have a separation)

        *---------------------------------------------------------------
                +=======+
              (3.2  -A-  4.7)            +================+
                                        (6.5   -B-     9.2)      returns True (clause #2 below)



                      +======+
                     (4.0 -B- 5.4)     +=========+
                                      (6.3 -A-  7.5)             returns True (clause #1)



               +==============+
               (3.2  -A-     5.0)
                           +===============+
                           (4.5  -B-     7.0)                    returns False (both clauses fail)
        """
        return extA[0] > extB[1] or extB[0] > extA[1]


# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# @ SPHERE-COLLIDER class            @
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
class SphereCollider(BaseCollider):
    def __init__(self, radius, position, color):
        super().__init__(position, color)
        self.mRadius = radius
        self.mRadiusSquared = self.mRadius ** 2

    def draw(self, surf, draw_highlighted, draw_colliding):
        if draw_highlighted:
            pygame.draw.circle(surf, BaseCollider.selected_color, self.mPosition.i, self.mRadius)
        if draw_colliding:
            color = BaseCollider.colliding_color
        else:
            color = self.mColor
        pygame.draw.circle(surf, color, self.mPosition.i, self.mRadius, 2)

    def get_projection(self, axis, reference_pt):
        offset = self.mPosition - reference_pt
        proj = vector.dot(offset, axis)
        return [proj - self.mRadius, proj + self.mRadius]

    def point_inside(self, pt):
        offset = pt - self.mPosition
        return offset.magnitudeSquared() <= self.mRadiusSquared

    def ray_collision(self, ray_origin, ray_direction, max_dist = -1):
        """ returns a list of pairs (object, point) where the ray hits this object, or an empty list if no hits"""
        offset = self.mPosition - ray_origin
        offset_mag_sq = vector.dot(offset, offset)

        # If this ray originates outside the circle (first clause) and is facing away from the sphere (second clause),
        # there can't be a hit -- no need to go farther
        if offset_mag_sq > self.mRadiusSquared and vector.dot(offset, ray_direction) < 0:
            return []

        para_dist = vector.dot(offset, ray_direction)
        para_dist_sq = para_dist ** 2
        perp_dist_sq = offset_mag_sq - para_dist_sq
        if perp_dist_sq <= self.mRadiusSquared:
            # The ray goes through the sphere.  Start to calculate the intersection points
            closest = ray_origin + para_dist * ray_direction
            if perp_dist_sq == self.mRadiusSquared:
                # Incredibly rare, but oh well.  Just one intersection at the tip of the sphere
                dist = (closest - ray_origin).magnitude()
                if max_dist < 0 or dist < max_dist:
                    return [closest]
                else:
                    return []
            else:
                closest_offset = (self.mRadiusSquared - perp_dist_sq) ** 0.5
                if offset_mag_sq < self.mRadiusSquared:
                    # The ray originates inside the sphere.  There will just be one collision
                    hit_pt = closest + closest_offset * ray_direction
                    dist = (hit_pt - ray_origin).magnitude()
                    if max_dist < 0 or dist < max_dist:
                        return [hit_pt]
                    else:
                        return []
                else:
                    # More common case: the ray originates outside, there're two collisions
                    hit_points = [closest - closest_offset * ray_direction, closest + closest_offset * ray_direction]
                    for i in range(len(hit_points) - 1, -1, -1):
                        dist = (hit_points[i] - ray_origin).magnitude()
                        if max_dist >= 0 and dist > max_dist:
                            del hit_points[i]
                    return hit_points

        # If we get here, no hits!
        return []



# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# @ BOX-COLLIDER class              @
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
class BoxCollider(BaseCollider):
    def __init__(self, extents, position, color):
        super().__init__(position, color)
        self.mExtents = extents.copy()                        # The "half-extents" in reality.  This is a
                                                              # Vector2 in 2d, Vector3 in 3d.  The first
                                                              # element indicates how for we move from the origin
                                                              # in the +x direction to get to one side.  We would
                                                              # also move that distance in the -x direction to get
                                                              # to the opposing side.

    def get_points(self):
        """
        Gets a collection of points that bound the object.     (ptB)----------------------(ptA)
                                                                 |                          |
                                                                 |           +y             |
                                                                 |           |              |
                                                                 |           *-- +x         |
                                                                 |                          |
                                                                 |                          |
                                                              (ptC)-----------------------(ptD)

        If this were in 3d, we'd need a collection of 8 points (the first 4 have +extents[2] * mAxes[2] added to them.
        The next 4 have -extents[2] * mAxes[2] added to them.
        """
        ptA = self.mPosition + self.mExtents[0] * self.mAxes[0] + self.mExtents[1] * self.mAxes[1]
        ptB = self.mPosition - self.mExtents[0] * self.mAxes[0] + self.mExtents[1] * self.mAxes[1]
        ptC = self.mPosition - self.mExtents[0] * self.mAxes[0] - self.mExtents[1] * self.mAxes[1]
        ptD = self.mPosition + self.mExtents[0] * self.mAxes[0] - self.mExtents[1] * self.mAxes[1]
        return (ptA, ptB, ptC, ptD)

    def draw(self, surf, draw_highlighted, draw_colliding):
        points = self.get_points()
        if draw_highlighted:
            pygame.draw.polygon(surf, BaseCollider.selected_color, points)
        super().draw(surf, draw_highlighted, draw_colliding)
        if draw_colliding:
            color = BaseCollider.colliding_color
        else:
            color = self.mColor
        pygame.draw.polygon(surf, color, points, 2)

    def get_projection(self, axis, reference_pt):
        # assumption: axis is normalized
        points = self.get_points()
        extents = [None, None]
        for p in points:
            offset = p - reference_pt
            result = vector.dot(offset, axis)
            if extents[0] == None:
                extents[0] = extents[1] = result
            elif result < extents[0]:
                extents[0] = result
            elif result > extents[1]:
                extents[1] = result
        return extents



    def point_inside(self, pt):
        offset = pt - self.mPosition
        for i in range(2):
            proj = abs(vector.dot(offset, self.mAxes[i]))
            if proj > self.mExtents[i]:
                return False
        return True

    def ray_collision(self, ray_origin, ray_direction, max_dist = -1):
        planes = [(self.mAxes[0], vector.dot(self.mPosition + self.mAxes[0] * self.mExtents[0], self.mAxes[0])),
                  (-self.mAxes[0], vector.dot(self.mPosition - self.mAxes[0] * self.mExtents[0], -self.mAxes[0])),
                  (self.mAxes[1], vector.dot(self.mPosition + self.mAxes[1] * self.mExtents[1], self.mAxes[1])),
                  (-self.mAxes[1], vector.dot(self.mPosition - self.mAxes[1] * self.mExtents[1], -self.mAxes[1]))
                  ]

        results = []
        for i in range(len(planes)):
            cur_plane_normal, cur_plane_d = planes[i]
            # Do a ray plane intersection to see if we hit that plane
            denominator = vector.dot(cur_plane_normal, ray_direction)
            if denominator == 0:
                # Plane is parallel to the ray direction -- no hits
                continue
            else:
                # Get the point where we hit the plane
                t_value = (cur_plane_d - vector.dot(cur_plane_normal, ray_origin)) / denominator
                if t_value >= 0 and (max_dist < 0 or t_value < max_dist):
                    hit_point = ray_origin + t_value * ray_direction

                    # We might have a hit.  Check to see if it's in-bounds.
                    # The idea (in 2d) is we check against the opposing extents (so if this
                    # is a plane perpendicular to the x-axis, we check the y-extents).  If 3d,
                    # this will be like this: for a plane perpendicular to the x-axis we check for the
                    # point being withing the y and z boundaries.  For a y-axis plane, we check against the x-z bounds, and so on.

                    axis_to_ignore = i // 2    # Integer division
                    in_bounds = True
                    epsilon = 0.0001           # A small "fudge-factor" for float-roundoff
                    for j in range(2):          # 3 for 3d
                        if j == axis_to_ignore:
                            continue
                        else:
                            # Project that point onto that axis
                            proj = vector.dot(hit_point - self.mPosition, self.mAxes[j])
                            if abs(proj) > self.mExtents[j]:
                                # We hit the plane, but it's out of bounds -- not a hit.
                                in_bounds = False
                    if in_bounds:
                        results.append(hit_point)

        return results







# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# @ COLLISION functions              @
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
def box_box_collision(boxA, boxB):
    # Project both boxes on B's axes.  It actually doesn't matter what
    # we choose as our reference point (A or B's position) -- just be consistent
    for i in range(2):
        A_on_Baxis_ext = boxA.get_projection(boxB.mAxes[i], boxA.mPosition)
        B_on_Baxis_ext = boxB.get_projection(boxB.mAxes[i], boxA.mPosition)
        if BaseCollider.has_separation(A_on_Baxis_ext, B_on_Baxis_ext):
            return False

    # If we get here, we couldn't find a separation.  Also look for a separation
    # on A's axes
    for i in range(2):
        A_on_Aaxis_ext = boxA.get_projection(boxA.mAxes[i], boxA.mPosition)
        B_on_Aaxis_ext = boxB.get_projection(boxA.mAxes[i], boxA.mPosition)
        if BaseCollider.has_separation(A_on_Aaxis_ext, B_on_Aaxis_ext):
            return False

    # If we get here, we couldn't find a separation -- we're colliding!
    return True


def sphere_sphere_collision(sphereA, sphereB):
    offset = sphereA.mPosition - sphereB.mPosition
    return offset.magnitudeSquared() < (sphereA.mRadius + sphereB.mRadius) ** 2


def box_sphere_collision(box, sphere):
    # First, project the sphere and box onto the box's axes
    for i in range(2):
        S_on_Box_ext = sphere.get_projection(box.mAxes[i], box.mPosition)
        Box_on_Box_ext = box.get_projection(box.mAxes[i], box.mPosition)
        if BaseCollider.has_separation(S_on_Box_ext, Box_on_Box_ext):
            return False

    # Create a vector from the sphere center to the box and project both objects onto
    # that and do another similar test
    offset = (box.mPosition - sphere.mPosition).normalized()
    S_on_offset = sphere.get_projection(offset, box.mPosition)
    Box_on_offset = box.get_projection(offset, box.mPosition)
    if BaseCollider.has_separation(S_on_offset, Box_on_offset):
        return False

    # We couldn't find a separating axis -- we're colliding!
    return True


def collide(shapeA, shapeB):
    """ This is the main collision routine.  It looks at the type of object
        and calls the appropriate helper function above.  Note: this is a down-side
        of this approach: as the number of primitives increases, the number of
        combinations dramatically increases"""
    if isinstance(shapeA, BoxCollider) and isinstance(shapeB, BoxCollider):
        return box_box_collision(shapeA, shapeB)
    elif isinstance(shapeA, SphereCollider) and isinstance(shapeB, SphereCollider):
        return sphere_sphere_collision(shapeA, shapeB)
    elif isinstance(shapeA, BoxCollider) and isinstance(shapeB, SphereCollider):
        return box_sphere_collision(shapeA, shapeB)
    elif isinstance(shapeA, SphereCollider) and isinstance(shapeB, BoxCollider):
        return box_sphere_collision(shapeB, shapeA)
    else:
        raise NotImplementedError("can't collide those two objects!")


