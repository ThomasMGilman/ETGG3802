import ogre_module

# Load some scripts so we have access to the classes within
# If we had a little more time, I'd have us write some way
# to auto-discover scripts so this step wouldn't be necessary
ogre_module.load_script("../media/invader_media/ship.py")
ogre_module.load_script("../media/invader_media/invader.py")

# Load the main scene (note: Application no long does this)
ogre_module.load_scene("InvaderGroup", "../media/invader_media/main_scene.scene")