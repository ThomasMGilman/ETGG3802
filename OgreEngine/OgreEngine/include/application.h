#pragma once

#include <stdafx.h>
#include "log_manager.h"
#include <Singleton.h>

#define APPLICATION Application::getSingletonPtr()

namespace OgreEngine {
	class GameObjectManager;
	class Application : public OgreBites::ApplicationContext, public OgreBites::InputListener, public Singleton<Application>
	{
		/// Public Application Constructor and Functions ///
	public:

		Application();
		~Application();

		virtual void setup(void) override;

		virtual void shutdown(void) override
		{
			// Delete all of the GameObjects
			OgreBites::ApplicationContext::shutdown();
		};

		virtual bool frameStarted(const Ogre::FrameEvent& e) override;

		bool keyPressed(const OgreBites::KeyboardEvent& evt) override;

		bool keyReleased(const OgreBites::KeyboardEvent& evt) override;

		Ogre::SceneManager* get_scene_manager() { return this->mScnMgr; };

	private:
		//std::vector<GameObject*> objects;

		Ogre::Root* mRoot;
		Ogre::SceneManager* mScnMgr;
		Ogre::Timer* mTimer;

		std::unordered_set<int> mKeysDown;
		std::stringstream mStringStream;

		LogManager* mLogger;
		GameObjectManager* mGOM;
		ScriptManager* mSM;
	};
}