import math

class VectorN(object):
    """ This class represents a general-purpose vector class.  We'll
        add more to this in later labs.  For now, it represents a
        position and/or offset in n-dimensonal space. """

    def __init__(self, *args):
        """
        The constructor
        :param args: This is a variable-length argument-list.  In reality, you create a VectorN like this:
               v = VectorN(1, 2, 3)
        :return: N/A for constructors
        """
        self.mData = []
        self.mDim = len(args)
        for value in args:
            self.mData.append(float(value))

        # This might be poor OOP (from a Java programmer's perspective, for example), but it does make
        # the code a little cleaner.  I'm changing the type to a "specialized" version if we have a sub-class,
        # since the sub-class will always be more "powerful" than the vanilla VectorN class
        if self.mDim == 2:
            self.__class__ = Vector2
        if self.mDim == 3:
            self.__class__ = Vector3

    def __str__(self):
        """
        Note: You don't normally call this directly.  It is called indirectly when you do something like:
            v = VectorN(1, 2, 3)
            x = str(v)               # Same as x = v.__str__()
            print(v)                 # print calls str internally
        :return: The string-representation of this VectorN
        """
        s = "<Vector" + str(len(self)) + ": "
        for i in range(self.mDim):
            s += str(self.mData[i])
            if i < self.mDim - 1:
                s += ", "
        s += ">"
        return s

    def __len__(self):
        """
        Note: You don't normally call this method directly.  It's called by the built-in len function
            v = VectorN(1, 2, 4)
            print(len(v))           # 3
        :return: An integer indicating the dimension of this vector
        """
        return self.mDim

    def __getitem__(self, index):
        """
        Note: You don't normally call this method directly.  It's called by using [] on a VectorN
            v = VectorN(1, 2, 3)
            print(v[0])                 # 1
            print(v[-1])                # 3
        :param index: An integer.  A python exception will be thrown if it's not a valid position in self.mData
        :return: The float value at position index.
        """
        return self.mData[index]

    def __setitem__(self, index, newval):
        """
        This method is similar to __getitem__, but it is called when we assign something to an index
           v = VectorN(1, 2, 3)
           v[0] = 99
           print(v)                 # <Vector3: 1.0, 2.0, 99.0>
        :param index: An integer.  A python exception will be thrown if it's not a valid position in self.mData
        :param newval: A value that can be converted to a float using the float function
        :return: None
        """
        self.mData[index] = float(newval)

    def __eq__(self, other):
        """
        Note: This method isn't normally called directly.  Instead, it is called indirectly when a VectorN
              is on the left-hand side of an ==.  It returns True if the values within the other vector
              are the same as those in this vector.
        :param other: any value
        :return: a boolean.  True if the other thing is a VectorN *and* has the same values as this VectorN.
        """
        if isinstance(other, VectorN) and len(self) == len(other):
            for i in range(self.mDim):
                if self[i] != other[i]:
                    return False
            return True
        else:
            return False


    def __add__(self, otherV):
        """
        Adds this vector to the other vector (on the right-hand side of the '+' operator), producing a new vector
        :param otherV: A VectorN of the same size as this vectorN
        :return: The Vector sum (of the same type as the left-hand side of the '+' operator)
        """
        if not isinstance(otherV, VectorN) or len(otherV) != len(self):
            n = str(len(self))
            raise ValueError("You can only add another Vector" + n + " to this Vector" + n + " (you passed '" + str(otherV) + "')")
        r = self.copy()
        for i in range(self.mDim):
            r[i] += otherV[i]
        return r


    def __sub__(self, otherV):
        """
        Subtracts the other vector (on the right-hand side of the '+' operator) from this vector, producing a new vector
        :param otherV: A VectorN of the same size as this vectorN
        :return: The Vector sum (of the same type as this object)
        """
        if not isinstance(otherV, VectorN) or len(otherV) != len(self):
            n = str(len(self))
            raise ValueError("You can only add another Vector" + n + " to this Vector" + n + " (you passed '" + str(otherV) + "')")
        r = self.copy()
        for i in range(self.mDim):
            r[i] -= otherV[i]
        return r


    def __mul__(self, scalar):
        """
        Multiplies this vector with a scalar.
        :param scalar: the value to multiply this vector by (integer or float)
        :return: a copy of this vector with all values multiplied by the scalar
        """
        if not isinstance(scalar, int) and not isinstance(scalar, float):
            n = "Vector" + str(self.mDim)
            #raise TypeError("You can only multiply this " + n + " and a scalar. You attempted to multiply by '" + str(scalar) + "'")
            return NotImplemented
        r = self.copy()
        for i in range(self.mDim):
            r[i] *= scalar
        return r


    def __rmul__(self, scalar):
        """
        This performs vector-scalar multiplication (like __mul__).  This method is necessary, though, in the
        case in which the scalar is on the left hand side of the '*' (rather than the right-hand side in the __mul__
        method.
        :param scalar: the value to multiply this vector by (integer or float)
        :return: a copy of this vector with all values multiplied by the scalar
        """
        if not isinstance(scalar, int) and not isinstance(scalar, float):
            n = "Vector" + str(self.mDim)
            raise TypeError("You can only multiply this " + n + " and a scalar. You attempted to multiply by '" + str(scalar) + "'")
        r = self.copy()
        for i in range(self.mDim):
            r[i] *= scalar
        return r



    def __truediv__(self, scalar):
        """
        Returns a copy of this vector divided by the given scalar
        :param scalar: the value to multiply this vector by (integer or float)
        :return: a copy of this vector with all values multiplied by the scalar
        """
        if not isinstance(scalar, int) and not isinstance(scalar, float):
            n = "Vector" + str(self.mDim)
            raise TypeError("You can only divide this " + n + " by a scalar. You attempted to divide by '" + str(scalar) + "'")
        r = self.copy()
        for i in range(self.mDim):
            r[i] /= scalar
        return r



    def __neg__(self):
        """
        :return: A negated copy of this vector
        """
        r = self.copy()
        for i in range(self.mDim):
            r[i] = -r[i]
        return r



    def magnitude(self):
        """
        :return: The scalar magnitude of this vector
        """
        m = 0.0
        for val in self.mData:
            m += val ** 2
        return m ** 0.5



    def magnitudeSquared(self):
        """
        :return: The magnitude of this vector squared.  Wherever possible, use this method rather than
                 magnitude as this method doesn't involve a square root (which is costly to compute)
        """
        m = 0.0
        for val in self.mData:
            m += val ** 2
        return m



    def normalized(self):
        """
        :return: A normalized copy of this vector.   If this vector is a zero vector, a copy is returned.
        """
        r = self.copy()
        mag = self.magnitude()
        if mag == 0.0:
            return r
        for i in range(self.mDim):
            r[i] /= mag
        return r



    def isZero(self):
        """
        :return: True if this vector is a zero-vector.
        """
        for val in self.mData:
            if val != 0.0:
                return False
        return True



    def copy(self):
        """
        Creates a 'deep' copy of this VectorN and returns it
        :return: a new VectorN copy of this VectorN
        """
        v = VectorN(*self.mData)
        # No longer necessary -- see the change at the bottom of __init__
        #v.__class__ = self.__class__
        return v


    @property
    def i(self):
        """
        :return: A tuple containing all elements of this VectorN, converted to integers
        """
        L = []
        for val in self.mData:
            L.append(int(val))
        return tuple(L)


    def clamp(self, low = 0.0, high = 1.0):
        """
        :param low: a scalar (which must be less than or equal to high
        :param high: a scalar
        :return: a copy of this vector where any elements less than low are set to low and any elements higher than
                 high are set to high.
        """
        if low > high:
            raise ValueError("You must pass a low value which is less than or equal to high")
        c = self.copy()
        for i in range(len(self)):
            if c[i] < low:
                c[i] = low
            elif c[i] > high:
                c[i] = high
        return c





