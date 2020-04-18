import ssuge

# Load some scripts so we have access to the classes within
# If we had a little more time, I'd have us write some way
# to auto-discover scripts so this step wouldn't be necessary
ssuge.load_script("../media/invader_media/ship.py")
ssuge.load_script("../media/invader_media/invader.py")
ssuge.load_script("../media/invader_media/fleet.py")
ssuge.load_script("../media/invader_media/bullet.py")

# Load the main scene (note: Application no long does this)
ssuge.load_scene("InvaderGroup", "../media/invader_media/main_scene.scene")

# For testing purposes, make all colliders have a volume visualization
ssuge.load_collider_visualizations()

# This shouldn't be necessary, but there's currently a bug in my
# blender => ogre exporter (or I'm not using it properly).  The problem
# is "edge lists" (a critical piece of shadow generation) aren't built.
# This function tells ogre to build them.  Ask, and I'll give you the
# code for this function and a few other changes you need to make (mainly
# to resources.cfg)
ssuge.build_edge_lists("InvaderGroup")