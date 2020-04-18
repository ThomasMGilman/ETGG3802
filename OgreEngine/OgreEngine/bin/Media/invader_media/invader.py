import ssuge
import random

class Invader(ssuge.GameObject):
	def create(self, *args):
		self.mTimeToFire = random.uniform(5.0, 20.0)
		#self.mTimeToFire = 1.0
		self.mBulletCounter = 0
		self.create_collider_visualization()
		
	def update(self, dt):
		self.mTimeToFire -= dt
		if self.mTimeToFire <= 0:
			#self.mTimeToFire = 1.0
			self.mTimeToFire = random.uniform(5.0, 10.0)
			
			# This if statement is temporary.  I used it to just let the back-left (Invader.003)
			# and front-left (Invader.011) fire
			#if self.name() in ("Invader.003", "Invader.011"):
			if True:
				# Do a raycast to see if we're behind another invader.  Only fire if it fails (we have a clean shot).
				# This function only returns a list of (object, point) pairs.  If the list is empty, we hit nothing
				#if len(ssuge.ray_cast(self.get_world_position(), (0, -1, 0), 0x01, (self,))) == 0:
				ray_origin = self.get_relative_point(0, -1, 0)
				ray_direction = (0, -1, 0)
				ray_dist = 100
				ray_mask = 0x01                 # Only look for hits with layer0 (invaders)
				ray_result = ssuge.raycast(ray_origin, ray_direction, ray_mask, ray_dist)
				if len(ray_result) == 0:
					new_bull = ssuge.create_game_object("InvaderGroup", \
								self.name() + "_bullet" + str(self.mBulletCounter), 101, "Bullet", False)
					self.mBulletCounter += 1
					new_bull_position = list(self.get_world_position())
					new_bull_position[1] -= 0.65
					new_bull_position[2] += 0.3
					new_bull.set_world_position(*new_bull_position)
					#msg = self.name() + "-raycast hit nothing -- firing!"
					#ssuge.log(msg, (0.4, 1, 0.4), 3)
				#else:
					# For testing...
					#for pair in ray_result:
					#	hit_pt, hit_obj = pair
					#	msg = self.name() + "-raycast hit " + hit_obj.name() + " at " + str(hit_pt)
					#	ssuge.log(msg, (0.4, 0.4, 0.4), 3)
					
					

	def collision_start(self, other):
		if other.tag() == 100:
			# Player bullet -- kaboom!
			ssuge.queue_destroy(self)
		else:
			# Assume it's a wall
			parent = self.parent()
			if parent != None:
				parent.bump()