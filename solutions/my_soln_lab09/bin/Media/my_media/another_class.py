import ssuge

class AnotherClass(ssuge.GameObject):
	""" AnotherClass's docstring """
	def create(self, *args):
		ssuge.log("I'm an instance of 'AnotherClass'", (0.5, 0.5, 1), 10)

	# I don't have an update method!	
