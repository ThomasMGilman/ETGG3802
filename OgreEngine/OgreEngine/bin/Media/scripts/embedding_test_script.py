import ogre_module
import random			# If this fails, check that you have the runtime python components in bin/Debug and bin/Release

ogre_module.set_skybox("Examples/SpaceSkyBox")
ogre_module.log("this will just go to the log file")
ogre_module.log("a test visual / file log message", (1, 1, 0.2), 5.0)
rand_color = tuple((random.uniform(0.5, 1.0) for i in range(3)))    # Nifty python generator!
ogre_module.log("random color message", rand_color, 5.0)

# Each of these, if uncommented, should produce an error in the log
#print("Hello"						        # A syntax error (missing paren)
#printx("Hello")					        # A name error
#ogre_module.log("a message", (2, 1, 0), 6)		# Value Error (from the 2 in the color)
#ssuge.log("a message", (1, 1, 1))			# Value Error (missing the time)