class Vector2(VectorN):
    """ This is a specialization of VectorN.  This will mainly be used for 2d applications and makes
        accessing the components (by name) a little easier as well as adding some polar conversion properties """
    @property
    def x(self):
        return self.mData[0]

    @x.setter
    def x(self, new_val):
        self.mData[0] = float(new_val)

    @property
    def y(self):
        return self.mData[1]

    @y.setter
    def y(self, new_val):
        self.mData[1] = float(new_val)

    @property
    def degrees(self):
        return math.degrees(math.atan2(self.mData[1], self.mData[0]))

    @property
    def degrees_inv(self):
        return math.degrees(math.atan2(-self.mData[1], self.mData[0]))

    @property
    def radians(self):
        return math.atan2(self.mData[1], self.mData[0])

    @property
    def radians_inv(self):
        return math.atan2(-self.mData[1], self.mData[0])

    @property
    def perpendicular(self):
        v = VectorN(self.x, self.y, 0)
        c = cross(v, VectorN(0, 0, 1))
        return Vector2(c[0], c[1])





class Vector3(VectorN):
    """ This is a specialization of VectorN.  This will mainly be used for 3d applications and makes
            accessing the components (by name) a little easier """
    @property
    def x(self):
        return self.mData[0]

    @x.setter
    def x(self, new_val):
        self.mData[0] = float(new_val)

    @property
    def y(self):
        return self.mData[1]

    @y.setter
    def y(self, new_val):
        self.mData[1] = float(new_val)

    @property
    def z(self):
        return self.mData[2]

    @z.setter
    def z(self, new_val):
        self.mData[2] = float(new_val)



