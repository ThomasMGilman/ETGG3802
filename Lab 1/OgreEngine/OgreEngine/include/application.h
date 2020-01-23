#pragma once

#include "Ogre.h"
#include "OgreApplicationContext.h"
#include "log_manager.h"
#include <unordered_set>
#include <sstream>

#define RAND_COLOUR_VAL static_cast<float>(rand()) / static_cast<float>(RAND_MAX)


class Application : public OgreBites::ApplicationContext, public OgreBites::InputListener
{
	/// Public Application Constructor and Functions ///
	public:
	
		Application();
		~Application();

		virtual void setup(void) override;

		virtual bool frameStarted(const Ogre::FrameEvent& e) override;

		bool keyPressed(const OgreBites::KeyboardEvent& evt) override;

		bool keyReleased(const OgreBites::KeyboardEvent& evt) override;

	private:
		Ogre::Root* mRoot;
		Ogre::SceneManager* mScnMgr;
		Ogre::Timer* mTimer;

		std::unordered_set<int> mKeysDown;
		std::stringstream mStringStream;

		LogManager* mLogger;
};