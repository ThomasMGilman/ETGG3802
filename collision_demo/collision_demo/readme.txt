In Lab9, we'll implement our own custom collision-detection (no libraries needed!).
We'll talk more about the math in lecture9, but one of our tasks in lab9 is to 
make a C++ ComponentCollider class (possibly with sub-classes for rotate-able spheres/boxes, possibly
all in the same class) and implement this math in the colliders.py file.  Important: even
though we *could* (since we have the embedded interpreter) do the hit detection in python, I think
it would be easier and more efficient to implement it in c++ and provide wrappers to the component
like we're doing with everything else.

Note#2: I'm including my ETGG1803 vector library.  That was just for testing in python, though.
Ogre has a fullly-featured math library -- I would just use that.