#pragma once

#include "Ogre.h"
#include "OgreApplicationContext.h"


class Application : public OgreBites::ApplicationContext, public OgreBites::InputListener
{
	/// Public Application Constructor and Functions ///
	public:
	
		Application();

		virtual void setup(void) override;

		virtual bool frameStarted(const Ogre::FrameEvent& e) override;

		bool keyPressed(const OgreBites::KeyboardEvent& evt) override;

	private:
		Ogre::Root* root;
		Ogre::SceneManager* scnMgr;
		Ogre::Timer* timer;
};