import ssuge

class Ship(ssuge.GameObject):
	def create(self, *args):
		ssuge.log("starting Ship.create...", (0, 1, 0), 5)
		self.mMoveRate = 5.0
		ssuge.register_input_listener(self)
		ssuge.log("...done with Ship.create", (0, 1, 0), 5)
		
	def update(self, dt):
		dx = ssuge.get_axis("horizontal") * dt
		self.translate_local(dx, 0, 0)
		
	def action_pressed(self, name):
		if name == "fire":
			ssuge.log("PEW", (1, 0, 0), 2)
			
	def action_released(self, name):
		if name == "fire":
			ssuge.log("un-PEW", (0.5, 0.1, 0.1), 2)
			
	def axis_updated(self, axis, value):
		if abs(value) > 0.9:
			ssuge.log("axis '" + axis + "' is to the limit!")