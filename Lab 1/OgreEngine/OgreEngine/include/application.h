#pragma once

#include "Ogre.h"
#include "OgreApplicationContext.h"
#include "log_manager.h"
#include <unordered_set>


class Application : public OgreBites::ApplicationContext, public OgreBites::InputListener
{
	/// Public Application Constructor and Functions ///
	public:
	
		Application();

		virtual void setup(void) override;

		virtual bool frameStarted(const Ogre::FrameEvent& e) override;

		bool keyPressed(const OgreBites::KeyboardEvent& evt) override;

		bool keyReleased(const OgreBites::KeyboardEvent& evt) override;

	private:
		Ogre::Root* root;
		Ogre::SceneManager* mScnMgr;
		Ogre::Timer* timer;

		std::unordered_set<int> keysDown;

		LogManager* logger;
};