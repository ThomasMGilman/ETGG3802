import ogre_module

class Ship(ogre_module.GameObject):
	def create(self, *args):
		ogre_module.log("starting Ship.create...", (0, 1, 0), 5)
		self.mMoveRate = 5.0
		ogre_module.register_input_listener(self)
		ogre_module.log("...done with Ship.create", (0, 1, 0), 5)
		
	def update(self, dt):
		dx = ogre_module.get_axis("horizontal") * dt
		self.translate_local(dx, 0, 0)
		
	def action_pressed(self, name):
		if name == "fire":
			ogre_module.log("PEW", (1, 0, 0), 2)
			
	def action_released(self, name):
		if name == "fire":
			ogre_module.log("un-PEW", (0.5, 0.1, 0.1), 2)
			
	def axis_updated(self, axis, value):
		if abs(value) > 0.9:
			ogre_module.log("axis '" + axis + "' is to the limit!")