#pragma once

#include <stdafx.h>
#include "log_manager.h"
#include "game_object_manager.h"
#include "input_manager.h"
#include "script_manager.h"
#include "collision_manager.h"
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
			// Shutdown Managers
			delete(mGOM);	// Game Object Manager
			delete(mSM);	// Script Manager
			delete(mIM);	// Input Manager
			delete(mCM);	// Collision Manager
			delete(mLM);	// Log Manager

			// Shutdown Ogre
			OgreBites::ApplicationContext::shutdown();
		};

		/// Updates Everything given the elapsed time
		virtual bool frameStarted(const Ogre::FrameEvent& e) override;

		/// Call the Quite Function
		void quite();

		/// Retrieves the root of the application
		Ogre::Root* get_root() { return this->mRoot; }

		/// Retrieves the current scene manager
		Ogre::SceneManager* get_scene_manager() { return this->mScnMgr; };

	private:
		/// Ogre Application and Scene required components
		Ogre::Root* mRoot;
		Ogre::SceneManager* mScnMgr;

		/// Game Manager Objects
		LogManager* mLM;
		GameObjectManager* mGOM;
		ScriptManager* mSM;
		InputManager* mIM;
		CollisionManager* mCM;
	};
}