def polar_to_vector2(radians, hypotenuse, invertY = False):
    """ Converts the given polar coordinates to an equivalent euclidean vector.  If invertY is used, the
        y-component of the new vector is negated (useful in pygame)"""
    if invertY:
        return Vector2(hypotenuse * math.cos(radians), -hypotenuse * math.sin(radians))
    else:
        return Vector2(hypotenuse * math.cos(radians), hypotenuse * math.sin(radians))


def dot(v, w):
    """ Calculates the dot product of two equal-sized VectorN objects. """
    if not isinstance(v, VectorN) or not isinstance(w, VectorN) or len(v) != len(w):
        raise ValueError("You can only take the dot product of two equally-sized VectorN's (you passed" + str(v) + \
                         " and " + str(w) + ")")
    result = 0.0
    for i in range(len(v)):
        result += v[i] * w[i]
    return result


def cross(v, w):
    """ Calculates the cross product of two Vector3's. """
    if not isinstance(v, VectorN) or not isinstance(w, VectorN) or len(v) != len(w) or len(v) != 3:
        raise ValueError("You can only take the dot product of two Vector3's (you passed" + str(v) + \
                         " and " + str(w) + ")")
    result = Vector3(0, 0, 0)

    result[0] = v[1] * w[2] - v[2] * w[1]
    result[1] = v[2] * w[0] - v[0] * w[2]
    result[2] = v[0] * w[1] - v[1] * w[0]

    return result


def pair(v, w):
    """ Computes the pair-wise product of the two equal-sized VectorN's.  Note: this is somewhat similar to
        dot product, but the individual products are kept as a vector rather than being added together. """
    if not isinstance(v, VectorN) or not isinstance(w, VectorN) or len(v) != len(w):
        raise ValueError("You must pass two equal-sized VectorN's to this method")
    c = v.copy()
    for i in range(len(c)):
        c[i] *= w[i]
    return c


if __name__ == "__main__":
    a = Vector3(4, 3, -7)
    b = Vector3(0, 2, 5)

    print(str(a) + "-dot-" + str(b) + "=" + str(dot(a, b)))

    print(str(a) + "-cross-" + str(b) + "=" + str(cross(a, b)))


