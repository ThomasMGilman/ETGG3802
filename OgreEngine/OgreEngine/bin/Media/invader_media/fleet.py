import ogre_module

class Fleet(ogre_module.GameObject):
	def create(self, *args):
		self.mMovingLeft = True
		self.mSpeed = 0.1
		self.mForwardTimer = None
		ogre_module.log("made a fleet", (1, 1, 1), 1)
		
	def update(self, dt):
		dx = dy = 0
	
		if self.mForwardTimer != None:
			dy = self.mSpeed * dt
			self.mForwardTimer -= dt
			if self.mForwardTimer <= 0:
				self.mForwardTimer = None
		elif self.mMovingLeft:
			dx = -self.mSpeed * dt
		elif not self.mMovingLeft:
			dx = self.mSpeed * dt
		
		self.translate_local(dx, -dy, 0)
		
	def bump(self):
		if self.mForwardTimer == None:
			ogre_module.log("BUMP!", (1, 1, 1), 3)
			self.mForwardTimer = 1.5
			self.mMovingLeft = not self.mMovingLeft
	