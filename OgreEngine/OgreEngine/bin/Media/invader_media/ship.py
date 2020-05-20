import ogre_module

class Ship(ogre_module.GameObject):
	def create(self, *args):
		""" This method will be called right after the game object is created.
		    If this object is created by calling ssuge.create_game_object, anything
			passed to that function beyond the required parameters will be passed to
			this method in the args tuple """
		self.mMoveRate = 15.0
		ogre_module.register_input_listener(self)
		self.mBulletNum = 0
		
		# The last position we were at (right now it's the same as our current position)
		self.mLastPosition = self.get_world_position()
		
		self.create_collider_visualization()
		
	def update(self, dt):
		""" This method will be called once per frame (before rendering) """
		# Get our position before we move
		self.mLastPosition = self.get_world_position()
	
		dx = ogre_module.get_axis("horizontal") * dt * self.mMoveRate
		self.translate_local(dx, 0, 0)
		
		# Not real space-invaders, but helpful for finding problems in my hit detection
		dy = ogre_module.get_axis("vertical") * dt * self.mMoveRate
		self.translate_local(0, dy, 0)
		
	def action_pressed(self, name):
		""" This method will be called once when an action button is pressed if this object has 
			registered to become an input listener.  See your bindings.xml file for the names of bound actions """
		if name == "fire":
			#new_bull = ssuge.create_game_object("InvaderGroup", "invader_bullet" + str(self.mBulletNum), 100, "Invader", True)
			new_bull = ogre_module.create_game_object("InvaderGroup", "player_bullet_" + str(self.mBulletNum), 100, "Bullet", True)
			new_bull_position = list(self.get_world_position())
			new_bull_position[1] += 1.5
			new_bull.set_world_position(*new_bull_position)
			self.mBulletNum += 1
			
	#def action_released(self, name):
	#	""" This method will be called once when an action button is released """
	#	if name == "fire":
	#		ssuge.log("un-PEW", (0.5, 0.1, 0.1), 2)
			
	#def axis_updated(self, axis, value):
	#	""" This method will be called whenever an axis changes values.  If using gamepads,
	#       it will be called nearly every frame (as there's a lot of "noise" on the axes)
	#	ssuge.log("axis " + axis + " " + str(value), (1, 1, 1), 0.1)
	#	if abs(value) > 0.9:
	#		ssuge.log("axis '" + axis + "' is to the limit!")
	
	def collision_continue(self, other):
		""" This method is called once per frame as long as a collision persists"""
		self.collision_start(other)
	
			
	def collision_start(self, other):
		""" This method will be called when an object starts colliding with another object """
		if other.tag() == 1:
			# Hit a bumper -- restore our last-known good position
			ogre_module.log(self.name() + " just hit a bumper", (1, 1, 0.2), 3)
			self.set_world_position(*self.mLastPosition)
			
	#def collision_end(self, other):
	#	""" This method will be called when an object stops colliding with another object """
	#	ssuge.log(self.name() + " stopped hitting " + other.name(), (0.2, 0.2, 1.0), 3)