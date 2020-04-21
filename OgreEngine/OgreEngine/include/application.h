#pragma once

#include <stdafx.h>
#include "log_manager.h"
#include "game_object_manager.h"
#include "input_manager.h"
#include "script_manager.h"
#include <Singleton.h>

#define APPLICATION Application::get_singleton_ptr()

namespace OgreEngine {
	class GameObjectManager;
	class Application : public OgreBites::ApplicationContext, public Singleton<Application>
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

		/// Updates Everything given the elapsed time
		virtual bool frameStarted(const Ogre::FrameEvent& e) override;

		/// Call the Quite Function
		void quite();

		/// Retrieves the current scene manager
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
		InputManager* mIM;
	};
}