#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <stdafx.h>
#include <log_manager.h>
#include <singleton.h>
#include <game_object_manager.h>
#include <script_manager.h>
#include <input_manager.h>
#include <collision_manager.h>

// A way to shorten the code to access Application's singleton
#define APPLICATION ssuge::Application::get_singleton_ptr()


/// S.S.U.G.E (SSU Game Engine) is a simple, but functional game engine.  
namespace ssuge
{
	// Forward declaration to camera component
	class ComponentCamera;

	/// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ 
	/// The Application class is our central hub.  It initializes all of our managers,       
	/// Has access to the ogre objects, etc. and contains the main loop.  Most of the *real* 
	/// work is delegated to the various managers (ScriptManager, InputManager, etc.)         
	/// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ 
	class Application : public OgreBites::ApplicationContext, public Singleton<Application>
	{
		// ****** ATTRIBUTES ****** //
	protected:
		/// The scene manager
		Ogre::SceneManager* mSceneManager;

		/// The GameObjectManager (or GOM)
		GameObjectManager* mGameObjectManager;

		/// Our Log Manager
		LogManager* mLogManager;

		/// Our script manager
		ScriptManager* mScriptManager;

		/// Input Manager
		InputManager* mInputManager;

		/// Collision Manager
		CollisionManager* mCollisionManager;

		/// Text Matrix to show (debug) stats
		TextMatrix * mDebugText;

		// ****** CONSTRUCTORS / DESTUCTOR ******
	public:
		/// Default constructor -- calls the initialize method.
		Application();

		/// The destructor -- does nothing.  The shutdown method will be called by ApplicationContext.
		~Application() {  }

		// ***** GETTERS / SETTERS *****
	public:
		/// Gets the SceneManager
		Ogre::SceneManager* get_scene_manager() { return mSceneManager; }

		/// Sets the active camera.  If we support multiple viewports, we'd
		/// need to indicate which viewport to attach the camera to.
		void set_active_camera(ComponentCamera* cam_comp);

		// ***** OVERRIDES (from inherited classes) *****
	protected:
		/// Our callback used to set up everything (all managers, the scene, etc.)
		void setup() override;

		/// Our callback used to shut down everything (de-allocate everything)
		void shutdown() override;

		/// Our callback used to update the scene (called before rendering) -- we update all our managers and stats panel here.
		bool frameStarted(const Ogre::FrameEvent& evt) override;


		// ***** GETTERS / SETTERS ***** 
	public:
		/// Tells ogre to start its loop
		void run() { mRoot->startRendering(); }

		/// Changes the skybox material
		void set_skybox(std::string mat_name, float distance); 

	// ***** OTHER METHODS ******
	protected:
		/// Sets up the scene (and possibly the viewport)
		void create_scene();

	public:
		/// Show / Hide the debug overlay
		void show_debug_stats(bool show);

		/// Toggle the visibility of the debug overlay
		void toggle_debug_stats();

		/// Sets a text entry in the debug stats (does nothing if index is invalid)
		void set_debug_stat(int index, std::string text);

	public:
		/// Starts the shutdown process
		void quit();
	public:
	};
}

#endif
