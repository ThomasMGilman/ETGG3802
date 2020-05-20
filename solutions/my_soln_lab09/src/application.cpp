#include <stdafx.h>
#include <application.h>


// Necessary to initialize the static singleton Application inherited from Singleton
template<>
ssuge::Application* ssuge::Singleton<ssuge::Application>::msSingleton = NULL;


ssuge::Application::Application() : OgreBites::ApplicationContext("ssuge 2020"), mLogManager(NULL),
		mGameObjectManager(NULL), mSceneManager(NULL), mScriptManager(NULL), mDebugText(NULL),
	    mInputManager(NULL), mCollisionManager(NULL)
{
	// Intentionally empty -- the initializers above did the real work.
}


void ssuge::Application::set_active_camera(ComponentCamera* cam_comp)
{
	// See if we need to make a viewport
	Ogre::RenderWindow* win = getRenderWindow();
	if (win->getNumViewports() == 0)
		win->addViewport(cam_comp->mCamera);
	else
		win->getViewport(0)->setCamera(cam_comp->mCamera);
}

void ssuge::Application::setup()
{
	// Call the base-class setup method (important!)
	OgreBites::ApplicationContext::setup();

	Ogre::Root* root = getRoot();
	mSceneManager = root->createSceneManager();

	// register our scene manager with the RTSS
	Ogre::RTShader::ShaderGenerator* shadergen = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
	shadergen->addSceneManager(mSceneManager);

	// Create our managers
	const int num_debug_lines = 15;
	mLogManager = new LogManager("ssuge_log.txt", num_debug_lines, Ogre::Vector2(0.02f, 0.52f), Ogre::Vector2(0.96f, 0.48f));
	mGameObjectManager = new GameObjectManager();
	mScriptManager = new ScriptManager();
	mInputManager = new InputManager("..\\input_bindings.xml");
	mCollisionManager = new CollisionManager();

	// Make the application's text matrix
	mDebugText = new TextMatrix("debug_stats", num_debug_lines, Ogre::Vector2(0.75f, 0.02f), Ogre::Vector2(0.23f, 0.48f), false);
	mDebugText->set_visibility(true);
	mDebugText->set_background_material("TransparentGray");


	// Important -- overlays won't render without this!
	mSceneManager->addRenderQueueListener(getOverlaySystem());

	// Turn on the debug stats
	show_debug_stats(true);

	// Let this function finish setting up the scene
	create_scene();

	// Set up shadows
	mSceneManager->setAmbientLight(Ogre::ColourValue(0, 0, 0));
	mSceneManager->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
	//mSceneManager->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_MODULATIVE);
	//mSceneManager->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE);
}

void ssuge::Application::set_skybox(std::string mat_name, float distance)
{
	if (mSceneManager)
		mSceneManager->setSkyBox(true, mat_name, distance, false); 
}

void ssuge::Application::create_scene()
{
	// This is all create_scene needs to do now!  Everything else that used to
	// have to go here, now goes in the init.py (which can create game objects, load
	// other scripts, load scene files, etc.)
	mScriptManager->run_script("..\\media\\invader_media\\init.py");
}


void ssuge::Application::show_debug_stats(bool show) 
{ 
	mDebugText->set_visibility(show); 
	mLogManager->set_visibility(show);
}

/// Toggle the visibility of the debug overlay
void ssuge::Application::toggle_debug_stats()
{
	bool new_visibility = !mDebugText->get_visible();
	mDebugText->set_visibility(new_visibility);
	mLogManager->set_visibility(new_visibility);
	mCollisionManager->set_debug_visibility(new_visibility);
}



void ssuge::Application::shutdown() 
{
	// Destroy our managers -- destroy the GOM first since it destroys many of the components
	// that are used in other managers.
	if (mGameObjectManager)
		delete mGameObjectManager;

	// Needs to come after GOM destruction (it decref's all script twins)
	if (mScriptManager)
		delete mScriptManager;
	if (mLogManager)
		delete mLogManager;
	if (mCollisionManager)
		delete mCollisionManager;
	if (mInputManager)
		delete mInputManager;

	OgreBites::ApplicationContext::shutdown();
	
}




bool ssuge::Application::frameStarted(const Ogre::FrameEvent & evt)
{
	// Important: processes input!  I commented this out because now the InputManager is doing all
	// input processing.
	//OgreBites::ApplicationContext::frameStarted(evt);

	float dt = evt.timeSinceLastFrame;

	// Update our managers
	mLogManager->update(dt);
	mGameObjectManager->update(dt);
	mInputManager->update(dt);
	mCollisionManager->update(dt);

	// Set some debug stats (if visible)
	Ogre::RenderWindow* win = getRenderWindow();
	Ogre::RenderTarget::FrameStats win_stats = win->getStatistics();
	set_debug_stat(0, "FPS: " + std::to_string(win_stats.avgFPS));
	set_debug_stat(1, "Triangle Count: " + std::to_string(win_stats.triangleCount));
	set_debug_stat(2, "GameObject count: " + std::to_string(GAME_OBJECT_MANAGER->get_num_game_objects()));
	if (INPUT_MANAGER->has_axis("horizontal"))
		set_debug_stat(3, "HorizontalAxis: " + std::to_string(INPUT_MANAGER->get_axis("horizontal")));
	if (INPUT_MANAGER->has_axis("vertical"))
		set_debug_stat(4, "VerticalAxis: " + std::to_string(INPUT_MANAGER->get_axis("vertical")));

	return true;
}


void ssuge::Application::set_debug_stat(int index, std::string text)
{
	mDebugText->update_text(text, index, Ogre::ColourValue(1, 0.5f, 0.5f));
}


void ssuge::Application::quit()
{
	mRoot->queueEndRendering();
}