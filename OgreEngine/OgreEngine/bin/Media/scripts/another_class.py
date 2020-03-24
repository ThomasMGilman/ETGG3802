import ogre_module

class AnotherClass(ogre_module.GameObject):
	""" AnotherClass's docstring """
	def create(self, *args):
		ogre_module.log("I'm an instance of 'AnotherClass'", (0.5, 0.5, 1), 10)

	# I don't have an update method!	
