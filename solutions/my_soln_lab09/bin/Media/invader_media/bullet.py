import ssuge

class Bullet(ssuge.GameObject):
	def create(self, *args):
		# Assumption: user will pass us a boolean (True for player bullets, False for enemy)
		if args[0]:
			# Player Bullet
			self.mMoveSpeed = 5
			self.create_mesh_component("PlayerBullet.mesh")
			self.create_collider_component("sphere", 0.096, 5, 0x07)
		else:
			# Enemy Bullet
			self.mMoveSpeed = -1
			self.create_mesh_component("EnemyBullet.mesh")
			self.create_collider_component("sphere", 0.096, 1, 0x3c)
		self.create_collider_visualization()
			
	def update(self, dt):
		self.translate_local(0, self.mMoveSpeed * dt, 0)

			
	def collision_start(self, other):
		ssuge.log(self.name() + " " + str(self.get_world_position()) + " hit " + other.name() + " " + str(other.get_world_position()), (0.2, 1, 0.2), 3)
		ssuge.queue_destroy(self)