import ssuge

# Sneakily re-defining print -- python is awesome!
def print(s):
	ssuge.log(str(s), (1, 0, 0), 10.0)



# Phase I testing: make sure we have a GameObject class
print(dir(ssuge))
print(ssuge.GameObject.__name__)
print(ssuge.GameObject.__doc__)
print("dir(ssuge.GameObject)")
print("=====================")
for item in dir(ssuge.GameObject):
	# Ignore a few methods that we aren't interested in
	stuff_to_ignore = ('__delattr__', '__dict__', '__dir__',  '__eq__', \
				'__format__', '__ge__', '__getattribute__', '__gt__', \
				'__hash__', '__le__', '__lt__', '__ne__',  '__reduce__', \
				'__reduce_ex__', '__repr__', '__setattr__', \
				'__subclasshook__', '__weakref__')
	if str(item) not in stuff_to_ignore:
		print("   " + str(item))

# Phase II: Create a non-script aware game object
simple_gobj = ssuge.create_game_object("test_group", "simple_object", 55)
simple_gobj.create_mesh_component("robot.mesh")
simple_gobj.rotate_world(90, 1, 0, 0)
simple_gobj.scale(0.03, 0.03, 0.03)
simple_gobj.translate_world(-2.5, 7, 0)

# Phase III testing: Make a GameObject-derived class then make an instance of it
class TestClass(ssuge.GameObject):
	""" TestClass's docstring """
	def create(self, *args):
		if len(args) >= 1 and isinstance(args[0], float):
			self.mRate = args[0]
		else:
			self.mRate = 20.0
		if len(args) >= 2 and isinstance(args[1], str):
			self.create_mesh_component(args[1])
		self.rotate_world(90, 1, 0, 0)
		self.translate_world(3, 5, 1.5)
		self.scale(0.3, 0.3, 0.3)

	def update(self, dt):
		ssuge.log("updating.  dt=" + str(dt) + " rate=" + str(self.mRate), (1, 1, 1), 5)
		self.rotate_world(self.mRate * dt, 0, 0, 1)


# Note: I want the first 3 arguments to be required.  If the user passes a 4th
# argument, it should be the name of a ssuge.GameObject-derived
# class (which could've been loaded from another python script, or is in
# this python script)  If there are any arguments more than 5, they should be
# passed on to the create function in that class (if it has one)
gobj = ssuge.create_game_object("test_group", "test_object", 42, "TestClass", 90.0, "Sinbad.mesh")
print("gobj's name = ''" + gobj.name() + "'")
print(dir(gobj))

# This should test the other way of gettin a class
ssuge.load_script("../media/my_media/another_class.py")
another_gobj = ssuge.create_game_object("test_group", "another_obj", 99, "AnotherClass")
